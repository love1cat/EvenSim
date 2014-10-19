//
//  simulation.cpp
//  EvenSim
//
//  Created by Yuan on 10/18/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#include <cassert>

#include "alg_input.h"
#include "lp_alg.h"
#include "output.h"
#include "grdalg1.h"
#include "grdalg_generic.h"
#include "limited_optalg.h"
#include "enhance_alg1.h"
#include "enhance_alg2.h"

#include "simulation.h"

namespace {
  const int TARGET_NUM = 4;
  const int LOCATION_SCEN_NUM = 50; // how many random starting scenarios we generate
  const int WIDTH = 500;
  const int HEIGHT = 500;
  const int BATTERY_BASE = 3; // how many batteries low sensor carry
  
  const int AREA_WIDTH = 500; // area WIDTH for generating locations
  const int AREA_HEIGHT = 500; // area HEIGHT for generating locations
  const int XOFFSET = 0; // x point of the area for generating locations
  const int YOFFSET = 0; // y point of the area for generating locations
  const int SEED = 1124;
  const int BATTERY_POWER = 8829; // energy (J) per battery
  const int ENERGY_PER_METER = 28;
  
  double Mean(const std::vector<double>& v) {
    if (v.empty()){
      return 0.0;
    }
    double mean = 0;
    for (int i = 0; i < v.size(); ++i) {
      mean += v[i];
    }
    return mean / v.size();
  }
  
  double StdDev(const std::vector<double>& v) {
    double mean = Mean(v);
    double deviation = 0;
    for (int i = 0; i < v.size(); ++i) {
      deviation += (v[i] - mean) * (v[i] - mean);
    }
    return sqrt(deviation / v.size());
  }
  
  int DiffPerc(const int val1, const int val2) {
    return (double) ((val1 - val2)*100) / (double) val1;
  }
  
  double ApproxRatio(const int val1, const int val2) {
    return (double) val1 / (double) val2;
  }
}

namespace even_energy {
  Simulation::Simulation(const std::string &prefix) : prefix_(prefix) {
    assert(prefix == "even" || prefix == "line");
    GenerateTargets();
    prefix_ += "_";
  }
  
