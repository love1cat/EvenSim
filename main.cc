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

int targetnum = 4;
int sensornum = 40;
int multiplier = 1; // hack
int location_scen_num = 100;
int width = 500;
int height = 500;
const int batttery_base = 3;

int areawidth = 500; // area width for generating locations
int areaheight = 500; // area height for generating locations
int xoffset = 0; // x point of the area for generating locations
int yoffset = 0; // y point of the area for generating locations
int seed = 1124;

std::string prefix;

struct Result {
  double obj_mean;
  double objratio_mean;
  double min;
  double max;
  int wincount;
  int count;
  
  double objratio_mean_enh;
  double min_enh;
  double max_enh;
};

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

Result OneRun(double higher_sensor_ratio, int location_num, int highsensor_diff, int sensornum) {
  sensornum *= multiplier;
  const int total_battery = sensornum * batttery_base;
  const int high_battery = highsensor_diff / 8829 + batttery_base;
  const int high_sensor_num = sensornum * higher_sensor_ratio;
  const int total_high_battery = high_sensor_num * high_battery;
  const int low_sensor_num = (total_battery - total_high_battery ) / batttery_base;
  
  const int location_scenario_num = location_scen_num;
  const int sensor_per_location = (low_sensor_num + high_sensor_num) / location_num;
  const int higher_sensor_per_location = high_sensor_num / location_num;
  //  const int sensor_per_location = sensornum / location_num;
  
  // Create default input
  even_energy::AlgorithmInputWriter aiw(areawidth, areaheight, xoffset, yoffset);
  aiw.WriteAlgorithmInputFiles(targetnum, location_num, 1, location_scenario_num, 8829*batttery_base, 28, seed, "alginput.txt", "./", prefix);
  even_energy::AlgorithmInputReader air("./EEinput/alginput.txt", sensor_per_location, higher_sensor_per_location, highsensor_diff);
  
  even_energy::GreedyAlg1 grdalg1(true);
  even_energy::GreedyAlgGeneric grdalg_gen_ass(true);
  even_energy::GreedyAlgGeneric grdalg_gen(false);
  even_energy::LPAlg lpa(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), true); // LP-relax, For comparison
  even_energy::LPAlg lpa_int(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), false); // Integer LP
  
  even_energy::EnhanceAlg1 enhanc1;
  
  std::vector<double> grdalg_gen_obj; // objective value of greedy enhancing algorithm
  std::vector<double> grdalg_gen_ass_obj; // objective value of greedy enhancing algorithm
  std::vector<double> grdalg_gen_obj_enh;
  
  std::vector<double> lp_obj;
  std::vector<double> obj_ratio;
  std::vector<double> obj_ratio_enh;
  
  Result r;
  r.wincount = 0;
  r.count = 0;
  while (air.ReadNextInputSet()) {
    // For comparison
    lpa_int.SolveNextScenario(air.GetDataMatrix());
    int lpval = lpa_int.GetCurrentResult().GetObjValue();
    lp_obj.push_back(lpval);
    
    int grdalg_gen_objval = grdalg_gen.Solve(air); // no enhancing greedy alg
    grdalg_gen.enhance(&enhanc1); // apply enhance alg 1
    int grdalg_gen_objval_enh = grdalg_gen.get_obj(); // get obj after enhancing
    grdalg_gen_obj.push_back(grdalg_gen_objval);
    grdalg_gen_obj_enh.push_back(grdalg_gen_objval_enh);
    
    
    int grdalg_gen_ass_objval = grdalg_gen_ass.Solve(air);
    grdalg_gen_ass_obj.push_back(grdalg_gen_ass_objval);
    
    if (grdalg_gen_objval >= grdalg_gen_ass_objval) {
      ++r.wincount;
    }
    ++r.count;
    
    obj_ratio.push_back((double)grdalg_gen_objval / (double)lpval);
    obj_ratio_enh.push_back((double)grdalg_gen_objval_enh / (double)lpval);
  }
  //double ratio = Mean(grdalg_gen_obj) / Mean(lp_obj);
  //std::cout << "*******Ratio = " << ratio << "*******" << std::endl;
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
  
  return r;
}

void OutputRunCount(int &run_count) {
  std::cout << "******** Run = " << ++run_count << "********" << std::endl;
}

