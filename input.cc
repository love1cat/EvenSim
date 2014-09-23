//
//  input.cc
//  even_energy
//
//  Created by YUAN SONG on 5/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include "input.h"
#include "exception.h"
#include "datatypes.h"

namespace even_energy{
    void InputWriter::GenerateLocationsFile(const int dataset_number, const int point_number, const std::string& filename){
        int seed = seed_;
        std::ofstream outfile(filename.c_str());
        outfile << dataset_number << std::endl;
        for(int i=0;i<dataset_number;++i, ++seed){
            GenerateLocations(seed, point_number);
            WritePointsToFile(i, outfile);
        }
        outfile.close();
    }
    
    
    void InputWriter::GenerateLocations(const int seed, const int number){
        // Generate points without repeat
        // basic idea: turn area into one dimensional array
        // shuffle array and select first n number
        // convert seleceted numbers into points
        
        // edges should have width of 1
        srand(seed);
        long ntotal_points = (width_-2)*(length_-2);
        if (number > ntotal_points) throw Exception("for the given size of the area, the number of points needed is too large!");
        pptr_.reset(new Point[number]);
        
        // create a temparary array
        IntegerArray tmp_array(new int[ntotal_points]);
        for(int i=0;i<ntotal_points;++i) tmp_array[i] = i;
        
        // shuffle temp array
        for(int i=0;i<number;++i){
            // pick a random index in the array from the index > i
            // and swap with current position
            int random_id = rand() % (ntotal_points-i)+i;
            // swap
            int tmp = tmp_array[i];
            tmp_array[i] = tmp_array[random_id];
            tmp_array[random_id] = tmp;
        }
        
        // convert first n numbers
        for (int i=0;i<number;++i){
            int xval = tmp_array[i] % (width_-2);
            int yval = tmp_array[i] / (width_-2);
            pptr_[i].x = xval;
            pptr_[i].y = yval;
        }
        
        // save number
        cur_count_ = number;
        
        // set flag
        is_points_generated_ = true;
    }

    void InputWriter::WritePointsToFile(const int dataset_id, std::ofstream& outfile){
        // write generated points to file
        if (!is_points_generated_) return;
        outfile << dataset_id << std::endl;
        outfile << cur_count_ <<std::endl;
        outfile << length_ << '\t' << width_ << std::endl;
        for(int i=0;i<cur_count_;++i){
            outfile << (int)(xoffset_ + pptr_[i].x) << '\t' << (int)(yoffset_ + pptr_[i].y) << std::endl;
        }
    }
    
    bool InputReader::ReadDatasetCount(){
        if(infile_.is_open())
            infile_ >> dataset_count_;
        if (infile_.good()) return true;
        else return false;
    }
    
    bool InputReader::ReadNextPointsSetFromFile(){
        if (infile_.is_open() && !infile_.eof()){
            int x, y;
            int count;
            infile_ >> cursetid_;
            if(!infile_.good()) return false;
            infile_ >> count;
            infile_ >> length_ >> width_;
            if (count > 0){
                // only reset pointer if current array is smaller
                if(count > capacity_){
                    pptr_.reset(new Point[count]);
                    capacity_ = count;
                }
            }
            else throw Exception("The number of points is not correctly read!");
            int curid = 0;
            while(infile_.good()){
                infile_>>x>>y;
                pptr_[curid].x = x;
                pptr_[curid].y = y;
                if(++curid >= count) break;
            }
            count_ = curid;
            return true;
        } else{
            return false;
        }
    }
    
} // namespace even_energy
