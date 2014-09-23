/* 
 * File:   datatypes.h
 * Author: andy
 *
 * Created on July 19, 2012, 8:40 PM
 */

#ifndef DATATYPES_H
#define	DATATYPES_H

#include <boost/shared_array.hpp>

typedef boost::shared_array<int> IntegerArray;
typedef boost::shared_array<IntegerArray> IntegerMatrix;
typedef boost::shared_array<double> DoubleArray;
typedef boost::shared_array<DoubleArray> DoubleMatrix;
typedef boost::shared_array<bool> BoolArray;

#endif	/* DATATYPES_H */

