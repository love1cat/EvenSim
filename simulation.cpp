//
//  simulation.cpp
//  EvenSim
//
//  Created by Yuan on 10/18/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#include <cassert>
#include <ctime>

#include "alg_input.h"
#include "lp_alg.h"
#include "output.h"
#include "grdalg_generic.h"
#include "limited_optalg.h"
#include "enhance_alg1.h"
#include "enhance_alg2.h"

#include "simulation.h"

namespace {
  const int TARGET_NUM = 4;
  const int LOCATION_SCEN_NUM = 100; // how many random starting scenarios we generate
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
  
  double DiffPerc(const int val1, const int val2) {
    return (double) ((val1 - val2)*100) / (double) val1;
  }
  
  double ApproxRatio(const int val1, const int val2) {
    return (double) val1 / (double) val2;
  }
  
  void AssignMinMaxAvg(const std::vector<double> &vec, even_energy::Range &r) {
    r.avg = Mean(vec);
    r.max = *(std::max_element(vec.begin(), vec.end()));
    r.min = *(std::min_element(vec.begin(), vec.end()));
  }
}

namespace even_energy {
  Simulation::Simulation(const std::string &prefix) : prefix_(prefix) {
    assert(prefix == "even" || prefix == "line");
    GenerateTargets();
    prefix_ += "_";
  }
  
  std::vector<Result> Simulation::OneRun(const RunArgs &ra) const {
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
    //  even_energy::GreedyAlgGeneric grdalg_gen_ass(true);
    int alg_count = 0;

    even_energy::LPAlg lpa(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), true); // LP-relax, For comparison
    even_energy::LPAlg lpa_int(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), false); // Integer LP
    const int LP_ALG_ID = alg_count++;
    
    even_energy::GreedyAlgGeneric grdalg_gen;
    const int GRD_ALG_ID = alg_count++;
    
    even_energy::EnhanceAlg1 enhanc1;
    const int ENH_ALG_ID = alg_count++;
    even_energy::EnhanceAlg2 enhanc2;
    const int ENH2_ALG_ID = alg_count++;
    
    /** Result vectors **/
    typedef std::vector<double> Values;
    typedef std::vector<Values> ResultVec;
    ResultVec objs(alg_count);
    ResultVec obj_ratios(alg_count);
    ResultVec runtimes(alg_count);
    ResultVec runtime_ratios(alg_count);
    
    std::vector<Result> r(alg_count);
    for (int i = 0; i < alg_count; ++i) {
      r[i].count = 0;
    }
    
    /** Start simulation loop **/
    while (air.ReadNextInputSet()) {
      std::vector<double> vals(alg_count);
      std::vector<double> durations(alg_count);
      double lpval = 0;
      double lp_runt = 0.0;
      
      
      // LP algorithms
      clock_t start = clock();
      if (ra.use_lp_relax) {
        // LP-relax
        lpa.SolveNextScenario(air.GetDataMatrix());
        lpval = lpa.GetCurrentResult().GetObjValue();
      } else {
        // Integer LP
        lpa_int.SolveNextScenario(air.GetDataMatrix());
        lpval = lpa_int.GetCurrentResult().GetObjValue();
      }
      vals[LP_ALG_ID] = lpval;
      clock_t duration = clock() - start;
      double duration_sec = (double)duration / CLOCKS_PER_SEC;
      lp_runt = duration_sec;
      durations[LP_ALG_ID] = duration_sec;
      
      start = clock();
      vals[GRD_ALG_ID] = grdalg_gen.Solve(air); // no enhancing greedy alg
      
      duration = clock() - start;
      duration_sec = (double)duration / CLOCKS_PER_SEC;
      durations[GRD_ALG_ID] = duration_sec;
      
      // apply enhance alg 1
      grdalg_gen.enhance(&enhanc1);
      vals[ENH_ALG_ID] = grdalg_gen.get_obj(); // get obj after enhancing
      
      duration = clock() - start;
      duration_sec = (double)duration / CLOCKS_PER_SEC;
      durations[ENH_ALG_ID] = duration_sec;
      
      // apply enhance alg 2
      grdalg_gen.enhance(&enhanc2);
      vals[ENH2_ALG_ID] = grdalg_gen.get_obj(); // get obj after enhancing

      duration += clock() - start; // duration of enhance 2 = duration of enhance 1 + actual running time
      duration_sec = (double)duration / CLOCKS_PER_SEC;
      durations[ENH2_ALG_ID] = duration_sec;
      
      assert(vals.size() == alg_count &&
             durations.size() == alg_count &&
             objs.size() == alg_count &&
             obj_ratios.size() == alg_count &&
             runtimes.size() == alg_count &&
             runtime_ratios.size() == alg_count &&
             r.size() == alg_count);
      for (int i = 0; i < alg_count; ++i) {
        objs[i].push_back(vals[i]);
        if (lpval != 0.0) {
          obj_ratios[i].push_back((double)vals[i] / lpval);
        }
        runtimes[i].push_back(durations[i]);
        if (durations[i] != 0.0) {
          runtime_ratios[i].push_back(lp_runt / durations[i]);
        }
        ++r[i].count;
      }
    }
    
    for (int i = 0; i < alg_count; ++i) {
      AssignMinMaxAvg(objs[i], r[i].obj);
      AssignMinMaxAvg(obj_ratios[i], r[i].obj_ratio);
      AssignMinMaxAvg(runtimes[i], r[i].runtime);
      AssignMinMaxAvg(runtime_ratios[i], r[i].rt_ratio);
    }
    
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
  
  void Simulation::RunOptimalGrd(const std::string &outfile) const {
    const int TOTAL_BATTERY = 3 * 6 * 9 * 2;
    const int BATTERIES[] = {3, 6, 9};
    const int SENSORPATTERN = 3;
    int location_num = 1;
    int target_scen_number = 1;
    
    std::vector<double> vec_optval;
    
    even_energy::OutputWriter ow("./opt_grd/" + prefix_ + outfile, true);
    
    even_energy::Point areasize, offsets[5];
    areasize.x = areasize.y = 50;
    int count = 0;
    for (int i = 0; i < 5; ++i) {
        for (int k = 0; k < SENSORPATTERN; ++k) {
        std::cout << "Run " << count++ <<std::endl;
        offsets[i].x = offsets[i].y = 50 * i;
        // Create default input
        even_energy::AlgorithmInputWriter aiw(areasize.x, areasize.y, offsets[i].x, offsets[i].y);
        aiw.WriteAlgorithmInputFiles(TARGET_NUM, location_num, target_scen_number, LOCATION_SCEN_NUM, BATTERY_POWER * BATTERIES[k], 28, SEED, "alginput.txt", "./", prefix_);
        even_energy::AlgorithmInputReader air("./EEinput/alginput.txt", TOTAL_BATTERY / BATTERIES[k], 0, 0);
        
        even_energy::GreedyAlgGeneric grdalg_gen;
        
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
