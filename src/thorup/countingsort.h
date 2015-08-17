/*
 *  counting_sort.h
 *  
 *  Created by Joseph Crobak on 03/07/06.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */

#include<iostream>
#include<algorithm>
#include"Graph.h"
#include"unionfind.h"

using namespace std;
#ifndef COUNTINGSORT_H
#define COUNTINGSORT_H 

undirectedEdge* countingSort
(undirectedEdge* &edges, undirectedLength num, undirectedLength maxEdgeLen=0); 

//template<typename LengthType> 
undirectedEdge* countingSort
(undirectedEdge* &edges, undirectedLength num, undirectedLength maxEdgeLen) 
{
    if (maxEdgeLen == 0) { // must find largest edge
        for (undirectedLength i=0; i<num; i++) {
            if (edges[i].length > maxEdgeLen)
                maxEdgeLen = edges[i].length;
        }
    }
    size_t *count = new size_t[maxEdgeLen+2];

    undirectedLength i;

    for (i=0; i<=maxEdgeLen+1; i++)
        count[i] = 0;

    // set the value of count[i] to the number of
    // elements in array with value i-1
    for (i=0; i<num; i++) {
        count[edges[i].length+1]++;
    }

    // update count[i] to be the number of elements
    // with values less than or equal to i
    for (i=1; i<=maxEdgeLen; i++){
        count[i] += count[i-1];
    }

    // copy the elements of array into sorted array
    //  in sorted order
    undirectedEdge*sorted = new undirectedEdge[num];
    for (i=0; i<num; i++) {
        size_t c = count[edges[i].length];
        sorted[c].from = edges[i].from;
        sorted[c].other = edges[i].other;
        sorted[c].length = edges[i].length;
        sorted[c].id = edges[i].id;
   
        // increment count so that the next element
        // with the same value as the current element
        // is placed into its own position in sorted
        count[edges[i].length]++;
    }
    delete[] count;
    return sorted;
}

#endif
