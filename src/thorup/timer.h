/*
 *  timer.h
 *  
 *  Created by Joseph Crobak on 1/13/06.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */

#ifndef TIMER_H
#define TIMER_H

#include <time.h>

class Timer {
public:
    Timer(char *msg,bool p=true) : timerString(msg),print(p) {}
    void start() {
        myClock = clock();
    
    }
    double finish() {
        double seconds;
        seconds = (clock() - myClock) / (double) CLOCKS_PER_SEC;
        if (print)
            printf("%s: %f seconds.\n",timerString,seconds);
        return seconds;
    }
    
    char *timerString;
    clock_t myClock;
    bool print;
};

#endif
