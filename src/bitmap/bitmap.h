/* 
 * Copyright 2009, 2010, Jake Askeland, jake(dot)askeland(at)gmail(dot)com
 * 
 *  * This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *  * This file is part of Topological all shortest paths automatique' (TASPA).
 * 
 *     TASPA is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     TASPA is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with TASPA.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <vector>
#include <set>

#include "../location/location.hpp"
#include "basic_bitmap.h"

class bitmap : public basic_bitmap {

protected:
	
    //bool (*distiller)(location, bitmap*);

    /* List of locations which lie on boundaries */
    location::Set boundaryLocation;

    // Find and stores obstacle boundary locations in this map.
    void FindBoundaries();

    /* Write a bitmap's information to file 'fileName' */
    void WriteRawFile (const std::string &fileName)
            throw (catch_all_exception);

    void MakeRunner(char* runner, int width);

public:
        
    ////////////////////////////////////////////////////////////////
    // Finds the obstacle boundaries on x-axis offset 'x'. Useful for 
    // reporting the remaining run-time of finding all boundary locations.
    void FindOneLineOfBoundaries(int x);

    ////////////////////////////////////////////////////////////////
    // Returns an std::set<location> object of the obstacle boundary 
    // locations in this map.
    location::Set GetBoundary();

    bool IsPassible (int x, int y);
    bool IsPassible (const location& loc);

    bool IsConnectedLatticeEdge (int x, int y);    
    location ExternalCellLocation(location l);
    location ExternalCellLocation(int x, int y);
    location LatticeToCell(location loc);
    
    location::Vector GetNonBorderLocs (location loc);

    ////////////////////////////////////////////////////////////////
    // Returns an std::vector<location> object of the locations along
    // a digital line from a to b.
    location::Vector GetLine(location a, location b);

    ////////////////////////////////////////////////////////////////
    // Returns true if the locations along a digital line from a to b are
    // traversable.		
    bool HasLineOfSight(location a, location b);

    /** Writes an ascii representation of a bitmap's mono mask.
        @param string& filename
        @param basic_bitmap* bbmp
    */
    void WriteLogFile (const std::string &filename) 
            throw (catch_all_exception);
            
    virtual void ByteCast(int x, int y, char* pointer) = 0;

    /** When overridden, should return a monochrome 
        distillation of the pixel at location #loc#.
        @exception catch_all_exception
        @memo
    */
    virtual mono Mono (const location& loc) = 0;

    /** When overridden, should return a monochrome 
        distillation of the pixel at location (#x#,#y#).
        @exception catch_all_exception
        @memo
    */
    virtual mono Mono (int x, int y) = 0;

    virtual unsigned char Luminance(location loc) = 0;
    virtual unsigned char Luminance(int x, int y) = 0;

    /** When overridden, should read a 
        bitmap file into an appropriate container. 
        @exception catch_all_exception
        @memo
    */
    virtual void ReadBitmapFile(const std::string &fileName)
            throw (catch_all_exception) {}

    /** When overridden, should write to a bitmap file. 
        @exception catch_all_exception
        @memo
    */
    virtual void WriteBitmapFile(const std::string &fileName) {}
    
    virtual void MarkVertices ( 
            const location::Vector& concaveVector,
            const location::Vector& convexVector ) {}
    
    virtual void MarkVertices 
        ( const location::Vector& vertices, rgba color ) {}

    virtual void resize(int width, int height) {}
    
    virtual location max() = 0;
    
    virtual void SetPixel ( int x, int y, rgba color ) {}
    virtual rgb GetPixel ( int x, int y ) = 0;
    
    virtual void FillHeaders () {}

};

///////////////////////////////////////////////////////////////////////
// Global function returning the L2 normal (Euclidean, Geodesic, etc.) 
// distance between src and dest, multiplied by an integer.
// We're using integer weights and we need to discern
// between diagonal distances which in reality are less than one unit 
// of one another. So we multiply everything under the radical by
// some low power of two (like 2^5), take the integer approx. of the 
// square root, and our final weight is close to 8 times that of the
// actual real-valued distance.
undirectedLength GetWeight(location, location, bitmap*);

#endif
