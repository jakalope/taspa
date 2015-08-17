/*
 *  Graph.h
 *  
 *  Created by Joseph Crobak on 12/31/05.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */
#ifndef GRAPH_H
#define GRAPH_H

#include <limits.h>
 
template<typename EdgeType>
class Vertex;

template<typename LengthType>
class Edge {
public:
	LengthType length;
	Vertex<Edge<LengthType> > *other;
	int id;
};

template<typename LengthType>
class UndirectedEdge {
public:
    LengthType length;
    int id;
    Vertex<UndirectedEdge<LengthType> > *from, *other;
};

template<typename EdgeType>
class Vertex {
public:
	Vertex():first(NULL),edgeCount(0),id(0),next(NULL),prev(NULL){}

    EdgeType* last() {return (first+edgeCount);}
	
    EdgeType* first;
	int edgeCount;
	int id;
    int bucket;

    Vertex<EdgeType>* next, *prev;
};

const char WHITE=0;
const char GRAY=1;
const char BLACK=2;

typedef long directedLength;
typedef Edge<directedLength> edge;
const directedLength DIRECTED_EDGE_MAX = LONG_MAX;

typedef long undirectedLength;
typedef UndirectedEdge<undirectedLength> undirectedEdge;
const undirectedLength UNDIRECTED_EDGE_MAX = LONG_MAX;
#endif

