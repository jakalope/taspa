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

#include "AdjacencyMatrix.h"
#include "../std_extensions/set_operations.h"
#include <list>
#include <utility>
#include "../thorup/Graph.h"

#define MAX_TIMES_USED 4

#define DBG_LOC_ONE 114,387
#define DBG_LOC_TWO 268,262

#ifdef DEBUG
const location loc_one(DBG_LOC_ONE);
const location loc_two(DBG_LOC_TWO);
#endif

/** Calls #initialize# to build a matrix containing all 
	edge weights in G. Run time is inherently 
	$O(\sum |E_i| + V^2/2)$ -- the sum of undirected 
	edge weights in G plus the time to parse each vertex 
	to vertex combination. In short, for a dense graph, 
	this is a lengthy process.
	@param Graph A set of locations as vertices.
	@param bitmap A bitmap with edge weight function.
	@memo 
*/
AdjacencyMatrix::AdjacencyMatrix
(location::Set& G, region::List &vList, bitmap* bm, std::ostream& out) 
{
	initialize(G,vList,bm,out);
}


/** Builds a sub-matrix, including only those elements with 
	edges directly to 'head'.
	@param AdjacencyMatrix Matrix of edge and weight information.
	@param location Vertex whose edges we are interested in.
	@memo
*/
AdjacencyMatrix::AdjacencyMatrix
(AdjacencyMatrix& parent, location head) 
{	
    MIN_USABLE = parent.MIN_USABLE;
	bmp = 0;
	AdjacencyMatrix::iterator headRef = parent.find(head);
	
	// to do: throw catch all exception
	if (headRef == parent.end()) return;
	
	PolyEdgeMap::iterator iter = headRef->second.begin();
	PolyEdgeMap::iterator done = headRef->second.end();
	
	 while (iter != done) {
	 	if (iter->second.usable) 
			insert(*parent.find(iter->first));
		iter++;
	 }
}


AdjacencyMatrix::AdjacencyMatrix
(AdjacencyMatrix& orig) 
{	
    EdgeIter e = orig.GetEdgeIterator();
    for (e.ResetCol(); e.HasNextCol(); e.NextCol())
    if (e.ColIsConvex())
        for (e.ResetRow(); e.HasNextRow(); e.NextRow())
        if (e.RowIsConvex())
            (*this)[e.ColLoc()].insert(*e.Row());
}


//============================================================
// Constructor: Builds a sub-matrix, including only those elements with 
//		edges directly to 'head'.
//	Input:
//		AdjacencyMatrix Matrix of edge and weight information.
//		iterator Vertex whose edges we are interested in.
//
AdjacencyMatrix::AdjacencyMatrix(AdjacencyMatrix* parent, iterator headRef) 
{
    initializeSubmatrix(parent,headRef);
}


void AdjacencyMatrix::initializeSubmatrix
(AdjacencyMatrix* parent, iterator headRef)
{
    MIN_USABLE = parent->MIN_USABLE;
    
	// assert self-usability of head node.
	// if this fails, we would need to insert the head node explicitly
	// instead of assuming it will be adjacent to itself and therefor
	// inserted implicitly in our while loop.
	
    EdgeIter i = parent->GetEdgeIterator(headRef->first, headRef->first);
    i.SetUsable(true);

    // Get edges from vertex h
    EdgeIter hi = parent->GetEdgeIterator(headRef->first, headRef->first);

    // Set edge parent->hh.usable := true
    hi.SetUsable(true);

    // For all edges from h
    for (hi.ResetRow(); hi.HasNextRow(); hi.NextRow()) {

        // If hi is usable,
        if (hi.Usable()) {

        // For all edges from i
        EdgeIter ij = parent->GetEdgeIterator(hi.RowLoc());
        for (ij.ResetRow(); ij.HasNextRow(); ij.NextRow()) {
        
            // If edges ij and jh are usable,
            if (ij.Usable()) {    
            
            EdgeIter jh = parent->GetEdgeIterator(ij.RowLoc(), hi.ColLoc());
            if (jh.IsEdge() && jh.Usable()) {

            // Add edge sub->ij := parent->ij
            (*this)[ij.ColLoc()][ij.RowLoc()] = ij.Row()->second;
            }   }
        }   
        
        (*this)[ij.ColLoc()].usable = -1;
        (*this)[ij.ColLoc()].connected = -1;

        }
    }
}

