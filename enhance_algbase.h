//
//  enhance_alg.h
//  EvenSim
//
//  Created by Yuan on 9/28/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#ifndef __EvenSim__enhance_alg__
#define __EvenSim__enhance_alg__

#include "grdalg_base.h"

namespace even_energy{

class EnhanceAlgBase {
public:
  virtual void enhance(GreedyAlgBase * grd_alg) const = 0;
};
  
}

#endif /* defined(__EvenSim__enhance_alg__) */
