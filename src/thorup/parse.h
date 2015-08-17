/*
 *  parse.h
 *  
 *  Created by Joseph Crobak on 12/31/05.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */
 
#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <exception>

#include "../location/location.hpp"
#include "../polygon/AdjacencyMatrix.h"

#include "Graph.h"

using namespace std;

void input_error(char *error_msg, int line);

template <typename LengthType>
void copy(Edge<LengthType>&edge1,Edge<LengthType>&edge2);

template <typename LengthType>
void copy(UndirectedEdge<LengthType>&edge1,UndirectedEdge<LengthType>&edge2);

template <typename edgeType,typename LengthType>
void setInfo(UndirectedEdge<LengthType>&edge,int &id, Vertex<edgeType> *other,
             LengthType &length, Vertex<edgeType> *from);

template <typename edgeType, typename LengthType>
void setInfo(Edge<LengthType>&edge,int id, Vertex<edgeType> *other,
             LengthType length, Vertex<edgeType> *from);

template<typename LengthType, typename edgeType>
void parse(ifstream &infile,
           int &numVerts,
		   int &numEdges,
		   Vertex<edgeType> *&vertices,
		   edgeType *&edges,
		   Vertex<edgeType> *&source,
           string &problemDesc,
           bool undirected=false);

template<typename LengthType, typename edgeType>
void matrix_to_arrays(AdjacencyMatrix& A,
           PathMatrix& P,
           int numVerts,
		   int& numEdges,
		   int sourceNum,
		   Vertex<edgeType> *&vertices,
		   edgeType *&edges,
		   Vertex<edgeType> *&source,
		   undirectedEdge* inEdges,
		   int* fromVertices,
		   int* start,
           bool undirected=false);


void input_error(char *error_msg, int line) {

	cerr << "line " << line << ": " << error_msg << endl;
	exit(1);
}

template <typename LengthType>
void copy(Edge<LengthType>&edge1,Edge<LengthType>&edge2) {
    edge1.id = edge2.id;
    edge1.other = edge2.other;
    edge1.length = edge2.length;
}

template <typename LengthType>
void copy(UndirectedEdge<LengthType>&edge1,UndirectedEdge<LengthType>&edge2) {
    edge1.id = edge2.id;
    edge1.other = edge2.other;
    edge1.length = edge2.length;
    edge1.from = edge2.from;
}

template <typename edgeType,typename LengthType>
void setInfo(UndirectedEdge<LengthType>&edge,int &id, Vertex<edgeType> *other,
             LengthType &length, Vertex<edgeType> *from) {
    edge.id = id;
    edge.other = other;
    edge.length = length;
    edge.from = from;
}

template <typename edgeType, typename LengthType>
void setInfo(Edge<LengthType>&edge,int id, Vertex<edgeType> *other,
             LengthType length, Vertex<edgeType> *from) {
    edge.id = id;
    edge.other = other;
    edge.length = length;
}