/** Builds a matrix containing all edge weights in G. Run 
	time is inherently $O(\sum |E_i| + V^2/2)$ -- the sum 
	of undirected edge weights in G plus the time to parse 
	each vertex to vertex combination. In short, for a 
	dense graph, this is a lengthy process.
	@param Graph A set of locations as vertices.
	@param bitmap A bitmap with edge weight function.
	@memo 
*/
void AdjacencyMatrix::initialize
(location::Set& G, region::List &vList, bitmap* bm, std::ostream& out) 
{
    MIN_USABLE = MIN_USABLE_INIT;
	bmp = bm;
	ConnectedUnusable = 0;
	beginThorough = false;
	finished = false;

	// Continue initialization of the matrix
	location::SetIter v = G.begin();
	location::SetIter done = G.end();

    size_t G_size = G.size();
    size_t completed = 0;
	
	while (v != done) {
        
		/* Take advantage of Graph elements begin sorted and unique
		 * when G is an undirected graph.
		 * If a directed graph is needed, simply change 'v' to 'G.begin()'
		 * in the next line and comment out the specified line below */
		location::SetIter u = v;
		PolyEdgeMap m;
		
		while (u != done) {
			
			undirectedLength weight = GetWeight(*v, *u, bm);
			if (weight != INT_MAX) {
				PolyEdge p(true, false, weight);
				m[*u] = p;
			}
			u++;
		}
		
		(*this)[*v] = m;
//        Display(GetEdgeIterator(*v,*v),std::cerr);
		v++;
		completed ++;
        
        if (! (completed % 10)) {
            out << completed << " / " << G_size 
                << " vertices' edge weights found...                \r";
            out.flush();
        }
	}
	
    out << completed << " / " << G_size 
        << " vertices' edge weights found...                \r";
    out.flush();
	out << "\n";

	/* For a directed graph, comment this line and see instructions above. */
	AddTranspose();

	// Connect edges around impassible regions to ensure polygon construction
	// doesn't take advantage of diagonal side-stepping 'holes'.
	// Also, set the region that each vertex belongs to.
	int j = 0;
	for (region::List::iterator i = vList.begin(); i != vList.end(); i++) {
		ConnectEdges(i->toVector());
		SetRegion(*i, j);
		j ++;
	}
}


std::ostream& AdjacencyMatrix::Display(std::ostream& out) 
{    
    EdgeIter i = GetEdgeIterator();
    
    for (; i.HasNextCol(); i.NextCol()) {

        // Print vertices
        out << "\n" << i.ColLoc() << " ->";
        for (i.ResetRow(); i.HasNextRow(); i.NextRow())
            out << "\n   " << i.RowLoc() 
                << "\t" << i.Usable() 
                << "\t" << i.Connected();
                
        out << "\n";
        
    }
    
    return out;
}


std::ostream& AdjacencyMatrix::Display(EdgeIter i, std::ostream& out) 
{
    // Print vertices
    EdgeIter t = i;

    t.ResetRow();
    out << "\n" << t.ColLoc() << " -> " << t.RowLoc() 
        << "\tusbl\tcnctd";
    for (t.ResetRow(); t.HasNextRow(); t.NextRow())
        if (t.Usable()) {
            out << "\n   " << t.RowLoc() 
                << "\t " << (t.Usable() ? "u" : " ")
                << "\t " << (t.Connected() ? "c" : " ")
                << "\t " << t.Used();
        }
            
    out << "\n";        

    return out;
}


bool AdjacencyMatrix::CheckTranspose() {
    EdgeIter i = GetEdgeIterator();
    
    for (i.ResetCol(); i.HasNextCol(); i.NextCol()) 
    for (i.ResetRow(); i.HasNextRow(); i.NextRow()) {
        if (i.Transpose().IsEdge() == false) {
            return false;
    }   }
    
    return true;
}


void AdjacencyMatrix::RemoveDisconnected() 
{	
	EdgeIter i = GetEdgeIterator();
	
	while (i.HasNextCol()) {
		
		if (i.size() < 2) i.EraseCol();
		else i.NextCol();
		
	}
}


//===================================================================
// StillUsable(): Predicate defining when an edge is usable. a1-a2 is
// 		an edge in a new polygon perimeter and b1-b2 is an edge begin tested.
// Returns: True if edge a1-a2 either shares a point with edge b1-b2 or
// 		if a1-a2 does not interset b1-b2. Otherwise, false.
//
bool StillUsable(location a1, location a2, location b1, location b2) 
{
	// Look for double convergence with vertices in the polygon.
	// (this would imply a diagonal and therefor, removal.)
	//if (VectorDoesntHaveElement(Vp, a1)
	// If the two edges converge, its usability is unaffected by the polygon.
	if (Converges(a1,a2,b1,b2)) return true;
	// Otherwise, its inversely related to the edges intersecting.
	return !(Intersects(a1,a2,b1,b2));
}


