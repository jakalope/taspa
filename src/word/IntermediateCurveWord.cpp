/* 
 * Copyright 2009, 2010, Jake Askeland, jake(dot)askeland(at)gmail(dot)com
 * 
 *  * This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *  * This file is part of Topological all shortest paths automatique' (TASPA).
 * 
 *     TASPA is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     TASPA is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with TASPA.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "IntermediateCurveWord.h"
#include "../region/region.h"
#include <utility>

InterCurveWord::InterCurveWord
(bitmap& bmp, const location::Vector& boundary, const std::string& relWord) :
    std::string(SetConcaveCurvature(GetCurvatureWord(bmp,boundary,relWord)))
{ }


std::string InterCurveWord::GetCurvatureWord
(bitmap& bmp, const location::Vector& boundary, const std::string& relWord) 
{
    std::string word = relWord;
    
    char Convex  [2] = { 0, 'X' };
    char Concave [2] = { 0, 'C' };

    size_t n = 0;
    for (; n < relWord.length(); n++)
    {
        if ((relWord[n] == 'L' || relWord[n] == 'R') && 
            region::IsConvexLocation(bmp,boundary[n%boundary.size()])) {
            Convex [0] = relWord[n];
            Concave[0] = ((Convex[0] == 'R') ? 'L' : 'R');
            break;
        }
        
        if ((relWord[n] == 'L' || relWord[n] == 'R') && 
            region::IsConcaveLocation(bmp,boundary[n%boundary.size()])) {
            Concave[0] = relWord[n];
            Convex [0] = ((Concave[0] == 'R') ? 'L' : 'R');
            break;
        }
        
    }
    
    for (n = 0; n < relWord.length(); n++) 
    {
        if      (relWord[n] == Convex [0]) word[n] = Convex [1];
        else if (relWord[n] == Concave[0]) word[n] = Concave[1];
    }    

    return word;
}


Stair::Case InterCurveWord::Find_CX_Staircases(const std::string& curveWord)
{
    std::string pattern = "CFX";
    Stair::Case CX_stairs;
    size_t F_string=0;
    
	for (size_t _index=0; _index < curveWord.length();) {
		
		bool foundPattern = 
            curveWord.substr(_index, pattern.length()) == pattern;

        // Didn't find the pattern
		if (!foundPattern) { 
            // Store Fs that precede the pattern as length F_string.
		    if (curveWord[_index] == 'F') F_string++;
		    // If curveWord_i is not 'F', restart F_string at 0.
		    else F_string = 0;
			_index ++;
		}

        // Found the pattern
		else { 
            // Move index past the pattern
			_index +=  pattern.length();
			// Store a stair of length F_string
			CX_stairs.push_back( Stair(_index-1,F_string) );
			// Restart F_string from 0.
			F_string = 0;
		}
	}
	
	return CX_stairs;
}


Stair::Case InterCurveWord::Find_XC_Staircases(const std::string& curveWord)
{
    const std::string pattern = "XC";
    Stair::Case XC_stairs;
    const char inc = 'F';

	for (size_t _index=0; _index < curveWord.length();) {
		
		bool foundPattern = 
            curveWord.substr(_index, pattern.length()) == pattern;
		
		if (!foundPattern) { 
			_index ++;
		}

		else { 
		    size_t _offset = _index;
			_index +=  pattern.length();
			size_type _prev = _index;

            // Store Fs that succede the pattern.
			while (curveWord[_index] == inc && _index < curveWord.length()) 
                _index ++; 
			
			XC_stairs.push_back( Stair(_offset, _index - _prev) );
		}
	}
	
	return XC_stairs;
}


std::string InterCurveWord::SetConcaveCurvature(std::string curveWord)
{
    // The last step in a CX...CX staircase is not at risk.
    // The first step in a XC...XC staircase is not at risk.
    // The reverse are at risk of being considered concave.
    
    // Look for staircases.
    Stair::Case CX_stairs = Find_CX_Staircases(curveWord);
    Stair::Case XC_stairs = Find_XC_Staircases(curveWord);
        
    Stair::Case::iterator i;
    Stair::Case::iterator j;

    // For each staircase, look for concave ascent.  Change X to x.
    if (CX_stairs.size() > 0) {
        i = CX_stairs.begin();
        j = CX_stairs.begin();
        while ( ++j != CX_stairs.end() ) 
        {
            bool isAdjacent = j->first - i->first == j->second + 3;
            if (isAdjacent && i->second > j->second) 
                curveWord[i->first] = 'x';

            /* Added to handle single step size straightness without
                dealing with true digital straightness */
            else if (isAdjacent && i->second == j->second)
                curveWord[i->first] = 's';
            /* (end) */
            
            i ++;
        }
    }
    
    // For each staircase, look for concave descent. Change X to x.
    if (XC_stairs.size() > 0) {
        i = XC_stairs.begin();
        j = XC_stairs.begin();
        while ( ++j != XC_stairs.end() ) 
        {
            bool isAdjacent = j->first - i->first == i->second + 2;
            if (isAdjacent && i->second < j->second) 
                curveWord[j->first] = 'x';

            /* Added to handle single step size straightness without
                dealing with true digital straightness */
            else if (isAdjacent && i->second == j->second)
                curveWord[j->first] = 's';
            /* (end) */

            i ++;
        }
    }
    
    return curveWord;
}
