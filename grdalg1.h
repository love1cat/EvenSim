/* 
 * File:   grdalg1.h
 * Author: andy
 *
 * Created on December 18, 2012, 11:38 PM
 */

#ifndef GRDALG1_H
#define	GRDALG1_H

#include "grdalg_base.h"

namespace even_energy{
    class GreedyAlg1 : public GreedyAlgBase{
	public:
		GreedyAlg1(bool use_assumed_energy = true) : use_assumed_energy_(use_assumed_energy) {}
		bool use_assumed_energy() {return use_assumed_energy_;}
	protected:
		virtual void SolveSubRoutine();
	private:
		bool use_assumed_energy_;
    };
}


#endif	/* GRDALG1_H */