//===================================================================
// DoesntClip(): Predicate to test clipping of an edge a1-a2 on a 
//		polygon P.
// Returns: True if a1-a2 doesn't intersect any edges in the perimeter 
// 		of P, otherwise, false.
//
bool AdjacencyMatrix::DoesntClip
(const location::Vector& Vp, location a1, location a2) 
{
	// If Vp holds 0 points, nothing intersects it.
	if (Vp.size() == 0) return true;
	
	// If Vp holds just 1 point, test a1-a2 against that point.
	if (Vp.size() == 1)
		return (StillUsable(a1,a2,Vp.front(),Vp.front()));


	// If a1-a2 is connected then any intersection with
	// elements in Vp implies not all necessary unusable
	// edges were marked as such previously. In other words:
	// If this fails, there is a problem with the edge removal code.
	assert(!IsConnected(a1,a2));

	location::ConstVectorIter p = Vp.begin();
	location::ConstVectorIter n = Vp.end();
	
	location p0 = *p;
	location p1 = *p; p ++;
	location p2 = *p;
	
	while (p != n) {
		p ++;

		// Check for intersections with polygon perimeter
		if (!StillUsable(a1,a2,p1,p2)) {
			return false;
		}
		p1 = p2;
		p2 = *p;
	}
	
	if (!StillUsable(a1,a2,p0,p1)) return false;

	return true;
}


//===================================================================
// AddTranspose(): Applies the transform A := A + A^T where 
// 		A begins upper triangular.
//
void AdjacencyMatrix::AddTranspose() 
{
	EdgeIter i = GetEdgeIterator();
	
	for (i.ResetCol(); i.HasNextCol(); i.NextCol())
	for (i.FindRow(i.ColLoc()); i.HasNextRow(); i.NextRow()) {			
		(*this)[i.RowLoc()][i.ColLoc()] = i.Row()->second;
	}

	i = GetEdgeIterator();
	
	for (i.ResetCol();i.HasNextCol();i.NextCol()) {
		i.SetRow(i.ColLoc());
		i.SetConnected(true);
		// to do: should i just use the standard convention that in an un-
		// directed graph, self-connectivity is false?
		i.Used() = MAX_TIMES_USED; 
	}		
}


//===================================================================
// RemoveExternalClipping(): Examines all edges in the graph represented by
// 		this matrix, removing all edges intersecting 'edges' from 
//		usability.
// Input:  location vector whose adjacent vertex combinations are
//		perimiter edges in a polygon.
//
void AdjacencyMatrix::RemoveExternalClipping 
(const location::Vector& Vp) 
{	
	EdgeIter thisEdge = GetEdgeIterator();
	
	while ( thisEdge.HasNextCol() ) {
		
		thisEdge.FindRow(thisEdge.ColLoc());
		thisEdge.NextRow();
		
		while ( thisEdge.HasNextRow() ) {
			
			
			// Unusable implies unconnected <=> Usable or connected 
			// <=> If unusable then unconnected.
			//assert(L1P2->second.usable || L1P2->second.connected == false);

			// Check edge for instersections with perimeter of polygon.
			// Since all polygon edges are now connected,
			// we are only looking for non-Vp edges to test against Vp edges.
			// We use DoesntClip because edges with a vertex shared by
			// the polygon Vp should remain usable.

//			location head = Vp.front();
			if (thisEdge.Usable() && !thisEdge.Connected()) {
				thisEdge.SetUsableBidirectional( 
					DoesntClip(Vp, thisEdge.ColLoc(), thisEdge.RowLoc()));
				//thisEdge.Transpose().SetUsable(thisEdge.Usable());
			}
			 
			//L1P2++; 
			thisEdge.NextRow();
		} 
		
		thisEdge.NextCol(); 
	}
	
}


//===================================================================
// RemoveDiagonals(): Removes edges in polygon P that are not part of
//		of the perimeter of P.
//
void AdjacencyMatrix::RemoveDiagonals
(const location::Vector& Vp) 
{	
	// For triangles, lines, and points, there are no internal clips.
	if (Vp.size() < 4) return;
	
    for (size_t pivot = 1; pivot < Vp.size()+2; pivot ++) 
    for (size_t scan  = 1; scan  < Vp.size()+1; scan  ++) {
        
        size_t a = (pivot-1)%Vp.size();
        size_t b = (scan -1)%Vp.size();

        if (abs(a-b) > 1) {
            EdgeIter i = GetEdgeIterator(Vp[a],Vp[b]);
            if (!i.Connected()) i.SetUsableBidirectional(false);
        }
    
    }
}	


