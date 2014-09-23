//
//  lp_alg.cc
//  even_energy
//
//  Created by YUAN SONG on 7/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <ilcplex/ilocplex.h>
#include "lp_alg.h"
#include "random_generator.h"

namespace even_energy {
    void LPAlg::SolveProblem(const IntegerMatrix& imx) {
        IloEnv env;
        IloModel model(env);
        IloNumVarArray var(env);
        IloRangeArray con(env);
        CreateProblem(imx, model, var, con);
        IloCplex cplex(model);

        // Optimize the problem and obtain solution.
        // nodes file is quite large and we change node file setting to "save to disk"
        cplex.setParam(IloCplex::NodeFileInd, 3);
        if (!cplex.solve()) {
            env.error() << "Failed to optimize LP" << std::endl;
            throw (-1);
        }
        
        // Save results
        assign_result_.Clear();
        assign_result_.SetOptimizationStatus(cplex.getStatus());
        IloNumArray vals(env);
        cplex.getValues(vals, var);
        if (!is_relax_) {
            assign_result_.SetObjValue(cplex.getObjValue());
            int curid = 0;
            for (int i = 0; i < sensor_count_; ++i)
                for (int j = 0; j < target_count_; ++j)
                    assign_result_.SetAssignment(i, j, vals[curid++]);
        } else {
            // LP-relax, need to round result
            int curid = 0;
            for (int i = 0; i < sensor_count_; ++i)
                for (int j = 0; j < target_count_; ++j)
                    relax_result_[i][j] = vals[curid++];
            RoundRelaxResult(imx);
            // find obj value
            int mine = RemainingEnergy(0, imx);
            for(int i=1;i<target_count_;++i) {
                int cure = RemainingEnergy(i, imx);
                if(cure < mine) mine = cure; 
            }
            assign_result_.SetObjValue(mine);
        }
        assign_result_.SetEnergy(imx);
        
        env.end();
    }

    void LPAlg::CreateProblem(const IntegerMatrix& e, IloModel model, IloNumVarArray x, IloRangeArray c) {
        // create the maxmin problem
        /**** Original Problem ****/
        // M sensors, N targets
        // Index: i - sensor, j - target
        // OBJECTIVE: max min_j sum_{i=1}^M x(i,j)e(i, j)
        // CONSTRAINTS:
        //      sum_{i=1}^M x(i,j) >= 1, j~[1,N]
        //      sum_{j=1}^N x(i,j) = 1, i~[1,M]
        //      sum_{j=1}^N x(i,j)e(i,j)>=1, i~[1,M]
        /**************************/

        /**** Converted problem ****/
        // OBJECTIVE: max U
        // CONSTRAINTS:
        //      sum_{i=1}^M x(i,j)e(i,j) - U >= 0, j~[1,N]
        //      sum_{i=1}^M x(i,j) >= 1, j~[1,N]
        //      sum_{j=1}^N x(i,j) = 1, i~[1,M]
        //      sum_{j=1}^N x(i,j)e(i,j)>=1, i~[1,M]    
        /***************************/
        
        const int M = sensor_count_;
        const int N = target_count_;

        // Create a maximization problem
        IloEnv env = model.getEnv();
        IloObjective obj = IloMaximize(env);

        // Add in all variables, totally M*N+1
        // M*N: integer (0,1)
        // 1: integer (0, battery_power * M)
        if (!is_relax_) {
            // ILP
            for (int i = 0; i < M * N; ++i) {
                x.add(IloNumVar(env, 0, 1, ILOINT));
            }
            x.add(IloNumVar(env, 0, battery_power_ * M, ILOINT));
        } else{
            // LP-RELAX
            for (int i = 0; i < M * N; ++i) {
                x.add(IloNumVar(env, 0, 1, ILOFLOAT));
            }
            x.add(IloNumVar(env, 0, battery_power_ * M, ILOFLOAT));
        }

        // record last variable
        const int UID = M*N; // 0-indexed, last variable

        /***** Objective *****/
        // Set coefficient of U (assistant variable in converted problem)
        obj.setLinearCoef(x[UID], 1);
        /*********************/

        /***** Constraints *****/
        int cst_id = 0; // current constraint id
        int cst_start_id = 0; // start constraint id of current constraint
        int i;
        // (1) sum_{i=1}^M x(i,j)e(i,j) - U >= 0, j~[1,N]
        // Totally N constraints for different j
        for (i = 0, cst_id = cst_start_id; i < N; ++i, ++cst_id) {
            c.add(IloRange(env, 0, IloInfinity));
            for (int j = 0; j < M; ++j)
                c[cst_id].setLinearCoef(x[ID(j, i)], e[j][i]);
            c[cst_id].setLinearCoef(x[UID], -1);
        }

        // (2) sum_{i=1}^M x(i,j) >= 1, j~[1,N]
        // Totally N constraints for different j
//        cst_start_id = cst_id;
//        for (i = 0, cst_id = cst_start_id; i < N; ++i, ++cst_id) {
//            c.add(IloRange(env, 1, IloInfinity));
//            for (int j = 0; j < M; ++j)
//                c[cst_id].setLinearCoef(x[ID(j, i)], 1);
//        }

        // (3) sum_{j=1}^N x(i,j) = 1, i~[1,M]
        // Totally M constraints for different i
        cst_start_id = cst_id;
        for (i = 0, cst_id = cst_start_id; i < M; ++i, ++cst_id) {
            c.add(IloRange(env, 1, 1));
            for (int j = 0; j < N; ++j)
                c[cst_id].setLinearCoef(x[ID(i, j)], 1);
        }

        // (4) sum_{j=1}^N x(i,j)e(i,j)>0, i~[1,M] 
        // Totally M constraints for different i
//        cst_start_id = cst_id;
//        for (i = 0, cst_id = cst_start_id; i < M; ++i, ++cst_id) {
//            //c.add(IloRange(env, 1, IloInfinity));
//            c.add(IloRange(env, 0, IloInfinity));
//            for (int j = 0; j < N; ++j)
//                c[cst_id].setLinearCoef(x[ID(i, j)], e[i][j]);
//        }
        /***********************/

        model.add(obj);
        model.add(c);
    }
    
