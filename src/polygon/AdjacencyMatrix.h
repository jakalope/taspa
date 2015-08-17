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

#ifndef ADJACENCYMATRIX_H
#define ADJACENCYMATRIX_H

#include <assert.h>
#include <map>
#include <limits.h>
#include <algorithm>
#include "../bitmap/bitmap.h"
#include "../location/location.hpp"
#include "math_vector.hpp"
#include "../region/region.h"

#define MIN_USABLE_INIT 1

struct PolyEdge {

	PolyEdge() : usable(false), connected(false), used(0), weight(INT_MAX) { }
	PolyEdge(bool _u, bool _c, int _w) : 
		usable(_u), connected(_c), used(0), weight(_w) { }

	bool usable    ;//: 1;      // Is available as polygon edge
	bool connected ;//: 1;		// Has been used as a polygonal edge
	int used      ;//: 4;       // Number of times this edge has been used
	undirectedLength weight;       		    // Distance between vertices (INT_MAX for N/A)
};

typedef std::pair<location,PolyEdge> PolyEdgePair;
class  PolyEdgeMap : public std::map <location,PolyEdge> {
	public: bool wasHead; int region; int pos; int usable; int connected;
        int unused;
	PolyEdgeMap() : 
        wasHead(false), region(-1), pos(-1), usable(-1), connected(-1), 
        unused(-1) { }
	PolyEdgeMap(int _region, int _pos) : 
		wasHead(false), region(_region), pos(_pos), usable(-1), connected(-1), 
		unused(-1) { }
};
typedef std::pair<location,PolyEdgeMap> AdjacencyPair;

////////////////////////////////////////////////////////////////////////////////
/** Undirected adjacency matrix designed to aide building polygons on the 
	Z^2 plane.
	
	@version 0.1.0 Build 0
	@author J. Askeland
	@memo
*/
class AdjacencyMatrix : public std::map<location,PolyEdgeMap> {
	private:
	
	int ConnectedUnusable;
	bool beginThorough;
	bool finished;
    size_t MIN_USABLE;
	bitmap* bmp;
	
	protected:
	
	struct Edge {
		
		Edge(location _one, location _two) : one(_one), two(_two) { }
		
		bool notSelf() { return one != two; }
		
		location one;
		location two;
		bool usable;
		bool connected;
		
	};
	
	// AddTranspose(): Applies the transform A := A + A^T where 
	// 		A is upper triangular.
	void AddTranspose();
	
	// DoesntClip(): Predicate to test clipping of an edge a1-a2 on a 
	//		polygon P.
	// Returns: True if a1-a2 doesn't intersect any edges in the perimeter 
	// 		of P, otherwise, false.
	bool DoesntClip(
			const location::Vector& Vp, location a1, location a2);

	// RemoveExternalClipping(): Examines all edges in the graph represented by
	// 		this matrix, removing all edges intersecting 'edges' from 
	//		usability.
	// Input:  location vector whose adjacent vertex combinations are
	//		perimiter edges in a polygon.
	void RemoveExternalClipping (const location::Vector& Vp);

	// RemoveDiagonals(): Removes edges in polygon P that are not part of
	//		of the perimeter of P.
	void RemoveDiagonals(const location::Vector& Vp);

	// RemoveInternalClipping(): Removes edges from the vertices 
	// 		in polygon P which are not in the perimeter of P but which
	//		intersect the perimeter of P.
	void RemoveInternalClipping(const location::Vector& Vp);

	// ConnectEdges(): Connects edges of the perimeter of polygon P.
	bool ConnectEdges (const location::Vector& Vp);

	void SetRegion (region& Vp, int j);

	// PolygonEdgesRemainUsable(): To assert proper removal of edges.
	bool PolygonEdgesRemainUsable(const location::Vector& Vp);

    bool CheckTranspose();
	bool ZeroWeightRemainsUsable();
	void SetZeroWeightToUsable();

	// GetVertexWithFewestEdges(): Retrieves the vertex in this matrix
	//		with the fewest edges (usable or otherwise). This is mostly
	//		useful when used on a submatrix, which is created without
	//		any unusable edges, for the purpose of distilling the sub-
	//		matrix down to a convex polygon's vertices.
	// Returns: an iterator to the vertex in the matrix with the fewest
	//		edges; stores the number of edges in minSize.
	iterator GetVertexWithFewestEdges(size_t& minSize);
	
