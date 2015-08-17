#include "SquareLatticeWalker.h"
#include <assert.h>

static const char RFL[4]  = { 'R', 'F', 'L' };
static const char WENS[4] = { 'W', 'E', 'N', 'S' };


// II
const SquareLatticeWalker::LatticeDir 
SquareLatticeWalker::RotationMap[4][3] = {
    //     Offset, NextDir, FarCorner, NearCorner,       Directed, Looking
    { { offst( 0,-1), N, offst(-1, 0), offst( 0,-2) },    // West, Right
      { offst(-1, 0), W, offst(-1, 0), offst( 0,-2) },    // West, Forward
      { offst( 0, 1), S, offst(-1,-2), offst( 0, 0) },},  // West, Left 

    { { offst( 1, 1), S, offst( 1,-2), offst( 0, 0) },    // East, Right
      { offst( 1, 0), E, offst( 1,-2), offst( 0, 0) },    // East, Forward
      { offst( 1,-1), N, offst( 1, 0), offst( 0,-2) },},  // East, Left 

    { { offst( 0,-1), E, offst(-1,-3), offst( 0,-1) },   // North, Right
      { offst( 0,-2), N, offst(-1,-3), offst( 0,-1) },   // North, Forward
      { offst(-1,-1), W, offst( 0,-3), offst(-1,-1) },}, // North, Left

    { { offst(-1, 1), W, offst( 0, 1), offst(-1,-1) },   // South, Right
      { offst( 0, 2), S, offst( 0, 1), offst(-1,-1) },   // South, Forward
      { offst( 0, 1), E, offst(-1, 1), offst( 0,-1) } }  // South, Left
};


SquareLatticeWalker::SquareLatticeWalker() { }


SquareLatticeWalker::SquareLatticeWalker(bitmap* _bmp, std::ostream& out)
{
    initialize(_bmp,out);
}


void SquareLatticeWalker::initialize(bitmap* _bmp, std::ostream& out)
{
    bmp = _bmp;
    
    // Initialize connected
    for (int x = 0; x <= bmp->max().x; x ++) 
    {
        out << ((x+1)*100)/bmp->GetWidth() << "%  \r";
        out.flush();
            
        for (int y = 0; y <= bmp->max().y*2+1; y ++) 
        {
            if (bmp->IsConnectedLatticeEdge(x,y))
                connected.insert(latticeEdge(x,y));
        }
    }
}


bool SquareLatticeWalker::HasNext()
{
    return connected.size() > 0;
}


void SquareLatticeWalker::ProcessNextBoundary()
{
    if (!HasNext()) return;
    
    boundaryList.clear();
    rellist.clear();

    latticeEdge firstEdge = *connected.begin();
    latticeEdge nextEdge  = firstEdge;
    AbsDir nextAbsDir = W;

    if (HasNext()) {
        FindNextLatticeEdge(nextEdge, nextAbsDir);
        connected.erase(nextEdge);
    }

//    rellist.push_back('F');
    boundaryList.push_back(bmp->ExternalCellLocation(nextEdge));
//    connected.erase(nextEdge);
    
    do {
        
        FindNextLatticeEdge(nextEdge, nextAbsDir);
        connected.erase(nextEdge);
        
    } while (nextEdge != firstEdge && HasNext());
    
//    if (HasNext()) {
//        FindNextLatticeEdge(nextEdge, nextAbsDir);
//        connected.erase(nextEdge);
//    }

//    if (rellist.size() < boundaryList.size()) {
//        boundaryList.pop_front();
//    }

    relword.clear();
    relword.insert(relword.begin(), rellist.begin(), rellist.end());
    
    absword.clear();
    absword.insert(absword.begin(), abslist.begin(), abslist.end());
    
    boundary.clear();
    boundary.insert(boundary.begin(), boundaryList.begin(), boundaryList.end());
}


void SquareLatticeWalker::FindNextLatticeEdge
(latticeEdge &nextEdge, AbsDir &nextAbsDir)
{
    for (int offsetIndex = R; offsetIndex <= L; offsetIndex ++)
    {
        LatticeDir  check = RotationMap[nextAbsDir][offsetIndex];
        
        latticeEdge checkEdge = check.offset + nextEdge;
        
        location    checkLoc  = 
            bmp->ExternalCellLocation(checkEdge);
        
        location    checkFar  = 
            bmp->LatticeToCell(check.farCorner  + nextEdge);

        location    checkNear = 
            bmp->LatticeToCell(check.nearCorner + nextEdge);
        
        // Check offset (A) for lattice edge. If it is and if bitmap
        // squares (D) or (E) are traversable, record relative 
        // direction RFL[offsetIndex] and set 
        if (bmp->IsConnectedLatticeEdge(checkEdge.x, checkEdge.y) &&
           (bmp->IsPassible(checkFar) || bmp->IsPassible(checkNear) ||
            IsACorner(checkLoc))) 
        {
            // When traversing forward, we need to check both sets of
            // near/far corners for blockage.
            if (offsetIndex == F) {
                LatticeDir otherCheck = RotationMap[nextAbsDir][2];
                location checkOtherFar = 
                    bmp->LatticeToCell(otherCheck.farCorner + nextEdge);
                location checkOtherNear =
                    bmp->LatticeToCell(otherCheck.nearCorner + nextEdge);
                if (!(bmp->IsPassible(checkOtherFar) || 
                    bmp->IsPassible(checkOtherNear) ||
                    IsACorner(checkLoc)))
                        continue;
            }
            
            // handle II.b cases here

            // II.b (b)
            // Did we skip an outside corner?
            // If so, we need to account for this by adding nextEdge + (D) to 
            // boundaryList and 'F' to rellist (and updating indexMap).
            if (bmp->IsPassible(checkFar) && offsetIndex != F)
            {
                boundaryList.push_back
                    (bmp->LatticeToCell(check.farCorner + nextEdge));
                rellist.push_back('F');
            }
            
            // set next edge and direction
            nextEdge = checkEdge;
            nextAbsDir = check.nextDirection;
            
            // II.b
            // store relative letter and boundary location
            rellist.push_back(RFL[offsetIndex]);
            abslist.push_back(WENS[nextAbsDir]);
            boundaryList.push_back(checkLoc);
            
            return;          
        }
    }
    
    // Failed to find the next lattice edge.
    assert(false);
}


bool SquareLatticeWalker::IsACorner(location loc)
{
    const location corner[4] = { 
        location(0,0), location(bmp->max().x,0), 
        location(0,bmp->max().y), bmp->max() };

    return (loc == corner[0]) || (loc == corner[1]) || 
           (loc == corner[2]) || (loc == corner[3]);
}


std::string SquareLatticeWalker::GetRelativeWord()
{
    return relword;
}


std::string SquareLatticeWalker::GetAbsoluteWord()
{
    return absword;
}


location::Vector SquareLatticeWalker::GetBoundary()
{
    return boundary;
}


location::Vector::const_iterator SquareLatticeWalker::begin()
{
    return boundary.begin();
}


location::Vector::const_iterator SquareLatticeWalker::end()
{
    return boundary.end();
}


int SquareLatticeWalker::size()
{
    return connected.size();
}
