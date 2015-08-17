#ifndef STAIR_H
#define STAIR_H

#include <list>

class Stair : public std::pair< size_t, size_t > { 
    public:
    typedef std::list< Stair > Case;

    Stair(size_t index, size_t length) : 
        std::pair<size_t,size_t>(index,length) { }
    
    size_t& Index() { return first; }
    size_t& length() { return second; }
};

#endif
