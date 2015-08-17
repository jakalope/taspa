/*
 *  ch.h - Component Hierarchy
 *  
 *
 *  Created by Joseph Crobak on 02/25/06.
 *
 */

#include<iostream>
#include<algorithm>
#include<cmath>
#include<vector>
#include<set>
#include<map>
#include<assert.h>
#include"Graph.h"
#include"unionfind.h"
#include"chnode.h"
#include"kruskal.h"

#ifndef COMPONENTHIERARCHY_H
#define COMPONENTHIERARCHY_H
using namespace std;

template<typename LengthType>
class ComponentHierarchy {
private:

    bool *S;
    Vertex<undirectedEdge> *vertices;
    int numVerts;
    undirectedEdge *edges;
    int numEdges;
    Vertex<undirectedEdge> *source;

    LengthType *dist;          // array of distances indexed by vertex->id
    Vertex<undirectedEdge> **pred; // array of predecessors indexed by vertex->id

    vector< chnode<LengthType>* > chnodes;

public:
    // parameters:
    //   pVertices - pointers to vertices array
    //   pNumVerts - size of vertices array
    //   pEdges    - pointers to edges array
    //   pNumEdges - size of edges array

    // ch(vertices,numVerts,edges,numEdges,pred,dist,S);

    ComponentHierarchy(bool* s,
                       Vertex<undirectedEdge> *pVertices,
                       int pNumVerts,
                       undirectedEdge *pEdges,
                       int pNumEdges,
                       LengthType *d,
                       Vertex<undirectedEdge> **p) :
        S(s), vertices(pVertices), numVerts(pNumVerts), edges(pEdges),
        numEdges(pNumEdges), dist(d), pred(p) { }

    ~ComponentHierarchy() {
        for (unsigned i=0; i<chnodes.size(); i++) 
            delete chnodes[i];
    }

