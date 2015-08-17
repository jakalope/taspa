#ifndef RGB_FLOAT_H
#define RGB_FLOAT_H

#include "rgb.h"

struct rgb_float {
    float r, g, b;
    rgb_float(rgb x) : r(x.r), g(x.g), b(x.b) { }
        
    static rgb_float unit(rgb s) {
        rgb_float scaled(s);
        
//			scaled.r /=  8.432;
//			scaled.g /= 16.425;
//			scaled.b /=  3.176;

        float magnitude = 
            sqrt(scaled.r*scaled.r + scaled.g*scaled.g + scaled.b*scaled.b);
        
        scaled.r /= magnitude; 
        scaled.g /= magnitude; 
        scaled.b /= magnitude;
        
        return scaled;
    }
};

#endif
