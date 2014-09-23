//
//  limited_optalg.cc
//  EvenSim
//
//  Created by Yuan on 11/15/13.
//  Copyright (c) 2013 Yuan. All rights reserved.
//

#include "limited_optalg.h"
#include "alg_input.h"

namespace even_energy {
	LimitedOptimalAlgorithm::LimitedOptimalAlgorithm(const int k, AlgorithmInputReader& air_k) : K_(k){
		// Copy data in air_k to local imx_k_
		int t_num = air_k.GetTargetCount();
		int s_num = air_k.GetSensorCount();
		assert(s_num >= k);
		if(!air_k.ReadNextInputSet()) {
			throw "air_k does not have data!";
		}
		imx_k_.reset(new IntegerArray[s_num]);
		for (int i = 0; i < k; ++i) {
			imx_k_[i].reset(new int[t_num]);
			for (int j = 0; j < t_num; ++j) {
				imx_k_[i][j] = air_k.GetData(i, j);
			}
		}
	}
	
	void LimitedOptimalAlgorithm::SolveHelper(AlgorithmInputReader& air, int cur, const int target_number, std::vector<int> &adj_vec, int &max_rm) {
		if (cur == K_) {
			// clean up
			locationVec_.clear();
			targetVec_.clear();
			
			init(air);
			
			// All remaining energy adjustment are generated.
			for (int i = 0; i < adj_vec.size(); ++i) {
				int tid = adj_vec[i];
				targetVec_[tid]->IncreaseTotalRM(imx_k_[i][tid]);
			}
			
			// Solve problem.
			SolveSubRoutine();
			
			// Get lowest energy.
			int lowid = GetLowestTarget();
			int lowenergy = targetVec_[lowid]->TotalRM();
			if (lowenergy > max_rm) {
				max_rm = lowenergy;
			}
			return;
		}
		
		for (int i = 0; i < target_number; ++i) {
			adj_vec[cur] = i;
			SolveHelper(air, cur + 1, target_number, adj_vec, max_rm);
		}
	}
	
	
	int LimitedOptimalAlgorithm::Solve(AlgorithmInputReader& air, bool isEnhance){
		int target_number = air.GetTargetCount();
		
		// Define a adjustment vector
		std::vector<int> adj_vec(K_);
		
		int max_rm = 0;
		SolveHelper(air, 0, target_number, adj_vec, max_rm);
		return max_rm;
	}
}