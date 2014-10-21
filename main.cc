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
  std::string outfile, toutfile;
  
  double locnum[] = {1, 2, 5, 10};
  
  // Diff location and min, max, avg of objratio
  outfile = sim.prefix() + "out_diff_loc_minmaxavg.txt";
  toutfile = sim.prefix() + "out_diff_loc_rt_ratio.txt";
  {
    // Clean the file.
    ee::OutputWriter ow(dir + outfile, true);
    ee::OutputWriter ow2(dir + "enh_" + outfile, true);
    ee::OutputWriter ow3(dir + "enh2_" + outfile, true);
    
    ee::OutputWriter tow(dir + toutfile, true);
    ee::OutputWriter tow2(dir + "enh_" + toutfile, true);
    ee::OutputWriter tow3(dir + "enh2_" + toutfile, true);
  }
  {
    ee::OutputWriter ow(dir + outfile, true);
    ee::OutputWriter ow2(dir + "enh_" + outfile, true);
    ee::OutputWriter ow3(dir + "enh2_" + outfile, true);
    
    ee::OutputWriter tow(dir + toutfile, true);
    ee::OutputWriter tow2(dir + "enh_" + toutfile, true);
    ee::OutputWriter tow3(dir + "enh2_" + toutfile, true);
    int count = 0;
    for (int i = 0; i < 4; ++i) {
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      
      tow.WriteVal(locnum[i]);
      tow.WriteVal("\t");
      
      // Run now.
      ra.location_num = locnum[i];
      ee::Result r = sim.OneRun(ra);
      
      count += r.count;
      WriteRange(ow, r.g_ratio);
      ow.WriteEndOfLine();
      
      tow.WriteVal(r.runtime);
      tow.WriteVal("\t");
      tow.WriteEndOfLine();
      
      ow2.WriteVal((double)locnum[i]);
      ow2.WriteVal("\t");
      WriteRange(ow2, r.g_enh_ratio);
      ow2.WriteEndOfLine();
      
      tow2.WriteVal((double)locnum[i]-0.2);
      tow2.WriteVal("\t");
      WriteRange(tow2, r.rt_enh_ratio);
      tow2.WriteEndOfLine();
      
      ow3.WriteVal(locnum[i]);
      ow3.WriteVal("\t");
      WriteRange(ow3, r.g_enh2_ratio);
      ow3.WriteEndOfLine();
      
      tow3.WriteVal((double)locnum[i]+0.2);
      tow3.WriteVal("\t");
      WriteRange(tow3, r.rt_enh2_ratio);
      tow3.WriteEndOfLine();
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
        ee::Result r = sim.OneRun(ra);
        int objval = r.obj_mean_enh2;
        ow.WriteVal(objval / 1000);
        ow.WriteVal("\t");
        ow.WriteVal(r.g_enh2_ratio.avg);
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
//  /** Run small scale 1 **/
//  ra.sensornum = 40;
//  ra.use_lp_relax = false;
//  ra.higher_sensor_ratio = 0.25;
//  ra.highsensor_diff = 3;
//  dir = "./small/";
//  RunScen(sim, ra, dir);
  
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
  
  /** Run optimal greedy **/
  //sim.RunOptimalGrd("opt_diff_area.txt");
  
  return 0;
}

