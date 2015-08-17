#ifndef POTENTIAL_LINE_H
#define POTENTIAL_LINE_H

#include <vector>
#include <string>
#include <set>

#include "Stair.h"

class PotentialLine
{
    public:
    
    typedef std::set<PotentialLine> Set;
    typedef std::vector<size_t> SymbolVector;

    protected:
    
    std::vector<size_t> symbol;
    
    size_t majoritySymbol;
    size_t minoritySymbol;
    
    size_t majorityCount;
    size_t minorityCount;
    
    size_t index;
    
    size_t r, c;
    
    public:

    PotentialLine() : majoritySymbol(0), minoritySymbol(0), majorityCount(0), 
            minorityCount(0), index(0), r(0), c(0) { symbol.clear(); }
    
    // Precondition: s must be a container of a single stair type (CX xor XC)
    PotentialLine(Stair::Case& s);    
    
    void GetSymbolInfo();
        
    void SetInfo(int a, int b, size_t a_count, size_t b_count);    
    
    bool operator < (PotentialLine p) 
        { return r < p.r || (r == p.r && symbol < p.symbol); }
            
    /* "A string of integers is said to be fully partitioned when the following
        conditions are met:
        
        (Predcondition)
        (1) A finite string over an alphabet of two consecutive positive 
        integers... [any finite string with only one distinct symbol] is 
        fully partitioned.
        
        (2)When the two symbols do not occur the same number of times, the 
        minority symbol partitions the string into |m|+1 non-empty
        substrings of majority symbols, called majority symbol substrings,
        where |m| denotes the number of times the minority symbol occurs
        in the substring.

        (3)When the two symbols occur the same number of times, the 
        minority symbol partitions the string into |m| non-empty substrings
        of majority symbols..." - Geer, McLaughlin, Unsworth (2003).
    */    
    bool IsFullyPartitioned();
    
    /* "For all integers, j ≥ 1, if S(j−1) is fully partitioned,
          i. replace each of the majority symbol substrings of S (j−1), 
            by its length,
         ii. delete the minority symbols of S( j−1),
        iii. define the resulting string to be the jth order derived string 
            of S and denote it by S ( j) ."
        - Geer, McLaughlin, Unsworth (2003).
    */
    PotentialLine Derivative();    
    
    // For failed potential lines, remove last symbol to try again.
    void RemoveLastSymbol()
    {
        symbol.pop_back();
        GetSymbolInfo();
    }
    

    bool IsLine();
        
    // If we begin with something from the right alphabet but it makes multiple
    // lines, we need to extract only the first line.
    void ShortenUntilLinear() {
        while (symbol.size() > 1 && !IsLine()) RemoveLastSymbol();
    }
    

    /* Occurs at least as frequently as the minority symbol. */
    size_t MajoritySymbol() { return majoritySymbol; }
    
    /* Occurs at most as frequently as the majority symbol. */
    size_t MinoritySymbol() { return minoritySymbol; }
    
    // Return the length of the string used to generate this potential line.
    // "+ symbol.size()" corrects the offset from the alphabet mapping.
    size_t length() const { return r + symbol.size(); }

    size_t Index() const { return index; }
    
    size_t MajorityCount() const { return majorityCount; }
};

#endif
