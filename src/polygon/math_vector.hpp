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

#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

#include <math.h> // for atan2 and sqrt
#include <set>
#include "../location/location.hpp"

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

class math_vector {
	public:
		typedef std::multiset<math_vector> Graph;
		typedef std::multiset<math_vector>::iterator GraphIter;
		typedef std::multiset<math_vector>::const_iterator ConstGraphIter;
	
		double x;
		double y;

		math_vector ( double setX, double setY ) : x(setX), y(setY) { }

		math_vector ( int setX, int setY ) :
			x(static_cast<double>(setX)), y(static_cast<double>(setY)) { }

		math_vector ( location set ) :
			x(static_cast<double>(set.x)), y(static_cast<double>(set.y)) { }

		math_vector ( location first, location second ) :
			x(static_cast<double>(second.x-first.x)),
			y(static_cast<double>(second.y-first.y)) { }

		double angle() const {
			if (x == 0 && y == 0) return 0.;
			double rads = atan2(y,x);
			return (rads < 0.) ? rads+(2*M_PI) : rads;
		}

		double magnitude() const { return sqrt(x*x+y*y); }

		math_vector operator- ( math_vector arg )
			{ arg.x -= x; arg.y -= y; return arg; }

		math_vector operator+ ( math_vector arg )
			{ arg.x += x; arg.y += y; return arg; }

		math_vector &operator-= ( math_vector &arg )
			{ arg.x -= x; arg.y -= y; return arg; }

		math_vector &operator+= ( math_vector &arg )
			{ arg.x += x; arg.y += y; return arg; }

		math_vector &operator= ( math_vector &arg )
			{ x = arg.x; y = arg.y; return *this; }
			
		inline bool operator< (const math_vector v) const { 
			return angle() < v.angle();
		}
		
		operator location () const 
            { const location loc((int)x,(int)y); return loc; }
};

#endif

