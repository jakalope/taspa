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
 
#include "../region/region.h"
//#include "../std_extensions/iterative_fstream.h"
#include "PathMatrix.h"
#include <fstream>
#include <assert.h>
#include "../thorup/thorup.h"
#include "../thorup/Graph.h"

void PathMatrix::ConstructMapping() { 
    for (size_t i = 0; i < int_to_ver.size(); i ++)
        ver_to_int[int_to_ver[i]] = i;
}


bool MarkAPath
 ( PathMatrix& P, bitmap* inputBmp, bitmap* outputBmp, 
   location::Vector& mv, bool saveLines, std::ofstream& logStrm )
{
	size_t count = 0;
	location::Vector path;
	location src(286,223);
	location dest(120,295);
	bool adjPathWorks = false;
	
	for (;path.size() < 20-(count/50) && count < 1000; count ++) {			
		src.assign(rand()%inputBmp->max().x+1,rand()%inputBmp->max().y+1);
		dest.assign(rand()%inputBmp->max().x+1,rand()%inputBmp->max().y+1);

		path = P.ShortestPath(src,dest);
    }

//    path = P.GeodesicPath(location(13,67), 
//        location(9,66));
	
//    path = P.ShortestPath(location(inputBmp->size().x, inputBmp->size().y), 
//        location(0,0));

	rgba lineColor(64 + rand()%128, 64 + rand()%128, 64 + rand()%128);

	if (path.size() > 1) {
	
		location::VectorIter lit = path.begin();
		location from = *lit;
		location to;
		lit ++;
	
		for (; lit != path.end(); lit++) {
			to = *lit;
			location::Vector line = inputBmp->GetLine(from,to);
			outputBmp->MarkVertices(line, lineColor);
			from = to;
		}
	}
	
	/* print the path to stdout */
	if (saveLines){
		logStrm << "Path lines\n";
		logStrm << LogLines(path, inputBmp->GetHeight());
	}
	
//	P.Display(std::cerr);
	
	return adjPathWorks;
}


void PathMatrix::SaveToDisk(std::ofstream& outfile) {
    #ifdef DEBUG
    if (!outfile) {
        std::cerr << "Bad ofstream passed to SaveToDisk.\n";
        return;
    }
    #endif
    
    // Write matrix dimensions.
    size_t vertex_count  = Height();
    outfile.write(reinterpret_cast<char*>(&vertex_count), sizeof(vertex_count));
    
    // Write conversion vector.
    outfile.write(
        reinterpret_cast<char*>(&int_to_ver[0]), 
        int_to_ver.size()*sizeof(location));
    
    // Write vertex to integer map
    // Unnecessary as this map can be reconstructed from int_to_ver.
    //Write(outfile,ver_to_int); // from iterative_fstream.h
    
    // Write the matrix itself.
    char* chars = CharCast();
    outfile.write(chars, byte_size());
    delete chars;
}
		

void PathMatrix::LoadFromDisk(std::ifstream& infile) {
    #ifdef DEBUG
    if (!infile) {
        std::cerr << "Bad ofstream passed to SaveToDisk.\n";
        return;
    }
    #endif

    // Read matrix dimensions
    size_t vertex_count;    
    infile.read(reinterpret_cast<char*>(&vertex_count), sizeof(vertex_count));
    
    // Resize matrix
    resize(vertex_count,vertex_count);    
    int_to_ver.resize(vertex_count);
    
    // Read conversion vector
    infile.read(
        reinterpret_cast<char*>(&int_to_ver[0]), 
        vertex_count*sizeof(location) );

    // Allocate memory for one row of matrix entries
    char* chars = new char[row_size()];
    
    // Grab a row at a time of matrix entires
    for (size_t r = 0; r < vertex_count; r ++) {
        infile.read(chars, row_size());
        memcpy(&(*this)[r][0], chars, row_size());
    }

    // Clean up
    delete chars;
}
		

