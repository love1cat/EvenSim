/* 
 * File:   output.h
 * Author: andy
 *
 * Created on July 19, 2012, 8:39 PM
 */

#ifndef OUTPUT_H
#define	OUTPUT_H

#include "datatypes.h"
#include "algbase.h"
namespace even_energy{
    class OutputWriter{
    public:
        inline OutputWriter(const std::string& filename, bool isOverwrite):outfilename_(filename){
            if(isOverwrite) Reset();
        }
        void WriteOutputFile(const AssignmentResult& assign_result);
        template<typename T>
        void WriteVal(const T& val){
            std::ofstream outfile(outfilename_.c_str(), std::ios_base::out | std::ios_base::app);
            outfile<<val;
            outfile.close();
        }
        inline void WriteEndOfLine(){
            std::ofstream outfile(outfilename_.c_str(), std::ios_base::out | std::ios_base::app);
            outfile<<std::endl;
            outfile.close();           
        }
        void Reset();
    private:
        int index_;
        std::string outfilename_;
    };
}

#endif	/* OUTPUT_H */

