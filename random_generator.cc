#include "random_generator.h"
#include "exception.h"

namespace even_energy{
    bool RandomGenerator::IsProbability(const double probability){
        if(probability < 0 || probability > 1) throw Exception("Wrong probability!");
        return rand() % precision_ < probability * precision_;
    }
}