bool PathMatrix::IsEdge(location a, location b) {
    return  ver_to_int.find(a) != ver_to_int.end() &&
            ver_to_int.find(b) != ver_to_int.end();
}


bool PathMatrix::HasPath(location a, location b) {
    if (PathLength(a,b) == UNDIRECTED_EDGE_MAX) return false;
    return true;
}
    

location PathMatrix::NextTurn(location a, location b) {
    if (ver_to_int.find(a) == ver_to_int.end()) return NotALoc;
    if (ver_to_int.find(b) == ver_to_int.end()) return NotALoc;

    int next = matrix<PathStep>::GetValue
        (ver_to_int[a], ver_to_int[b]).next;

    if ((size_t)next > int_to_ver.size() || next < 0) return NotALoc;
    return int_to_ver[next];
}
    

undirectedLength PathMatrix::PathLength(location a, location b) {
    if (ver_to_int.find(a) != ver_to_int.end() && 
        ver_to_int.find(b) != ver_to_int.end())
        return matrix<PathStep>::
            GetValue(ver_to_int[a], ver_to_int[b]).pathLength;
    return UNDIRECTED_EDGE_MAX;
}


bool PathMatrix::HasPath(location a) {
    if (ver_to_int.find(a) != ver_to_int.end()) {
        for (int i = 0; i < Height(); i ++)
        if (matrix<PathStep>::GetValue(ver_to_int[a], i).pathLength 
            != UNDIRECTED_EDGE_MAX) 
            return true;
    }
    
    #ifdef DEBUG
    // location a is not a vertex.
    assert(false);
    #endif
    return false;
}


location::Pair PathMatrix::ShortestPathKey
(const location& src, const location& dest, undirectedLength& length) 
{	
	undirectedLength shortPathLength = UNDIRECTED_EDGE_MAX;
    length = shortPathLength;
	location::Pair empty;
	empty.first  = location(INT_MAX, INT_MAX);
	empty.second = location(INT_MAX, INT_MAX);
	location::Pair shortPathPair;
		
	if (src == dest) {
	    length = 0;
	    return empty;
	}
	
    if (!bmp->IsPassible(src) || !bmp->IsPassible(dest))
        return empty;

	undirectedLength s = 2*int_to_ver.size()-1;
	
	if (bmp->HasLineOfSight(src,dest)) {
		shortPathPair.first  = src;
		shortPathPair.second = dest;
		length = L2scaled(src, dest, s);
		return shortPathPair;
	}
	
	// Find nearest vertices to src and to dest and store in lists
	location::List srcVisible;
	location::List destVisible;
	
	typedef location::List::iterator ListIter;
	
    // Find/store all vertices in line of sight from source location.
    location::Pair source(src,src);

	if (!HasPath(src,src)) {
	    for (location::VectorIter i = int_to_ver.begin(); 
            i != int_to_ver.end(); i ++)
            
            if (bmp->HasLineOfSight(*i, src))
				srcVisible.push_back(*i);
	}
	
    // If source is a vertex, just use source.
	else srcVisible.push_back(src);

    // Find/store all vertices in line of sight from dest location.
	if (!HasPath(dest,dest)) {
	    for (location::VectorIter i = int_to_ver.begin(); 
            i != int_to_ver.end(); i ++)
            
            if (bmp->HasLineOfSight(*i, dest))
				destVisible.push_back(*i);

	}
    // If dest is a vertex, just use dest.
	else destVisible.push_back(dest);

    // Look for any vertices visible from both src and dest,
    // Consider all ordered pairs of vertices visible from ( {src},{dest} )
    // for longer paths.
	for (ListIter i = srcVisible.begin();  i != srcVisible.end();  i ++)
	for (ListIter j = destVisible.begin(); j != destVisible.end(); j ++) {
	
        // Get the total path length, using vertices (i,j) as initial
        // and final vertices in the psudo-path.

		undirectedLength thisPathLength = PathLength(*i,*j);

        // Thorup's algorithm cannot handle 0 length edges, so 
        // this line corrects for PathMatrix diagonal entries.
        if (*i == *j) thisPathLength = 0;
        
		thisPathLength += L2scaled(src,*i,s) + L2scaled(dest,*j,s);
		if (thisPathLength < 0) thisPathLength = UNDIRECTED_EDGE_MAX;
		location::Pair thisPathPair(*i, *j);
		
		if (thisPathLength < shortPathLength) {
			shortPathPair = thisPathPair;
			shortPathLength = thisPathLength;
		}
	}
    
    length = shortPathLength;
	if (shortPathLength == UNDIRECTED_EDGE_MAX) return empty;
    return shortPathPair;
}
		

