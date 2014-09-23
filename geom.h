//
//  geom.h
//  even_energy
//
//  Created by YUAN SONG on 5/28/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef even_energy_geom_h
#define even_energy_geom_h

#include <cmath>

namespace even_energy{
    
class Point{
public:
    Point():x(0.0), y(0.0){}
    Point(const double xval, const double yval):x(xval), y(yval){}
    inline static double PointsDist(const Point p1, const Point p2){
        return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
    }
    double x;
    double y;
};
    
}
#endif
