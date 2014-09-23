/* 
 * File:   algbase.h
 * Author: andy
 *
 * Created on July 22, 2012, 6:19 PM
 */

#ifndef ALGBASE_H
#define	ALGBASE_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "alg_input.h"

namespace even_energy {

    class AssignmentResult {
    public:

        inline AssignmentResult() : obj_value_(0) {
        }
        AssignmentResult(const int sensor_number, const int target_number);
        void Reset(const int sensor_number, const int target_number);
        void Clear();

        inline int GetAssignment(const int sensor_id, const int target_id) const {
            return imx_result_[sensor_id][target_id];
        }

        inline void SetAssignment(const int sensor_id, const int target_id, const int val) {
            imx_result_[sensor_id][target_id] = val;
        }

        inline int GetObjValue() const {
            return obj_value_;
        }

        inline void SetObjValue(const int val) {
            obj_value_ = val;
        }

        inline int GetOptimizationStatus() const {
            return status_;
        }

        inline void SetOptimizationStatus(const int val) {
            status_ = val;
        }

        inline int GetSensorNumber() const {
            return sensor_number_;
        }

        inline int GetTargetNumber() const {
            return target_number_;
        }

        inline int GetEnergy(const int sensor_id, const int target_id) const {
            return imx_energy_[sensor_id][target_id];
        }

        inline void SetEnergy(const IntegerMatrix& energy) {
            imx_energy_ = energy;
        }
    private:
        IntegerMatrix imx_result_;
        IntegerMatrix imx_energy_;
        int obj_value_;
        int status_;
        int sensor_number_;
        int target_number_;
    };

    class AlgorithmBase {
    public:
        AlgorithmBase(const int sensor_count, const int target_count, const int battery_power);
        virtual void SolveNextScenario(const IntegerMatrix& e);

        inline const AssignmentResult& GetCurrentResult() {
            return assign_result_;
        }
    protected:
        virtual void SolveProblem(const IntegerMatrix& energy_matrix) = 0;
        int sensor_count_;
        int target_count_;
        int battery_power_;
        AssignmentResult assign_result_;
    };
}

#endif	/* ALGBASE_H */

