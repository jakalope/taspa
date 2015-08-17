#include "PotentialLine.h"
#include <limits.h>
#include <stdlib.h>
#include <math.h>

/// Precondition: s must be a container of a single stair type (CX xor XC)
PotentialLine::PotentialLine
(Stair::Case& s) : index(s.front().first) 
{
    int a = INT_MAX;
    int b = INT_MAX;
    size_t a_count = 0;
    size_t b_count = 0;
    
    Stair::Case::iterator i;
    for (i = s.begin(); i != s.end(); i ++) 
    {
        // The symbol we're looking at should correspond to 
        // the symbols in "Cellular Lines"
        int symbol_i = i->second+1;
        
        // Find the first symbol
        if (a == INT_MAX) a = symbol_i;
        
        // Find the second symbol
        else if (b == INT_MAX && a != symbol_i) 
            b = symbol_i;
        
        // If this is an instance of the first symbol,
        if (a == symbol_i) {
            a_count ++;
            symbol.push_back(a);
        }
        
        // If this is an instance of the second symbol,
        else if (b == symbol_i) {
            b_count ++;
            symbol.push_back(b);
        }
        
        // If it is neither symbol, we have changed alphabets; stop.
        else break;
    }
    
    SetInfo(a,b,a_count,b_count);
}


void PotentialLine::GetSymbolInfo() 
{
    size_t a = INT_MAX;
    size_t b = INT_MAX;
    size_t a_count = 0;
    size_t b_count = 0;
    
    size_t i;
    for (i = 0; i < symbol.size(); i ++) {
        
        if (a == INT_MAX) a = symbol[i];
        else if (b == INT_MAX && a != symbol[i]) b = symbol[i];
        
        if (a == symbol[i]) a_count ++;
        else if (b == symbol[i]) b_count ++;
        
        else break;
    }
    
    SetInfo(a, b, a_count, b_count);
}


void PotentialLine::SetInfo
(int a, int b, size_t a_count, size_t b_count)
{
    // No symbols found.
    if (a == INT_MAX) {
        majoritySymbol = 0;
        minoritySymbol = 0;
        majorityCount = 0;
        minorityCount = 0;
        r = 0;
        c = 0;
    }
    
    // only one symbol found
    else if (b == INT_MAX) {
        majoritySymbol = a;
        minoritySymbol = 0;
        majorityCount = a_count;
        minorityCount = 0;
    }
    
    // a is the majority symbol
    else if (a_count > b_count) {
        majoritySymbol = a;
        minoritySymbol = b;
        majorityCount = a_count;
        minorityCount = b_count;
    }
    
    // b is the majority symbol
    else {
        majoritySymbol = b;
        minoritySymbol = a;
        majorityCount = b_count;
        minorityCount = a_count;
    }

    r = majoritySymbol * majorityCount + minoritySymbol * minorityCount;
    c = symbol.size();
}


/* "A string of integers is said to be fully partitioned when the following
    conditions are met:
    
    (Predcondition)
    (1) A finite string over an alphabet of two distinct, consecutive 
    non-negative symbols... any finite string with only one distinct symbol is 
    fully partitioned, since the second symbol is 0.
    
    (2)When the two symbols do not occur the same number of times, the 
    minority symbol partitions the string into |m|+1 non-empty
    substrings of majority symbols, called majority symbol substrings,
    where |m| denotes the number of times the minority symbol occurs
    in the substring.

    (3)When the two symbols occur the same number of times, the 
    minority symbol partitions the string into |m| non-empty substrings
    of majority symbols..." - Geer, McLaughlin, Unsworth (2003).
*/    
bool PotentialLine::IsFullyPartitioned() 
{ 
    // (1) This is the simple case. Only one symbol occurs.
    if (minoritySymbol == 0 && majoritySymbol != 0) return true;
    
    // We acheive testing for (2) by making sure no two minority symbols
    // occur as neighbors, nor do they occur at the end-points.
    if (majorityCount != minorityCount) {
        if (symbol.front() == minoritySymbol) return false;
        if (symbol.back()  == minoritySymbol) return false;
    }

    // Testing for (3) only requires a check that no neighboring minorities
    // occur.
    for (size_t i = 1; i < symbol.size(); i++) 
        if (symbol[i] == symbol[i-1] && symbol[i] == minoritySymbol) 
            return false;
    
    // Passed all the tests.
    return true;
}


/* For all integers, j ≥ 1, if S( j−1) is fully partitioned,
      i. replace each of the majority symbol substrings of S ( j−1), 
        by its length,
     ii. delete the minority symbols of S( j−1),
    iii. define the resulting string to be the jth order derived string 
        of S and denote it by S ( j) .
*/
PotentialLine PotentialLine::Derivative() 
{
    // (iii)
    PotentialLine dl;

    // derivative will be nonlinear.  
    if (!IsFullyPartitioned()) return dl;
    
    // find derivative.
    size_t i=0;
    while (i < symbol.size()) 
    {
        
        // (i)
        size_t j = i;
        while (i < symbol.size() && symbol[i] == majoritySymbol) { i++; }
        if (i != j) { dl.symbol.push_back(i-j); }
        
        // (ii)
        while (i < symbol.size() && symbol[i] != majoritySymbol) { i++; }
    }

    // fill in symbol information
    dl.GetSymbolInfo();
    
    return dl;
}


bool PotentialLine::IsLine() 
{
    // Minority symbol found but majority symbol and minority symbol are
    // diff by more than 1.
    bool symbolSizesImplyNonlinear = 
        (minoritySymbol != 0) && (abs(majoritySymbol - minoritySymbol) > 1);
        
    symbolSizesImplyNonlinear = 
        symbolSizesImplyNonlinear || (majorityCount == 0);

    if (symbolSizesImplyNonlinear) return false;
        
    // From Existance assertion (i)        
    // for 1 <= c <= r :
    if (1 <= c && c <= r) 
    {

        size_t letter = (r - (r%c))/c;
        
        bool majoritySymIsInAlphabet = 
            (majoritySymbol == letter || majoritySymbol == letter+1);
        
        bool minoritySymIsInAlphabet = 
            (minoritySymbol == letter || minoritySymbol == letter+1);
        
        // ... any finite string with only one distinct symbol is 
        // fully partitioned, since the second symbol is 0.
        minoritySymIsInAlphabet = 
            minoritySymIsInAlphabet || minoritySymbol == 0;
        
        if (!majoritySymIsInAlphabet || !minoritySymIsInAlphabet)
            return false;
    }
    
    // From definition of cellular line.
    PotentialLine dl = Derivative();
    
    while(dl.IsFullyPartitioned() && dl.symbol.size() > 1) {
        dl = dl.Derivative();
    }
    
    return dl.symbol.size() == 1;
}
    