    void LPAlg::RoundRelaxResult(const IntegerMatrix& e){
        RandomGenerator rg(0);
        for (int i = 0; i < sensor_count_; ++i)
            for (int j = 0; j < target_count_; ++j){
                if(rg.IsProbability(relax_result_[i][j]))
                    assign_result_.SetAssignment(i, j, 1);
                else assign_result_.SetAssignment(i, j, 0);
            }
        
        // if a sensor is assigned to multiple targets
        // Assign sensor to the target it has maximum remaining energy
        for(int i=0;i<sensor_count_;++i){
            int selectedTID;
            // find first assigned target
            int tid=0;
            while(tid<target_count_ && assign_result_.GetAssignment(i, tid)!=1) ++tid;
            if(tid==target_count_)
            {
                // sensor is not assigned to any target
                // find the target with maximum remaining energy and assign to it
                selectedTID = MaxTarget(i, e);
                assign_result_.SetAssignment(i, selectedTID, 1);
            }
            else{
                selectedTID = tid;
                int maxre = e[i][tid];
                for (int j = tid+1; j < target_count_; ++j) {
                    if (assign_result_.GetAssignment(i,j)==1 && e[i][j] > maxre) {
                        selectedTID = j;
                        maxre = e[i][j];
                    }
                }
            }
            // disconnect the assignment with other targets
            for(int j=0;j<target_count_;j++) if(j!=selectedTID){
                assign_result_.SetAssignment(i,j,0);
            }
        }
    }
    
    int LPAlg::MaxTarget(const int sid, const IntegerMatrix& e){
        int tid = 0;
        int maxre = e[sid][0];
        for(int i=1;i<target_count_;++i) if(e[sid][i]>maxre){
            tid = i;
            maxre = e[sid][i];
        }
        return tid;
    }
    
    int LPAlg::RemainingEnergy(const int tid, const IntegerMatrix& e){
        int result = 0;
        for(int i=0;i<sensor_count_;++i) if(assign_result_.GetAssignment(i, tid)==1){
            result += e[i][tid];
        }
        return result;
    }
}