/** Removes all edges which intersect the perimeter of polygon P. 
	@param $V_p$	A vector of vertices in P.
	@memo
*/
void AdjacencyMatrix::RemoveClipping(const location::Vector& Vp) 
{	
//    assert(CheckTranspose());
	RemoveDiagonals(Vp);
	RemoveExternalClipping(Vp);
}


bool AdjacencyMatrix::ZeroWeightRemainsUsable() 
{	
	for (iterator headRef = begin(); headRef != end(); headRef ++)
		if (headRef->second.find(headRef->first)->second.usable == false) 
			return false;
	return true;
}


void AdjacencyMatrix::SetZeroWeightToUsable() 
{    
    EdgeIter i = GetEdgeIterator();
    
    for (i.ResetCol(); i.HasNextCol(); i.NextCol()) {
        i.SetRow(i.ColLoc());
        i.SetUsable(true);
        i.SetConnected(true);
    }
}


/** Connects edges of the perimeter of polygon P.
	@param $V_p$	A vector of vertices in P.
	@memo
*/
bool AdjacencyMatrix::ConnectEdges
(const location::Vector& Vp) 
{
	if (Vp.size() == 0) return false;

	location::ConstVectorIter v1 = Vp.begin(); 			// Let v1 := V_1
	location::ConstVectorIter v2 = Vp.begin(); v2 ++;	// Let v2 := V_2
	location::ConstVectorIter vn = Vp.end();			// Let vn := V_n

    EdgeIter f = GetEdgeIterator(*v1, Vp.back());
    if (f.IsEdge() && f.Usable()) {
        f.SetConnectedBidirectional(true); // Connect vi and vi+1
        f.IncTimesUsed();
        f.Transpose().IncTimesUsed();
        if (f.TimesUsed() > MAX_TIMES_USED && *v1 != Vp.back()) {
            f.SetUsableBidirectional(false);
        }
    }
    
    else return false;
    
	// in the special case where v2 == vn already, Vp is a single edge (2 verts)
	while (v2 != vn) {
		EdgeIter f = GetEdgeIterator(*v1, *v2);
        if (f.IsEdge() && f.Usable()) {
            f.SetConnectedBidirectional(true); // Connect vi and vi+1
            f.IncTimesUsed();
            f.Transpose().IncTimesUsed();
            if (f.TimesUsed() > MAX_TIMES_USED) {
                f.SetUsableBidirectional(false);
            }
        }
        
        else return false;

		v2++; v1++;
	}
	
	return true;
}


void AdjacencyMatrix::SetRegion (region& Vp, int j) 
{
	location::ConstVectorIter v = Vp.toVector().begin(); 	// Let v1 := V_1
	location::ConstVectorIter vn = Vp.toVector().end();		// Let vn := V_n
		
	int k = 0;
	while (v != vn) {
		iterator a = find(*v);
		if (a != end()) {
			a->second.region = j;
			a->second.pos = k;
		}
		
		k++;
		v++;
	}
}


//===================================================================
// PolygonEdgesRemainUsable():
//
bool AdjacencyMatrix::PolygonEdgesRemainUsable
(const location::Vector& Vp) 
{	
	location::ConstVectorIter v1 = Vp.begin(); 			// Let v1 := V_1
	location::ConstVectorIter v2 = Vp.begin(); v2 ++;	// Let v2 := V_2
	location::ConstVectorIter vn = Vp.end();			// Let vn := V_n

	if (!IsUsable(*v1, Vp.back())) return false;	// Check vi and vi+1

	while (v2 != vn) {
		if (!IsUsable(*v1, *v2)) return false;	// Check vi and vi+1
		v2++; v1++;
	}
	
	return true;
}


//===================================================================
// GetVertexWithFewestEdges(): Retrieves the vertex in this matrix
//		with the fewest edges (usable or otherwise). This is mostly
//		useful when used on a submatrix, which is created without
//		any unusable edges, for the purpose of distilling the sub-
//		matrix down to a convex polygon's vertices.
// Returns: an iterator to the vertex in the matrix with the fewest
//		edges; stores the number of edges in minSize.
//
AdjacencyMatrix::iterator 
AdjacencyMatrix::GetVertexWithFewestEdges (size_t& minSize) 
{	
	AdjacencyMatrix::iterator minIter = this->begin();
	AdjacencyMatrix::iterator done = this->end();
	AdjacencyMatrix::iterator minEdges = this->begin();

	size_t previousMinSize = INT_MAX;
	
	while (minIter != done) {

		if (previousMinSize > minIter->second.size()) {
			previousMinSize = minIter->second.size();
			minEdges = minIter;
		}

		minIter++;
	}

	minSize = previousMinSize;
	return minEdges;
}


