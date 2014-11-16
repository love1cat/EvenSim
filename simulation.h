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
  struct Range {
    double min, max, avg;
  };
  
  struct Result {
    Range obj;
    Range obj_ratio; // greedy / lp
    int count;
    Range runtime;
    Range rt_ratio;
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
    std::vector<Result> OneRun(const RunArgs &ra) const;
    void RunOptimalGrd(const std::string &outfile) const;
    const std::string & prefix() const {return prefix_;}
  private:
    void GenerateTargets() const;
    std::string prefix_;
  };
}

#endif
