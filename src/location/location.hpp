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

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <set>
#include <list>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <math.h>
#include "../thorup/Graph.h"

//////////////////////////////////////////////////////////////////////////////
/** Location Object

	Two-dimentional coordinates (int x, int y). Defines many stl typedefs
	and is a base class to 

	@version 0.1.0 Build 0
	@author J. Askeland
*/
class location {
	public:

		typedef std::set<location> Set;
		typedef Set::iterator SetIter;
		typedef Set::const_iterator ConstSetIter;
		typedef std::vector<Set> SetVector;
		typedef SetVector::iterator SetVectorIter;
		typedef std::list<location> List;
		typedef std::list<location>::iterator ListIter;
		typedef std::list<location>::const_iterator ConstListIter;
		typedef std::vector<location> Vector;
		typedef std::vector<location>::iterator VectorIter;
		typedef std::vector<location>::const_iterator ConstVectorIter;
        typedef std::pair<location,location> Line;
        typedef std::pair<location,location> Pair;
		typedef std::vector<Line> LineVector;
		typedef std::vector<Line>::iterator LineVectorIter;
		typedef std::list<Vector> ListOfVectors;
		
		/////////////////////////////////////////////////////
		/** @name Constructors **/
		//@{
		/// Empty constructor.
		location () : x(0),y(0) { }

		/// Integer-assigned constructor.
		location (int _x, int _y) : x(_x), y(_y) { }

		//@}

		/////////////////////////////////////////////////////
		/** @name Comparison operators **/
		//@{
		/// Equal
		bool operator== (const location _loc) const
			{ return (_loc.x==x && _loc.y==y); }

		/// Not equal
		bool operator!= (const location _loc) const
			{ return !(_loc.x==x && _loc.y==y); }

		/** Less
			x's are compared first, then y's.
		**/
		bool operator< (const location _loc) const {
			return (x < _loc.x || (x == _loc.x && y < _loc.y));
		}

		/** Less/Equal
			x's are compared first, then y's.
		**/
		bool operator<= (const location _loc) const {
			if (x == _loc.x && y == _loc.y) return false;
			if (x == _loc.x) return y <= _loc.y;
			return x <= _loc.x;
		}

		/** More
			x's are compared first, then y's.
		**/
		bool operator> (const location _loc) const {
			if (x == _loc.x && y == _loc.y) return false;
			if (x == _loc.x) return y > _loc.y;
			return x > _loc.x;
		}

		/** More/Equal
			x's are compared first, then y's.
		**/
		bool operator>= (const location _loc) const {
			if (x == _loc.x && y == _loc.y) return false;
			if (x == _loc.x) return y >= _loc.y;
			return x >= _loc.x;
		}
		//@}

		/////////////////////////////////////////////////////
		/** @name Assignment methods */
		//@{
		/// := location assignment.
		const location &operator= (const location &_loc) {
			x = _loc.x;
			y = _loc.y;
			return *this;
		}

		/// Integer assignment.
		void assign(int _x, int _y)
			{ x = _x; y = _y; }

		/// Location assignment.
		void assign(location _loc)
			{ x = _loc.x; y = _loc.y; }

		//@}

        location operator + (location add) 
            { return location(x + add.x, y + add.y); }

		location &operator++ () { x++; y++; return *this; }

		static bool LessThan (const location& loc_1, const location& loc_2) {
			if (loc_1.x == loc_2.x && loc_1.y == loc_2.y) return false;
			if (loc_1.x == loc_2.x) return loc_1.y < loc_2.y;
			return loc_1.x < loc_2.x;
		}

		friend std::ostream &operator<<
			(std::ostream &str, location const &loc);
			
		int L2sqrd(location dest) const
			{ return (x-dest.x)*(x-dest.x)+(y-dest.y)*(y-dest.y); }

        /////////////////////////////////////////////////////////////////////
        // L2scaled:
        // We're using integer weights and we need to discern
        // between diagonal distances which in reality are less than one unit 
        // of one another. So we multiply everything under the radical by
        // some low power of two (like 2^5), take the integer approx. of the 
        // square root, and our final weight is close to 8 times that of the
        // actual real-valued distance.
        
        /** @precondition $||src - dest||_\{infty} < 2^23 - 1$
        **/
		undirectedLength L2scaled(location dest) const {
		    // Locations are integer pairs, thus:
		    undirectedLength x_ = x - dest.x;  // Max value is 2^23 - 1
		    undirectedLength y_ = y - dest.y;  // Max value is 2^23 - 1
            
            // Max value of x^2 + y^2 = 2(2^23 - 1)(2^23 - 1)
            //                        = 2(2^46 - 2^24 + 1)
            //                        = 2^47 - 2^25 + 2
            
            // Let floor(log2(s)) = log2(2^63 - 1) - log2(2^46 - 2^25 + 2), 
            // then s = 2^16 = 65536
            // Max of s*(x^2 + y^2) = (2^16)*(2^47 - 2^25 + 2)
            //                      = 2^63 - 2^41 + 2^17 < 2^63 - 1
            // Thus s*(x^2 + y^2) does not overflow long.
            
		    const undirectedLength s = 65536;

            // Max Sqrt(s*(x^2 + y^2)) = (2^63 - 2^41 + 2^17)^(1/2)
            //                         ~ 3.0370e+09
            //                         = 2^(31.5) < 2^32 - 1
            // Thus Sqrt(s*(x^2 + y^2)) does not overflow unsigned int.
            
            return (undirectedLength)(sqrtl(s*(x_*x_ + y_*y_)));
        }


		int L2norm(location dest) const
			{ return (int)(sqrt((x-dest.x)*(x-dest.x)+(y-dest.y)*(y-dest.y))); }
		
		int L1norm(location dest) const
			{ return (abs(x-dest.x))+(abs(y-dest.y)); }

		int x;
		int y;
};


/////////////////////////////////////////////////////
/** @name Non-member methods */
//@{

/// Distance between two locations.
int L1_distance (location src, location dest);

/// Determines if an intersection occurs on two line segments.
bool Intersects (location a1, location a2, location b1, location b2);
bool Converges (location a1, location a2, location b1, location b2);

//@}

#endif
