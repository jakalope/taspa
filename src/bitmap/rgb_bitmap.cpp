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

#include <assert.h>
#include "rgb_bitmap.h"
#include "../std_extensions/set_operations.h"

rgb_bitmap::rgb_bitmap() { }

rgb_bitmap::rgb_bitmap(Distiller _distiller) {
	distiller = _distiller;
}

rgb_bitmap::rgb_bitmap(bitmap* bm) {
	*(bitmap *)this = *bm;
	distiller = ((rgb_bitmap*)bm)->distiller;
	data = ((rgb_bitmap*)bm)->data;
}

//rgb_bitmap::rgb_triple_iterator rgb_bitmap::operator[] (const location& loc) {
//	assert((int)loc.x <= max().x && (int)loc.y <= max().y);
//	return data(loc);
//}
//
//rgb_bitmap::rgb_triple_iterator 
//		rgb_bitmap::operator() (int x, int y) {
//	assert(x <= max().x && y <= max().y);
//	return data(x,y);
//}
//
//rgb_bitmap::rgb_triple_iterator 
//		rgb_bitmap::iterator (int x, int y) {
//	assert(x <= max().x && y <= max().y);
//	return data(x,y);
//}

rgb_bitmap::rgba_iterator rgb_bitmap::operator[] (const location& loc) {
	assert((int)loc.x <= max().x && (int)loc.y <= max().y);
	return data(loc);
}

rgb_bitmap::rgba_iterator
		rgb_bitmap::operator() (int x, int y) {
	assert(x <= max().x && y <= max().y);
	return data(x,y);
}

rgb_bitmap::rgba_iterator
		rgb_bitmap::iterator (int x, int y) {
	assert(x <= max().x && y <= max().y);
	return data(x,y);
}

//void rgb_bitmap::SetMonoFilter(rgba min, rgba max) {
//	filterMin = min;
//	filterMax = max;
//}

//basic_bitmap::mono rgb_bitmap::Mono (const location& loc) {
//	assert(loc.x <= max().x && loc.y <= max().y);
//	return distiller(RgbToRgba(data.GetValue(loc.x, loc.y)), filterMin, filterMax);
//}
//
//basic_bitmap::mono rgb_bitmap::Mono (int x, int y) {
//	assert(x <= data.max_x() && y <= data.max_y());
//	return distiller(RgbToRgba(data.GetValue(x,y)), filterMin, filterMax);
//}
//

basic_bitmap::mono rgb_bitmap::Mono (const location& loc) {
	return data.GetValue(loc.x,loc.y).a;
}

/* From http://www.freescale.com/webapp/sps/site/overview.jsp?
		nodeId=0162468rH3bTdGmKqW5Nf2F9DHMbVXVDcM
		
	The formulae for converting to YCbCr based on ITU-CCITT 601-2 are:
	Y  = [( 8432 R + 16425 G +  3176 B)/32768] + 16
	Cb = [(-4818 R -  9527 G + 14345 B)/32768] + 128
	Cr = [(14345 R - 12045 G -  2300 B)/32768] + 128
*/
basic_bitmap::mono rgb_bitmap::Mono (int x, int y) {
	return data.GetValue(x,y).a;
}


// returns the luminance (Y) at location x,y
unsigned char rgb_bitmap::Luminance(location loc) {
	return Luminance(loc.x,loc.y);
}

// returns the luminance (Y) at location x,y
unsigned char rgb_bitmap::Luminance(int x, int y) {
	rgb c = data.GetValue(x, y); 
	return ((c.r<<13) + (c.g<<14) + 3176*c.b)/(27752);
}

