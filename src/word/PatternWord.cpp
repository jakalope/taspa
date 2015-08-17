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

#include "PatternWord.h"
#include <iostream>

const int PatternWord::LETTER_CUTOFF = 256;

// Stores: A pattern-word occupied by {a,b,c,...}'s representing specified
//		patterns.
// Inputs:
//		word -- Word to convert to a pattern-word.
//		pattern -- Pattern from which to start new counter.
//		inc -- Counting letter.
PatternWord::PatternWord
		(const std::string& word, const std::string& pattern, char inc) {
	Initialize(word,pattern,inc);
}

void PatternWord::Initialize
		(const std::string& word, const std::string& pattern, char inc) {

	__length = 0;
	size_type _index = 0;

	while (_index < word.length()) {
		
		bool foundPattern = word.substr(_index, pattern.length()) == pattern;
		
		if (!foundPattern) { 
			/* push_back next Letter. */
			push_back((int)word[_index]);
			_index ++;
		}

		else { 
			size_type _prev = _index;
			_index +=  pattern.length();
			while (word[_index] == inc && _index < word.length()) _index ++; 
			
			/* push_back patterns as their length + LETTER_CUTOFF, since */
			/* a pattern's uniqueness is its length and because we */
			/* don't want colisions with Letters. */
			push_back(_index - _prev + LETTER_CUTOFF);
		}
//		int value = FindPattern(word, _index, pattern, inc);
//		if (value >= LETTER_CUTOFF) _index += value-LETTER_CUTOFF;
//		else _index ++;
//		push_back(value);

	}
}


int PatternWord::FindPattern
		(const std::string& word, 
		 size_t start, 
		 const std::string& pattern, 
		 char inc) {
		 	
	///////////////////////////////////////////////////
	// Check for pattern[0], inc, inc, ..., pattern[1].
	///////////////////////////////////////////////////
	
	// If word[start] is not pattern[0], return word[start].
	if (word[start] != pattern[1]) return word[start];
	size_t index = start+1;
	
	// If word[start] is pattern[0], check for inc or pattern[1] at word[n].
	// If inc is found, keep looking for pattern[1].
	// If pattern[0] or 'B' is found before pattern[1], return word[start].
	while (word[index] == inc) index ++;
	
	// Return letter value or pattern value.
	return (word[index] == pattern[0]) ? 
		index-start+LETTER_CUTOFF : word[start];
}


void PatternWord::AssignLetterSet(iterator& index, iterator& stop) {		
	while (index != stop && *index < LETTER_CUTOFF) {
		push_back(*index);
		index ++;
	}
}

void PatternWord::AssignPatternSet(iterator& wIndex, iterator& wEnd) {		

	bool thirdPatternFound = false;
	bool aLetter = false;
	int first = 0;
	int second = 0;
	
	// incriment storage for letters or a third pattern.
	while (wIndex != wEnd && !thirdPatternFound && !aLetter) {

		// check for letter
		if (*wIndex < LETTER_CUTOFF) aLetter = true;
			
		// push_back length of first pattern and add to pFilter
		else if (first == 0) {
			first = *wIndex; 
			push_back(*wIndex); 
			wIndex ++;
		}
			
		// push_back length of second pattern and add to pFilter
		else if (*wIndex != first && second == 0) {
			second = *wIndex; 
			push_back(*wIndex); 
			wIndex ++;
		}
			
		// check for third pattern
		else if (*wIndex != first && *wIndex != second)
			thirdPatternFound = true;
		
		// add to pFilter
		else {
			push_back(*wIndex);
			wIndex ++;
		}		
	}
}

// FilteredPattern:
// Inputs:
//		word -- A pattern-word containing Letters and pattern lengths.
// Returns: A list of pattern-words occupied by Letters and by up to two 
//		unique pattern lengths each.
PatternWord::PatternWordList PatternWord::GetFilteredPattern () {
	PatternWordList filtered;
	
	iterator wIndex = begin();
	iterator wEnd = end();
	
	// iterate through word;
	while (wIndex != wEnd) {
		
		// Add letters to an entry.
		{ 
			PatternWord pFilter;
			pFilter.AssignLetterSet(wIndex, wEnd);
			if (pFilter.size() != 0) filtered.push_back(pFilter);
		}
		
		// Add patterns to an entry.
		{		
			PatternWord pFilter;
			pFilter.AssignPatternSet(wIndex, wEnd);
			if (pFilter.size() != 0) filtered.push_back(pFilter);
		}
	}
	
	return filtered;
}

std::string PatternWord::decompress(iterator pattern) {
	int _size = *pattern - LETTER_CUTOFF;
	
	std::string str;
	
	if (_size > 0) {
		str.assign(_size, 'O');
		str[0] = 'V';
	}
	
	return str;
}

std::string PatternWord::toString() {
	
	std::string out;
	out.reserve(__length);
	
	for (iterator iter = begin(); iter != end(); iter ++) {
		if (*iter < LETTER_CUTOFF) out += (char)(*iter);
		else out.append(decompress(iter));
	}
	
//	std::cout<<"toString() "<<out<<"\n";
	
	return out;
}