	// GetVertexWithMinUsableEdges():
	location GetVertexWithMinUsableEdges(size_t& minSize);

	// GetVertexWithMaxUsableEdges(): 
	location GetVertexWithMaxUsableEdges(size_t& maxSize);

	// EdgeCount:
	size_t EdgeCount(location loc);

	// UsableCount:
	int UsableCount(location loc);

	// ConnectedCount:
	int ConnectedCount(location loc);

	// UnconnectedUsableCount:
	int UnconnectedUsableCount(location loc);

	// UsableEdges:
	location::List UsableEdges(location loc);
	
	// EraseVertex: Erases a single vertex from this matrix by
	//		both eliminating references to it and eliminating
	//		the branch itself.
	void EraseVertex(location loc);

    void SetUnusable(location loc);

	// EraseUnusable:
	void EraseUnusable();

    void RemoveRegionConcavities(location::Vector& Vp);

	location::Vector QuickGuessForMaximumPolygon(bool& wasHeadBefore);
    location::Vector IncrimentalGuessForMaximumPolygon();
    location::Vector GetFristUsableTriangle();

	Edge GetLongestLine(location::Vector& V);

	location OrderEdgesByAngle(location::List& Vp);
	
	// Constructor: Builds a sub-matrix, including only those elements with 
	//		edges directly to 'head'.
	//	Input:
	//		AdjacencyMatrix Matrix of edge and weight information.
	//		iterator Vertex whose edges we are interested in.
	AdjacencyMatrix(AdjacencyMatrix* parent, iterator headRef);

    void initializeSubmatrix(AdjacencyMatrix* parent, iterator headRef);
    
	public:

	/////////////////////////////////////////////////////
	/** @name Constructors **/
	//@{

	// Allocates an empty matrix.
	AdjacencyMatrix() : 
		ConnectedUnusable(0), beginThorough(false), finished(false), 
		MIN_USABLE(MIN_USABLE_INIT), bmp(0) {  }
	
	// Builds a matrix containing all edge weights in G.
	AdjacencyMatrix
        (location::Set& G, region::List &vList, bitmap* bm, std::ostream& out);
	
    AdjacencyMatrix(AdjacencyMatrix& orig);

	// Builds a sub-matrix, including only those elements with 
	// edges directly to 'head'.
	AdjacencyMatrix(AdjacencyMatrix& parent, location head);

	//@}


	/////////////////////////////////////////////////////
	/** @name Public Types **/
	//@{
	
	class EdgeIter {
		
		private:
		
		bool initialized;
		AdjacencyMatrix* mat;
		
		iterator col;
		PolyEdgeMap::iterator row;
		
		public:
		
		EdgeIter(AdjacencyMatrix* _mat) : 
			initialized(true), mat(_mat), 
			col(mat->begin()), row(col->second.begin()) { }
		
		EdgeIter(location _col, AdjacencyMatrix* _mat) {
			initialized = false;
			mat = _mat;
			col = mat->find(_col);
			if (col != mat->end()) {
				row = col->second.begin();
				initialized = true;
			}
//            #ifdef DEBUG
//            if (not IsEdge()) {
//                std::cerr << "Not a vertex: " << _col << "\n";
//            }
//            #endif
		}
	
		EdgeIter(location _col, location _row, AdjacencyMatrix* _mat) {
			initialized = false;
			mat = _mat;
			col = mat->find(_col);
			if (col != mat->end()) {
				row = col->second.find(_row);
				if (row != col->second.end())
					initialized = true;
			}
//            #ifdef DEBUG
//            if (not IsEdge()) {
//                std::cerr << "Not an edge: " << _col << ", " << _row << "\n";
//            }
//            #endif
		}
		
		bool IsEdge() const { return initialized; }

        bool IsEdge(int a0, int a1, int b0, int b1) {
            return (col->first.x == a0 &&
                    col->first.y == a1 &&
                    row->first.x == b0 &&
                    row->first.y == b1);
        }