// returns the scaled luminance (Y) at location x,y
//unsigned char rgb_bitmap::ScaledLuminance(int x, int y) {
//	
//	rgb c = data.GetValue(x, y); 
//	
//	unsigned int Y_numerator = 
//		(8432*c.r() + 16425*c.g() + 3176*c.b()) * 255;
//		
//	unsigned int Y_denominator = 
//		(8432+16425+3176) * _max_luminance;
//	
//	return Y_numerator/Y_denominator;
//}
//
/* Load a bitmap's information into this containter */
void rgb_bitmap::ReadBitmapFile (const std::string &fileName) 
		throw (catch_all_exception) {

	char* pixelBuffer = basic_bitmap::ReadRawFile(fileName);
	int width  = GetWidth();
	int height = GetHeight();

	// Collect pixel data and store them under an incrimented index
	data.resize ( width, height );
	double bytesPerSample = bmiInfoHeader.bitCount / 8.000;
	location loc(0,0);
	int& x = loc.x;
	int& y = loc.y;

	for (; y < height; y ++) {
	for (; x <  width; x ++) {

		int bufferOffset = (int)((x*bytesPerSample)+(y*paddedByteWidth));
		const int BLUE = 0;
		const int GREEN= 1;
		const int RED  = 2;

		rgba rgbt(
				pixelBuffer[bufferOffset+BLUE], 
				pixelBuffer[bufferOffset+GREEN],
				pixelBuffer[bufferOffset+RED],
                0
			);
			
//		red.add(rgbt.b);
//		green.add(rgbt.g);
//		blue.add(rgbt.r);

		SetPixel(x,y,rgbt);
        distiller(loc,this);
		SetPixel(x,y,rgbt);

	}	}

	delete[] pixelBuffer;
}

//===================================================================

// MarkVertices: Alter a bitmap object such that all vertices are marked
//      Input:  bitmap = pointer to a bitmap object
//      Return:	N/A

void rgb_bitmap::MarkVertices ( 
			const location::Vector& concaveVector,
			const location::Vector& convexVector 
		) {

	const rgb rgbPurple ( 200, 0, 200 );
	const rgb rgbYellow ( 0, 255, 255 );
	const rgb rgbRed    ( 0, 0, 255 );
	const rgb rgbBlue   ( 255, 0, 0 );
	const rgb rgbGreen  ( 0, 255, 0 );

	for (location::ConstVectorIter itr = concaveVector.begin();
				itr != concaveVector.end(); itr ++) {

		SetPixel(itr->x,itr->y,rgbBlue);
	}

	for (location::ConstVectorIter itr = convexVector.begin();
				itr != convexVector.end(); itr ++) {

		SetPixel(itr->x,itr->y,rgbRed);
	}
}

//void rgb_bitmap::MarkPixels ( const location::Vector& vertices, rgba color) {
//
//	if (vertices.size() == 1) {
//		SetPixel(vertices[0].x,vertices[0].y,color);
//	}
//	
//	else if (vertices.size() > 1) {
//		for (location::ConstVectorIter itr = vertices.begin();
//					itr != vertices.end(); itr ++) {
//			SetPixel(itr->x,itr->y,color);
//		}		
//	}
//}

void rgb_bitmap::MarkVertices ( const location::Vector& vertices, rgba color) {
    
	if (vertices.size() == 1) {
		SetPixel(vertices[0].x,vertices[0].y,color);
	}
	
	else if (vertices.size() > 1) {
		for (location::ConstVectorIter itr = vertices.begin();
					itr != vertices.end(); itr ++) {
			SetPixel(itr->x,itr->y,color);
		}		
	}
}

/* Load a bitmap's information into this containter */
void rgb_bitmap::WriteBitmapFile (const std::string &fileName) 
		throw (catch_all_exception) {
	bitmap::WriteRawFile(fileName);
}

void rgb_bitmap::FillHeaders () {
		
	bmiInfoHeader.bitCount = 24;
	bmiInfoHeader.clrImportant = 0; // no color table
	bmiInfoHeader.clrUsed = 0; // no color table
	bmiInfoHeader.compression = 0; // no compression
	bmiInfoHeader.height = data.max_y()+1;
	bmiInfoHeader.width = data.max_x()+1;
	bmiInfoHeader.planes = 1;
	bmiInfoHeader.size = 40; // standard header size  

	paddedByteWidth = bmiInfoHeader.width*3;
	while ((paddedByteWidth*3) % 4 != 0) 
		paddedByteWidth ++;

	bmiInfoHeader.sizeImage = paddedByteWidth*bmiInfoHeader.height;
	bmiInfoHeader.xPelsPerMeter = 2835;
	bmiInfoHeader.yPelsPerMeter = 2835;
	
	bmiHeader.offset = 0x36; // no color table required for 888 rgb
	bmiHeader.type = 0x4d42;
	bmiHeader.size = 
		bmiHeader.offset + 
		bmiInfoHeader.sizeImage;
	bmiHeader.reserved1 = 0;
	bmiHeader.reserved2 = 0;

	initialized = true;	/* 0 for success, else error code */
	pixelCount = bmiInfoHeader.width*bmiInfoHeader.height;
	palletSize = 0;
	colorTable.clear();
}
