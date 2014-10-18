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

/** Run small scale **/
void RunScen(const ee::Simulation& sim) {
  std::string outfile;
  ee::RunArgs ra;
  ra.sensornum = 40;
  ra.use_lp_relax = false;
  ra.higher_sensor_ratio = 0.25;
  ra.highsensor_diff = 3;
  
  double locnum[] = {1, 2, 5, 10};
  
  int run_count = 0;
  // Diff location and min, max, avg of objratio
  outfile = sim.prefix() + "out_diff_loc_minmaxavg.txt";
  {
    // Clean the file.
    ee::OutputWriter ow(outfile, true);
    ee::OutputWriter ow2("enh_" + outfile, true);
    ee::OutputWriter ow3("enh2_" + outfile, true);
  }
  {
    ee::OutputWriter ow(outfile, true);
    ee::OutputWriter ow2("enh_" + outfile, true);
    ee::OutputWriter ow3("enh2_" + outfile, true);
    int wincount = 0;
    int count = 0;
    for (int i = 0; i < 4; ++i) {
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      
      // Run now.
      ra.location_num = locnum[i];
      ee::Result r = sim.OneRun(ra);
      
      count += r.count;
      ow.WriteVal(r.objratio_mean);
      ow.WriteVal("\t");
      ow.WriteVal(r.min);
      ow.WriteVal("\t");
      ow.WriteVal(r.max);
      ow.WriteVal("\t");
      ow.WriteEndOfLine();
      
      ow2.WriteVal(locnum[i]);
      ow2.WriteVal("\t");
      ow2.WriteVal(r.objratio_mean_enh);
      ow2.WriteVal("\t");
      ow2.WriteVal(r.min_enh);
      ow2.WriteVal("\t");
      ow2.WriteVal(r.max_enh);
      ow2.WriteVal("\t");
      ow2.WriteEndOfLine();
      
      ow3.WriteVal(locnum[i]);
      ow3.WriteVal("\t");
      ow3.WriteVal(r.objratio_mean_enh2);
      ow3.WriteVal("\t");
      ow3.WriteVal(r.min_enh2);
      ow3.WriteVal("\t");
      ow3.WriteVal(r.max_enh2);
      ow3.WriteVal("\t");
      ow3.WriteEndOfLine();
    }
  }
}

