#include "CellularWord.h"

CellularWord::CellularWord
(Stair::Case& cx, Stair::Case& xc, std::string& word) : std::string(word)
{
    Stair::Case::iterator i;
    Stair::Case::iterator j;
    std::list<Stair::Case> CaseList;
    Stair::Case staircase;
    SimpleLineList R,S;
    bool wasAdjacent = false;
    
    
    // Look through cx staircases for sets of adjacent cases
    if (cx.size() > 0) 
    {
        i = cx.begin();
        j = cx.begin();
        
        j ++;
        
        while ( j != cx.end() ) 
        {
            bool isAdjacent = j->first - i->first == j->second + 3;
            
            // Start a new potential line staircase.
            if (isAdjacent && !wasAdjacent) {
                staircase.clear();
                staircase.push_back(*i);
                staircase.push_back(*j);
            }
            
            // Continue an existing potential line staircase.
            else if (isAdjacent && wasAdjacent) {
                staircase.push_back(*j);
            }
            
            // Finish the current potential line staircase.
            else if (!isAdjacent && wasAdjacent) {
                CaseList.push_back(staircase);
                staircase.clear();
            }

            wasAdjacent = isAdjacent;
            i ++;
            j ++;
        }
    }

    if (wasAdjacent) {
        CaseList.push_back(staircase);
        staircase.clear();
    }

    // Each i is a staircase of adjacent cx stairs.
    // We are adding the cases to two sets: 
    //    (S) is the forward staircases
    //    (R) is the reverse staircases
    for (std::list<Stair::Case>::iterator i = CaseList.begin(); 
            i != CaseList.end(); i ++) {
                
        S.push_back(GetLongestLine(i->begin(),  i->end(),  word));
        R.push_back(GetLongestLine(i->rbegin(), i->rend(), word));
    }

    CaseList.clear();

    // Look through xc staircases for sets of adjacent cases
    if (xc.size() > 0) 
    {
        i = xc.begin();
        j = xc.begin();
        
        wasAdjacent = false;
        j ++;
        
        while ( j != xc.end() ) 
        {
            bool isAdjacent = j->first - i->first == i->second + 2;

            // Start a new potential line staircase.
            if (isAdjacent && !wasAdjacent) {
                staircase.clear();
                staircase.push_back(*i);
                staircase.push_back(*j);
            }
            
            // Continue an existing potential line staircase.
            else if (isAdjacent && wasAdjacent) {
                staircase.push_back(*j);
            }
            
            // Finish the current potential line staircase.
            else if (!isAdjacent && wasAdjacent) {
                CaseList.push_back(staircase);
                staircase.clear();
            }

            wasAdjacent = isAdjacent;

            i ++;
            j ++;
        }
    }

    if (wasAdjacent) {
        CaseList.push_back(staircase);
        staircase.clear();
    }
    
    // Each i is a staircase of adjacent xc stairs.
    // We are adding the cases to two sets: 
    //    (S) is the forward staircases
    //    (R) is the reverse staircases
    for (std::list<Stair::Case>::iterator i = CaseList.begin(); 
            i != CaseList.end(); i ++) {
                
        S.push_back(GetLongestLine(i->begin(),  i->end(),  word));
        R.push_back(GetLongestLine(i->rbegin(), i->rend(), word));
    }
            
    SimpleLineSet lineSet;

    for (LLineIter r = R.begin(); r != R.end(); r ++)
        lineSet.insert(*r);
    for (LLineIter s = S.begin(); s != S.end(); s ++)
        lineSet.insert(*s);
        
//    AddToLineSet(S.begin(), S.end(), R.begin(), R.end(), lineSet);
//    AddToLineSet(R.begin(), R.end(), S.begin(), S.end(), lineSet);

    SLineIter I;
    for (I = lineSet.begin(); I != lineSet.end(); I++)
        ReplaceIntermediates(*I, 's', word);
}


void CellularWord::AddToLineSet
(LLineIter A_begin, LLineIter A_end, 
 LLineIter B_begin, LLineIter B_end, SimpleLineSet& C)
{
    for (LLineIter r = A_begin; r != A_end; r ++) {
        LLineIter s_plus_1 = B_begin;
        LLineIter s;
        
    for (s = B_begin; s != B_end && s->Index() < r->Index(); s = s_plus_1) 
    {
        s_plus_1 = s; s_plus_1 ++;
        
        bool beginning_of_r_falls_between_endpts_of_s =
            s->Index() < r->Index() && r->Index() <= s_plus_1->Index();
            
        bool end_of_r_falls_after_end_of_s =
            r->Index() + r->length() > s_plus_1->Index();
        
        if (beginning_of_r_falls_between_endpts_of_s &&
            end_of_r_falls_after_end_of_s) 
        {
            // If s is shorter than or equal in length to r,
            if (s->length() <= r->length()) {
                // Cut off s at beginning of r
                s->length() = r->Index() - s->Index();            
            }
            
            // If r is shorter than s,
            else {
                // Cut off r at the end of s
                int diff = r->Index();
                r->Index() = s->Index() + s->length();
                diff = r->Index() - diff;
                r->length() -= diff;
            }

            // This should ensure any given line gets cut down
//            C.insert(*r);
//            C.insert(*s);
//            break;
        }
        
        else C.insert(*s);
    }
    }
}


void CellularWord::ReplaceIntermediates
(const SimpleLine& p, char val, std::string& word)
{
    // Starting from p.index, until p.length...
    int beginIndex = p.getIndex();
    int endIndex = p.getIndex() + p.getLength();
    
//    while (beginIndex+1 < word.length() &&
//           word[beginIndex] != 'X') beginIndex ++;
//    
//    beginIndex++;
//
//    while (beginIndex+1 < word.length() &&
//           word[beginIndex] != 'X') beginIndex ++;
//    
//    beginIndex++;
//
//    while (endIndex > 1 && word[endIndex] != 'X') endIndex --;
//    endIndex --;
//
//    while (endIndex > 1 && word[endIndex] != 'X') endIndex --;
//    endIndex --;
//    
//    while (endIndex > 1 && word[endIndex] != 'X') endIndex --;
//    endIndex --;
    
    // Replace 'x' and 'X' with 's'
    for (int i = beginIndex; i < endIndex; i ++)
        if (word[i] == 'X' || word[i] == 'x') at(i%this->length()) = val;
}