    void compute(undirectedEdge* mstEdges) {
        // there are at most 2*numVerts - 1 components in the tree
        kruskal<LengthType>(vertices,numVerts,edges,numEdges,mstEdges);

        int *vidTOcid = new int[numVerts+1]; // maps vertex id to a component id
        LengthType *s  = new LengthType[numVerts+1]; // maps vertex id to the sum of the weights of the
                              // edges of component with canonical element v with id=id.
        bool *X = new bool[numVerts+1];   // set of old canonical elements for the components united
                              // since last components were collected
                              // X[i] = true if vertex #i is in the set

        // vertices are read in as 1...numVerts, not 0...numVerts-1;
        UnionFind uf(numVerts);

        chnodes.reserve(numVerts*2+2);
        // create a leaf chnode for each vertex in the graph
        //   -- verts go up to numVerts
        for (int i=0; i<=numVerts; i++) {

            chnodes.push_back(new chnode<LengthType>(0,0)); // level 0, children 0 
            chnodes.back()->vertex     = &vertices[i];
            chnodes.back()->id         = vertices[i].id;

            s[i] = 0;        // e.g. s[0] = s[1] = ... = 0 initially
            vidTOcid[i] = i; // each vid maps to component with that id, initially
                                   // e.g. vidTOcid[0] = 0;
        }

        // initialize stuff
        for (int i=0; i<=numVerts; i++)
            X[i] = false;

        // determine number of MST edges;
        int numMSTEdges =0;
        for (int i=0; i<numVerts-1; i++) {
            if (mstEdges[i].from != NULL)
                numMSTEdges++;
        }

        int c=numVerts; // c is a counter for numbering chnodes
        for (int i=0; i<numMSTEdges; i++) {
                int v = mstEdges[i].from->id;
                int w = mstEdges[i].other->id;
                LengthType weight = mstEdges[i].length;

                #ifdef DEBUG
//                printf("edge: (%i,%i), weight %li\n",v,w,weight);
                #endif

                // G.3.2
                X[uf.findset(v)] = true;
                X[uf.findset(w)] = true;

                #ifdef DEBUG
//                printf("adding to X: %i, %i\n",uf.findset(v),uf.findset(w));
                #endif

                // G.3.3
                LengthType sum = s[uf.findset(v)] + s[uf.findset(w)] + weight;
                #ifdef DEBUG
//                printf("sum of %i:%li = %li + %li + %li \n",uf.findset(v),sum,s[uf.findset(v)],s[uf.findset(w)],weight);
                #endif

                // G.3.4
                uf.u(v,w);
                #ifdef DEBUG
//                printf("union: %i %i\n",v,w);
                #endif

                // G.3.5
                s[uf.findset(v)] = sum;
                #ifdef DEBUG
//                printf("s[%i] := %li\n",uf.findset(v),sum);
                #endif

            // G.3.6
            if ( (i == numMSTEdges-1) || // last edge
                 (msb(mstEdges[i].length) < msb(mstEdges[i+1].length))
               ) {
                int *c2 = new int[numVerts+1];

                // G.3.6.1
                set<LengthType> X2; // set works as long as it doesn't allow dupes
                //for(set<int>::iterator it=X.begin(); it != X.end(); it++) {
                //    X2.insert(uf.findset(*it));
                //}
                for (int k=0; k<=numVerts; k++) {
                    if (X[k] == true) {
                        X2.insert(uf.findset(k));
                    }
                }
                // G.3.6.2
                int level = msb(mstEdges[i].length);
                //for (int i=0; i<X2.size(); i++) {
                for(set<undirectedLength>::iterator it = X2.begin(); it != X2.end(); it++) {
                    #ifdef DEBUG
//                    printf("X2 contains: %i\n",*it);
                    #endif
                    // G.3.6.2.1
                    c++;   
                    chnodes.push_back(new chnode<LengthType>(level,0)); 
                    chnodes.back()->id = c;
                    #ifdef DEBUG
//                    printf("creating chnode: %i, with level %i\n",c,level);
                    #endif
                    // G.3.6.2.2
                    c2[*it] = c;
                    #ifdef DEBUG
//                    printf("c2[%i]: %i\n",*it,c);
                    #endif
                }
                // G.3.6.3
                for(int vid = 0; vid<=numVerts; vid++) {
                    if (X[vid] == true) { // vid is in the set
                        chnode<LengthType> *n = chnodes[vidTOcid[vid]];
                        
                        int nodeID = c2[uf.findset(vid)];
                        n->parent = chnodes[nodeID];
                        n->parent->addChild(n);
                        #ifdef DEBUG
//                        printf("setting parent of %i to %i\n",n->id,n->parent->id);
                        #endif
                    }
                }
                // G.3.6.4
                for(set<undirectedLength>::iterator it = X2.begin(); it != X2.end(); it++) {
                    // G.3.6.4.1
                    vidTOcid[*it] = c2[*it];
                    int level = msb(mstEdges[i].length); 
                    LengthType delta;
                    if (level < 32) {
                        delta = (LengthType)ceil( (double)(s[*it]) / (1 << (level-1)) );
                    }
                    else {
                        delta = (LengthType)ceil( (double)(s[*it]) / pow(2.0,level-1) );
                    }
                    // G.3.6.4.2
                    #ifdef DEBUG
//                    printf("setting delta of %i to %li (sum=%li, msb=%i, len=%li)\n",
//                            vidTOcid[*it],delta,
//                            s[*it],msb(mstEdges[i].length),mstEdges[i].length);
                    #endif
                    chnodes[vidTOcid[*it]]->delta = delta;
                }
                // G.3.6.5
                #ifdef DEBUG
//                printf("clearing X\n");
                #endif
                //X.clear();
                for (int i=0; i<=numVerts; i++)
                    X[i] = false;

                delete[]c2;
            }
        }
        // G.4

        delete [] vidTOcid;
        delete [] s;
        delete [] X;
    }


        int msb(LengthType x) {
            int msb = 0;
            while (x > 0) {
                x = x >> 1;
                msb++;
            }
            return msb;
        }