//===================================================================
// GetVertexWithMinUsableEdges(): 
//
location AdjacencyMatrix::GetVertexWithMinUsableEdges
(size_t& minSize) 
{	
    /*  if we're dealing with the last remaining triangles to be made,
        we want to look for the other vertex of the unused edge from 
        the head vertex. we then want the two edges whose vertices form 
        the other two sides of the triangle */
    
	// if two vertices are tied for min usable, we want the one
	// (a) which has a greater number of connected edges.
	
	EdgeIter minEdges         = GetEdgeIterator();
//	EdgeIter minEdges_notHead = GetEdgeIterator();
	EdgeIter index            = GetEdgeIterator();

	minSize = INT_MAX;
	size_t minsConnected = INT_MAX;
	size_t minsUnused = INT_MAX;

	for (index.ResetCol(); index.HasNextCol(); index.NextCol()) {

		size_t connectedCount = index.ConnectedEdgeCount();
		size_t usableCount = index.UsableEdgeCount();
		size_t unusedCount = index.UnusedEdgeCount();
		
		bool hasFewerUsable = minSize > usableCount;
		bool hasSameUsable = minSize == usableCount;
//		bool hasMoreConnected = minsConnected < connectedCount;
		bool hasFewerUnused = minsUnused < unusedCount;
		

		// if tie in usable count or
		// if this one has fewer usable than current min
        if (((hasSameUsable && hasFewerUnused) ||
                hasFewerUsable) &&
                usableCount > MIN_USABLE) {// &&
                //usableCount >= connectedCount) {
			
            minsUnused = unusedCount;
			minsConnected = connectedCount;
			minSize  = usableCount;
			minEdges = index;
		}
	}

	minSize = minEdges.UsableEdgeCount();
	return minEdges.ColLoc();
}


//===================================================================
// GetVertexWithMaxUsableEdges(): 
//
location AdjacencyMatrix::GetVertexWithMaxUsableEdges(size_t& maxSize) 
{	
	// if two vertices are tied for max usable, we want the one
	// (a) which has not been the head vertex before, and
	// (b) which has fewer connected edges, and
	// (c) which doesn't have edges used more than once.
	
	EdgeIter maxEdges         = GetEdgeIterator();
	EdgeIter maxEdges_notHead = GetEdgeIterator();
	EdgeIter index            = GetEdgeIterator();

	size_t previousMaxSize = 0;
	size_t previousMaxSize_notHead = 0;
	size_t previousConnected = ULONG_MAX;
	long previousScore = -1;
	size_t previousUnusedCount = 0;
	
	for (index.ResetCol(); index.HasNextCol(); index.NextCol()) 
	{
        long score = 0;
		size_t connectedCount = index.ConnectedEdgeCount();
		size_t usableCount = index.UsableEdgeCount();
		size_t unusedCount = index.UnusedEdgeCount();
		
		score = usableCount - connectedCount;// - unusedCount;
		
		bool hasMoreUsable_notHead = previousMaxSize_notHead < usableCount;
		bool hasntBeenHead = !index.WasHead();

        // if u - c = 0 and u = 3, only a single polygon can be made
        // from this vertex, because its a point of maximum obstacle
        // concavity and it's been used to connect the edges of that
        // obstacle.
		if (index.size() == 3 && usableCount - connectedCount == 0)
            score = -1;
		
        if (score > previousScore) {
			previousMaxSize = usableCount;
			previousConnected = connectedCount;
			maxEdges = index;
            previousScore = score;
            previousUnusedCount = unusedCount;
		}
		
		if (hasntBeenHead && hasMoreUsable_notHead) {
			previousMaxSize_notHead = usableCount;
			maxEdges_notHead = index;
		}
		
	}
	
	if (previousScore == 1) beginThorough = true;

	if (previousMaxSize_notHead != 0) {
		maxSize = previousMaxSize_notHead;
		maxEdges = maxEdges_notHead;
	}
	else {
		maxSize = previousMaxSize;
	}
	
	return maxEdges.ColLoc();
}


size_t AdjacencyMatrix::TotalEdgeCount() 
{
	int count = 0;
	EdgeIter i = GetEdgeIterator();
	
	for (i.ResetCol(); i.HasNextCol(); i.NextCol())		
		count += i.size();
	
	return count;
}


