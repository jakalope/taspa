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

#ifndef INDEXED_BITMAP_H
#define INDEXED_BITMAP_H

#include <assert.h>
#include "bitmap.h"
#include "matrix.h"

////////////////////////////////////////////////////////////////////////////////
/**	Base class for various bitmap formats. Contains header information and 
	virtual functions for basic bitmap functionality.

	@version 0.1.0 Build 0
	@author J. Askeland
	@memo
*/
class indexed_bitmap : public bitmap {

	public: 
		typedef unsigned char indexed;

	protected:
		matrix<indexed> data;
		bool (*distiller)(location, bitmap*);
		rgba bestFitMin;
		rgba bestFitMax;

	public:

		/////////////////////////////////////////////////////
		/** @name Constructors **/
		//@{

		/** Create an empty bitmap container */
		indexed_bitmap();
		indexed_bitmap(bool (*_distiller)(location, bitmap*));

		//@}


		/////////////////////////////////////////////////////
		/** @name Operators **/
		//@{

		typedef std::vector<indexed>::iterator indexed_iterator;
		indexed_iterator operator[] (const location& loc);
		indexed_iterator operator() (int x, int y);
		indexed_iterator iterator (int x, int y);

		//@}


		/////////////////////////////////////////////////////
		/** @name Public Members **/
		//@{
		
		void ByteCast(int x, int y, char* pointer) {
			*pointer = *data(x,y);
		}
		
		/** Returns a monochrome distillation of the pixel at location #loc#.
			@memo
		*/
		basic_bitmap::mono Mono (const location& loc);

		/** Returns a monochrome distillation of the pixel at 
			location (#x#,#y#).
			@memo
		*/
		basic_bitmap::mono Mono (int x, int y);

		unsigned char Luminance(location loc);
		unsigned char Luminance(int x, int y);

		/** Reads a bitmap file into an appropriate container. 
			@exception catch_all_exception
			@memo
		*/
		void ReadBitmapFile (const std::string &fileName) 
			throw (catch_all_exception);

		/** Writes to a bitmap file. 
			@exception catch_all_exception
			@memo
		*/
		void WriteBitmapFile (const std::string &fileName)
			throw (catch_all_exception);

		// to do: find a way to impliment these virtually in basic_bitmap
		// or find a way to access these methods polymophically.
		/** Return pointer to pixel map */
		const matrix<indexed>* GetPixelMapPtr() const 
			{ return &data; }

		/** Sets the specified pixel (#x#,#y#) to a new value #color#. */
		void SetPixel ( int x, int y, rgba color ) {
			
			assert(initialized);
			
			for (unsigned char index = 0; index < colorTable.size(); index ++)
				if (colorTable[index] == color) {
					data.SetValue(x,y,index);
					return;
				}
		}

		/** Sets the specified pixel (#x#,#y#) to a new value #color#. */
		void SetPixel ( int x, int y, indexed color ) {
			
			assert(initialized);
			data.SetValue(x,y,color);
		}
		
		rgb GetPixel ( int x, int y ) 
			{ return colorTable[data.GetValue(x,y)]; }
		
//		void SetMonoFilter(rgba min, rgba max);
		
		void MarkVertices ( 
			const location::Vector& concaveVector,
			const location::Vector& convexVector );

		void MarkVertices ( const location::Vector& vertices, rgba color );

		size_t GetNearestIndex(rgba color);
		
		location max() { return data.max(); }

		void FillHeaders ();
		
		rgba::Vector& ColorTable() { return colorTable; }
		
		void OpenPallet(std::string filename);

		//@}
};

#endif
