//
//  enhance_alg2.cpp
//  EvenSim
//
//  Created by Yuan on 9/28/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#include "enhance_alg2.h"


namespace even_energy{
  
  void EnhanceAlg2::enhance(GreedyAlgBase * grd_alg) const {
    std::vector<TargetPtr> &tvec = grd_alg->GetTargetVec();
    const IntegerMatrix &rm = grd_alg->GetLocRM();
    int ediff = grd_alg->GetEnergyDiff();
    
    int lowid = GetLowestTarget(tvec);
    for (int i = 0; i < tvec.size() - 1; ++i) {
      if (i != lowid) {
        const SensorSet &ss1 = tvec[lowid]->GetSensorSet();
        const SensorSet &ss2 = tvec[i]->GetSensorSet();
        bool is_swapped = false;
        for (SensorSet::iterator it1 = ss1.begin(); it1 != ss1.end(); ++it1) {
          for (SensorSet::iterator it2 = ss2.begin(); it2 != ss2.end(); ++it2) {
            const SensorPtr sp1 = *it1;
            const SensorPtr sp2 = *it2;
            int rm11 = rm[sp1->locationID][lowid];
            int rm12 = rm[sp1->locationID][i];
            if (sp1->type != 0) {
              rm11 += ediff;
              rm12 += ediff;
            }
            
            int rm21 = rm[sp2->locationID][lowid];
            int rm22 = rm[sp2->locationID][i];
            if (sp2->type != 0) {
              rm21 += ediff;
              rm22 += ediff;
            }
            
            const int &trm1 = tvec[lowid]->TotalRM();
            const int &trm2 = tvec[i]->TotalRM();
            if (rm21 - rm11 > 0 && trm2 + rm12 - rm22 > trm1) {
              // Swap increase both
              tvec[lowid]->Remove(sp1);
              tvec[i]->Remove(sp2);
              tvec[lowid]->Insert(sp2);
              tvec[i]->Insert(sp1);
              
              // Target i may be the lowest one now
              if (tvec[i]->TotalRM() < tvec[lowid]->TotalRM()) {
                lowid = i;
              }
              is_swapped = true;
              break;
            }
          }// for it2
          
          if (is_swapped) {
            break;
          }
        }// for it1
      }// if i != lowid
    }// for int i
  }
  
  int EnhanceAlg2::GetLowestTarget(std::vector<TargetPtr> &tvec) const {
    int low = std::numeric_limits<int>::max();
    int id = 0;
    for (int i = 0; i < tvec.size(); ++i)
      if (tvec[i]->TotalRM() < low) {
        low = tvec[i]->TotalRM();
        id = i;
      }
    return id;
  }
  
}