void RunScen() {
  std::string outfile;
  int highsensor_diff = 8829 * batttery_base;
  
  double ediff[] = {0, 8829 * batttery_base};
  double locnum[] = {1, 2, 5, 10};
  double ratio[] = {0.0, 0.25, 0.5};
  
  // Small test for interger LP
  
  int run_count = 0;
  // Diff location and min, max, avg of objratio
  outfile = prefix + "out_diff_loc_minmaxavg.txt";
  {
    // Clean the file.
    even_energy::OutputWriter ow(outfile, true);
  }
  {
    even_energy::OutputWriter ow2(outfile + "_enh", true);
    
    even_energy::OutputWriter ow(outfile, true);
    int wincount = 0;
    int count = 0;
    for (int i = 0; i < 4; ++i) {
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      Result r = OneRun(0.25, locnum[i], 8829, sensornum);
      wincount += r.wincount;
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
      ow2.WriteVal(CapOne(r.objratio_mean_enh));
      ow2.WriteVal("\t");
      ow2.WriteVal(CapOne(r.min_enh));
      ow2.WriteVal("\t");
      ow2.WriteVal(CapOne(r.max_enh));
      ow2.WriteVal("\t");
      ow2.WriteEndOfLine();
      
      OutputRunCount(run_count);
    }
    std::cout<<"*****WINCOUNT="<<wincount<<"*****"<<std::endl;
    std::cout<<"*****COUNT="<<count<<"*****"<<std::endl;
  }
  
  // Diff ratio and min, max, avg of objratio
  outfile = prefix + "out_diff_ratio_minmaxavg.txt";
  {
    // Clean the file.
    even_energy::OutputWriter ow(outfile, true);
  }
  {
    even_energy::OutputWriter ow2(outfile + "_enh", true);
    
    even_energy::OutputWriter ow(outfile, true);
    int wincount = 0;
    int count = 0;
    for (int i = 0; i < 3; ++i) {
      ow.WriteVal(ratio[i]);
      ow.WriteVal("\t");
      Result r = OneRun(ratio[i], 5, 8829, sensornum);
      wincount += r.wincount;
      count += r.count;
      ow.WriteVal(CapOne(r.objratio_mean));
      ow.WriteVal("\t");
      ow.WriteVal(CapOne(r.min));
      ow.WriteVal("\t");
      ow.WriteVal(CapOne(r.max));
      ow.WriteVal("\t");
      ow.WriteEndOfLine();
      
      ow2.WriteVal(ratio[i]);
      ow2.WriteVal("\t");
      ow2.WriteVal(CapOne(r.objratio_mean_enh));
      ow2.WriteVal("\t");
      ow2.WriteVal(CapOne(r.min_enh));
      ow2.WriteVal("\t");
      ow2.WriteVal(CapOne(r.max_enh));
      ow2.WriteVal("\t");
      ow2.WriteEndOfLine();
      
      OutputRunCount(run_count);
    }
    std::cout<<"*****WINCOUNT="<<wincount<<"*****"<<std::endl;
    std::cout<<"*****COUNT="<<count<<"*****"<<std::endl;
  }
  // Diff location, diff high ratio
  outfile = prefix + "out_diff_loc_ratio.txt";
  {
    // Clean the file.
    even_energy::OutputWriter ow(outfile, true);
  }
  {
    even_energy::OutputWriter ow(outfile, true);
    for (int i = 0; i < 4; ++i) {
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      for (int j = 0; j < 3; ++j) {
        Result r = OneRun(ratio[j], locnum[i], highsensor_diff, sensornum);
        int objval = r.obj_mean;
        ow.WriteVal(objval / 1000 / multiplier);
        ow.WriteVal("\t");
        ow.WriteVal(r.objratio_mean);
        ow.WriteVal("\t");
        OutputRunCount(run_count);
      }
      ow.WriteEndOfLine();
    }
  }
  
  // Diff location, diff energy diff
  outfile = prefix + "out_diff_loc_ediff.txt";
  {
    // Clean the file.
    even_energy::OutputWriter ow(outfile, true);
  }
  {
    even_energy::OutputWriter ow(outfile, true);
    for (int i = 0; i < 4; ++i) {
      ow.WriteVal(locnum[i]);
      ow.WriteVal("\t");
      for (int j = 0; j < 2; ++j) {
        Result r = OneRun(0.25, locnum[i], ediff[j], sensornum);
        int objval = r.obj_mean;
        ow.WriteVal(objval / 1000 / multiplier);
        ow.WriteVal("\t");
        ow.WriteVal(r.objratio_mean);
        ow.WriteVal("\t");
        OutputRunCount(run_count);
      }
      ow.WriteEndOfLine();
    }
  }
  
  //// Diff high ratio, diff location
  //highsensor_diff = 3*8829;
  //outfile = prefix + "out_diff_ratio_loc.txt";
  //{
  //	// Clean the file.
  //	even_energy::OutputWriter ow(outfile, true);
  //}
  //{
  //	even_energy::OutputWriter ow(outfile, true);
  //	for (int i = 0; i < 3; ++i) {
  //		ow.WriteVal(ratio[i] * sensornum);
  //		ow.WriteVal("\t");
  //		for (int j = 0; j < 4; ++j) {
  //			Result r = OneRun(ratio[i], locnum[j], highsensor_diff, sensornum);
  //			int objval = r.obj_mean;
  //			ow.WriteVal(objval / 1000 / multiplier);
  //			ow.WriteVal("\t");
  //			OutputRunCount(run_count);
  //		}
  //		ow.WriteEndOfLine();
  //	}
  //}
}

