//
//  grdalg_generic.cpp
//  EvenSim
//
//  Created by Yuan on 12/14/13.
//  Copyright (c) 2013 Yuan. All rights reserved.
//

#include <algorithm>
#include "grdalg_generic.h"

namespace even_energy {
  
  void GreedyAlgGeneric::SolveSubRoutine() {
    // Init preferred_sensor_location_list_
    for (int i = 0; i < targetVec_.size(); ++i) {
      std::vector<int> location_list;
      for (int j = 0; j < locationVec_.size(); ++j) {
        location_list.push_back(j);
      }
      preferred_sensor_location_list_.push_back(location_list);
    }
    
    for (int i = 0; i < targetVec_.size(); ++i) {
      int targetid = i;
      // Init preferred_sensor_location_list_ for this target
      std::sort(preferred_sensor_location_list_[i].begin(), preferred_sensor_location_list_[i].end(), SensorLocationComparator(locrm_, targetid));
    }
    
    // Run greedy algorithm.
    int lowid = 0;
    while (IsSensorAvailable()) {
      lowid = GetLowestTarget();
      TargetPtr& tp = targetVec_[lowid];
      int favloc = tp->FavLocation();
      assert(favloc == preferred_sensor_location_list_[lowid][0]);
      for (int i = 0; i < preferred_sensor_location_list_[lowid].size(); ++i) {
        int locid = preferred_sensor_location_list_[lowid][i];
        if (locationVec_[locid]->IsEmpty()) {
          continue;
        }
        // Non-empty sensor location, grab a sensor here
        SensorPtr sp = locationVec_[locid]->GetSensor();
        int e = locrm_[favloc][lowid];
        if (sp->type == 1) {
          e += energy_diff_;
        }
        // always use high energy of favourite location for curRm
        sp->realRm = e;
        tp->Insert(sp);
        break;
      }
    }
    
    return;
  }
}