        void thorup(Vertex<undirectedEdge> *vertices,
                    long numVerts,
                    Vertex<undirectedEdge> *pSource
                    ) {
            source = pSource;

            //dist = new LengthType[numVerts+1];
            //pred = new Vertex<undirectedEdge>*[numVerts+1];

            // copy to pointers that were passed in by reference
            //d = dist;

            // Initialization stuff
            for (int i=0; i<=numVerts; i++) {
                S[i] = false;
            }

            for (int i=0; i<=numVerts; i++) {
                pred[i] = NULL;
                dist[i] = INT_MAX;
            }

            int sz = chnodes.size();
            for (int i=0; i< sz; i++) {
                chnodes[i]->init();
            }

            dist[source->id] = 0;
            pred[source->id] = source;

            // Relax all the edges out of the source
            // and for every vertex adjacent to the source,
            // decrease its minD to the new value.
            undirectedEdge *it;
            #ifdef DEBUG
//            printf("Relaxing all vertices out of source(%i)\n",source->id);
            #endif
            for (it = source->first; it != source->last(); it++) {
                #ifdef DEBUG
//                printf("  Edge (%i,%i)\n",source->id,it->other->id);
                #endif
                if (dist[source->id] + it->length < dist[it->other->id]) {
                    dist[it->other->id] = dist[source->id] + it->length;
                    chnodes[it->other->id]->decreaseMinD(dist[it->other->id]);
                    pred[it->other->id] = source;
                    #ifdef DEBUG
//                    printf("    Setting dist[%i] to %li\n",it->other->id,dist[it->other->id]);
                    #endif
                }
            }
            
            // find the root chnode for the source's component;
            chnode<LengthType> *root = chnodes[source->id];
            while (root->parent != NULL)
                root = root->parent;
            #ifdef DEBUG
//            printf("Finding root chnode for source: chnode #%i\n",root->id);
            #endif


            root->isRoot = true;
            //S.insert(source);

            visitComponent(root);

            #ifdef DEBUG
            for (int i=0; i<=numVerts; i++) {
//                printf("dist[%i] = %li\n",i,dist[i]);
            }
            #endif
        }

        // Thorup's algorithm D.
        void expandComponent(chnode<LengthType> *n) {
//            #if DEBUG
//            printf("expand component: %i, level=%i, delta=%li\n",
//                    n->id,n->level,n->delta);
//            #endif
            // D.1
            n->ix0 = n->getMinD() >> (n->level-1);
            // D.2
            n->ixinf = n->ix0 + n->delta;
            // D.3 
            n->allocBuckets();
            // D.4.a
            n->isRoot = false;

 //           #if DEBUG
//            printf("  ix0 <- %li, ixinf <- %li\n",n->ix0,n->ixinf);
//            #endif


            // D.4.b and D.5
//            #if DEBUG
//            printf("  bucketing children of %i\n",n->id);
//            #endif
            //printf("chnode #%i has %i children.\n",n->id,n->children.size());
            typename list< chnode<LengthType>*>::iterator aChild;
            for (aChild = n->children.begin();
                 aChild != n->children.end(); 
                 aChild++) {
                if ((*aChild)->vertex == source)
                    continue;
                // D.4.b
                (*aChild)->isRoot = true;
                // D.5.1
                LengthType minD = (*aChild)->getMinD();
                
                LengthType index = (minD >> (n->level-1));
//                #if DEBUG
//                printf("    bucketing child %i with minD = %li and level = %i\n",
//                        (*aChild)->id,minD,n->level);
//                #endif
                if (index > n->ixinf)
                    index = n->ixinf;
                n->addToBucket(index - n->ix0,*aChild);
            } 
        }

        // Thorup's algorithm E
        void visitLeafComponent (chnode<LengthType> *n) {
            #ifdef DEBUG
//            printf("visit leaf component: %i\n",n->id);
            #endif

            typedef Vertex<undirectedEdge> vertex;

            vertex *v = n->vertex;
            assert(v!= NULL);
            if (S[v->id] == true)
                return;
            // E.1
            S[v->id] = true;
    
            // E.2
            #ifdef DEBUG
//            printf("Relaxing all vertices out of vertex: %i\n",v->id);
            #endif
            for (undirectedEdge *it = v->first; it != v->last(); it++) {
                vertex *w = it->other;

                #ifdef DEBUG
//                printf("  Edge (%i,%i)\n",v->id,w->id);
                #endif
                if ((dist[v->id] + it->length) < dist[w->id]) {
                    // E.2.1
                    //chnode<LengthType> *wh = chnodes[w->id]->unvisitedRoot();
                    //chnode<LengthType> *wi = wh->visitedParent();
                    //#ifdef DEBUG
                    //printf("    Unvisited Root of %i is node #%i\n",w->id,wh->id);
                    //printf("    Visited Parent of %i is node #%i\n",wh->id,wi->id);
                    //#endif

                    //LengthType oldVal = wh->getMinD() >> (wi->level - 1);
                    dist[w->id] = dist[v->id] + it->length;

                    // E.2.2
                    chnodes[w->id]->decreaseMinD(dist[w->id]);

                    pred[w->id] = v;

                    #ifdef DEBUG
//                    printf("    Setting dist[%i] to %li\n",w->id,dist[w->id]);
                    #endif
                }
            }
            if (n->parent == NULL) {
//                printf("No other vertices are reachable from the source.\n");
                return;
            }
            #ifdef DEBUG
//            printf("END visitLeafComponent(%i)\n",n->id);
            #endif
        }

