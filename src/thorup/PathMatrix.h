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
 
#ifndef PATHMATRIX_H
#define PATHMATRIX_H

#include <vector>
#include <map>
#include <limits.h>
#include <iostream>
#include "../bitmap/bitmap.h"
#include "../bitmap/matrix.h"
#include "../location/location.hpp"
#include "../thorup/Graph.h"

class PathStep
{
    public:
	int next;              // Next location on the way to this location
    undirectedLength pathLength; // Length of traversable path, A_ij, i -> j
    
    PathStep() : next(INT_MAX), pathLength(UNDIRECTED_EDGE_MAX) { }
    PathStep(int _next, undirectedLength _pathLength) : next(_next), 
        pathLength(_pathLength) { }
        
    bool operator==(PathStep Q) const
        { return next == Q.next && pathLength == Q.pathLength; }

    bool operator==(PathStep& Q) const
        { return next == Q.next && pathLength == Q.pathLength; }

    bool operator==(PathStep* Q) const
        { return next == Q->next && pathLength == Q->pathLength; }
};


const location NotALoc(INT_MAX,INT_MAX);

class PathMatrix : public matrix<PathStep>
{
    private:
    
    std::vector<location> int_to_ver;
    std::map<location, int> ver_to_int;    
    bitmap* bmp;

    void ConstructMapping();

    public:
    
    PathMatrix() : bmp(0) { }
    
    PathMatrix( std::vector<location>& _int_to_ver, 
                bitmap* _bmp) : 
        matrix<PathStep>(_int_to_ver.size(),_int_to_ver.size()), 
        int_to_ver(_int_to_ver), 
        bmp(_bmp) { ConstructMapping(); }
        
    bool operator==(PathMatrix& Q) {
        return matrix<PathStep>::operator==(Q) && int_to_ver == Q.int_to_ver;
    }

    class EdgeIter {
        
        private:
        
        PathMatrix* mat;
        int col;
        int row;
        
        public:
        
        EdgeIter(PathMatrix* _mat) : 
            mat(_mat), col(0), row(0) { }
                    
        EdgeIter(PathMatrix* _mat, int _col) : 
            mat(_mat), col(_col), row(0) { }

        EdgeIter(PathMatrix* _mat, int _col, int _row) : 
            mat(_mat), col(_col), row(_row) { }

        EdgeIter(PathMatrix* _mat, location _col) : 
            mat(_mat), col(_mat->ver_to_int.find(_col)->second), row(0) { }

        EdgeIter(PathMatrix* _mat, location _col, location _row) : 
            mat(_mat), col(_mat->ver_to_int.find(_col)->second), 
            row(_mat->ver_to_int.find(_row)->second) { }
            
        bool HasNextCol() { return col < mat->Height(); }
        bool HasNextRow() { return row < mat->Height(); }
        bool HasNextEdge() { return (col < mat->Height() && row < mat->Height()); }
        
        void ResetCol() { col = 0; }
        void ResetRow() { row = 0; }
        void Reset() { col = 0; row = 0; }
        
        void NextCol() { col ++; }
        void NextRow() { row ++; }
        void NextEdge() { 
            if (HasNextRow()) NextRow(); 
            else { ResetRow(); NextCol(); } 
        }
        
        int GetNext() { 
            return (*mat)[col][row].next; 
        }
        
        undirectedLength GetPathLength() { 
            return (*mat)[col][row].pathLength; 
        }
        
        PathStep& operator*  () { return (*mat)[col][row]; }
        PathStep& operator-> () { return (*mat)[col][row]; }

        location ColLoc()  const { return mat->int_to_ver[col];  }
        location RowLoc()  const { return mat->int_to_ver[row];  }
        location NextLoc() const 
            { return mat->int_to_ver[(*mat)[col][row].next]; }
        
        std::vector<int> ExtractPath(Vertex<undirectedEdge>** pred) 
        {
            int src = col;
            int dest = row;
            std::list<int> path_list;            
            
            while (dest != src && dest != INT_MAX && pred[dest] != NULL) {
                path_list.push_front(dest);
                if (pred[dest] != NULL) dest = pred[dest]->id;
            }
            
            std::vector<int> path(path_list.begin(), path_list.end());
            return path;        
        }
        
