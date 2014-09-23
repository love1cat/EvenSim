//
//  limited_optalg.h
//  EvenSim
//
//  Created by Yuan on 11/15/13.
//  Copyright (c) 2013 Yuan. All rights reserved.
//

#ifndef EvenSim_limited_optalg_h
#define EvenSim_limited_optalg_h

#include <boost/shared_ptr.hpp>
#include "grdalg1.h"
#include "datatypes.h"

namespace even_energy {
	typedef boost::shared_ptr<GreedyAlg1> GreedyAlg1Ptr;
	class LimitedOptimalAlgorithm : public GreedyAlg1 {
	public:
		LimitedOptimalAlgorithm(const int k, AlgorithmInputReader& air_k);
		virtual int Solve(AlgorithmInputReader& air, bool isEnhance= false);
	private:
		void SolveHelper(AlgorithmInputReader& air, int cur, const int target_number, std::vector<int> &adj_vec, int &max_rm);
		IntegerMatrix imx_k_;
		const int K_;
	};
}

#endif