//void RunLPCompareTest() {
//  sensornum *= MULTIPLIER;
//  double higher_sensor_ratio = 0.1;
//  int location_num = 5;
//  int highsensor_diff = 8829;
//  int sensornum = 50;
//  const int location_scenario_num = 100; // large number may be too slow.
//  const int total_battery = sensornum * BATTERY_BASE;
//  const int high_battery = highsensor_diff / 8829 + BATTERY_BASE;
//  const int high_sensor_num = sensornum * higher_sensor_ratio;
//  const int total_high_battery = high_sensor_num * high_battery;
//  const int low_sensor_num = (total_battery - total_high_battery ) / 3;
//  const int sensor_per_location = (low_sensor_num + high_sensor_num) / location_num;
//  const int higher_sensor_per_location = high_sensor_num / location_num;
//  //  const int sensor_per_location = sensornum / location_num;
//
//  // Create default input
//  even_energy::AlgorithmInputWriter aiw(AREA_WIDTH, AREA_HEIGHT, XOFFSET, YOFFSET);
//  aiw.WriteAlgorithmInputFiles(TARGET_NUM, location_num, 1, location_scenario_num, 8829*BATTERY_BASE, 28, SEED, "alginput.txt", "./", prefix);
//  even_energy::AlgorithmInputReader air("./EEinput/alginput.txt", sensor_per_location, higher_sensor_per_location, highsensor_diff);
//
//  even_energy::LPAlg lpa_relax(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), true); // LP-relax
//  even_energy::LPAlg lpa_int(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), false); // Integer LP, For comparison
//
//  std::vector<double> lp_relax_obj;
//  std::vector<double> lp_int_obj;
//  std::vector<double> obj_ratio;
//
//  Result r;
//  r.wincount = 0;
//  r.count = 0;
//  while (air.ReadNextInputSet()) {
//    // For comparison
//    lpa_relax.SolveNextScenario(air.GetDataMatrix());
//    int lp_relax_val = lpa_relax.GetCurrentResult().GetObjValue();
//    lp_relax_obj.push_back(lp_relax_val);
//
//    lpa_int.SolveNextScenario(air.GetDataMatrix());
//    int lp_int_val = lpa_int.GetCurrentResult().GetObjValue();
//    lp_int_obj.push_back(lp_int_val);
//
//    if (lp_int_val > lp_relax_val) {
//      ++r.wincount;
//    }
//    ++r.count;
//
//    obj_ratio.push_back((double)lp_relax_val / (double)lp_int_val);
//  }
//  //double ratio = Mean(grdalg_gen_obj) / Mean(lp_obj);
//  //std::cout << "*******Ratio = " << ratio << "*******" << std::endl;
//  std::ofstream ratiofile("lp_int_relax_ratio.txt", std::fstream::app);
//  ratiofile << "Mean: " << Mean(obj_ratio) << std::endl;
//  ratiofile << "Max: " << *(std::max_element(obj_ratio.begin(), obj_ratio.end())) << std::endl;
//  ratiofile << "Min: " << *(std::min_element(obj_ratio.begin(), obj_ratio.end())) << std::endl;
//  ratiofile.close();
//
//  r.obj_mean = Mean(lp_int_obj);
//  r.objratio_mean = Mean(obj_ratio);
//  r.max = *(std::max_element(obj_ratio.begin(), obj_ratio.end()));
//  r.min = *(std::min_element(obj_ratio.begin(), obj_ratio.end()));
//}
//
//void RunLimitedHeter() {
//  int num_regsensor[] = {90, 88, 86, 84, 82, 80};
//  int location_num = 1;
//  int target_scen_number = 1;
//  double highsensor_ratio = 0.0;
//  int highsensor_diff = 8829 * BATTERY_BASE;
//
//  std::vector<double> vec_limoptval;
//
//  even_energy::AlgorithmInputWriter aiw_k(AREA_WIDTH, AREA_HEIGHT, 0, 0);
//  // Create input for k heterogeneous sensors
//  const int MAXK = 5;
//  aiw_k.WriteAlgorithmInputFiles(TARGET_NUM, MAXK, target_scen_number, 1, 26487, 28, SEED, "limalginput_k.txt", "./", prefix);
//  even_energy::AlgorithmInputReader air_k("./EEinput/limalginput_k.txt", 1, 1, highsensor_diff);
//
//  even_energy::OutputWriter ow(prefix + "out_lim_diffarea.txt", true);
//
//  even_energy::Point areasize, offsets[5];
//  areasize.x = areasize.y = 50;
//  int count = 0;
//  for (int k = 0; k <= MAXK; ++k) {
//    ow.WriteVal(k);
//    ow.WriteVal("\t");
//    for (int i = 0; i < 5; ++i) {
//      std::cout << "Run " << count++ <<std::endl;
//      offsets[i].x = offsets[i].y = 50 * i;
//      // Create default input
//      even_energy::AlgorithmInputWriter aiw(areasize.x, areasize.y, offsets[i].x, offsets[i].y);
//      aiw.WriteAlgorithmInputFiles(TARGET_NUM, location_num, target_scen_number, LOCATION_SCEN_NUM, 26487, 28, SEED, "limalginput.txt", "./", prefix);
//      even_energy::AlgorithmInputReader air("./EEinput/limalginput.txt", num_regsensor[k], highsensor_ratio, highsensor_diff);
//
//      // Create input for k heterogeneous sensors
//      air_k.reset();
//      even_energy::LimitedOptimalAlgorithm limopt(k, air_k);
//
//      int limoptval = 0;
//      while(air.ReadNextInputSet()) {
//        // Limited heterougeous
//        limoptval = limopt.Solve(air);
//        vec_limoptval.push_back(limoptval);
//      }
//      ow.WriteVal((int)(Mean(vec_limoptval) / 1000)); // /1000 = KJ
//      ow.WriteVal("\t");
//    }
//    ow.WriteEndOfLine();
//  }
//  std::cout << "Done Limited case." << count++ <<std::endl;
//}

int main(int argc, const char * argv[]) {
  //    if (argc < 4) {
  //        printf("Please use following format:\n\tprog location_scenario_num higher_sensor_ratio out_put_filename\nThe program will exit now.\n\n");
  //        return 1;
  //    }
  //    const int location_scenario_num = atoi(argv[1]);
  //    const double higher_sensor_ratio = atof(argv[2]);
  
  //    const int location_num = 10;
  //    const int sensor_per_location = 100 / location_num;
  //    const int highsensor_diff = 8829;
  if (argc < 2) {
    printf("Please specify \"even\" or \"line\" as argument!");
    exit(1);
  }
  std::string prefix = argv[1];
  
  // clean the time file
  timefp = fopen("./runtime.txt", "w");
  fclose(timefp);

  ee::Simulation sim(prefix);

  
  //RunSimScen(prefix, TARGET_NUM, sensornum, LOCATION_SCEN_NUM, AREA_WIDTH, AREA_HEIGHT, XOFFSET, YOFFSET);
  //RunLimitedHeter();
  RunScen(sim);
  //RunLPCompareTest();
  
  return 0;
}

