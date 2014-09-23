//
//  exception.h
//  even_energy
//
//  Created by YUAN SONG on 6/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef even_energy_exception_h
#define even_energy_exception_h

#include <cstdio>

namespace even_energy{
    class Exception{
    public:
        Exception(const char * error_message){
            printf("Error occured: %s\n", error_message);
            exit(0);
        }
    };
}


#endif
