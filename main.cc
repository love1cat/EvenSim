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
#include "alg_input.h"
#include "lp_alg.h"
#include "output.h"
#include "grdalg1.h"
#include "grdalg_generic.h"
#include "limited_optalg.h"
#include "enhance_alg1.h"
#include "enhance_alg2.h"

FILE *timefp;

namespace {
  const int TARGET_NUM = 4;
  const int MULTIPLIER = 1; // hack
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
  
  std::string prefix;
  
  struct Result {
    double obj_mean;
    double objratio_mean;
    double min;
    double max;
    int count;
    
    double objratio_mean_enh;
    double min_enh;
    double max_enh;
    
    double objratio_mean_enh2;
    double min_enh2;
    double max_enh2;
  };
  
  struct RunArgs {
    RunArgs() :
    higher_sensor_ratio(0.1),
    location_num(4),
    highsensor_diff(3),
    sensornum(100),
    use_lp_relax(true) {}
    
    double higher_sensor_ratio; // ratio of high sensors among all sensors
    int location_num;
    int highsensor_diff; // how many more batteries high sensor carry compared to low sensor
    int sensornum;
    bool use_lp_relax;
  };
  
}

double CapOne(double val) {
  return val > 1.0 ? 1.0 : val;
}

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

/** Run each algorithm in a number of randomly generated scenarios (LOCATION_SCEN_NUM) **/
Result OneRun(const RunArgs &ra) {
  //  sensornum *= MULTIPLIER;
  assert(ra.sensornum % ra.location_num == 0 && "sensors should be able to divide over locations evenly.");
  const int sensor_per_location = ra.sensornum / ra.location_num;
  
  const int highsensor_num = ra.sensornum * ra.higher_sensor_ratio;
  assert((double)(highsensor_num / ra.sensornum) == ra.higher_sensor_ratio && "higher sensor ratio should give integer higher sensor number.");
  assert((int)(ra.sensornum * ra.higher_sensor_ratio) % ra.location_num == 0 && "higher sensors should be able to divide over locations evenly.");
  const int higher_sensor_per_location = ra.sensornum * ra.higher_sensor_ratio / ra.location_num;

  
  // Create default input
  even_energy::AlgorithmInputWriter aiw(AREA_WIDTH, AREA_HEIGHT, XOFFSET, YOFFSET);
  aiw.WriteAlgorithmInputFiles(TARGET_NUM, ra.location_num, 1, LOCATION_SCEN_NUM, BATTERY_POWER * BATTERY_BASE, ENERGY_PER_METER, SEED, "alginput.txt", "./", prefix);
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
  
  std::ofstream ratiofile("lp_grd_ratio.txt", std::fstream::app);
  ratiofile << "Mean: " << Mean(obj_ratio) << std::endl;
  ratiofile << "Max: " << *(std::max_element(obj_ratio.begin(), obj_ratio.end())) << std::endl;
  ratiofile << "Min: " << *(std::min_element(obj_ratio.begin(), obj_ratio.end())) << std::endl;
  ratiofile.close();
  
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

/** Run small scale **/
void RunScen() {
  std::string outfile;
  RunArgs ra;
  ra.sensornum = 40;
  ra.use_lp_relax = false;
  ra.higher_sensor_ratio = 0.25;
  ra.highsensor_diff = 3;
  
  double locnum[] = {1, 2, 5, 10};
  
  // Small test for interger LP
  
  int run_count = 0;
  // Diff location and min, max, avg of objratio
  outfile = prefix + "out_diff_loc_minmaxavg.txt";
  {
    // Clean the file.
    even_energy::OutputWriter ow(outfile, true);
    even_energy::OutputWriter ow2("enh_" + outfile, true);
    even_energy::OutputWriter ow3("enh2_" + outfile, true);
  }
  {
    even_energy::OutputWriter ow(outfile, true);
    even_energy::OutputWriter ow2("enh_" + outfile, true);
    even_energy::OutputWriter ow3("enh2_" + outfile, true);
    int wincount = 0;
    int count = 0;
    for (int i = 0; i < 4; ++i) {
      ra.location_num = locnum[i];
      
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      Result r = OneRun(ra);
      count += r.count;
      ow.WriteVal(CapOne(r.objratio_mean));
      ow.WriteVal("\t");
      ow.WriteVal(CapOne(r.min));
      ow.WriteVal("\t");
      ow.WriteVal(CapOne(r.max));
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
  prefix = argv[1];
  
  // clean the time file
  timefp = fopen("./runtime.txt", "w");
  fclose(timefp);
  // write target file
  
  if (prefix == "line") {
    // line
    int pointheight = HEIGHT / 2;
    int interval = WIDTH / (TARGET_NUM + 1);
    FILE* fp = fopen("./EEinput/line_TargetFile.txt", "w");
    fprintf(fp, "1\n0\n%d\n500\t500\n", TARGET_NUM);
    for (int i = 0; i < TARGET_NUM; ++i) {
      fprintf(fp, "%d\t%d\n", (i + 1) * interval, pointheight);
    }
    fclose(fp);
  } else{
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
  
  prefix = prefix + "_";
  
  //RunSimScen(prefix, TARGET_NUM, sensornum, LOCATION_SCEN_NUM, AREA_WIDTH, AREA_HEIGHT, XOFFSET, YOFFSET);
  //RunLimitedHeter();
  RunScen();
  //RunLPCompareTest();
  
  return 0;
}

