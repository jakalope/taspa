/*
 *  kruskal.h
 *  
 *  Created by Joseph Crobak on 02/22/06.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */

#ifndef KRUSKAL_H
#define KRUSKAL_H

#include<iostream>
#include<algorithm>
#include <limits.h>
#include"Graph.h"
#include"unionfind.h"
#include"countingsort.h"

using namespace std;

template<typename LengthType>
bool operator< 
(const UndirectedEdge<LengthType> &lhs, const UndirectedEdge<LengthType> & rhs); 

template<typename LengthType>
void kruskal (
          Vertex<UndirectedEdge<LengthType> > *vertices,
          size_t numVerts,
          UndirectedEdge<LengthType> *edges,
          size_t numEdges,
          UndirectedEdge<LengthType> *mstedges
          );


template<typename LengthType>
bool operator< 
(const UndirectedEdge<LengthType> &lhs, const UndirectedEdge<LengthType> & rhs) 
{
    return lhs.length < rhs.length;
}


template<typename LengthType>
void kruskal (
          Vertex<UndirectedEdge<LengthType> > *vertices,
          size_t numVerts,
          UndirectedEdge<LengthType> *edges,
          size_t numEdges,
          UndirectedEdge<LengthType> *mstedges
          ) {

    #ifdef DEBUG
//    printf("----KRUSKAL'S MST ALGORITHM----\n");
    #endif
    // there are n-1 MST edges
    
    size_t nextEdge = 0; // index into mstEdges to add new MST edge

    for (size_t i=0; i<numVerts-1; i++) {
        mstedges[i].from = mstedges[i].other = NULL;
    }

    LengthType maxEdgeLen = 0;
    for (size_t i=0; i<numEdges; i++) {
        if (edges[i].length > maxEdgeLen)
            maxEdgeLen = edges[i].length;
    }   
    UndirectedEdge<LengthType>*tempEdges;
    if (maxEdgeLen > 400000000) {
        tempEdges = new UndirectedEdge<LengthType>[numEdges];
        for (size_t i=0; i<numEdges; i++) {
            tempEdges[i].from = edges[i].from;
            tempEdges[i].other = edges[i].other;
            tempEdges[i].length = edges[i].length;
            tempEdges[i].id = edges[i].id;
        }
        // sort the edges into nondecreasing order by weight
        sort(tempEdges,tempEdges+numEdges);

    } else {
        tempEdges = countingSort(edges,numEdges);
    }

//    for (int i=0; i<numEdges; i++) {
//      assert (csorted[i].length == tempEdges[i].length);
//    }
   
    // union-find data structure from CLRS
    UnionFind uf(numEdges);

    for (size_t i=0; i < numEdges && nextEdge < numVerts; i++) {
        int u = tempEdges[i].from->id;
        int v = tempEdges[i].other->id;
        if (uf.findset(u) != uf.findset(v)) {
            #ifdef DEBUG
//            printf("Kruskal: adding edge (%i,%i), weight = %li\n",
//                    u,v,tempEdges[i].length);
            #endif
            mstedges[nextEdge].id     = tempEdges[i].id;
            mstedges[nextEdge].length = tempEdges[i].length;
            mstedges[nextEdge].from   = tempEdges[i].from;
            mstedges[nextEdge].other  = tempEdges[i].other;
            uf.u(u,v);

            nextEdge++;
        }
        #ifdef DEBUG
        else {
//            printf("Kruskal: NOT adding edge (%i,%i), weight = %li\n",
//                    u,v,tempEdges[i].length);
        }
        #endif


    }

    delete []tempEdges;
}

#endif