        // dist stores 
        void StorePath(std::vector<int>& path, undirectedLength* dist) {
            if (path.size() == 1) {
                (*mat)[col][row].next = row;
                (*mat)[col][row].pathLength = dist[row];
            }
            
            int dest = row;
            
            for (size_t i=1; i < path.size(); i ++) {
                int src = path[i-1];
                (*mat)[src][dest].next = path[i];
                (*mat)[src][dest].pathLength = dist[dest] - dist[src];
            }
        }
    };

	EdgeIter GetEdgeIterator() { return EdgeIter(this); }

	EdgeIter GetEdgeIterator(int _col) 
		{ return EdgeIter(this,_col); }	
	EdgeIter GetEdgeIterator(int _col, int _row) 
		{ return EdgeIter(this,_col,_row); }	

	EdgeIter GetEdgeIterator(location _col) 
		{ return EdgeIter(this,_col); }	
	EdgeIter GetEdgeIterator(location _col, location _row) 
		{ return EdgeIter(this,_col,_row); }	

    void Set(   std::vector<location> _int_to_ver, 
                bitmap* _bmp) {
        resize(_int_to_ver.size(),_int_to_ver.size());
        int_to_ver = _int_to_ver; 
        ConstructMapping();
        bmp = _bmp;
    }
    
    void SetValue 
    (location a, location b, location _next, undirectedLength _pathLength) 
    { 
        matrix<PathStep>::SetValue(
            ver_to_int[a], 
            ver_to_int[b], 
            PathStep(ver_to_int[_next],_pathLength));
    }

    void SetValue (int a, int b, int _next, undirectedLength _pathLength) { 
        matrix<PathStep>::SetValue(a, b, PathStep(_next,_pathLength));
    }

    void Display(std::ostream& out) {
        
        for (int i = 0; i < matrix<PathStep>::max_x()+1; i ++) {
            
            out << "\n" << int_to_ver[i] << "\n"; 
            
        for (int j = 0; j < matrix<PathStep>::max_y()+1; j ++)
        
            out << "  " << int_to_ver[j] << " -> " 
                << matrix<PathStep>::GetValue(i,j).next << " : " 
                << matrix<PathStep>::GetValue(i,j).pathLength << "\n";
        
        }
    }
    
    PathStep GetValue (location a, location b) { 
        #ifdef DEBUG
        PathStep p = matrix<PathStep>::GetValue( ver_to_int[a], ver_to_int[b]);
        return p;
        #endif
        return matrix<PathStep>::GetValue( ver_to_int[a], ver_to_int[b]);
    }

    bool IsEdge(location a, location b);
    bool HasPath(location a, location b);
    location NextTurn(location a, location b);
    undirectedLength PathLength(location a, location b);
    bool HasPath(location a);

	location::Pair ShortestPathKey
        (const location& src, const location& dest, undirectedLength& length);
	location::Vector ShortestPath(const location& src, const location& dest);
    location::Vector GeodesicPath(const location& src, const location& dest);

    bool AllPathsFound(location a);

    bool IsVertex(location a) { 
        for (size_t i=0; i < int_to_ver.size(); i ++)
            if (int_to_ver[i] == a) return true;
        return false;
    }

    int VerToInt_linear(location a) { 
        for (size_t i=0; i < int_to_ver.size(); i ++)
            if (int_to_ver[i] == a) return i;
        return INT_MAX;
    }

    int VerToInt(location a) { 
        if (ver_to_int.find(a) != ver_to_int.end())
            return ver_to_int.find(a)->second; 
        return INT_MAX;
    }
    
    location IntToVer(int i) { return int_to_ver[i]; }
    const std::vector<location>& GetIntToVer() { return int_to_ver; }
    
    void SaveToDisk(std::ofstream& outfile);
    void LoadFromDisk(std::ifstream& infile);

};


bool MarkAPath(PathMatrix& A, bitmap* inputBmp, 
            bitmap* outputBmp, location::Vector& mv, bool saveLines, 
            std::ofstream& logStrm);

#endif
