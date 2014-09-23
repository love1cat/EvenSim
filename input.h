//
//  input.h
//  even_energy
//
//  Created by YUAN SONG on 5/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef even_energy_input_h
#define even_energy_input_h

#include <fstream>
#include <boost/shared_array.hpp>
#include "geom.h"

namespace even_energy{

    typedef boost::shared_array<Point> PointArrPtr;
        
    class InputWriter{
    public:
        inline InputWriter(const int length, const int width, const int xoffset = 0, const int yoffset = 0)
			:length_(length), width_(width), seed_(0), is_points_generated_(false), xoffset_(xoffset), yoffset_(yoffset){}
        void GenerateLocationsFile(const int set_number, const int point_number, const std::string& filename);
        inline void SetSeed(int seed){seed_ = seed;}
    private:
        const int length_;
        const int width_;
        int seed_;
        bool is_points_generated_;
        int cur_count_;
        PointArrPtr pptr_;
		int xoffset_;
		int yoffset_;
        
        void GenerateLocations(const int seed, const int number);
        void WritePointsToFile(const int dataset_id, std::ofstream& outfile);
        inline const Point& GetPoint(const int index) const {return pptr_[index];}
    };
    
    class InputReader{
    public:
        inline InputReader (const std::string filename):length_(0), width_(0), capacity_(0), count_(0), cursetid_(-1), dataset_count_(0), filename_(filename), infile_(filename.c_str()){ReadDatasetCount();}
        inline ~InputReader(){infile_.close();}
        inline int GetPointsCount(){return count_;}
        inline int GetAreaLength(){return length_;}
        inline int GetAreaWidth(){return width_;}
        inline int GetDatasetCount(){return dataset_count_;}
        inline void Reset(){
            infile_.close();
            infile_.open(filename_.c_str());
            ReadDatasetCount();
        }

        bool ReadNextPointsSetFromFile();
        inline const Point& GetPoint(const int index) const {return pptr_[index];}
    private:
        int length_;
        int width_;
        int count_;
        int capacity_;
        int cursetid_;
        int dataset_count_;
        PointArrPtr pptr_;
        std::string filename_;
        std::ifstream infile_;
        bool ReadDatasetCount();
    };
    
}

#endif
