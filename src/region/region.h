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

#ifndef REGION_H
#define REGION_H

#include <ostream>
#include <fstream>
#include <stdexcept>

#include "../exception/catch_all_exception.hpp"
#include "../bitmap/rgb_bitmap.h"
#include "../location/location.hpp"
#include "region_bounds.hpp"
#include "SquareLatticeWalker.h"

enum region_type {
	REGION_ERROR		= 0,
	REGION_SOLID		= 1,
	REGION_IRREGULAR	= 2,
	REGION_OPEN			= 3,
	REGION_NARROW		= 4
};

struct region_type_info {
	region_type	type;
	char		name[10];
};

const region_type_info RegionTypeList[5] = {
	{ REGION_ERROR,		"Error",		},
	{ REGION_SOLID,		"Solid",		},
	{ REGION_IRREGULAR,	"Irregular",	},
	{ REGION_OPEN,		"Open",			},
	{ REGION_NARROW,	"Narrow"		}
};

struct subset_position {
	size_t index;
	location loc;
	std::string subset;
};

class region : public region_boundary {
    
    public:

    typedef std::list<region> List;

	protected:

	struct indexPair { size_t src; size_t dest; bool index_dir; };

	// ===============================================
	// Protected member variables

	// Points at which the contour's curvature changes
    location::Vector    convexVertex;
	location::Vector    linearVertex;
    std::string         curveWord;

	// ===============================================
	// Protected member funcitions

	int SetMajorVertices(bitmap& bmp, std::string& word, char val);

    int  FindConvexVertices (bitmap& bmp);
    
	size_t PreviousConvex   (bitmap& bmp, size_t index);
	bool   IsntConvexVertex (bitmap& bmp, size_t index);
	bool   IsConvexVertex   (bitmap& bmp, size_t index);
	bool   IsConcaveVertex  (bitmap& bmp, size_t index);

	size_t Cardinality(char letter, std::string alphabet);
	
	// IsBorder: Returns true if given location is passible and borders impassible
	bool IsBorder(const location& loc);

	// PaddedWord: Last letter appended to the front and first letter to the back
	std::string PaddedWord ( std::string word );
	
	public:

	// ===============================================
	// Public member funcitions

	/////////////////////////////////////////////////////
	/** @name Constructors **/
	//@{
	// Create an empty container
	region ();

	// Create and populate a container for a region's curvature extrema
	region (bitmap& bmp, location::Set& boundaryList);
	
	region (SquareLatticeWalker& walker);
	
	//@}

	/////////////////////////////////////////////////////
	/** @name Public Members **/
	//@{

	// ImportRegionBounds: Distill boundary locations into a vertex list
	int ImportRegionBounds (bitmap& bmp, location::Set& boundaryList);

	size_t size()
		{ return linearVertex.size(); }

	const location::Vector& GetConvexVertices() { return convexVertex; }
	const location::Vector& GetLinearVertices() { return linearVertex; }
	void SetLinearVertex(const location::Vector& V) { linearVertex = V; }
	const location::Vector& toVector() { return linearVertex; }

	static bool IsConcaveLocation(bitmap& bmp, location v);
    static bool IsConvexLocation(bitmap& bmp, location v);

	// LogVertices: Send textual log of what was found to outfile
	std::ofstream &LogVertices ( std::ofstream &outfile );
	std::string toString(int h);

// to do: remove
//    int FindPathfindingVertices
//        (bitmap& bmp, location::Set &boundaryList, Relative);
	
	location::Vector    concaveSegment;
	location::Vector    convexSegment;
	location::Vector    straightSegment;
	//@}
};

std::string LogLines(location::Vector vertices, int h);

void ParseBoundary
    (location::Set& lin, location::Set& cvx, region::List& rvList, 
    bitmap* inputBmp, SquareLatticeWalker& walker, location::Vector& convex, 
    location::Vector& concave, location::Vector& straight, std::ostream& verbs,
    bool storeWords, std::ofstream& logfile)  throw (catch_all_exception);


#endif
