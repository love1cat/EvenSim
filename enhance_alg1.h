//
//  enhance_alg1.h
//  EvenSim
//
//  Created by Yuan on 9/28/14.
//  Copyright (c) 2014 Yuan. All rights reserved.
//

#ifndef __EvenSim__enhance_alg1__
#define __EvenSim__enhance_alg1__

#include "enhance_algbase.h"

namespace even_energy{
  
class EnhanceAlg1 : public EnhanceAlgBase {
public:
  virtual void enhance(std::vector<TargetPtr>& tvec, const IntegerMatrix &rmmat) const;
  
private:
  int GetLowestTarget(std::vector<TargetPtr> &tvec) const;
};
  
}

#endif /* defined(__EvenSim__enhance_alg1__) */
