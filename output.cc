#include <fstream>
#include "output.h"

namespace even_energy {
    /* OutputWriter Ctor & Methods */
    void OutputWriter::Reset() {
        std::ofstream outfile(outfilename_.c_str(), std::ios_base::out | std::ios_base::trunc);
        index_ = 0;
        outfile.close();
    }

    void OutputWriter::WriteOutputFile(const AssignmentResult& assign_result) {
        std::ofstream outfile(outfilename_.c_str(), std::ios_base::out | std::ios_base::app);
        outfile << "#######################################" << std::endl;
        outfile << "Scenario " << index_ << std::endl;
        outfile << "---------------------------------------" << std::endl;
        outfile << "***************************************" << std::endl;
        outfile << "Remaining Energy: " << assign_result.GetObjValue() << std::endl;
        outfile << "---------------------------------------" << std::endl;
        outfile << "\t\t";
        for (int i = 0; i < assign_result.GetTargetNumber(); ++i) {
            outfile << "Target" << i << '\t';
        }
        outfile << std::endl << std::endl;
        for (int i = 0; i < assign_result.GetSensorNumber(); ++i) {
            outfile << "Sensor " << i << ":\t";
            for (int j = 0; j < assign_result.GetTargetNumber(); ++j) {
                outfile << assign_result.GetEnergy(i, j) << '\t';
            }
            outfile << std::endl;
        }
        outfile << std::endl;
        outfile << "***************************************" << std::endl;
        outfile << "Assignment Result" << std::endl;
        outfile << "---------------------------------------" << std::endl;
        outfile << "Objective Value: " << assign_result.GetObjValue() << std::endl;
        for (int i = 0; i < assign_result.GetSensorNumber(); ++i) {
            outfile << "Sensor " << i << ":\t";
            for (int j = 0; j < assign_result.GetTargetNumber(); ++j) {
                outfile << assign_result.GetAssignment(i, j) << '\t';
            }
            outfile << std::endl;
        }
        outfile << "#######################################" << std::endl << std::endl;
        outfile.close();
        ++index_;
    }
}