        // Thorup's algorithm F
        void visitComponent (chnode<LengthType> *n) {

            #ifdef DEBUG
//            printf("Visit component (id = %i)\n",n->id);
            #endif
            // F.1
            if (n->level == 0) {
                // F.1.1
                visitLeafComponent(n);
                // F.1.2
                if (n->parent != NULL){
                    #ifdef DEBUG
//                    printf("  removing %i from parent %i\n",n->id,n->parent->id);
                    #endif
                    n->parent->removeFromBuckets(n);
                    //n->updateMinD(n->minD);
                }
                // F.1.3
                return;
            }

            // F.2
            if (n->isUnvisited()) {
                // F.2.1
                expandComponent(n);
                // F.2.2
                n->ix = n->ix0;
                #ifdef DEBUG
//                printf("  ix <- %li\n",n->ix);
                #endif
            }

            int j;
            if (n->parent != NULL)
                j = n->parent->level;
            else
                j = sizeof(LengthType)*8;
            LengthType base_shifted_ix = n->ix >> (j - n->level);

            #ifdef DEBUG
//            printf("comp: %i, n->ix: %li shifted: %li, j: %i, level: %i \n",n->id,n->ix,base_shifted_ix,j,n->level);
            #endif
            // F.3
            while (!n->isEmpty() && (n->ix >> (j - n->level)) == base_shifted_ix) {

                //cannot assert this! maintained call-wise!!!
                //assert ((n->ix >> (j - n->level)) == (n->minD >> (j-1)));
                //
                // this loop is necessary because the calls to visitComponent
                //  below could rebucket some children into this bucket.
                //  F.3.1
                while ( n->bucketCounts[n->ix - n->ix0] > 0) {
                    #ifdef DEBUG
//                    printf("node: %i, examining index:%li ix: %li, ix0: %li\n",
//                            n->id,n->ix-n->ix0,n->ix,n->ix0);
                    #endif
                    unsigned ind = (unsigned)n->ix-n->ix0;
//                    assert(ind < n->delta+1); // delta+1 = numBuckets

                    chnode<LengthType>* it = n->bucketStart(ind);

                    // F.3.1.1
                    chnode<LengthType>** toVisit = new chnode<LengthType>*[n->bucketCounts[ind]];
                    int count = n->bucketCounts[ind];
                    int k=0;
                    for (k=0; k < count; k++) {
                        assert (it != NULL);
                        toVisit[k] = it;
                        it = it->next;
                    }
                    for (k=0; k < count; k++) {
                        // F.3.1.2
                        //LengthType x = n->minD >> j-1;
                        visitComponent(toVisit[k]);
                        //if (n->minD >> j-1 > x) {
                       //     #ifdef DEBUG
                        //    printf("----> should be done with visitComponent(%i): minD was %li and now is %li\n",n->id,x,n->minD>>j-1);
                        //    #endif
                        //}
                    }
                    delete [] toVisit;
                }

                // F.3.2
                n->ix++;
                #ifdef DEBUG
//                printf("*** node: %i, incrementing ix, new ix=%li\n",n->id,n->ix);
                #endif
            }
            // F.4
            if (!n->isEmpty() && n->parent != NULL) {
                n->parent->removeFromBuckets(n);
                int bucketNum = (n->ix >> (n->parent->level - n->level)) - n->parent->ix0;
                n->parent->addToBucket(bucketNum,n);
            }
            else if (n->parent != NULL) {
                n->parent->removeFromBuckets(n);
            }
            if (n->parent == NULL && !n->isEmpty()) {
//                printf("uh oh\n");
                assert(false);
            }
            #ifdef DEBUG
//            printf("END visitComponent(%i)\n",n->id);
            #endif
        }


};

#endif