size_t AdjacencyMatrix::EdgeCountInSet
(const location::Vector& V) 
{
	int count = 0;
	location::ConstVectorIter i = V.begin();
	
	while (i != V.end()) {
		
		location::ConstVectorIter j = V.begin();
		
		while (j != V.end()) {
			if (*i != *j) count += (GetEdgeIterator(*i,*j).IsEdge()) ? 1 : 0;
			j ++;
		}
		
		i ++;
	}
	
	return count;
}


//===================================================================
// EdgeCount:
//		
size_t AdjacencyMatrix::EdgeCount(location loc) {	
	return GetEdgeIterator(loc).size();
}


//===================================================================
// UsableCount:
//		
int AdjacencyMatrix::UsableCount(location loc) {	
	return GetEdgeIterator(loc).UsableEdgeCount();
}


//===================================================================
// UsableEdges: Returns a vector of locations to which 'loc' has
// 		a usable edge.
//		
location::List AdjacencyMatrix::UsableEdges(location loc) {
    return GetEdgeIterator(loc).UsableEdges();
}


//===================================================================
// ConnectedCount:
//		
int AdjacencyMatrix::ConnectedCount(location loc) {
	return GetEdgeIterator(loc).ConnectedEdgeCount();
}


//===================================================================
// UnconnectedUsableCount:
//		
int AdjacencyMatrix::UnconnectedUsableCount(location loc) 
{	
	int connected = ConnectedCount(loc);
	int usable = UsableCount(loc);
	
	if (usable - connected == 1) {
		return 0;
	}
	
	return usable - connected;
}


//===================================================================
// EraseVertex: Erases a single vertex from this matrix by
//		both eliminating references to it and eliminating
//		the branch itself.
//
void AdjacencyMatrix::EraseVertex(location loc) 
{	
	EdgeIter i = GetEdgeIterator();
	
	while (i.HasNextCol()) {
		
		bool edgeIsSet = i.SetRow(loc);
		if (edgeIsSet) i.EraseEdge();
		i.NextCol();
	}
	
	bool colIsSet = i.SetCol(loc);
	if (colIsSet) i.EraseCol();
}


void AdjacencyMatrix::SetUnusable(location loc) 
{	
	EdgeIter i = GetEdgeIterator(loc);
	
    for (i.ResetRow(); i.HasNextRow(); i.NextRow()) {
        i.SetUsableBidirectional(false);
    }
}


void AdjacencyMatrix::EraseUnusable() {
	
	typedef PolyEdgeMap::iterator PemIterator;
	
	for (iterator row = begin(); row != end(); row++)	{

		for (PemIterator col = row->second.begin(); 
			col != row->second.end(); col++)
			
			if (col->second.usable == false || find(col->first) == end())
				row->second.erase(col++);
		
	}
}


location::Vector 
AdjacencyMatrix::QuickGuessForMaximumPolygon 
(bool& wasHeadBefore) 
{
    SetZeroWeightToUsable();
    
	// Get vertex with maximum usable edge count
	size_t edgesOfMax;
	location::Vector Empty;
	EdgeIter parentsMaxEdges = 
		GetEdgeIterator(GetVertexWithMaxUsableEdges(edgesOfMax));
		
    if (beginThorough == true) return Empty;

	wasHeadBefore = parentsMaxEdges.WasHead();
	parentsMaxEdges.WasHead() = true;

	// Remove unusable and inf. weight edges
	AdjacencyMatrix subMatrix(this, parentsMaxEdges.Col());
	
	// Determine which vertex will be first to go
	size_t edgesOfMin=0;
	location minUsable = subMatrix.GetVertexWithMinUsableEdges(edgesOfMin);
	
	// Reset headIter to the same vertex but in the subMatrix 
	// instead of its parent.
	EdgeIter headIter = subMatrix.GetEdgeIterator(parentsMaxEdges.ColLoc());
	edgesOfMax = headIter.UsableEdgeCount();
	
	// Reduce to a convex polygon
	while (edgesOfMin != edgesOfMax && edgesOfMax > MIN_USABLE) {
		
		// When debugging, be sure we haven't messed up code regarding 
		// the calculation of edgesOfMin or edgesOfMax.
		assert(edgesOfMin < edgesOfMax);
		
		// Erase a vertex which would cause our polygon to have a concavity.
		//subMatrix.EraseVertex(minUsable);
		subMatrix.SetUnusable(minUsable);
		
		// Get a new minimum
		minUsable = subMatrix.GetVertexWithMinUsableEdges(edgesOfMin);
        //assert(headIter.ColLoc() != minUsable);
		
		// Account for the vertex removed.
		edgesOfMax = headIter.UsableEdgeCount(); //--;

	}
	
//	EdgeIter min = GetEdgeIterator(minUsable);
	location::List polygonVertices(headIter.UsableEdges());
		
	//assert(minUsable.UsableEdges() == headIter.UsableEdges());
	OrderEdgesByAngle(polygonVertices);
	
	// If the last element in polygonVertices is not in the submatrix,
	// fail the assert...
	location::Vector Vp(polygonVertices.begin(), polygonVertices.end());
	
	return Vp;
}


