//
//  alg_input.h
//  even_energy
//
//  Created by YUAN SONG on 6/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef even_energy_alg_input_h
#define even_energy_alg_input_h

#include "input.h"
#include "datatypes.h"

namespace even_energy{
    
    class AlgorithmInputWriter{
    public:
        inline AlgorithmInputWriter(const int length, const int width, const int xoffset = 0, const int yoffset = 0):
			iw_(length, width, xoffset, yoffset) {}
        // for each target, generate a file contain scenario_num scenarios, each scenario contains sensor_num sensor locations
        void WriteAlgorithmInputFiles(const int target_num, const int location_num, const int target_scenario_num, 
                                      const int location_scenario_num, const int battery_power, const int energy_per_meter, 
                                      const int seed, const std::string& filename, const std::string path = "./", const std::string prefix="");
    private:
        InputWriter iw_;
        
        int ComputeRemainingEnergy(const int battery_power, const int energy_per_meter, const Point& target, const Point& sensor);
    };
    
    class AlgorithmInputReader{
    public:
        AlgorithmInputReader(const std::string& filename, int sensor_per_location, double higher_sensor_ratio = 0.0, int energy_diff=0);
        inline ~AlgorithmInputReader(){infile_.close();}
        inline int GetCurrentDatasetID() const{return cursetid_;}
        inline int GetData(const int sensorid, const int targetid) const{
            return imx_[sensorid][targetid];
        }
        inline const IntegerMatrix& GetDataMatrix() const{
            return imx_;
        }
        inline const IntegerMatrix& GetLocationRMMatrix() const {
            return locRm_;
        }
        inline int GetTargetCount() const{return target_count_;}
        inline int GetLocationCount() const{return location_count_;}
        inline int GetSensorCount() const{return sensor_count_;}
        inline int GetBatteryPower() const{return battery_power_;}
        inline int GetSensorPerLocation() const{return sensor_per_location_;}
        inline int GetEnergyDiff() const{return energy_diff_;}
        inline double GetHighSensorNumber() const{return higher_sensor_number_;}
        
        bool ReadNextInputSet();
        inline bool reset(){
            infile_.close();
            infile_.open(filename_.c_str());
            return ReadDataInfo();
        }
    private:
        std::string filename_;
        IntegerMatrix imx_;
        IntegerMatrix locRm_;
        std::ifstream infile_;
        int dataset_count_;
        int target_count_;
        int sensor_count_;
        int location_count_;
        int battery_power_;
        int cursetid_;
        int sensor_per_location_;
        double higher_sensor_number_;
        int energy_diff_;
        bool ReadDataInfo();
    };
}

#endif