location::Vector PathMatrix::ShortestPath
(const location& src, const location& dest) 
{    
    location::Vector shortPath;
    undirectedLength length;

    location::Pair key = ShortestPathKey(src, dest, length);
    if (key.first.x == INT_MAX) return shortPath;
    
	if (!IsEdge(key.first, key.second)) {
	    shortPath.push_back(src);
	    shortPath.push_back(dest);
	    return shortPath;
	}

    shortPath.push_back(src);

    location::Pair p = key;
    while ( IsEdge(p.first, p.second) && 
            NextTurn(p.first, p.second) != p.first && 
            NextTurn(p.first, p.second) != NotALoc) {
        shortPath.push_back(p.first);
        p.first = NextTurn(p.first, p.second);
    }

	if (p.second != dest) shortPath.push_back(p.second);
    shortPath.push_back(dest);
	
	return shortPath;
}


location::Vector PathMatrix::GeodesicPath
(const location& src, const location& dest) 
{
    location::Vector empty;
    location::Vector path;
    location::Pair pathPair;
    undirectedLength pathLength = UNDIRECTED_EDGE_MAX;
    location src2 = src;
    location dest2 = dest;
    
    undirectedLength s = 2*int_to_ver.size()-1;
    
    // If we start or end on non-traversable terrain, no path is available.
    if (! bmp->IsPassible(src) || ! bmp->IsPassible(dest)) return empty;

    location::Vector srcBorders  = bmp->GetNonBorderLocs(src);
    location::Vector destBorders = bmp->GetNonBorderLocs(dest);
    srcBorders.push_back(src);
    destBorders.push_back(dest);

    pathPair = ShortestPathKey(src,dest,pathLength);

    for (location::VectorIter i = srcBorders.begin(); 
        i != srcBorders.end(); i++)
    
    for (location::VectorIter j = destBorders.begin(); 
        j != destBorders.end(); j++) {

        undirectedLength thisLength;
        location::Pair thisPath = ShortestPathKey(*i,*j,thisLength);
        thisLength += (thisLength == UNDIRECTED_EDGE_MAX) ? 0 : L2scaled(src,  *i, s);
        thisLength += (thisLength == UNDIRECTED_EDGE_MAX) ? 0 : L2scaled(dest, *j, s);
        if (thisLength < pathLength) {
            pathLength = thisLength;
            pathPair   = thisPath;
            src2 = *i;
            dest2 = *j;
        }
    }
    
    path.push_back(src);
    if (src != src2) path.push_back(src2);
    location::Vector midPath = ShortestPath(pathPair.first, pathPair.second);
    path.insert(path.end(), midPath.begin(), midPath.end());
    path.push_back(dest2);
    if (dest != dest2) path.push_back(dest);
    
    return path;
}


bool PathMatrix::AllPathsFound
(location a)
{
    int a_index = ver_to_int[a];
    for (int b_index = 0; b_index < max_x()+1; b_index++)
        if (matrix<PathStep>::
            GetValue(a_index,b_index).pathLength == UNDIRECTED_EDGE_MAX)
            return false;

    return true;
}