location::Vector 
AdjacencyMatrix::GetFristUsableTriangle()
{
    location::Vector V;
    
    EdgeIter i = GetEdgeIterator();
    for (i.ResetCol(); i.HasNextCol(); i.NextCol()) {
        if (i.UsableEdgeCount() > 3 &&
            i.ConnectedEdgeCount() < i.UsableEdgeCount()) {
        
        /* find a usable triangle from this vertex:
            Let i.RowLoc() := the other vertex from the first unused edge
            Let t = i.ColLoc()
            Let u = i.RowLoc()
            Let v = some vertex with usable edge from from t and u

            if no such vertex v exists, try another unused edge
            if no more unused edges exist, try the next vertex
        */

        EdgeIter t = i; t.ResetRow();
        
        location::Vector t_unused = t.UnusedRowLocs();
        location::List t_usable = t.UsableEdges();
        location::Set t_used;
        
        std::set_difference(
            t_usable.begin(), t_usable.end(),
            t_unused.begin(), t_unused.end(),
            std::inserter(t_used, t_used.end()));
            
        location::VectorIter tu = t_unused.begin(); 
        location::Set unused;
        
        while (tu != t_unused.end() && unused.size() == 0) {
        
            EdgeIter u = GetEdgeIterator(*tu);
            
            location::List u_usable = u.UsableEdges();
            
            std::set_intersection(
                    t_used.begin(), t_used.end(), 
                    u_usable.begin(), u_usable.end(),
                    std::inserter(unused, unused.end()) );
            
            if (unused.size() == 0) tu ++;
            else {
                for (location::Set::reverse_iterator uu = unused.rbegin();
                        uu != unused.rend(); uu ++) {
                    V.push_back(i.ColLoc());
                    V.push_back(*tu);
                    V.push_back(*uu);
                    
                    if (IsColinear(V)) V.clear();
                    else return V;
                }
                tu++;
            }
        }
        
    }   }
    
    return V;
}


location::Vector 
AdjacencyMatrix::IncrimentalGuessForMaximumPolygon() 
{
    location::Vector V;
    V = GetFristUsableTriangle();
    if (V.size() != 3) V.clear();
    return V;
}


location::Vector 
AdjacencyMatrix::GetConvexPolygonWithMaxVertices() 
{
	location::Vector Vp;
	
	if (finished) return Vp;

    bool wasHeadBefore = false;
    while (!wasHeadBefore && 
        Vp.size() < MIN_USABLE + 1 && 
        beginThorough == false)
            Vp = QuickGuessForMaximumPolygon(wasHeadBefore);

    if (beginThorough) {
        Vp = IncrimentalGuessForMaximumPolygon();
    }
    
	if (ConnectEdges(Vp) == false) {
		Vp.clear();
		return Vp;
	}
	
	RemoveClipping(Vp);
	
	RemoveRegionConcavities(Vp);
	
	// Return edges of new polygon in a graph
	return Vp;
}


void AdjacencyMatrix::RemoveRegionConcavities
(location::Vector& Vp) 
{
    // If three elements have [the same region and consecutive positions]
    // in the column space of this matrix, erase the middle-positioned
    // element.
    
    if (Vp.size() < 3) return;

    location::Vector toErase;
    
    for (size_t i=0;   i < Vp.size()-2; i++)
    for (size_t j=i+1; j < Vp.size()-1; j++)
    for (size_t k=j+1; k < Vp.size()-0; k++) {
    
        iterator u = find(Vp[i]);
        iterator v = find(Vp[j]);
        iterator w = find(Vp[k]);
        
        int r_u = u->second.region;
        int r_v = v->second.region;
        int r_w = w->second.region;
        
        int p_u = u->second.pos;
        int p_v = v->second.pos;
        int p_w = w->second.pos;
        
        // to do: What if pos 0 and pos n on the same region?
    
        if ((r_u == r_v) && (r_u == r_w)) {
        
        int x = (p_u + p_v + p_w)/3;
        size_t center = INT_MAX;

        if      (x == p_u) center = i;
        else if (x == p_v) center = j;
        else if (x == p_w) center = k;
        
        bool x_in_set = center != INT_MAX;
        
        x --;
        bool x_minus_one_in_set = x == p_u || x == p_v || x == p_w;
        
        x += 2;
        bool x_plus_one_in_set = x == p_u || x == p_v || x == p_w;
        
        if (x_in_set && x_minus_one_in_set && x_plus_one_in_set)
            toErase.push_back(Vp[center]);
        
    }   }

    for (size_t i = 0; i < toErase.size(); i ++)
        EraseVertex(toErase[i]);
        
}