  Result Simulation::OneRun(const RunArgs &ra) const {
    /** Run each algorithm in a number of randomly generated scenarios (LOCATION_SCEN_NUM) **/
    assert(ra.sensornum % ra.location_num == 0 && "sensors should be able to divide over locations evenly.");
    const int sensor_per_location = ra.sensornum / ra.location_num;
    
    const int highsensor_num = ra.sensornum * ra.higher_sensor_ratio;
    assert((double)highsensor_num / (double)ra.sensornum == ra.higher_sensor_ratio && "higher sensor ratio should give integer higher sensor number.");
    assert((int)(ra.sensornum * ra.higher_sensor_ratio) % ra.location_num == 0 && "higher sensors should be able to divide over locations evenly.");
    const int higher_sensor_per_location = ra.sensornum * ra.higher_sensor_ratio / ra.location_num;
    
    
    // Create default input
    even_energy::AlgorithmInputWriter aiw(AREA_WIDTH, AREA_HEIGHT, XOFFSET, YOFFSET);
    aiw.WriteAlgorithmInputFiles(TARGET_NUM, ra.location_num, 1, LOCATION_SCEN_NUM, BATTERY_POWER * BATTERY_BASE, ENERGY_PER_METER, SEED, "alginput.txt", "./", prefix_);
    even_energy::AlgorithmInputReader air("./EEinput/alginput.txt", sensor_per_location, higher_sensor_per_location, ra.highsensor_diff * BATTERY_POWER);
    
    /** Algorithms **/
    even_energy::GreedyAlg1 grdalg1(true);
    //  even_energy::GreedyAlgGeneric grdalg_gen_ass(true);
    even_energy::GreedyAlgGeneric grdalg_gen(false);
    even_energy::LPAlg lpa(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), true); // LP-relax, For comparison
    even_energy::LPAlg lpa_int(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), false); // Integer LP
    
    even_energy::EnhanceAlg1 enhanc1;
    even_energy::EnhanceAlg2 enhanc2;
    
    /** Result vectors **/
    std::vector<double> grdalg_gen_obj; // objective value of greedy enhancing algorithm
    //  std::vector<double> grdalg_gen_ass_obj; // objective value of greedy enhancing algorithm
    std::vector<double> grdalg_gen_obj_enh;
    std::vector<double> grdalg_gen_obj_enh2;
    
    std::vector<double> lp_obj;
    std::vector<double> obj_ratio;
    std::vector<double> obj_ratio_enh;
    std::vector<double> obj_ratio_enh2;
    
    Result r;
    r.count = 0;
    
    /** Start simulation loop **/
    while (air.ReadNextInputSet()) {
      
      // LP algorithms
      int lpval = 0;
      if (ra.use_lp_relax) {
        // LP-relax
        lpa.SolveNextScenario(air.GetDataMatrix());
        lpval = lpa.GetCurrentResult().GetObjValue();
      } else {
        // Integer LP
        lpa_int.SolveNextScenario(air.GetDataMatrix());
        lpval = lpa_int.GetCurrentResult().GetObjValue();
      }
      
      lp_obj.push_back(lpval);
      
      int grdalg_gen_objval = grdalg_gen.Solve(air); // no enhancing greedy alg
      
      // apply enhance alg 1
      grdalg_gen.enhance(&enhanc1);
      int grdalg_gen_objval_enh = grdalg_gen.get_obj(); // get obj after enhancing
      
      // apply enhance alg 2
      grdalg_gen.enhance(&enhanc2);
      int grdalg_gen_objval_enh2 = grdalg_gen.get_obj(); // get obj after enhancing
      
      grdalg_gen_obj.push_back(grdalg_gen_objval);
      grdalg_gen_obj_enh.push_back(grdalg_gen_objval_enh);
      grdalg_gen_obj_enh2.push_back(grdalg_gen_objval_enh2);
      
      ++r.count;
      
      obj_ratio.push_back((double)grdalg_gen_objval / (double)lpval);
      obj_ratio_enh.push_back((double)grdalg_gen_objval_enh / (double)lpval);
      obj_ratio_enh2.push_back((double)grdalg_gen_objval_enh2 / (double)lpval);
    }
    
    r.obj_mean = Mean(lp_obj);
    r.objratio_mean = Mean(obj_ratio);
    r.max = *(std::max_element(obj_ratio.begin(), obj_ratio.end()));
    r.min = *(std::min_element(obj_ratio.begin(), obj_ratio.end()));
    
    r.objratio_mean_enh = Mean(obj_ratio_enh);
    r.max_enh = *(std::max_element(obj_ratio_enh.begin(), obj_ratio_enh.end()));
    r.min_enh = *(std::min_element(obj_ratio_enh.begin(), obj_ratio_enh.end()));
    
    r.objratio_mean_enh2 = Mean(obj_ratio_enh2);
    r.max_enh2 = *(std::max_element(obj_ratio_enh2.begin(), obj_ratio_enh2.end()));
    r.min_enh2 = *(std::min_element(obj_ratio_enh2.begin(), obj_ratio_enh2.end()));
    
    return r;
  }
  
  void Simulation::GenerateTargets() const {
    // write target file
    
    if (prefix_ == "line") {
      // line
      int pointheight = HEIGHT / 2;
      int interval = WIDTH / (TARGET_NUM + 1);
      FILE* fp = fopen("./EEinput/line_TargetFile.txt", "w");
      fprintf(fp, "1\n0\n%d\n500\t500\n", TARGET_NUM);
      for (int i = 0; i < TARGET_NUM; ++i) {
        fprintf(fp, "%d\t%d\n", (i + 1) * interval, pointheight);
      }
      fclose(fp);
    } else if (prefix_ == "even"){
      // even
      int targetperline = sqrt(TARGET_NUM);
      if(targetperline*targetperline != TARGET_NUM) {
        printf("No even deployment of targets available!\n");
        exit(1);
      }
      int winterval = WIDTH / (targetperline + 1);
      int hinterval = HEIGHT / (targetperline + 1);
      FILE* fp = fopen("./EEinput/even_TargetFile.txt", "w");
      fprintf(fp, "1\n0\n%d\n500\t500\n", TARGET_NUM);
      for(int i=0;i<targetperline;++i)
        for(int j=0;j<targetperline;++j){
          fprintf(fp, "%d\t%d\n", (i + 1) * winterval, (j + 1) * hinterval);
        }
      fclose(fp);
    }
  }
  
  void Simulation::RunOptimalGrd(const std::string &outfile) {
    int sensornum[] = {20, 40, 60, 80, 100};
    int location_num = 1;
    int target_scen_number = 1;
    
    std::vector<double> vec_optval;
    
    const int SENSORPATTERN = 5;
    
    even_energy::OutputWriter ow("./opt_grd/" + prefix_ + outfile, true);
    
    even_energy::Point areasize, offsets[5];
    areasize.x = areasize.y = 50;
    int count = 0;
    for (int k = 0; k < SENSORPATTERN; ++k) {
      ow.WriteVal(sensornum[k]);
      ow.WriteVal("\t");
      for (int i = 0; i < 5; ++i) {
        std::cout << "Run " << count++ <<std::endl;
        offsets[i].x = offsets[i].y = 50 * i;
        // Create default input
        even_energy::AlgorithmInputWriter aiw(areasize.x, areasize.y, offsets[i].x, offsets[i].y);
        aiw.WriteAlgorithmInputFiles(TARGET_NUM, location_num, target_scen_number, LOCATION_SCEN_NUM, 26487, 28, SEED, "alginput.txt", "./", prefix_);
        even_energy::AlgorithmInputReader air("./EEinput/alginput.txt", sensornum[k], 0, 0);
        
        even_energy::GreedyAlgGeneric grdalg_gen(false);
        
        while(air.ReadNextInputSet()) {
          int optval = grdalg_gen.Solve(air);
          vec_optval.push_back(optval);
        }
        ow.WriteVal((int)(Mean(vec_optval) / 1000)); // /1000 = KJ
        ow.WriteVal("\t");
      }
      ow.WriteEndOfLine();
    }
    std::cout << "Done optimal greedy with different area case." << count++ <<std::endl;
  }
}