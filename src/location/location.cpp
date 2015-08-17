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

#include <math.h>
#include "location.hpp"

////////////////////////////////////
// L1 normal (city block) distance
int L1_distance (location src, location dest) {
	return 		abs(src.x - dest.x)
			+ 	abs(src.y - dest.y);
}


////////////////////////////////////
// ostream interface. Outputs location as a coordinate pair.
std::ostream &operator<<(std::ostream &str, location const &loc) {

	str << "(" << loc.x << ", " << loc.y << ") ";
	return str;
}


////////////////////////////////////
// Returns true if lines A and B intersect
bool Intersects (location a0, location a1, location b0, location b1) {
	
	// detM = determinant of M
	int detM = (a1.x - a0.x) * (b1.y - b0.y) - (b1.x - b0.x) * (a1.y - a0.y);

	// special case: A and B are parallel or at least 
	// one of the lines is just 1 point.
	// when detM = 0, a bounds test is needed.
	if (detM == 0) {
		
		// special case: One or both lines are just 1 point
		// set the lines parallel to one another
		// if both are points, they will remain points
		// but the bounds test remains valid.
		if (a0 == a1) {
			a1.x += b1.x - b0.x;
			a1.y += b1.y - b0.y;
		}

		if (b0 == b1) {
			b1.x += a1.x - a0.x;
			b1.y += a1.y - a0.y;
		}

		// special case: A and B are vertical line segments.
		if (a0.x == a1.x && b0.x == b1.x) {
			// true if A and B are in the same vertical line.
			if (a0.x == b0.x)
				// true when some bounds on Ay are in the bounds of By.
				return (b0.y <= a0.y && a0.y <= b1.y) 
					|| (b0.y <= a1.y && a1.y <= b1.y);
			// different vertical lines, no intersection.
			else return false;
		}
		
		// for parallel lines to overlap, they need the same y-intercept.
		// integer relations to y-intercepts of A and B are as follows.
		int a_offset = ((a1.x-a0.x)*a0.y-(a1.y-a0.y)*a0.x) * (b1.x-b0.x);
		int b_offset = ((b1.x-b0.x)*b0.y-(b1.y-b0.y)*b0.x) * (a1.x-a0.x);
		
		// true when a_y_intercept == b_y_intercept.
		if (b_offset == a_offset)
						
			// true when some bounds on ax are in the bounds of bx.
			return (b0.x <= a0.x && a0.x <= b1.x) 
				|| (b0.x <= a1.x && a1.x <= b1.x);
		
		// different y intercepts; no intersection.		
		else return false;

	}
	
	// nMitc[0] = numerator_of_M_inverse_times_c0
	// nMitc[1] = numerator_of_M_inverse_times_c1
	int nMitc[2] = {
			(b0.x - a0.x) * (b1.y - b0.y) + (b0.y - a0.y) * (b0.x - b1.x),
			(b0.x - a0.x) * (a0.y - a1.y) + (b0.y - a0.y) * (a1.x - a0.x)
		};

	// true if an intersection between two non-parallel lines occurs between
	// the given segment points.
	return	((0 <= nMitc[0] && nMitc[0] <=  detM) 
		&&   (0 >= nMitc[1] && nMitc[1] >= -detM))
		||
			((0 >= nMitc[0] && nMitc[0] >=  detM) 
		&&   (0 <= nMitc[1] && nMitc[1] <= -detM));
}


bool Converges (location a1, location a2, location b1, location b2) {
	
	if (a1 == b1 || a1 == b2 || a2 == b1 || a2 == b2) return true;
	else return false;
}
	
	
