//
//  main.cpp
//  even_energy
//
//  Created by YUAN SONG on 5/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>

#include "output.h"
#include "simulation.h"

FILE *timefp;

namespace ee = even_energy;

namespace {
  void WriteRange(ee::OutputWriter &ow, const ee::Range &r) {
    ow.WriteVal(r.avg);
    ow.WriteVal("\t");
    ow.WriteVal(r.min);
    ow.WriteVal("\t");
    ow.WriteVal(r.max);
    ow.WriteVal("\t");
  }
}

/** Run designed simulation 1 **/
void RunScen(const ee::Simulation &sim, ee::RunArgs &ra, const std::string &dir) {
  std::string outfile;
  
  double locnum[] = {1, 2, 5, 10};
  
  // Diff location and min, max, avg of objratio
  outfile = sim.prefix() + "out_diff_loc_minmaxavg.txt";
  {
    // Clean the file.
    ee::OutputWriter ow(dir + outfile, true);
  }
  {
    ee::OutputWriter ow(dir + outfile, true);
    for (int i = 0; i < 4; ++i) {
      // Run now.
      ra.location_num = locnum[i];
      std::vector<ee::Result> r = sim.OneRun(ra);
      
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      for (int i = 0; i < r.size(); ++i) {
        WriteRange(ow, r[i].obj);
        WriteRange(ow, r[i].obj_ratio);
        WriteRange(ow, r[i].runtime);
        WriteRange(ow, r[i].rt_ratio);
        ow.WriteVal(r[i].count);
        ow.WriteVal("\t");
      }
      ow.WriteEndOfLine();
    }
  }
}

/** Run designed simulation 2 **/
void RunScen2(const ee::Simulation &sim, ee::RunArgs &ra, const std::string &dir) {
  
  // Diff location, diff high ratio
  double locnum[] = {1, 2, 5, 10};
  int high_sensor_num[] = {0, 10, 20};
  const int TOTAL_BATTERY = 120;
  const int HIGH_BATTERY = 6;
  const int LOW_BATTERY = 3;
  std::string outfile = sim.prefix() + "out_diff_loc_ratio.txt";
  {
    // Clean the file.
    even_energy::OutputWriter ow(dir + outfile, true);
  }
  {
    even_energy::OutputWriter ow(dir + outfile, true);
    for (int i = 0; i < 4; ++i) {
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      ra.location_num = locnum[i];
      for (int j = 0; j < 3; ++j) {
        assert(TOTAL_BATTERY >= HIGH_BATTERY * high_sensor_num[j]);
        int low_sensor_num = (TOTAL_BATTERY - HIGH_BATTERY * high_sensor_num[j]) / LOW_BATTERY;
        ra.sensornum = low_sensor_num + high_sensor_num[j];
        ra.higher_sensor_ratio = (double)high_sensor_num[j] / (double)ra.sensornum;
        std::vector<ee::Result> r = sim.OneRun(ra);
        const int ENH2_ALG_ID = r.size() - 1;
        int objval = r[ENH2_ALG_ID].obj.avg;
        ow.WriteVal(objval / 1000);
        ow.WriteVal("\t");
        int objratio = r[ENH2_ALG_ID].obj_ratio.avg;
        ow.WriteVal(objratio);
        ow.WriteVal("\t");
      }
      ow.WriteEndOfLine();
    }
  }
}

int main(int argc, const char * argv[]) {
  if (argc < 2) {
    printf("Please specify \"even\" or \"line\" as argument!");
    exit(1);
  }
  std::string prefix = argv[1];
  
  ee::Simulation sim(prefix);
  ee::RunArgs ra;
  std::string dir;
  /** Run small scale 1 **/
  ra.sensornum = 40;
  ra.use_lp_relax = false;
  ra.higher_sensor_ratio = 0.25;
  ra.highsensor_diff = 3;
  dir = "./small/";
  RunScen(sim, ra, dir);
//
//  /** Run small scale 2 **/
//  ra.use_lp_relax = false;
//  ra.highsensor_diff = 3;
//  dir = "./small2/";
//  RunScen2(sim, ra, dir);
  
  /** Run large scale **/
  ra.sensornum = 200;
  ra.use_lp_relax = true;
  ra.higher_sensor_ratio = 0.25;
  ra.highsensor_diff = 3;
  dir = "./large/";
  RunScen(sim, ra, dir);
  
//  /** Run optimal greedy **/
//  sim.RunOptimalGrd("opt_diff_area.txt");
  
  return 0;
}

