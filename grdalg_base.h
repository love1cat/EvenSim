/*
 * File:   grd_algbase.h
 * Author: andy
 *
 * Created on December 18, 2012, 8:35 PM
 */

#ifndef GRD_ALGBASE_H
#define	GRD_ALGBASE_H

#include <list>
#include <vector>
#include <set>
#include <boost/shared_ptr.hpp>
#include "alg_input.h"

namespace even_energy{
  class Sensor;
  class Location;
  class Target;
  class EnhanceAlgBase;
  
  typedef boost::shared_ptr<Sensor> SensorPtr;
  typedef boost::shared_ptr<Location> LocationPtr;
  typedef boost::shared_ptr<Target> TargetPtr;
  
  struct Sensor{
    inline Sensor(const int locIDval, const int typeval) : locationID(locIDval), type(typeval){id = count++;}
    const int locationID;
    const int type; // 0-low energy 1-high energy
    int curRm;
    int realRm;
    int id;
  private:
    static int count;
  };
  
  class Location{
  public:
    SensorPtr GetSensor();
    void Insert(SensorPtr sp);
    inline bool IsEmpty(){return sensorList_.empty();}
  private:
    std::list<SensorPtr> sensorList_;
    int highcount_;
    int lowcount_;
  };
  
  class SensorPtrComp{
  public:
    bool operator() (const SensorPtr& sp1, const SensorPtr& sp2) const{
      if(sp1->realRm == sp2->realRm) {
        return sp1->id < sp2->id;
      } else return sp1->realRm < sp2->realRm;
    }
  };
  
  class LocationPtrComp{
  public:
    bool operator() (const LocationPtr& lp1, const LocationPtr& lp2) const{
      return true; // TODO: Fix this.
    }
  };
  
  typedef std::set<SensorPtr, SensorPtrComp> SensorSet;
  
  class Target{
  public:
    Target(const int id, const int location_count, const int energy_diff, const IntegerMatrix& locrm);
    void Insert(SensorPtr sp);
    SensorPtr Remove(SensorPtr sp);
    SensorPtr LowSensor();
    inline int GetSize() const{return sensorSet_.size();}
    inline int TotalRM(){return totalrm_;}
    inline int TotalAssumedRM(){return assumedTotalrm_;}
    inline int FavLocation(){return favloc_;}
    inline const SensorSet& GetSensorSet(){return sensorSet_;}
    
    // Hack for k heterougeneous
    inline void IncreaseTotalRM(const int rm) {
      assumedTotalrm_ += rm;
      totalrm_ += rm;
    }
    
    inline void DecreaseTotalRM(const int rm) {
      assumedTotalrm_ -= rm;
      totalrm_ -= rm;
    }
  private:
    std::set<LocationPtr> closestLocations_;
    const IntegerMatrix locrm_;
    SensorSet sensorSet_;
    int totalrm_;
    int assumedTotalrm_;
    int favloc_;
    int energy_diff_;
    int id_;
    SensorPtr lowsp_;
  };
  
  class GreedyAlgBase{
  public:
    GreedyAlgBase();
    virtual int Solve(const AlgorithmInputReader& air);
    void enhance(EnhanceAlgBase *enhance_alg);
    int get_obj();
    inline const std::vector<TargetPtr>& GetTargetVec(){return targetVec_;}
    double GetTheoryBound();
  protected:
    virtual void SolveSubRoutine() = 0;
    int GetAssumedLowestTarget();
    int GetLowestTarget();
    void EnhanceAlg(std::vector<TargetPtr>& tvec);
    bool IsSensorAvailable();
    std::vector<TargetPtr> targetVec_;
    std::vector<LocationPtr> locationVec_;
    void init(const AlgorithmInputReader& air);
    int location_count_;
    int target_count_;
    double energy_diff_;
    double battery_power_;
    IntegerMatrix locrm_;
  };
}

#endif	/* GRD_ALGBASE_H */

