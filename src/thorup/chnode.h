/*
 *  chnode.h
 *  
 *  Created by Joseph Crobak on 02/25/06.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */

#include<iostream>
#include<algorithm>
#include<list>
#include<vector>
#include "Graph.h"
#include "unionfind.h"

#ifndef CHNODE_H
#define CHNODE_H

using namespace std;

template <typename LengthType>
class chnode {

    typedef Vertex<undirectedEdge>* vertptr;
    
    public:
        chnode *parent; // pointer to this node's parent int he component hierarchy

        Vertex<undirectedEdge> *vertex; // non-NULL when this node represents a single vertex (is a leaf)

        int id; // this chnode's unique identification number

        LengthType delta; // sum of all of the MST edges divided by 2^msb(l(e))
        
        list<chnode<LengthType> *> children; // pointers to this nodes children in the component hierarchy

        int level; // this is the 'i' in [v]i

        chnode<LengthType> **buckets; // 2-d array of chnodes.  buckets[i] is a doubly-linked list

        int *bucketCounts; // 1-d array of ints;


        LengthType ix0, ixinf, ix; // indices into the buckets; used by Thorup's algorithm F.


        chnode<LengthType> *next, *prev; // pointers for the doubly-linked list

        LengthType minD; // min D value of [v]i \ S

        int bucketNum; // index into parents buckets where we are stored. -1 if nowhere

        bool isRoot;

        bool isInS;

        int inBucketsCount;

        // parameters:
        //   lev - the level of this node
        //   csize - the number of children this node has.
        chnode(int lev, int csize) : 
            parent(NULL),vertex(NULL),delta(0),
            level(lev),
            buckets(NULL),bucketCounts(NULL),next(NULL),prev(NULL),
            minD((LengthType)INT_MAX),bucketNum(-1) {
            
        }

        // recursively call's the destructor on it's children
        ~chnode() {
            delete [] bucketCounts;
            delete [] buckets;
        }

        // must be called before a subsequent call to thorup()
        void init(){
            if (buckets) {
                delete[] buckets;
                buckets = NULL;
            }
            if (bucketCounts) {
                delete[] bucketCounts;
                bucketCounts = NULL;
            }
            minD = (LengthType)INT_MAX;
        }

        // this method is called by expandComponent()  It allocates delta()+1 
        // buckets.
        void allocBuckets() {
            LengthType d = delta + 1;

            if (buckets)
                delete[] buckets;
            if (bucketCounts)
                delete[] bucketCounts;
            buckets = new chnode<LengthType>*[d];
            bucketCounts = new int[d];
            for (undirectedLength i=0; i< d; i++) {
                buckets[i] = NULL;
                bucketCounts[i] = 0;
            }
            #ifdef DEBUG
//            printf("allocating %li buckets for node: %i\n",d,id);
            #endif
            inBucketsCount=0;
        }

        chnode<LengthType>* bucketStart(unsigned i) {
//            assert (i >= 0 && i < delta+1);
            return buckets[i];
        }
        
        chnode<LengthType>* bucketEnd(unsigned i) {
            assert (i >= 0 && i < delta+1);
            return buckets[i]+bucketCounts[i];
        }
        // adds node n to bucket i
        void addToBucket(unsigned i, chnode *n) {
//            assert (i >= 0 && i < delta+1);
            #ifdef DEBUG
//            printf("      node: %i addToBucket(bucket=%i,n->id=%i)\n",id,i,n->id);
            #endif
            chnode<LengthType>* oldFront = buckets[i];
            n->prev = NULL;
            n->next = oldFront;
            if (oldFront != NULL)
                oldFront->prev = n;
            buckets[i] = n;
            bucketCounts[i]++;
            n->bucketNum = i;

            inBucketsCount++;
        }

        // searches buckets to find the node n, and removes it.
        // returns true on success (n was found).  otherwise 
        // returns false
        bool removeFromBuckets(chnode *n) {
            if (n->bucketNum == -1)
                return false;

            if (n->prev == NULL) // must pick new front
                buckets[n->bucketNum] = n->next;
            else 
                n->prev->next = n->next;
            if (n->next != NULL)
                n->next->prev = n->prev;

            bucketCounts[n->bucketNum]--;
            n->bucketNum=-1;
            n->next = NULL;
            n->prev = NULL;

            inBucketsCount--;

            #ifdef DEBUG
//            printf("      node: %i removed child id=%i from buckets\n",id,n->id);
            #endif
            return true;
        }

