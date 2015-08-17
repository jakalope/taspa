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

#include "region.h"
#include "../word/CurveWord.h"
#include <assert.h>
#include <map>
#include <utility>
#include <limits.h>

void ParseBoundary
    (location::Set& lin, location::Set& cvx, region::List& rvList, 
    bitmap* inputBmp, SquareLatticeWalker& walker, location::Vector& convex, 
    location::Vector& concave, location::Vector& straight, std::ostream& verbs,
    bool storeWords, std::ofstream& logfile)  throw (catch_all_exception) {

	int originalSize = walker.size();

	while (walker.size() > 0) 
	{
        verbs << originalSize - walker.size() 
              << " / " << originalSize << "  \r";
        verbs.flush();

		try {		    

        region rv(walker);
        rvList.push_back(rv);

        location::Vector::const_iterator start;
        location::Vector::const_iterator stop;

        start = rv.GetLinearVertices().begin();
        stop  = rv.GetLinearVertices().end();
        lin.insert(start, stop);
        
        start = rv.GetConvexVertices().begin();
        stop  = rv.GetConvexVertices().end();
        cvx.insert(start, stop);
        
        convex.insert(convex.end(),rv.convexSegment.begin(),
            rv.convexSegment.end());

        concave.insert(concave.end(),rv.concaveSegment.begin(),
            rv.concaveSegment.end());
            
        straight.insert(straight.end(),rv.straightSegment.begin(),
            rv.straightSegment.end());
        
        // Append vertex data to log file
        if (storeWords){
            logfile << "Abs: " << rv.GetAbsoluteWord() << std::endl;
            logfile << "Rel: " << rv.GetRelativeWord() << std::endl;
        }				

		}
		// Upon error, print messages to stderr
		catch (catch_all_exception e) {	throw e; }	
	}
}
		

// Create an empty container
region::region () { }


region::region (SquareLatticeWalker& walker)
{
    walker.ProcessNextBoundary();
    boundary = walker.GetBoundary();
    RelWord = walker.GetRelativeWord();
    AbsWord = walker.GetAbsoluteWord();
    initialized = (RelWord.length() != 0);
    FindConvexVertices(*walker.BitmapPtr());
}


int region::SetMajorVertices
		(bitmap& bmp, std::string& bword, char val) {

	size_t length = bword.length();
	bool setNextConvex = false;

	for (size_t n = 0; n < length; n ++) {

		size_t n_minus_1 = (n == 0)      ? length-1 : n-1;
		size_t n_plus_1  = (n == length) ? 0        : n+1;

		/* Is it convex or concave? */

		if (setNextConvex == true 
			&& IsConvexVertex(bmp, n) 
			&& bword[n] != 'T') {
			
				setNextConvex = false;
				bword[n] = val;
		}

		if (bword[n] == 'T') {
			if (IsConvexVertex(bmp, n)) bword[n] = val;
			setNextConvex = true;
			size_t prevConvex = PreviousConvex (bmp, n);
			if (prevConvex != INT_MAX)
				bword[prevConvex] = val;
		}
		
		else if (   bword[n] == 'L'
			     || bword[n] == 'R') {

			if (IsConvexVertex(bmp, n)) {
				bword[n] = val;
				setNextConvex = true;
				size_t prevConvex = PreviousConvex (bmp, n);
				if (prevConvex != INT_MAX)
					bword[prevConvex] = val;
			}
				
			else if (IsntConvexVertex(bmp, n_minus_1)
			&& IsConcaveVertex(bmp, n) 
			&& IsntConvexVertex(bmp, n_plus_1))

				bword[n] = val;			
		}
	}

	return 0;
}

size_t region::PreviousConvex(bitmap& bmp, size_t index) {
	
	index --;
	while (index < INT_MAX && IsntConvexVertex(bmp, index)) index --;
	
	return index;
}

bool region::IsntConvexVertex(bitmap& bmp, size_t index) {
	return !IsConvexVertex(bmp, index);
}