        void SetUsable(bool _usable) { 
            #ifdef DEBUG
            // to do:
            // Why isn't this edge ever being set to unusable but
            // not showing up as a usable edge after polygon partitioning?
//            if (ColLoc().x == 89 && ColLoc().y == 107) {//&&
//                //RowLoc().x == 89) {// && RowLoc().y == 70) {
//                int k = 0;
//            }
            if (!IsEdge()) {
                std::cerr << "Not an edge!\n";
                assert(false);
            }
            #endif
            UsableEdgeCount();
            col->second.usable += (_usable - row->second.usable);
            row->second.usable = _usable;
        }

        void SetConnected(bool _connected) { 
            #ifdef DEBUG
            if (!IsEdge()) {
                std::cerr << "Not an edge!\n";
                assert(false);
            }
            #endif
            ConnectedEdgeCount();
            col->second.connected += (_connected - row->second.connected);
            row->second.connected = _connected; 
        }

        void SetUsableBidirectional(bool _usable) { 
            SetUsable(_usable);
            Transpose().SetUsable(_usable);
        }

        void SetConnectedBidirectional(bool _connected) { 
            SetConnected(_connected);
            Transpose().SetConnected(_connected);
        }

		const bool& Usable() const { return row->second.usable; }
		const bool& Connected() const { return row->second.connected; }
        const int& TimesUsed() const { return row->second.used; }
        void IncTimesUsed() { row->second.used++; }
		int& Used() const { return row->second.used; }
		undirectedLength& Weight() const { return row->second.weight; }
		bool& WasHead() const { return col->second.wasHead; }
		int& Region() const { return col->second.region; }
		int& Position() const { return col->second.pos; }
		
		iterator Col() const { return col; }
		PolyEdgeMap::iterator Row() const { return row; }
		
		void ResetCol() { col = mat->begin(); }
		void ResetRow() { row = col->second.begin(); }
		
		bool HasNextCol() const 
			{ return (initialized) ? col != mat->end()        : false; }
		bool HasNextRow() const 
			{ return (initialized) ? row != col->second.end() : false; }
			
		void NextCol()	{ col++; ResetRow(); } 
		void NextRow()	{ row++; } 
						
		bool SetCol(location loc) {
			if (initialized) { 
				col = mat->find(loc); 
                ResetRow();
				return (col != mat->end()); 
			}
                    
			return false;
		}
		
		bool SetRow(location loc) {
			if (initialized) { 
				row = col->second.find(loc); 
				return (row != col->second.end()); 
			} 
			
			return false;
		}
		
		size_t size() const { return (initialized) ? col->second.size() : 0; }
		
		size_t UsableEdgeCount() {
            if (col->second.usable == -1) {
                col->second.usable = 0;
                PolyEdgeMap::iterator _row = row;
                for (ResetRow();HasNextRow();NextRow()) 
                    col->second.usable += ((Usable()) ? 1 : 0);
                row = _row;
            }
            return col->second.usable;
		}
		
		size_t UnusedEdgeCount() {
            if (col->second.unused == -1) {
                col->second.unused = 0;
                PolyEdgeMap::iterator _row = row;
                for (ResetRow();HasNextRow();NextRow()) 
                    col->second.unused += ((Used() == 0) ? 1 : 0);
                row = _row;
            }
            return col->second.unused;
		}
		
		location::Vector UnusedRowLocs() {
			location::Vector unusedRowLocs;
			if (!IsEdge()) return unusedRowLocs;
            PolyEdgeMap::iterator _row = row;
			for (ResetRow(); HasNextRow(); NextRow()) {
				if (Usable() && Used() == 0) unusedRowLocs.push_back(RowLoc());
			}
            row = _row;
			return unusedRowLocs;
		}
		
		location::List UsableEdges() {
			location::List usableEdgeList;
			if (!IsEdge()) return usableEdgeList;
            PolyEdgeMap::iterator _row = row;
			for (ResetRow(); HasNextRow(); NextRow()) {
				if (Usable()) usableEdgeList.push_back(RowLoc());
			}
            row = _row;
			return usableEdgeList;
		}
		
		size_t ConnectedEdgeCount() {
            if (col->second.connected == -1) {
                col->second.connected = 0;
                PolyEdgeMap::iterator _row = row;
                for (ResetRow();HasNextRow();NextRow()) 
                    col->second.connected += (Connected() ? 1 : 0);
                row = _row;
            }
			return col->second.connected;
		}
		
		location ColLoc() { return col->first; }
		location RowLoc() { return row->first; }
				
