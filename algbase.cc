#include "algbase.h"

namespace even_energy {

    /* Assignment Ctor & Methods*/
    void AssignmentResult::Reset(const int sensor_number, const int target_number) {
        imx_result_.reset(new IntegerArray[sensor_number]);
        for (int i = 0; i < sensor_number; ++i) {
            imx_result_[i].reset(new int[target_number]);
        }
        obj_value_ = 0;
        sensor_number_ = sensor_number;
        target_number_ = target_number;
        imx_energy_.reset();
    }

    void AssignmentResult::Clear() {
        // Reset(sensor_number_, target_number_);
        for (int i = 0; i < sensor_number_; ++i) {
            memset(imx_result_[i].get(), 0, target_number_ * sizeof (int));
        }
        obj_value_ = 0;
        imx_energy_.reset();
    }

    AssignmentResult::AssignmentResult(const int sensor_number, const int target_number) {
        Reset(sensor_number, target_number);
    }

    /* AlgorithmBase Ctor & Methods*/
    AlgorithmBase::AlgorithmBase(const int sensor_count, const int target_count, const int battery_power)
    : sensor_count_(sensor_count), target_count_(target_count), battery_power_(battery_power) {
        assign_result_.Reset(sensor_count_, target_count_);
    }

    void AlgorithmBase::SolveNextScenario(const IntegerMatrix& energy_matrix) {
        SolveProblem(energy_matrix);
    }
}
