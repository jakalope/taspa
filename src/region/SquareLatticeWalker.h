#ifndef SQUARELATTICEWALKER_H
#define SQUARELATTICEWALKER_H

#include "../bitmap/bitmap.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>

enum Offset { R, F, L };
enum AbsDir { W, E, N, S };

class SquareLatticeWalker
{
    
private:
    
    // Types:
    typedef location latticeEdge;
    typedef location offst;
            
    struct LatticeDir
    {
        latticeEdge offset;     // (A)
        AbsDir nextDirection;      // (C) := (int)(AbsDir)%2
        latticeEdge farCorner;  // (D)
        latticeEdge nearCorner; // (E)
    };        
    
    // Constants:
    static const LatticeDir RotationMap[4][3];
    
    // Inputs:
    bitmap* bmp;
    
    // Temporary Storage:
    std::list<char> rellist;
    std::list<char> abslist;
    std::set<latticeEdge> connected;
    location::List boundaryList;
    
    // Outputs:
    std::string relword;
    std::string absword;
    location::Vector boundary;

    // Private Methods:
    void FindNextLatticeEdge(latticeEdge &, AbsDir &);
    bool IsACorner(location);

public:

    // Public Methods
    SquareLatticeWalker();
    SquareLatticeWalker(bitmap*, std::ostream&);
    void initialize(bitmap*, std::ostream&);
    bool HasNext();
    void ProcessNextBoundary();
    std::string GetRelativeWord();
    std::string GetAbsoluteWord();
    location::Vector GetBoundary();
    location::Vector::const_iterator begin();
    location::Vector::const_iterator end();
    int size();
    bitmap* BitmapPtr() { return bmp; }

};

#endif
