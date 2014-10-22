//
//  grdalg_generic.h
//  EvenSim
//
//  Created by Yuan on 12/14/13.
//  Copyright (c) 2013 Yuan. All rights reserved.
//

#ifndef GRDALG_GENERIC_H
#define	GRDALG_GENERIC_H

#include <vector>
#include "grdalg_base.h"
#include "grdalg_generic.h"

namespace even_energy{
  class GreedyAlgGeneric : public GreedyAlgBase{
  protected:
    virtual void SolveSubRoutine();
  private:
    // Each target has a list of perferred sensor location
    // Sorted in the decreasing order or remaining energy at this target
    class SensorLocationComparator {
    public:
      SensorLocationComparator(const IntegerMatrix& locrm, int targetid) : locrm_(locrm), targetid_(targetid) {}
      bool operator()(const int sensor_loc1, const int sensor_loc2) const {
        return locrm_[sensor_loc1][targetid_] > locrm_[sensor_loc2][targetid_];
      }
    private:
      const IntegerMatrix& locrm_;
      int targetid_;
    };
    
    std::vector<std::vector<int> > preferred_sensor_location_list_;
  private:
    bool use_assumed_energy_;
  };
}


#endif	/* GRDALG_GENERIC_H */
