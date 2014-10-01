//
//  enhance_alg2.h
//  EvenSim
//
//  Created by Yuan on 9/28/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#ifndef __EvenSim__enhance_alg2__
#define __EvenSim__enhance_alg2__

#include "enhance_algbase.h"

namespace even_energy{
  
  class EnhanceAlg2 : public EnhanceAlgBase {
  public:
    virtual void enhance(std::vector<TargetPtr>& tvec, const IntegerMatrix &rmmat) const;
  };
}

#endif /* defined(__EvenSim__enhance_alg2__) */
