//
//  simulation.h
//  EvenSim
//
//  Created by Yuan on 10/18/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#ifndef EvenSim_simulation_h
#define EvenSim_simulation_h

#include <vector>
#include <cmath>
#include <string>

namespace even_energy{
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
  
  class Simulation {
  public:
    Simulation(const std::string &prefix);
    Result OneRun(const RunArgs &ra) const;
    const std::string & prefix() const {return prefix_;}
  private:
    void GenerateTargets() const;
    std::string prefix_;
  };
}

#endif