bool region::IsConvexVertex(bitmap& bmp, size_t index) {
	location v = boundary[index];
	return IsConvexLocation(bmp, v);
}


bool region::IsConvexLocation(bitmap& bmp, location v) {
	// Look for > 2 passible locations
	// out of 4 neighbors.

	// If this corner is not passible, its adjacent tiles should be.
	if (!bmp.IsPassible(v.x+1,v.y+1)) {
		if (bmp.IsPassible(v.x,v.y+1) &&
			bmp.IsPassible(v.x+1,v.y))
			return true;
	}

	if (!bmp.IsPassible(v.x+1,v.y-1)) {
		if (bmp.IsPassible(v.x,v.y-1) &&
			bmp.IsPassible(v.x+1,v.y))
			return true;
	}

	if (!bmp.IsPassible(v.x-1,v.y+1)) {
		if (bmp.IsPassible(v.x,v.y+1) &&
			bmp.IsPassible(v.x-1,v.y))
			return true;
	}

	if (!bmp.IsPassible(v.x-1,v.y-1)) {
		if (bmp.IsPassible(v.x,v.y-1) &&
			bmp.IsPassible(v.x-1,v.y))
			return true;
	}

	return false;
}


bool region::IsConcaveVertex(bitmap& bmp, size_t index) {
	// Look for > 2 passible locations
	// out of 4 neighbors.
	location v = boundary[index];
	return IsConcaveLocation(bmp, v);
}


bool region::IsConcaveLocation(bitmap& bmp, location v) {
	// If this corner is not passible, its adjacent tiles should _not_ be.
	if (! bmp.IsPassible(v.x+1,v.y+1)) {
		if (! bmp.IsPassible(v.x,v.y+1) &&
			! bmp.IsPassible(v.x+1,v.y))
			return true;
	}

	if (! bmp.IsPassible(v.x+1,v.y-1)) {
		if (! bmp.IsPassible(v.x,v.y-1) &&
			! bmp.IsPassible(v.x+1,v.y))
			return true;
	}

	if (! bmp.IsPassible(v.x-1,v.y+1)) {
		if (! bmp.IsPassible(v.x,v.y+1) &&
			! bmp.IsPassible(v.x-1,v.y))
			return true;
	}

	if (! bmp.IsPassible(v.x-1,v.y-1)) {
		if (! bmp.IsPassible(v.x,v.y-1) &&
			! bmp.IsPassible(v.x-1,v.y))
			return true;
	}

	return false;
}


