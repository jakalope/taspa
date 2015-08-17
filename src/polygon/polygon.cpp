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

#include "polygon.hpp"
#include "../std_extensions/set_operations.h"  // For 'Conatins'
#include <assert.h>  // For assert

void GeneratePolygons 
 ( std::vector<polygon>& polyList, std::vector<polygon>& unusedPolyList, 
   location::Set& sharedSet, location::Set& U, AdjacencyMatrix& M, bool verbose, 
   std::ostream& outStrm )
{	
	polygon previous;
	int repeats=0;
	const int MAX_REPEATS = 8;
	location::Set existingSet;
	size_t prev_polyList_size = 0;
	
	while (M.HasUsable() && repeats < MAX_REPEATS) {

		// Report progress
		bool madeAPolygon = polyList.size() - prev_polyList_size;
		if (verbose && madeAPolygon) {
			outStrm << polyList.size() << " convex partitions generated...\r";
			outStrm.flush();
		}
		prev_polyList_size = polyList.size();

		// Make another polygon
		polygon p(M);
		
		// Store predicate "it isn't a 3 pt stright line"
		bool isntColinearTriple = !M.IsColinear(p);
		
		// If its not a 2 pt or 3 pt straight line, store the polygon.
		if (p.size() > 2 && isntColinearTriple) {
			
			// See if this vertex is shared by other polygons.
			for (polygon::iterator pIter = p.begin(); 
					pIter != p.end(); pIter++) {
				
				// If it is, store it in the set of shared vertices.
				if ( Contains( existingSet, *pIter ) )
					sharedSet.insert(*pIter);
				
				// Otherwise, store it in the set of all vertices.
				else existingSet.insert(*pIter);
			}
			
			// Store the polygon in polyList, 
			// since it isn't 2 or 3 pt straight line.
			polyList.push_back(p);
		}
		
		// Otherwise, store it in the un-used polygon list.
		else {
			unusedPolyList.push_back(p);
		}
		
		// If we're repeating, keep track.
		if (previous == p) repeats ++;
		else repeats = 0;
		//if (repeats >= MAX_REPEATS) M.ResetObstacleBoundaries();
		previous = p;
	}
	
	outStrm << std::endl;

	location::Set::iterator start2 = sharedSet.begin();
	location::Set::iterator start1 = existingSet.begin();
	
	location::Set::iterator end2 = sharedSet.end();
	location::Set::iterator end1 = existingSet.end();
	
	std::set_difference( start1, end1, start2, end2, 
		std::inserter(U, U.end()) );
}


/*  Let (v) denote the edge count from a vertex v.
 *  Let |v| denote the number of usable edges (edges-antiedges) from v.
 *  Let Gn denote the vertex on the tail of graph G, which is 
 *  sorted by # of edges from min to max, thus Gn is the vertex 
 *  with the most edges, which implies (Gn) = max(Gi).
 * 
 *  Since we want max|Gi|, we need to parse G, calculate |Gi|, compare
 *  it to |Gp|, the previous maximum |Gi| stored while parsing, and
 *  if |Gi| > |Gp|, set Gp = Gi.
 * 
 *  A formula we can use to end the parse early:
 *  Assume (Gi) <= |Gp|, then since |Gi| <= (Gi)  (totallogy),
 *  it is safe to assume |Gp| >= |Gk| where i >= k > 0 (remember,
 *  we're parsing G backwards from n to 1).
 * 
 *  Thus, when (Gi) <= |Gp|, |Gp| = max|Gi| and we're done.
 *  Worst case run time will be O((V*log(V))^2)/2), when all vertices
 *  have the same number of usable edges.
 */

/*  Maybe a better runtime would be achieved with a flat adjacency matrix
 *  using a binary search lookup method. That would wind up being O(2*log(V))
 *  for each lookup, and V
 */

polygon::polygon (AdjacencyMatrix& adj) { GeneratePolygon (adj); }


void polygon::GeneratePolygon(AdjacencyMatrix& adj) 
{	
	location::Vector Vp = adj.GetConvexPolygonWithMaxVertices();
	this->assign(Vp.begin(), Vp.end());
}


location polygon::NearestVertex (const location& loc) 
{	
	location dest(loc);
	int minDistance = INT_MAX;
	
	for (iterator index = begin(); index != end(); index ++) {
		if (L1_distance(*index, loc) < minDistance) {
			minDistance = L1_distance(*index, loc);
			dest = *index;
		}
	}
	
	return dest;
}


//Fast Winding Number Inclusion of a Point in a Polygon
//http://www.geometryalgorithms.com/Archive/algorithm_0103/algorithm_0103.htm#Monotone%20Polygons
//by Dan Sunday

//    a Point is defined by its coordinates {int x, y;}
//===================================================================
//
// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm "Area of 2D and 3D Triangles and Polygons"

inline int polygon::isLeft( location P0, location P1, location P2 ) 
{
	return ( (P1.x - P0.x) * (P2.y - P0.y)
			- (P2.x - P0.x) * (P1.y - P0.y) );
}


//===================================================================
//
// wn_PnPoly(): winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  wn = the winding number (=0 only if P is outside V[])

bool polygon::Contains( location pt ) 
{
	int wn = 0;    // the winding number counter

	if (pt.x < GetFirst().x) return false;
	if (pt.x > GetLast().x) return false;

	const_iterator ct = begin();	
	const_iterator nt = begin(); nt++;

	// loop through all edges of the polygon
	while (nt != end()) {   // edge from V[i] to V[i+1]
		if (ct->y <= pt.y) {         // start y <= P.y
			if (nt->y > pt.y)      // an upward crossing
				if (isLeft( *ct, *nt, pt ) > 0)  // P left of edge
					++wn;            // have a valid up intersect
		}
		else {                       // start y > P.y (no test needed)
			if (nt->y <= pt.y)     // a downward crossing
				if (isLeft( *ct, *nt, pt ) < 0)  // P right of edge
					--wn;            // have a valid down intersect
		}
		ct++;
		nt++;
	}

	return wn;
}	