void RunLPCompareTest() {
  sensornum *= multiplier;
  double higher_sensor_ratio = 0.1;
  int location_num = 5;
  int highsensor_diff = 8829;
  int sensornum = 50;
  const int location_scenario_num = 100; // large number may be too slow.
  const int total_battery = sensornum * batttery_base;
  const int high_battery = highsensor_diff / 8829 + batttery_base;
  const int high_sensor_num = sensornum * higher_sensor_ratio;
  const int total_high_battery = high_sensor_num * high_battery;
  const int low_sensor_num = (total_battery - total_high_battery ) / 3;
  const int sensor_per_location = (low_sensor_num + high_sensor_num) / location_num;
  const int higher_sensor_per_location = high_sensor_num / location_num;
  //  const int sensor_per_location = sensornum / location_num;
  
  // Create default input
  even_energy::AlgorithmInputWriter aiw(areawidth, areaheight, xoffset, yoffset);
  aiw.WriteAlgorithmInputFiles(targetnum, location_num, 1, location_scenario_num, 8829*batttery_base, 28, seed, "alginput.txt", "./", prefix);
  even_energy::AlgorithmInputReader air("./EEinput/alginput.txt", sensor_per_location, higher_sensor_per_location, highsensor_diff);
  
  even_energy::LPAlg lpa_relax(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), true); // LP-relax
  even_energy::LPAlg lpa_int(air.GetSensorCount(), air.GetTargetCount(), air.GetBatteryPower(), false); // Integer LP, For comparison
  
  std::vector<double> lp_relax_obj;
  std::vector<double> lp_int_obj;
  std::vector<double> obj_ratio;
  
  Result r;
  r.wincount = 0;
  r.count = 0;
  while (air.ReadNextInputSet()) {
    // For comparison
    lpa_relax.SolveNextScenario(air.GetDataMatrix());
    int lp_relax_val = lpa_relax.GetCurrentResult().GetObjValue();
    lp_relax_obj.push_back(lp_relax_val);
    
    lpa_int.SolveNextScenario(air.GetDataMatrix());
    int lp_int_val = lpa_int.GetCurrentResult().GetObjValue();
    lp_int_obj.push_back(lp_int_val);
    
    if (lp_int_val > lp_relax_val) {
      ++r.wincount;
    }
    ++r.count;
    
    obj_ratio.push_back((double)lp_relax_val / (double)lp_int_val);
  }
  //double ratio = Mean(grdalg_gen_obj) / Mean(lp_obj);
  //std::cout << "*******Ratio = " << ratio << "*******" << std::endl;
  std::ofstream ratiofile("lp_int_relax_ratio.txt", std::fstream::app);
  ratiofile << "Mean: " << Mean(obj_ratio) << std::endl;
  ratiofile << "Max: " << *(std::max_element(obj_ratio.begin(), obj_ratio.end())) << std::endl;
  ratiofile << "Min: " << *(std::min_element(obj_ratio.begin(), obj_ratio.end())) << std::endl;
  ratiofile.close();
  
  r.obj_mean = Mean(lp_int_obj);
  r.objratio_mean = Mean(obj_ratio);
  r.max = *(std::max_element(obj_ratio.begin(), obj_ratio.end()));
  r.min = *(std::min_element(obj_ratio.begin(), obj_ratio.end()));
}

