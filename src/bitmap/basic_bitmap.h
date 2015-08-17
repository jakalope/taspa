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

#ifndef BASIC_BITMAP_H
#define BASIC_BITMAP_H

#include <string>
#include <stdexcept>
#include <map>

#include "../exception/catch_all_exception.hpp"
#include "bitmap_typedef.hpp"
#include "../location/location.hpp"

////////////////////////////////////////////////////////////////////////////////
/**	Base class for various bitmap formats. Contains header information and 
	virtual functions for basic bitmap functionality.

	@version 0.1.0 Build 0
	@author J. Askeland
	@memo
*/
class basic_bitmap {
	protected:
		bool				initialized;	/* 0 for success, else error code */
		int 				pixelCount;		/* Number of pixels in vector */
		int					paddedByteWidth;    /* Width of the bitmap file */
		header				bmiHeader;		/* Image header */
		info_header			bmiInfoHeader;	/* Info header */
		unsigned short      palletSize;
		rgba::Vector          colorTable;
//		std::map<long long,location::Vector>	SightHash;		// hash set of visible distances

		void LoadHeader      (std::ifstream& bitmapFile);
		void LoadColorTable  (std::ifstream& bitmapFile);
		char* ReadRawFile (const std::string &fileName) 
				throw (catch_all_exception);
				
		void WriteHeader     (std::ofstream& bitmapFile);
		void WriteColorTable (std::ofstream& bitmapFile);

	public:

		/////////////////////////////////////////////////////
		/** @name Type definitions **/
		//@{

		typedef bool mono;
		enum { black, white };
		
		//@}


		/////////////////////////////////////////////////////
		/** @name Constructors **/
		//@{

		/** Create an empty bitmap container */
		basic_bitmap();

		//@}


		/////////////////////////////////////////////////////
		/** @name Operators **/
		//@{

		//@}


		/////////////////////////////////////////////////////
		/** @name Public Members **/
		//@{
		
		/* Various Getters */
		int GetWidth()  { return bmiInfoHeader.width;  }
		int GetHeight() { return bmiInfoHeader.height; }
		int GetPaddedByteWidth() { return paddedByteWidth; }
		image_format GetFormat(const std::string &fileName)
				throw (catch_all_exception);

		//@}

};

#endif
