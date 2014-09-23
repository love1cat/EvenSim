//
//  alg_input.cc
//  even_energy
//
//  Created by YUAN SONG on 6/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "alg_input.h"
#include "exception.h"

namespace even_energy {
    // path and file names
    const std::string kDefaultRootFolderName = "EEinput";
    const std::string kDefaultTargetFileName = "TargetFile.txt";
    const std::string kDefaultLocationFileName = "LocationFile.txt";
    //const std::string kDefaultLocationInfoFileName = "LocationInfoFile.txt";
    //const std::string kDefaultSensorFileName = "SensorFile.txt";
    const std::string kDefaultAlgorithmInputInitial = "alginp_";
    // limit the number of files created
    // since the number of files created is target_file_num * sensor_file_num !

    void AlgorithmInputWriter::WriteAlgorithmInputFiles(const int target_num, const int location_num, const int target_scenario_num,
            const int location_scenario_num, const int battery_power, const int energy_per_meter,
            const int seed, const std::string& filename, const std::string path, const std::string prefix) {
        const std::string rootpath = path + kDefaultRootFolderName + "/";
        mkdir(rootpath.c_str(), 0755);

        // write target file
        std::string targetfilepath = rootpath + prefix + kDefaultTargetFileName;
        //iw_.SetSeed(seed);
        //iw_.GenerateLocationsFile(target_scenario_num, target_num, targetfilepath);

        // write location file
        std::string locationfilepath = rootpath + kDefaultLocationFileName;
        iw_.SetSeed(seed);
        iw_.GenerateLocationsFile(location_scenario_num, location_num, locationfilepath);

        /*** Read target and sensor file, write location info file ***/
        // Create reader for target file and sensor file
        InputReader ir_target(targetfilepath);
        InputReader ir_location(locationfilepath);

        // Create algorithm input
        std::string algfilename = rootpath + filename;
        std::ofstream os(algfilename.c_str());
        os << target_scenario_num * location_scenario_num << std::endl;
        os << target_num << '\t' << location_num << std::endl; // write number of elements on each line, followed by number of lines
        os << battery_power << std::endl;
        int scenid = 0;
        // Read targets
        while (ir_target.ReadNextPointsSetFromFile()) {
            int cnt_target = ir_target.GetPointsCount();
            while (ir_location.ReadNextPointsSetFromFile()) {
                os << scenid << std::endl; // write scenario index 
                for (int i = 0; i < ir_location.GetPointsCount(); ++i) {
                    Point ps = ir_location.GetPoint(i);
                    for (int j = 0; j < cnt_target; ++j) {
                        Point pt = ir_target.GetPoint(j); //current target
                        os << ComputeRemainingEnergy(battery_power, energy_per_meter, ps, pt) << '\t';
                    }
                    os << std::endl;
                }
                ++scenid;
            }
            ir_location.Reset();
        }

        os.close();
    }

    int AlgorithmInputWriter::ComputeRemainingEnergy(const int battery_power, const int energy_per_meter, const Point& target, const Point& sensor) {
        double dist = Point::PointsDist(target, sensor);
        int remaining_energy = battery_power - dist * energy_per_meter;
        return remaining_energy > 0 ? remaining_energy : 0;
    }

    AlgorithmInputReader::AlgorithmInputReader(const std::string& filename, int sensor_per_location, double higher_sensor_number, int energy_diff)
    : filename_(filename), infile_(filename.c_str()), sensor_per_location_(sensor_per_location), higher_sensor_number_(higher_sensor_number), energy_diff_(energy_diff) {
        // Read basic info
        ReadDataInfo();
        sensor_count_ = sensor_per_location_ * location_count_;
        // Initialize 2D integer array
        imx_.reset(new IntegerArray[sensor_count_]);
        for (int i = 0; i < sensor_count_; ++i) {
            imx_[i].reset(new int[target_count_]);
        }
        
        // Initialize 2D remaining energy array
        locRm_.reset(new IntegerArray[location_count_]);
        for (int i = 0; i < location_count_; ++i) {
            locRm_[i].reset(new int[target_count_]);
        }
    }

    bool AlgorithmInputReader::ReadDataInfo() {
        if (infile_.is_open())
            infile_ >> dataset_count_ >> target_count_ >> location_count_ >> battery_power_;
        if (infile_.good()) return true;
        else return false;
    }

    bool AlgorithmInputReader::ReadNextInputSet() {
        int higher_sensor_num = higher_sensor_number_;
        if (infile_.is_open() && !infile_.eof()) {
            infile_ >> cursetid_;
            if (!infile_.good()) return false;
            for (int i = 0; i < location_count_; ++i) {
                int re;
                for (int j = 0; j < target_count_; ++j) {
                    infile_ >> re;
                    locRm_[i][j] = re;
                    int lowerid = i*sensor_per_location_;
                    int upperid = (i + 1) * sensor_per_location_ - 1;
                    for (int k = lowerid, count = 0; k <= upperid; ++k, ++count) {
                        if (count < higher_sensor_num) {
                            imx_[k][j] = re + energy_diff_;
                        } else{
                            imx_[k][j] = re;
                        }
                    }
                }
            }
            if (!infile_.good()) return false;
            return true;
        } else {
            return false;
        }
    }
}
