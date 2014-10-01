//
//  enhance_alg2.cpp
//  EvenSim
//
//  Created by Yuan on 9/28/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#include "enhance_alg2.h"


namespace even_energy{
  
  void EnhanceAlg2::enhance(std::vector<TargetPtr> &tvec, const IntegerMatrix &rm) const {
    for (int i = 0; i < tvec.size() - 1; ++i) {
      for (int j = i + 1; j < tvec.size(); ++j) {
        const SensorSet &ss1 = tvec[i]->GetSensorSet();
        const SensorSet &ss2 = tvec[j]->GetSensorSet();
        bool is_swapped = false;
        for (SensorSet::iterator it1 = ss1.begin(); it1 != ss1.end(); ++it1) {
          if (is_swapped) {
            break;
          }
          for (SensorSet::iterator it2 = ss2.begin(); it2 != ss2.end(); ++it2) {
            const SensorPtr sp1 = *it1;
            const SensorPtr sp2 = *it2;
            int &rm11 = rm[sp1->locationID][i];
            int &rm12 = rm[sp1->locationID][j];
            int &rm21 = rm[sp2->locationID][i];
            int &rm22 = rm[sp2->locationID][j];
            if (rm21 - rm11 > 0 && rm12 - rm22 > 0) {
              // Swap increase both
              tvec[i]->Remove(sp1);
              tvec[j]->Remove(sp2);
              tvec[i]->Insert(sp2);
              tvec[j]->Insert(sp1);
              is_swapped = true;
              break;
            }
          }// for it2
        }// for it1
      }// for int j
    }// for int i
  }
}