bool AdjacencyMatrix::HasColinearElements
(location::Vector& V)
{
    for (size_t i = 0; i < V.size(); i ++)
    {
        location::Vector v;
        v.push_back(V[i]);
        v.push_back(V[(i+1)%V.size()]);
        v.push_back(V[(i+2)%V.size()]);
        if (IsColinear(v)) return false;
    }
    
    return true;
}


AdjacencyMatrix::Edge 
AdjacencyMatrix::GetLongestLine (location::Vector& V) 
{
	location prev = V.front();
	int longestLineLength = prev.L2scaled(V.back());
	Edge longestLine(V.front(), V.back());
	
	for (size_t i = 1; i < V.size(); i ++) {
		int lineLength = V[i].L2scaled(prev);
		if (longestLineLength < lineLength) {
			longestLine.one = prev;
			longestLine.two = V[i];
			longestLineLength = lineLength;
		}
		prev = V[i];
	}
	
	return longestLine;
}


bool AdjacencyMatrix::IsColinear(location::Vector& V) 
{	
	if (V.size() != 3) return false;
	
	bool isColinear = 
		Contains (bmp->GetLine(V[0], V[1]), V[2]) ||
		Contains (bmp->GetLine(V[1], V[2]), V[0]) ||
		Contains (bmp->GetLine(V[2], V[0]), V[1]);
		
	return isColinear;
}


location AdjacencyMatrix::OrderEdgesByAngle(location::List& Vp) 
{	
	// Let Ov be the origin of the plot of the subgraph whose vertices are V 
	// and whose edges are the line segments between between each vertex.
	
	location Ov(0,0);
	location min(INT_MAX,INT_MAX);
	location max(0,0);
	
	// Precondition: this->size() > 0
	if (Vp.size() == 0) return Ov;

	for (location::ConstListIter p=Vp.begin(); p != Vp.end(); p ++) {
		if (max.x < p->x) max.x = p->x;
		if (max.y < p->y) max.y = p->y;
		if (min.x > p->x) min.x = p->x;
		if (min.y > p->y) min.y = p->y;
	}
		
	Ov.x = (max.x + min.x)/2;
	Ov.y = (max.y + min.y)/2;
	
	// Assign angles to each vertex from the positive x-axis, counterclockwise
	// such that any vertex directly to the right of Ov will have an angle
	// of zero radians and any vertex directly to the left will have an
	// angle of pi radians.
	std::map<math_vector,location> orderByAngle;
	typedef std::pair<math_vector,location> vectorPair;
	typedef std::map<math_vector,location>::iterator vectorIter;

	for (location::ConstListIter p=Vp.begin(); p != Vp.end(); p ++) {
		math_vector mv(Ov,*p);
		vectorPair vp(mv,*p);
		orderByAngle.insert(vp);
	}
	
	// Parse from least to greatest angle, adding to Vp.
	Vp.clear();
	for (vectorIter p=orderByAngle.begin(); p != orderByAngle.end(); p ++) {
		Vp.push_back(p->second);
	}

	return Ov;
}


bool AdjacencyMatrix::HasUsable() 
{	
	EdgeIter i = GetEdgeIterator();
	
	for (i.ResetCol(); i.HasNextCol(); i.NextCol()) {
		if (i.UsableEdgeCount() > MIN_USABLE) return true;
	}
	
	return false;	
}


location::LineVector AdjacencyMatrix::GetRemoved() 
{
	location::LineVector lv;

	for (iterator row = begin(); row != end(); row++) {
	for (PolyEdgeMap::iterator col = row->second.find(row->first);
			col != row->second.end(); col ++) {
		
		if (col->second.usable == false) 
			lv.push_back(std::make_pair(row->first,col->first));
			
	}	}
	
	return lv;
}


location::LineVector AdjacencyMatrix::GetUsable() 
{
	location::LineVector lv;

	for (iterator row = begin(); row != end(); row++) {
	for (PolyEdgeMap::iterator col = row->second.find(row->first);
			col != row->second.end(); col ++) {
		
		if (col->second.usable == true) 
			lv.push_back(std::make_pair(row->first,col->first));
			
	}	}
	
	return lv;
}