int region::FindConvexVertices(bitmap& bmp) 
{	
    curveWord = CurveWord::GetCurveWord(bmp, boundary, RelWord);
    std::string cWord = curveWord;

    const int STRAIGHT = 0;
    const int CONCAVE  = 1;
    const int NONE     = 2;

    std::vector<size_t> X_index;

    for (size_t i = 0; i < cWord.size(); i ++)
        if (cWord[i] == 'X') X_index.push_back(i);
    
    // If any 'X's are found,
    if (X_index.size() > 0) {
        // In the word, add the principal string before the first 'X' to
        // the back of the word so we can parse all the way through
        // without missing a pattern.
        std::string::iterator first_X = cWord.begin() + X_index.front() + 1;
        cWord.append(cWord.begin(), first_X);
        X_index.push_back(cWord.length()-1);
    }
    
    else {
        concaveSegment.insert
            (concaveSegment.end(), boundary.begin(), boundary.end());
    }

    for (size_t x = 0; x+1 < X_index.size(); x ++) {
        
        size_t i,j = i = X_index[x]+1;  // Let i,j := index of current 'X'
        size_t next_x = X_index[x+1];   // Let next_x := index of next 'X'
        int inflection = NONE;
        
        while (i < next_x && inflection == NONE) {
            if (cWord[i] == 'x') inflection = CONCAVE;

            if (inflection == NONE && cWord[i] == 'C'){

                size_t k = i;
                if (k >= 1 && k+2 <= next_x &&
                    cWord[k-1] == 'F' && cWord[k+2] == 'F')
                        inflection = CONCAVE;
                
                k ++;
                while (k < next_x && cWord[k] == 'F') k ++;
                if (cWord[k] == 'C') inflection = CONCAVE;
            }
            i ++;
        }
        
        i = next_x;
        
        if (inflection == CONCAVE) while (j <= i) {
            concaveSegment.push_back(boundary[j%boundary.size()]);
            j ++;
        }
        
        if (inflection == STRAIGHT) while (j < i) {
            straightSegment.push_back(boundary[j%boundary.size()]);
            j ++;
        }
        
        else while (j < i) {
            convexSegment.push_back(boundary[j%boundary.size()]);            
            j ++;
        }
    }
                
	// Insert the newly minted vertices into linearVertex and return
	
	// this may be incorrect: cWord is longer than boundary;
	// start from first 'X' ?
	if (X_index.size() > 1) 
	{
        /* For true digital straightness detection */
//        std::string word = 
//            InterCurveWord::GetCurvatureWord(bmp,boundary,RelWord);
//
//        Stair::Case cx = InterCurveWord::Find_CX_Staircases(word);
//        Stair::Case xc = InterCurveWord::Find_XC_Staircases(word);
//        
//        CellularWord lineWord(cx, xc, word);
//        std::cerr << (std::string)lineWord << std::endl;
//
//        for (size_t n = 0; n < lineWord.length(); n ++)
//            if (lineWord[n] == 's' && curveWord[n] != 'x') {
//                cWord[n] = 's';
//                cWord[n+boundary.size()] = 's';
//                straightSegment.push_back( boundary[n%boundary.size()] );
//            }
        /* (end) */

        for (size_t n = X_index[1]; n < X_index.back(); n ++)
            if (cWord[n] == 'X' && n != X_index[X_index.size()/2])
                convexVertex.push_back( boundary[n%boundary.size()] );
        
        location loc(boundary[X_index[X_index.size()/2]%boundary.size()-1]);
        if (IsConvexLocation(bmp,loc)) convexVertex.push_back(loc);        
	}
	
	return 0;
}


//===================================================================

// IsBorder: Returns true if given location is passible and borders impassible
//      Input:  loc = location to test
//      Return:	true for 'is a border', else false

bool region::IsBorder(const location& loc) {
	for (location::Vector::iterator n = boundary.begin();
		*n != loc; n++)

	if (*n == loc) return true;
	return false;
}


//===================================================================

// PaddedWord: Last letter appended to the front and first letter to the back
//      Input:  word = word to pad
//      Return:	padded word

std::string region::PaddedWord ( std::string word ) {

	std::string::iterator letter = word.begin();

	word.insert(letter,*(word.end()-1));

	word += word[1];

	return word;
}


//===================================================================

// LogVertices: Send textual log of what was found to outfile
//      Input:  outfile = output stream for log file
//      Return:	pointer to the output stream

std::ofstream &region::LogVertices ( std::ofstream &outfile ) {

	for (location::VectorIter itr = linearVertex.begin();
				itr != linearVertex.end(); itr ++) {

		outfile << "Vertices:";
		outfile << "( " << int(itr->x) << ", " << int(itr->y) << " )  ";
	}

	outfile << "\n";

	return outfile;
}


std::string region::toString(int h) {
	return LogLines(linearVertex, h);
}


std::string LogLines(location::Vector vertices, int h) {

	char logBuffer[64];
	std::string log;

	int max_y = h-1;

	if (vertices.size() < 3) return log;

	sprintf(logBuffer,"%u %u", 
		(vertices.front().x), (max_y-vertices.front().y));
	log = log + logBuffer;

	for (location::VectorIter itr = vertices.begin()+1;
				itr != vertices.end()-1; itr ++) {

		sprintf(logBuffer,"\t%u %u\n%u %u", 
			(itr->x), (max_y-itr->y), (itr->x), (max_y-itr->y));
		log = log + logBuffer;
	}

	sprintf(logBuffer,"\t%u %u\n", 
		(vertices.back().x), (max_y-vertices.back().y));
	log = log + logBuffer;

	return log;
}


