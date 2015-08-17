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

#ifndef PATTERNWORD_H
#define PATTERNWORD_H

#include <string>
#include <list>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
/** 
	@version 0.1.0 Build 0
	@author J. Askeland
	@memo
*/
class PatternWord : public std::vector<int> {
	private:
	
	int __length;
	
	protected:

	typedef std::list<PatternWord> PatternWordList;

	int FindPattern
			(const std::string& word, 
			 size_t start, 
			 const std::string& pattern, 
			 char inc);
	void AssignLetterSet(iterator& index, iterator& stop);
	void AssignPatternSet(iterator& wIndex, iterator& wEnd);
	std::string decompress(iterator pattern);
	
	public:

	/////////////////////////////////////////////////////
	/** @name Constructors **/
	//@{

	PatternWord() : __length(0) { }

	// Stores: A pattern-word occupied by {a,b,c,...}'s representing specified
	//		patterns.
	// Inputs:
	//		word -- Word to convert to a pattern-word.
	//		pattern -- Pattern from which to start new counter.
	//		inc -- Counting letter.
	PatternWord(const std::string& word, const std::string& pattern, char inc);

	//@}


	/////////////////////////////////////////////////////
	/** @name Operators **/
	//@{

	//@}


	/////////////////////////////////////////////////////
	/** @name Public Members **/
	//@{
	
	static const int LETTER_CUTOFF;
	
	void push_back(int _val) {
		__length += (_val >= LETTER_CUTOFF) ? _val-LETTER_CUTOFF : 1;
		std::vector<int>::push_back(_val);
	}
	
	size_t GetLength() { return __length; }
	
	PatternWordList GetFilteredPattern ();
	
	void Initialize
		(const std::string& word, const std::string& pattern, char inc);
	
	std::string toString();
	
	std::string GetLinearVertex(size_t i) {
		int len = at(i) - LETTER_CUTOFF;
		if (len > 0) {
			std::string out(len, 'O');
			out[0] = 'V';
			return out;
		}
		return "";
	}
	
	size_t GetElementLength(size_t i) {
		int len = at(i) - LETTER_CUTOFF;
		if (len < 1) len = 1;
		return len;
	}
	
	size_t GetUncompressedLength(size_t begin, size_t comp_length) {
		size_t out = 0;
		for (size_t i = begin; i < comp_length+begin; i ++)
			out += GetElementLength(i);
		return out;
	}
		
	//@}
};

#endif

