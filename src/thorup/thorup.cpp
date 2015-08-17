/*
 *  thorup.cc
 *  
 *  Created by Joseph Crobak on 02/26/06.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */
#include<string>
#include<fstream>

#include "../stopwatch/Stopwatch.h"
#include "thorup.h"

#include "parse.h"
#include "Graph.h"
#include "timer.h"
#include "ch.h"
#include "kruskal.h"

undirectedLength L2scaled(location src, location dest, undirectedLength scale) 
{
    return (undirectedLength)
        sqrt((src.x-dest.x)*(src.x-dest.x)*scale*scale
        +   (src.y-dest.y)*(src.y-dest.y)*scale*scale); 
}

void ThorupPaths
(PathMatrix& P, AdjacencyMatrix& A, std::ostream& out) 
{
	Stopwatch timer;
    timer.Start();
	
    int numVerts = P.Height();
	int numEdges = A.TotalEdgeCount() - numVerts; // (All edges) - (self edges)

    if (numVerts == 0) {
        std::cerr << "No vertices to process.\n";
        return;
    }

	Vertex<undirectedEdge>* vertices = 
        new (std::nothrow) Vertex<undirectedEdge>[numVerts+1];
    int* fromVertices        = new (std::nothrow) int[numEdges];
    undirectedEdge* inEdges  = new (std::nothrow) undirectedEdge[numEdges];
    undirectedEdge* edges    = new (std::nothrow) undirectedEdge[numEdges];
	int* start               = new (std::nothrow) int[numVerts+1];
    Vertex<undirectedEdge>** pred = 
        new (std::nothrow) Vertex<undirectedEdge>*[numVerts+1];
    undirectedLength* dist   = new (std::nothrow) undirectedLength[numVerts+1];
    bool* S                  = new (std::nothrow) bool[numVerts+1];
    undirectedEdge* mstEdges = new (std::nothrow) undirectedEdge[numVerts-1];
    
    if (!vertices || ! fromVertices || !inEdges || !edges || !start ||
        !pred || !dist || !S || !mstEdges) {            
            if (vertices)       delete[] vertices;
            if (fromVertices)   delete[] fromVertices;
            if (inEdges)        delete[] inEdges;
            if (edges)          delete[] edges;
            if (start)          delete[] start;
            if (pred)           delete[] pred;
            if (dist)           delete[] dist;
            if (S)              delete[] S;
            if (mstEdges)       delete[] mstEdges;
            
            std::cerr << "Could not allocate enough contiguous memory.\n";
            return;
    }
    
	int edgesSeen = 0;  // incremented every time a line desc a line is read in
	int from, to; 
	
    ///////////////////////////////////////////////////////////////////
    // Visit all edges in the adjacency matrix and add the information 
    // to various arrays.
        
    undirectedLength s = 2*numVerts-1;
    AdjacencyMatrix::EdgeIter e = A.GetEdgeIterator();
    for (e.ResetCol(); e.HasNextCol(); e.NextCol()) {
        
        from = P.VerToInt(e.ColLoc());
        if (from == INT_MAX) continue;
        
    for (e.ResetRow(); e.HasNextRow(); e.NextRow()) {

        to   = P.VerToInt(e.RowLoc());
        if (to == INT_MAX) continue;
        if (to == from) continue;
        assert (to != INT_MAX);
        
        undirectedLength weight = L2scaled(e.ColLoc(), e.RowLoc(), s);
        setInfo(inEdges[edgesSeen], edgesSeen, &(vertices[to]),
            weight, &(vertices[from]));

        fromVertices[edgesSeen] = from;
        vertices[from].edgeCount++;
        edgesSeen++;

	}   }
	

    ///////////////////////////////////////////////////////////////////
    // Complete thorup's for each vertex as source

    for (int v = 0; v < numVerts; v++) 
	{
        Vertex<undirectedEdge>* source;

        matrix_to_arrays<undirectedLength,undirectedEdge>
            (A,P,numVerts,numEdges,v,vertices,
            edges,source,inEdges,fromVertices,start,true);

        ComponentHierarchy<undirectedLength> 
            ch(S,vertices,numVerts,edges,numEdges,dist,pred);
        ch.compute(mstEdges);
        ch.thorup(vertices,numVerts,source);
        
        PathMatrix::EdgeIter e = P.GetEdgeIterator(v);
        for (e.ResetRow(); e.HasNextRow(); e.NextRow())
        {
            std::vector<int> path = e.ExtractPath(pred);
            e.StorePath(path, dist);
        }
        P.SetValue(v,v,v,0);

        float etc = (float)(numVerts - v - 1) * (timer.Lap()/(float)(v+1));
        
        char hrs[4];
        char min[3];
        char sec[3]; 
        
        sprintf(hrs, "%03d", (int)(floorf(etc/3600.)));
        sprintf(min, "%02d", (int)(floorf(etc/60.))%60);
        sprintf(sec, "%02d", (int)(etc)%60);
        
	    out << v+1 << " / " << numVerts << " vertices processed.  T-" 
            << hrs << ":" << min << ":" << sec << "   \r";
	    out.flush();
	}
	
    delete []dist;
    delete []pred;
    delete []vertices;
    delete []edges;
    delete []fromVertices;
    delete []inEdges;
    delete []start;
    delete []S;
    delete []mstEdges;

	out << "\n";
}

