//
//  lp_alg.h
//  even_energy
//
//  Created by YUAN SONG on 7/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef even_energy_lp_alg_h
#define even_energy_lp_alg_h

#include <string>
#include <boost/shared_ptr.hpp>
#include <ilcplex/ilocplex.h>
#include "algbase.h"

namespace even_energy{
    class LPAlg : public AlgorithmBase{
    public:
        inline LPAlg(const int sensor_count, const int target_count, const int battery_power, bool is_relax = false)
                : is_relax_(is_relax), AlgorithmBase(sensor_count, target_count, battery_power) {
            relax_result_.reset(new DoubleArray[sensor_count]);
            for (int i = 0; i < sensor_count; ++i) {
                relax_result_[i].reset(new double[target_count]);
            }
        }
    protected:
        virtual void SolveProblem(const IntegerMatrix& e);
    private:
        const bool is_relax_;
        DoubleMatrix relax_result_;
        
        void CreateProblem(const IntegerMatrix& e, IloModel model, IloNumVarArray x, IloRangeArray c);
        
        void RoundRelaxResult(const IntegerMatrix& e);
        
        int MaxTarget(const int sid, const IntegerMatrix& e);
        int RemainingEnergy(const int tid, const IntegerMatrix& e);
        
        /***** Mapping Functions *****/
        // map the two dimensional variable array to one dimensional number
        // The variables are in following order:
        //  x(0,0), x(0,1), ... , x(0,N), x(1,0), x(1,1), ... , x(1,N), ... , x(M,N)
        
        // map from 1D to 2D
        inline int SID(const int index){
            return index / target_count_;
        }
        
        inline int TID(const int index){
            return index % target_count_;
        }
        
        // map from 2D to 1D
        inline int ID(const int sensor_id, const int target_id){
            return sensor_id*target_count_ + target_id;
        }
        /*****************************/
    };
}

#endif
