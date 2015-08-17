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

#ifndef REGION_BOUNDS_HPP
#define REGION_BOUNDS_HPP

#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "../exception/catch_all_exception.hpp"
#include "../location/location.hpp"
#include "../bitmap/bitmap.h"

const int REGION_NO_FILTER = -2;

////////////////////////////////////////////////////////////////////////////////
/** Object defining a 2D map region as a vector of discrete points along its
	contour. The points are not necessarily vertices; rather, they are pixels
	in a bitmap's boundary mask, such that a staight line is given by the set
	of points making up that line.

	This object also defines the contour by both a relative and an absolute
	directional 'word'. A word in this sense is a chain of values, each
	associated with some larger meaning. In this case, the absolute word is
	a chain of values associated with the directions 'north', 'east', 'south',
	and 'west' and the relative word, with 'foward', 'right', 'about-face', and
	'left'. Given the corresponding entries in each of the two words, polygonal
	vertex information can be extrapolated later on.

	This is an intermediate object between a boundary mask and a set of
	regional vertices. One boundary mask is assumed to contain many sets of
	region boundaries and each region boundary will contain exactly one set
	of region vertices; thus, a region boundary is a base class of
	#region#.

	@version 0.1.0 Build 0
	@author J. Askeland
	@memo
*/
class region_boundary {
	protected:
		// 'initialized' is true after border is successfully traversed
		bool initialized;

		// Contour points: regional boundary
		location::Vector boundary;
		
		// Temporary storage
		location::List boundaryLocation;

		// FMI, see: Lothaire, M.: Applied combinatorics on words. (2005).
        std::string RelWord;
        std::string AbsWord;

	public:

		location::Vector& GetBorder() { return boundary; }

		const location::Vector *GetBorderPtr() const
			{ return &boundary; }

		size_t length() { return boundary.size(); }

		std::ofstream &LogAbsoluteWord (std::ofstream &outfile) {
            outfile << GetAbsoluteWord() << "\n";
            return outfile;
        }

		std::string LogAbsoluteWord() { return AbsWord; }
		
		std::string GetAbsoluteWord () { return AbsWord; }

		std::string GetRelativeWord () { return RelWord; }

		//@}
};

#endif