        void rebucket(chnode *n) {
            bool res = removeFromBuckets(n);
            if (res == false)
                return;
            LengthType index =  ixinf;
            if (n->getMinD() < (LengthType)INT_MAX) {
                index = n->getMinD() >> (level - 1);
                #ifdef DEBUG
//                printf("here.. n->getMind() = %li, level1 = %i\n",n->getMinD(),
//                        level-1);
                #endif
                if (index > ixinf)
                    index = ixinf;
            }
            #ifdef DEBUG
//            printf("REBUCKET (level=%i): n->id=%i, n->minD=%li, index=%li\n",
//                    level,n->id,n->minD,index);
            #endif
            addToBucket(index - ix0,n);
        }

        // A node is said to be "empty" when all the vertices in the node are
        // in S.  
        bool isEmpty() { return inBucketsCount == 0; } 

        // If allocBuckets wasn't called yet, then this node is unvisited
        bool isUnvisited() { return buckets == NULL; }

        // returns true if this node is a root in the component hierarchy.
        bool isUnvisitedRoot() { return isRoot && isUnvisited(); }

        // returns the root of the tree in the component hierarchy to which
        // this node belongs.
        chnode * unvisitedRoot() {
            if (isRoot)
                return this;

            chnode *root = parent;

            while (root && !root->isUnvisitedRoot())
                root = root->parent;

            return root;
        }

        // returns the first node that is an ancestor of this node
        // that has been visited.
        chnode * visitedParent() {
            chnode *visParent = parent;

            while (visParent && visParent->isUnvisited())
                visParent = visParent->parent;

            return visParent;
        }

        LengthType getMinD() { return minD; }

        // The component maintains a minimum D-value for vertices in its component
        // not in the set S (vertices with known distances).  When a vertex from the
        // component enters S, the min D-value might increase.
        //
        // If valLost == minD, then a new minD must be found by considering all of 
        // its children.
        //
        // parameters:
        //   valLost - the value that a child reports as no longer available as part of
        //   the component.
        // 
        void updateMinD(LengthType valLost) {
            #ifdef DEBUG
//            printf("node: %i updateMinD... valLost=%li minD=%li\n",id,valLost,minD);
            #endif

            if (valLost == minD) {
                LengthType best = (LengthType)INT_MAX;
                typename list<chnode<LengthType>*>::iterator it;
                for (it = children.begin(); it != children.end(); it++) {
                    LengthType x = (*it)->getMinD();
                    if ( x < best)
                        best = x;
                }
                minD = best;
                #ifdef DEBUG
//                printf("   ... new minD = %li\n",best);
                #endif
            }

            if (minD > valLost && parent != NULL) {
                // must rebucket in parents buckets, if needbe
                parent->rebucket(this);
                // most tell our parent. Two cases.
                // 1. valLost wasn't the minD, nothing happens.
                // 2. valLost was the minD, then either our new minD will become parents
                //    new minD or our parent will get a minD from some other child.
                parent->updateMinD(valLost);
            }

        }

        // If newVal is less than minD, then minD is set to newVal and decreaseMinD() is
        // called on parent.
        //
        // parameters:
        //   newVal - the proposed new value for minD.
        //
        void decreaseMinD(LengthType newVal) {
            assert (newVal > 0);
            //if (   (parent != NULL && (newVal >> (parent->level - 1)) < (minD >> (parent->level - 1)))
            //    || (parent == NULL && (newVal < minD)) ) {
            if(newVal < minD) {
                #ifdef DEBUG 
//                printf("      node: %i decreaseMinD(%li) -- decreased minD=%li\n",id,newVal,minD);
                #endif
                minD = newVal;
                if (parent != NULL) {
                    parent->rebucket(this);
                    parent->decreaseMinD(newVal);
                }
            }
            else {
                #ifdef DEBUG 
//                printf("      node: %i decreaseMinD(%li) -- did not change minD=%li\n",id,newVal,minD);
                #endif
            }
        }

        list<Vertex<undirectedEdge> >* verticesInComponent() {

            list<Vertex<undirectedEdge> >* vertices = new list<Vertex<undirectedEdge> >();

            addVertsToList(vertices);

            return vertices;
        }

        void addVertsToList(list<Vertex<undirectedEdge> > *&vertices) {
            if (vertex != NULL) {
                vertices->push_back(*vertex);
                return;
            }

            typename list<chnode<LengthType>*>::iterator it;
            for (it = children.begin(); it != children.end(); it++) {
                (*it)->addVertsToList(vertices);
            }
        }

        // adds a child to the children vector
        void addChild(chnode *&n) {
            children.push_back(n);
        }

};
#endif
