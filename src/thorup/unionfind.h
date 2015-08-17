/*
 *  unionfind.h
 *  
 *
 *  Created by Joseph Crobak on 02/22/06.
 *  Modified and included in Taspa by Jake Askeland, with explicit written 
 *  consent from Joseph Crobak on June 20, 2009.
 */
#ifndef UNIONFIND_H
#define UNIONFIND_H 
 
class UnionFind {
public:

    UnionFind(int sz) {
        size = sz+1;
        p = new int[size];
        rank = new int[size];
        for (int i=0; i<size; i++) {
            p[i] = i;
            rank[i] = 0;
        }
    }

    ~UnionFind() {
        delete[] p;
        delete[] rank;
    }
    
    void u(int x, int y) {
        link(findset(x),findset(y));
    }
    

    void link(int x, int y) {
        if (rank[x] > rank[y] ) {
            p[y] = x;
        }
        else {
            p[x] = y;
            if (rank[x] == rank[y]) {
                rank[y]++;
            }
        }
    }

    int findset(int x) {
        if (x != p[x])
            p[x] = findset(p[x]);

        return p[x];
    }

private:
	
	int size;
    int *p;
    int *rank;
};

#endif

