#include <set>
#include <limits>

#include "grdalg_base.h"
#include "enhance_algbase.h"

namespace even_energy {
  int Sensor::count = 0;
  SensorPtr Location::GetSensor() {
    if (IsEmpty()) throw "Get sensor from empty list";
    SensorPtr sp = sensorList_.back();
    if (sp->type == 0) --lowcount_;
    else --highcount_;
    sensorList_.pop_back();
    return sp;
  }
  
  void Location::Insert(SensorPtr sp) {
    sensorList_.push_back(sp);
    if (sp->type == 0) ++lowcount_;
    else ++highcount_;
  }
  
  Target::Target(const int id, const int location_count, const int energy_diff, const IntegerMatrix& locrm)
  : locrm_(locrm), energy_diff_(energy_diff) {
    totalrm_ = 0;
    id_ = id;
    favloc_ = 0;
    int max = 0;
    for (int i = 0; i < location_count; ++i) {
      if (locrm_[i][id] > max) {
        max = locrm_[i][id];
        favloc_ = i;
      }
    }
    lowsp_.reset(new Sensor(0, 0));
    lowsp_->realRm  = -1;
    lowsp_->id = -1;
  }
  
  void Target::Insert(SensorPtr sp) {
    // set realRm before insert!
    if(sp->type==0)
      sp->realRm = locrm_[sp->locationID][this->id_];
    else
      sp->realRm = locrm_[sp->locationID][this->id_] + energy_diff_;
    sensorSet_.insert(sp);
    totalrm_ += sp->realRm;
  }
  
  SensorPtr Target::Remove(SensorPtr sp) {
    if(sensorSet_.empty()) throw "remove from empty sensor set!";
    SensorSet::iterator iter = sensorSet_.find(sp);
    SensorPtr ret;
    if (iter != sensorSet_.end()) {
      ret = *iter;
      totalrm_ -= sp->realRm;
      sensorSet_.erase(iter);
    }
    return ret;
  }
  
  SensorPtr Target::LowSensor() {
    if(sensorSet_.empty()) return SensorPtr();
    SensorSet::iterator iter = sensorSet_.lower_bound(lowsp_);
    return *iter;
  }
  
  GreedyAlgBase::GreedyAlgBase() {
  };
  
  int GreedyAlgBase::Solve(const AlgorithmInputReader& air){
    // clean up
    locationVec_.clear();
    targetVec_.clear();
    
    init(air);
    SolveSubRoutine();
    
    return get_obj();
  }
  
  void GreedyAlgBase::enhance(EnhanceAlgBase *enhance_alg){
    enhance_alg->enhance(this);
  }
  
  int GreedyAlgBase::get_obj() {
    int lowid = GetLowestTarget();
    return targetVec_[lowid]->TotalRM();
  }
  
  int GreedyAlgBase::GetLowestTarget() {
    int low = std::numeric_limits<int>::max();
    int id = 0;
    for (int i = 0; i < targetVec_.size(); ++i)
      if (targetVec_[i]->TotalRM() < low) {
        low = targetVec_[i]->TotalRM();
        id = i;
      }
    return id;
  }
  
  bool GreedyAlgBase::IsSensorAvailable(){
    for(int i=0;i<locationVec_.size();++i){
      if(!locationVec_[i]->IsEmpty()){
        return true;
      }
    }
    return false;
  }
  
  void GreedyAlgBase::init(const AlgorithmInputReader& air) {
    // initilization
    location_count_ = air.GetLocationCount();
    target_count_ = air.GetTargetCount();
    energy_diff_ = air.GetEnergyDiff();
    locrm_ = air.GetLocationRMMatrix();
    battery_power_ = air.GetBatteryPower();
    // initialize sensors and location vector
    int highsensornum = air.GetHighSensorNumber();
    for (int i = 0; i < location_count_; ++i) {
      LocationPtr lp(new Location());
      for (int j = 0; j < air.GetSensorPerLocation(); ++j) {
        int type;
        if (j < highsensornum) type = 1;
        else type = 0;
        
        SensorPtr sp(new Sensor(i, type));
        lp->Insert(sp);
      }
      locationVec_.push_back(lp);
    }
    
    // initialize targets
    for(int i=0;i<target_count_;++i){
      TargetPtr tp(new Target(i, location_count_, energy_diff_, air.GetLocationRMMatrix()));
      targetVec_.push_back(tp);
    }
  }
  
  double GreedyAlgBase::GetTheoryBound(){
    // find energy ratio
    double mineratio = battery_power_ / (battery_power_ + energy_diff_);
    for(int i=0;i<location_count_;++i){
      double curmer = std::numeric_limits<double>::max();
      for(int j=0;j<target_count_;++j){
        double ratio = (double)locrm_[i][j] / (double)(locrm_[i][j] + energy_diff_);
        if(ratio < curmer) curmer = ratio;
      }
      if(curmer < mineratio) mineratio = curmer;
    }
    
    // find location energy ratio
    double minlocratio = std::numeric_limits<double>::max();
    for(int i=0;i<target_count_;++i){
      double min = std::numeric_limits<double>::max();
      double max = 0;
      for(int j=0;j<location_count_;++j){
        if(locrm_[j][i] < min) min = locrm_[j][i];
        if(locrm_[j][i] > max) max = locrm_[j][i];
      }
      double ratio  = (min + energy_diff_) / (max + energy_diff_);
      if(ratio < minlocratio) minlocratio = ratio;
    }
    
    return mineratio * minlocratio;
  }
}
