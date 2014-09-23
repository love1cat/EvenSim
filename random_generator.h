/* 
 * File:   random.h
 * Author: andy
 *
 * Created on August 12, 2012, 11:39 PM
 */

#ifndef RANDOM_GENERATOR_H
#define	RANDOM_GENERATOR_H

#include <cstdlib>

namespace even_energy{
    class RandomGenerator{
    public:
        inline RandomGenerator(const int seed, const int precision = 10000)
        : seed_(seed), precision_(precision){
            srand(seed_);
        }
        bool IsProbability(const double probability);
    private:
        const int seed_;
        const int precision_;
    };
}


#endif	/* RANDOM_GENERATOR_H */

