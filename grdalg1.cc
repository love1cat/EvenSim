#include <cstdio>
#include "grdalg1.h"

namespace even_energy {
	
    void GreedyAlg1::SolveSubRoutine() {
        int lowid = 0;
        while (IsSensorAvailable()) {
            lowid = GetAssumedLowestTarget();
            TargetPtr& tp = targetVec_[lowid];
            int favloc = tp->FavLocation();
            if (!locationVec_[favloc]->IsEmpty()) {
                // favourite location available, pull a sensor out
                SensorPtr sp = locationVec_[favloc]->GetSensor();
				if (!this->use_assumed_energy_) {
					sp->curRm = locrm_[sp->locationID][lowid];
				} else {
					int maxenergy = locrm_[favloc][lowid] + energy_diff_;
					// always use high energy of favourite location for curRm
					sp->curRm = maxenergy;
				}
                tp->Insert(sp);
            } else {
                // search other locations for available sensors
                for (int i = 0; i < locationVec_.size(); ++i) if (i != favloc) {
					if (!locationVec_[i]->IsEmpty()) {
						SensorPtr sp = locationVec_[i]->GetSensor();
						if (!this->use_assumed_energy_) {
							sp->curRm = locrm_[sp->locationID][lowid];
						} else {
							int maxenergy = locrm_[favloc][lowid] + energy_diff_;
							// always use high energy of favourite location for curRm
							sp->curRm = maxenergy;
						}
						tp->Insert(sp);
						break;
					}
				}
            }
        }
		
        
        // for testing
		//        for (int i = 0; i < targetVec_.size(); ++i) {
		//            printf("Target %d\n", i);
		//            const SensorSet& ss = targetVec_[i]->GetSensorSet();
		//            for(SensorSet::iterator iter=ss.begin();iter!=ss.end();++iter){
		//                printf("%d-%d-%d\n", (*iter)->locationID, (*iter)->type, (*iter)->realRm);
		//            }
		//            printf("\n");
		//        }
		
        return;
    }
}