void RunLimitedHeter() {
  int num_regsensor[] = {90, 88, 86, 84, 82, 80};
  int location_num = 1;
  int target_scen_number = 1;
  double highsensor_ratio = 0.0;
  int highsensor_diff = 8829 * batttery_base;
  
  std::vector<double> vec_limoptval;
  
  even_energy::AlgorithmInputWriter aiw_k(areawidth, areaheight, 0, 0);
  // Create input for k heterogeneous sensors
  const int MAXK = 5;
  aiw_k.WriteAlgorithmInputFiles(targetnum, MAXK, target_scen_number, 1, 26487, 28, seed, "limalginput_k.txt", "./", prefix);
  even_energy::AlgorithmInputReader air_k("./EEinput/limalginput_k.txt", 1, 1, highsensor_diff);
  
  even_energy::OutputWriter ow(prefix + "out_lim_diffarea.txt", true);
  
  even_energy::Point areasize, offsets[5];
  areasize.x = areasize.y = 50;
  int count = 0;
  for (int k = 0; k <= MAXK; ++k) {
    ow.WriteVal(k);
    ow.WriteVal("\t");
    for (int i = 0; i < 5; ++i) {
      std::cout << "Run " << count++ <<std::endl;
      offsets[i].x = offsets[i].y = 50 * i;
      // Create default input
      even_energy::AlgorithmInputWriter aiw(areasize.x, areasize.y, offsets[i].x, offsets[i].y);
      aiw.WriteAlgorithmInputFiles(targetnum, location_num, target_scen_number, location_scen_num, 26487, 28, seed, "limalginput.txt", "./", prefix);
      even_energy::AlgorithmInputReader air("./EEinput/limalginput.txt", num_regsensor[k], highsensor_ratio, highsensor_diff);
      
      // Create input for k heterogeneous sensors
      air_k.reset();
      even_energy::LimitedOptimalAlgorithm limopt(k, air_k);
      
      int limoptval = 0;
      while(air.ReadNextInputSet()) {
        // Limited heterougeous
        limoptval = limopt.Solve(air);
        vec_limoptval.push_back(limoptval);
      }
      ow.WriteVal((int)(Mean(vec_limoptval) / 1000)); // /1000 = KJ
      ow.WriteVal("\t");
    }
    ow.WriteEndOfLine();
  }
  std::cout << "Done Limited case." << count++ <<std::endl;
}

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
    int pointheight = height / 2;
    int interval = width / (targetnum + 1);
    FILE* fp = fopen("./EEinput/line_TargetFile.txt", "w");
    fprintf(fp, "1\n0\n%d\n500\t500\n", targetnum);
    for (int i = 0; i < targetnum; ++i) {
      fprintf(fp, "%d\t%d\n", (i + 1) * interval, pointheight);
    }
    fclose(fp);
  } else{
    // even
    int targetperline = sqrt(targetnum);
    if(targetperline*targetperline != targetnum) {
      printf("No even deployment of targets available!\n");
      exit(1);
    }
    int winterval = width / (targetperline + 1);
    int hinterval = height / (targetperline + 1);
    FILE* fp = fopen("./EEinput/even_TargetFile.txt", "w");
    fprintf(fp, "1\n0\n%d\n500\t500\n", targetnum);
    for(int i=0;i<targetperline;++i)
      for(int j=0;j<targetperline;++j){
        fprintf(fp, "%d\t%d\n", (i + 1) * winterval, (j + 1) * hinterval);
      }
    fclose(fp);
  }
  
  prefix = prefix + "_";
  
  //RunSimScen(prefix, targetnum, sensornum, location_scen_num, areawidth, areaheight, xoffset, yoffset);
  //RunLimitedHeter();
  RunScen();
  //RunLPCompareTest();
  
  return 0;
}