		void EraseEdge() { 
		    if (initialized) {
                col->second.usable -= row->second.usable;
                col->second.connected -= row->second.connected;
		        col->second.erase(row++);
		    }
        }
		
		void EraseCol() 
			{ if (initialized && col != mat->end()) mat->erase(col++); }
			
		bool FindCol(location loc) { 
			if (initialized) col = mat->find(loc);
			return col != mat->end();
		}

		bool FindRow(location loc) { 
			if (initialized) row = col->second.find(loc);
			return row != col->second.end();
		}
			
		EdgeIter Transpose() { 
		    #ifdef DEBUG
		    EdgeIter i(row->first, col->first, mat);
		    if (i.IsEdge()) return i;
		    else std::cerr << "Transpose doesn't exist: "
                << row->first << ", " << col->first << "\n";
            assert(false);
            #endif
			return EdgeIter(row->first, col->first, mat);
		}
		
		bool ColIsConvex() 
            {return !region::IsConcaveLocation(*mat->bmp, ColLoc());}
		bool RowIsConvex() 
            {return !region::IsConcaveLocation(*mat->bmp, RowLoc());}
	};
	
	//@}
	

	/////////////////////////////////////////////////////
	/** @name Operators **/
	//@{
	
	EdgeIter operator() (location _col, location _row) {
		return EdgeIter(_col,_row,this);
	}
	
	//@}


	/////////////////////////////////////////////////////
	/** @name Public Members **/
	//@{
	
	EdgeIter GetEdgeIterator() { return EdgeIter(this); }
	EdgeIter GetEdgeIterator(location _col) 
		{ return EdgeIter(_col,this); }	
	EdgeIter GetEdgeIterator(location _col, location _row) 
		{ return EdgeIter(_col,_row,this); }	

	void initialize
        (location::Set& G, region::List &vList, bitmap* bm, std::ostream& out);
	
    std::ostream& Display(std::ostream& out);
    std::ostream& Display(EdgeIter i, std::ostream& out);
	
	/** Removes all edges which intersect the perimeter of polygon P. 
		@param $V_p$	A vector of vertices in P.
		@memo */
	void RemoveClipping(const location::Vector& Vp);

	bool IsUsable(location v1, location v2) {
		return GetEdgeIterator(v1,v2).Usable();
	}

	bool IsConnected(location v1, location v2) {
		return GetEdgeIterator(v1,v2).Connected();
	}

	int TimesUsed(location v1, location v2) {
		return GetEdgeIterator(v1,v2).Used();
	}

	void IncTimesUsed(location v1, location v2) {
		GetEdgeIterator(v1,v2).Used() ++;
	}

	int Weight(location v1, location v2) {
		EdgeIter i = GetEdgeIterator(v1,v2);
		if (i.IsEdge()) return i.Weight();
		return INT_MAX;
	}
	
	void RemoveDisconnected();

	location::LineVector GetRemoved();
	
	location::LineVector GetUsable();
	size_t TotalEdgeCount();
	size_t EdgeCountInSet(const location::Vector& V);
		
	location::Vector GetConvexPolygonWithMaxVertices();

    void ResetObstacleBoundaries();

    bool HasColinearElements(location::Vector& V);

	bool IsColinear(location::Vector& V);
		
	bool HasUsable();
	
	location::Vector GetVertices() {
	    location::Vector v;
	    EdgeIter i = GetEdgeIterator();
	    for (i.ResetCol(); i.HasNextCol(); i.NextCol())
            v.push_back(i.ColLoc());
        return v;
	}
	
	location::Vector GetConvexVertices() {
	    location::Vector v;
	    EdgeIter i = GetEdgeIterator();
	    for (i.ResetCol(); i.HasNextCol(); i.NextCol())
            if (!region::IsConcaveLocation(*bmp, i.ColLoc()))
                v.push_back(i.ColLoc());
        return v;
	}
	
	void RemoveConcaveVertices() {
	    EdgeIter i = GetEdgeIterator();
	    for (i.ResetCol(); i.HasNextCol(); i.NextCol())
            if (region::IsConcaveLocation(*bmp, i.ColLoc())) {
                EraseVertex(i.ColLoc());
                i.ResetCol();
            }
	}
	
	//@}
};

bool StillUsable(location a1, location a2, location b1, location b2); 

#endif

