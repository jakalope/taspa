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

#ifndef RGB_BITMAP_H
#define RGB_BITMAP_H

#include "bitmap.h"
#include "matrix.h"
#include "stats.h"
#include "distillers.h"

////////////////////////////////////////////////////////////////////////////////
/**	Base class for various bitmap formats. Contains header information and 
	virtual functions for basic bitmap functionality.

	@version 0.1.0 Build 0
	@author J. Askeland
	@memo
*/
class rgb_bitmap : public bitmap {

	private:

	protected:
		matrix<rgba> data;
		
		Distiller distiller;
		
//		rgba filterMin;
//		rgba filterMax;
		
//		stats red;
//		stats green;
//		stats blue;

	public:

		/////////////////////////////////////////////////////
		/** @name Constructors **/
		//@{

		/** Create an empty bitmap container */
		rgb_bitmap();
		rgb_bitmap(Distiller _distiller);
		rgb_bitmap(bitmap* bm);
		
		//@}


		/////////////////////////////////////////////////////
		/** @name Operators **/
		//@{

//		typedef std::vector<rgb>::iterator rgb_triple_iterator;
//		rgb_triple_iterator operator[] (const location& loc);
//		rgb_triple_iterator operator() (int x, int y);
//		rgb_triple_iterator iterator (int x, int y);

		typedef std::vector<rgba>::iterator rgba_iterator;
		rgba_iterator operator[] (const location& loc);
		rgba_iterator operator() (int x, int y);
		rgba_iterator iterator (int x, int y);

		//@}


		/////////////////////////////////////////////////////
		/** @name Public Members **/
		//@{
		
		void ByteCast(int x, int y, char* pointer) {
			rgb rgbt = data.GetValue(x,y);
			pointer[0]= rgbt.b;
			pointer[1]= rgbt.g;
			pointer[2]= rgbt.r;
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
		const matrix<rgba>* GetPixelMapPtr() const 
			{ return &data; }

		/** Sets the specified pixel (#x#,#y#) to a new value #color#. */
		void SetPixel ( int x, int y, rgba color )
			{ data.SetValue(x,y,color); }

		/** Sets the specified pixel (#x#,#y#) to a new value #color#. */
//		void SetPixel ( int x, int y, rgb color )
//			{ data.SetValue(x,y,RgbToRgba(color)); }
			
		rgb GetPixel ( int x, int y ) { return data.GetValue(x,y); }

		void MarkVertices ( 
			const location::Vector& concaveVector,
			const location::Vector& convexVector );

		void MarkVertices ( const location::Vector& vertices, rgba color );

		void resize(int width, int height) {
			data.resize(width, height);
		}
		
//		void SetMonoFilter(rgba min, rgba max);
		
		location max() { return data.max(); }
		
		void FillHeaders ();

		//@}
};

#endif