// all pointers will point to allocated memory at the end of this
// function.  vertices and edges must be deallocated with c++ "delete[]"
template<typename LengthType, typename edgeType>
void parse(ifstream &infile,
           int &numVerts,
		   int &numEdges,
		   Vertex<edgeType> *&vertices,
		   edgeType *&edges,
		   Vertex<edgeType> *&source,
           string &problemDesc,
           bool undirected) {

	edgeType *inEdges;
	int *fromVertices;
	string line;
	int sourceNum = -1; // id of the source vertex
	int lineNum   = 0;  // line number of the input file
	int edgesSeen = 0;  // incremented every time a line desc a line is read in
	int from, to; 
    LengthType weight; // temp for reading in
	int *start;
	int retVal= -99;
	
	// look for the line with the problem size
	while (getline(infile,line)) {
		lineNum++;
		if (line.length() > 0 && line[0] == 'p') break;
	}
	
	// line looks like p sp <vertices> <edges>
	if (2 != (retVal = sscanf(line.c_str(),"%*c %*2s %d %d",
            &numVerts,&numEdges)))
		input_error("number of vertices and/or edges not supplied",lineNum);
	
	vertices     = new Vertex<edgeType>[numVerts+1];
    if (undirected) {
        numEdges *=2;
    }
    //try {
    fromVertices = new int[numEdges];
    inEdges      = new edgeType[numEdges];
    edges        = new edgeType[numEdges];

	start        = new int[numVerts+1];
    //}
    //catch (exception &e) {
    //    cout << "an exception occurred; number: " << e.what() << endl;
    //}
	while (getline(infile,line) && !infile.eof()) {	
		lineNum++;

		if (line.length() == 0)
			continue;
			
		switch(line[0]) {
			// skip empty lines
			case 'c':
			case '\n':
			case '\0':
				break; 
				
			// problem desciption
			case 't':
				problemDesc = line.substr(2);
				#ifdef DEBUG
//				cout << "Problem description: " << problemDesc << endl;
				#endif
				break;
				
			// source description
			case 'n':
				if (sourceNum != -1)
					input_error("multiple source vertices specified",lineNum);
				if (1 != sscanf(line.c_str(),"%*c %d",&sourceNum))
					input_error("source vertex id not supplied",lineNum);
				if (sourceNum < 1 || sourceNum > numVerts)
					input_error("invalid source vertex id",lineNum);
				
				#ifdef DEBUG
//				cout << "Source vertex: " << sourceNum << endl;
				#endif
				break;
				
			// edge (arc) description
			case 'a':
				if ( edgesSeen == numEdges) 
					input_error("too many edges in input",lineNum);
				if (3 != sscanf(line.c_str(),"%*c %d %d %ld",
                    &from,&to,&weight))
					input_error("too few parameters when describing arc",
					lineNum);
				if (from < 1 || from > numVerts)
					input_error("first vertex id is invalid",lineNum);
				if (to < 1 || to > numVerts)
					input_error("second vertex id is invalid",lineNum);
				if (weight < 0) 
					input_error("negative weight supplied",lineNum);
				setInfo(inEdges[edgesSeen],edgesSeen,&(vertices[to]),weight,
                    &(vertices[from]));

				fromVertices[edgesSeen] = from;
				vertices[from].edgeCount++;
				edgesSeen++;

                if (undirected) {
                    setInfo(inEdges[edgesSeen],edgesSeen,&(vertices[from]),
                        weight,&(vertices[to]));

                    fromVertices[edgesSeen] = to;
                    vertices[to].edgeCount++;
                    edgesSeen++;
                }
		} // switch
	} // while getline
	
	// inEdges,fromVertices: forall i=0 to numEdges-1, there is an edge 
	//   from fromVertices[i] to inEdges[i].other;
	// outCount[id]:         number of edges coming out vertex # id.
	// sourceNum:            number (id) of the source vertex
	
	// finishing the counting sort based on outCount
	// start[i] is the index into Edges 
	start[0] = 0;
	for (int i=1; i<=numVerts; i++)
		start[i] = start[i-1] + vertices[i-1].edgeCount;
		
	// provide each vertex with a pointer to it's first edge
	// and tell it how many edges it has.
	vertices[0].first = &edges[0]; // point to beginning
	vertices[0].id=0;
	for (int i=1; i<=numVerts; i++) {
		vertices[i].id = i;
		vertices[i].first = vertices[i-1].first + vertices[i-1].edgeCount;
	}
	// loop throught the inEdges and placing them in the correct place of 
	//  "edges" so that
	// each vertex's first points to the first edge, and all of its edges 
	//  come before the next
	// vertex's first
	for (int i=0; i<edgesSeen; i++) {
		int pos = start[fromVertices[i]]++;
        copy(edges[pos],inEdges[i]);
	}
	
    if (sourceNum >= 0) {
        source = &vertices[sourceNum];
    }
    else {
        source = &vertices[1];
    }
	
	#ifdef DEBUGLEVEL3
//	cout << "\"Sorted Edges\"" << endl;
//	for (int i=0; i<edgesSeen; i++)
//		cout << "Edge id: " << edges[i].id << endl;
//		
//	cout << "Vertices:" << endl;
//	for (int i=0; i<=numVerts; i++)
//		cout << "Vert id: " << vertices[i].id << ", start: "
//			 << vertices[i].first - edges << ", end: " 
//			 << vertices[i].last() - edges
//			 << endl;
	#endif
    
    delete []fromVertices;
    delete []inEdges;
    delete []start;
}


// all pointers will point to allocated memory at the end of this
// function.  vertices and edges must be deallocated with c++ "delete[]"

//    (A,P,numVerts,numEdges,v,vertices,
//    edges,source,inEdges,fromVertices,start,true);

template<typename LengthType, typename edgeType>
void matrix_to_arrays(AdjacencyMatrix& A,
           PathMatrix& P,
           int numVerts,
		   int& numEdges,
		   int sourceNum,
		   Vertex<edgeType> *&vertices,
		   edgeType *&edges,
		   Vertex<edgeType> *&source,
		   undirectedEdge* inEdges,
		   int* fromVertices,
		   int* start,
           bool undirected) {

    int edgesSeen = numEdges;
    
    ///////////////////////////////////////////////////////////////////
	// inEdges,fromVertices: forall i=0 to numEdges-1, there is an edge 
	//   from fromVertices[i] to inEdges[i].other;
	// outCount[id]:         number of edges coming out vertex # id.
	// sourceNum:            number (id) of the source vertex
	

    ///////////////////////////////////////////////////////////////////
	// finishing the counting sort based on outCount
	// start[i] is the index into Edges 
	start[0] = 0;
	for (int i=1; i<=numVerts; i++)
		start[i] = start[i-1] + vertices[i-1].edgeCount;
    
    
    ///////////////////////////////////////////////////////////////////
	// provide each vertex with a pointer to it's first edge
	// and tell it how many edges it has.
	vertices[0].first = &edges[0]; // point to beginning
	vertices[0].id=0;
	for (int i=1; i<=numVerts; i++) {
		vertices[i].id = i;
		vertices[i].first = vertices[i-1].first + vertices[i-1].edgeCount;
	}


    ///////////////////////////////////////////////////////////////////
	// loop throught the inEdges and placing them in the correct place of 
	//  "edges" so that
	// each vertex's first points to the first edge, and all of its edges 
	//  come before the next
	// vertex's first
	for (int i=0; i<edgesSeen; i++) {
		int pos = start[fromVertices[i]]++;
        copy(edges[pos],inEdges[i]);
	}
	
    if (sourceNum >= 0) {
        source = &vertices[sourceNum];
    }
    else {
        source = &vertices[1];
    }
}

#endif
