//
//  enhance_alg1.cpp
//  EvenSim
//
//  Created by Yuan on 9/28/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#include "enhance_alg1.h"

namespace even_energy{

  void EnhanceAlg1::enhance(std::vector<TargetPtr> &tvec) const {
    bool isSteady = false;
    while(!isSteady){
      isSteady = true;
      int lowid = GetLowestTarget(tvec);
      for(int i=0;i<tvec.size();++i) if(i!=lowid){
        SensorPtr sp = tvec[i]->LowSensor();
        if(tvec[i]->TotalRM() - tvec[lowid]->TotalRM() > sp->realRm){
          // not steady
          isSteady = false;
          // move sensor from i to lowid
          tvec[i]->Remove(sp);
          //sp->realRm = locrm_[sp->locationID][lowid];
          tvec[lowid]->Insert(sp);
          break;
        }
      }
    }
  }
  
  int EnhanceAlg1::GetLowestTarget(std::vector<TargetPtr> &tvec) const {
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