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
#include <limits.h>
#include <fstream>
#include "indexed_bitmap.h"

indexed_bitmap::indexed_bitmap() { }

indexed_bitmap::indexed_bitmap(bool (*_distiller)(location, bitmap*)) {
	distiller = _distiller;
}

//indexed_bitmap::indexed_iterator indexed_bitmap::operator[] 
//		(const location& loc) {
//	assert((int)loc.x <= data.max_x() && (int)loc.y <= data.max_y());
//	return data[loc];
//}

indexed_bitmap::indexed_iterator 
		indexed_bitmap::operator() (int x, int y) {
	assert(x <= data.max_x() && y <= data.max_y());
	return data(x,y);
}

indexed_bitmap::indexed_iterator 
		indexed_bitmap::iterator (int x, int y) {
	assert(x <= data.max_x() && y <= data.max_y());
	return data(x,y);
}

//void indexed_bitmap::SetMonoFilter(rgba min, rgba max) {
//
//	assert(initialized);
//	
//	bestFitMin = colorTable.front();
//	bestFitMax = colorTable.front();
//	
//	int minMagnitude = bestFitMin.l2norm();
//	int maxMagnitude = bestFitMax.l2norm();
//	
//	for (unsigned char index = 0; index < colorTable.size(); index ++) {
//		int indexMagnitude = (colorTable[index] - min).l2norm();
//
//		if (indexMagnitude < minMagnitude) {
//			bestFitMin = colorTable[index];
//			minMagnitude = indexMagnitude;
//		}
//	}
//	
//	for (unsigned char index = 0; index < colorTable.size(); index ++) {
//		
//		int indexMagnitude = (colorTable[index] - max).l2norm();
//		
//		if (indexMagnitude < maxMagnitude) {
//			bestFitMax = colorTable[index];
//			maxMagnitude = indexMagnitude;
//		}
//	}
//}

// to do: use distillation function to get return value
basic_bitmap::mono indexed_bitmap::Mono (const location& loc) {
	assert((int)loc.x <= data.max_x() && (int)loc.y <= data.max_y());
	return distiller(loc, this);
}

// to do: use distillation function to get return value
basic_bitmap::mono indexed_bitmap::Mono (int x, int y) {
	assert(x <= data.max_x() && y <= data.max_y());
	location loc(x,y);
	return distiller(loc, this);
}

unsigned char indexed_bitmap::Luminance(location loc) {
	return Luminance(loc.x, loc.y);
}

unsigned char indexed_bitmap::Luminance(int x, int y) {
	rgb c = colorTable[*data(x,y)]; 
	return (8432*c.r + 16425*c.g + 3176*c.b)/(8432+16425+3176);
}

/* Load a bitmap's information into this containter */
void indexed_bitmap::ReadBitmapFile (const std::string &fileName) 
		throw (catch_all_exception) {

	char* pixelBuffer = basic_bitmap::ReadRawFile(fileName);
	int width  = GetWidth();
	int height = GetHeight();

	// Collect pixel data and store them under an incrimented index
	data.resize ( width, height );

	for (int y = 0; y < height; y ++) {
	for (int x = 0; x <  width; x ++) {

		int bufferOffset = (x+(y*paddedByteWidth));
		*data(x,y) = pixelBuffer[bufferOffset];

	}	}

	delete[] pixelBuffer;
}

/* Load a bitmap's information into this containter */
void indexed_bitmap::WriteBitmapFile (const std::string &fileName) 
		throw (catch_all_exception) {
	bitmap::WriteRawFile(fileName);
}

void indexed_bitmap::MarkVertices ( 
			const location::Vector& concaveVector,
			const location::Vector& convexVector 
		) {

	int blueIndex=0;
	int greenIndex=0;
	int redIndex=0;

	for (int index = 0; index < colorTable.size(); index++) {
		short blue  = (short)colorTable[index].b;
		short green = (short)colorTable[index].g;
		short red   = (short)colorTable[index].r;
		
		blue  -= red   + green;
		green -= blue  + red;
		red   -= green + blue;
		
		if (blue  > blueIndex)  blueIndex  = index;
		if (green > greenIndex) greenIndex = index;
		if (red   > redIndex)   redIndex   = index;
		
		if (blueIndex && redIndex) index = colorTable.size();
	}

	for (location::ConstVectorIter itr = concaveVector.begin();
				itr != concaveVector.end(); itr ++) {

		SetPixel(itr->x,itr->y,blueIndex);
	}

	for (location::ConstVectorIter itr = convexVector.begin();
				itr != convexVector.end(); itr ++) {

		SetPixel(itr->x,itr->y,redIndex);
	}
}

void indexed_bitmap::MarkVertices 
		( const location::Vector& vertices, rgba color ) {

	size_t cIndex = GetNearestIndex(color);

	for (location::ConstVectorIter itr = vertices.begin();
				itr != vertices.end(); itr ++) {

		SetPixel(itr->x,itr->y,colorTable[cIndex]);
	}
}

size_t indexed_bitmap::GetNearestIndex(rgba color) {
	int bestIndexDiff = INT_MAX;
	int colorIndex = 0;
	
	for (size_t index = 0; index < colorTable.size(); index++) {
		short blue  = (short)colorTable[index].b;
		short green = (short)colorTable[index].g;
		short red   = (short)colorTable[index].r;
		
		blue  = abs(blue  - color.b);
		green = abs(green - color.g);
		red   = abs(red   - color.r);
		
		if (blue+green+red < bestIndexDiff) {
			bestIndexDiff = blue+green+red;
			colorIndex = index;
		}
	}
	
	return colorIndex;
}

void indexed_bitmap::FillHeaders () {
		
//	bmiInfoHeader.bitCount = 24;
//	bmiInfoHeader.clrImportant = 0; // no color table
//	bmiInfoHeader.clrUsed = 0; // no color table
//	bmiInfoHeader.compression = 0; // no compression
//	bmiInfoHeader.height = data.max_y()+1;
//	bmiInfoHeader.width = data.max_x()+1;
//	bmiInfoHeader.planes = 1;
//	bmiInfoHeader.size = 40; // standard header size  
//
//	paddedByteWidth = bmiInfoHeader.width;
//	while ((paddedByteWidth*bmiInfoHeader.bitCount/8) % 4 != 0) 
//		paddedByteWidth ++;
//
//	bmiInfoHeader.sizeImage = paddedByteWidth*bmiInfoHeader.height;
//	bmiInfoHeader.xPelsPerMeter = 5157;
//	bmiInfoHeader.yPelsPerMeter = 5157;
//	
//	bmiHeader.type = 0x4d42;
//	bmiHeader.size = 
//		sizeof(bmiHeader) + 
//		sizeof(bmiInfoHeader) + 
//		bmiInfoHeader.sizeImage;
//	bmiHeader.reserved1 = 0;
//	bmiHeader.reserved2 = 0;
//	bmiHeader.offset = 0x36; // no color table required for 888 rgb
//
//	initialized = true;	/* 0 for success, else error code */
//	pixelCount = bmiInfoHeader.width*bmiInfoHeader.height;
//	palletSize = 0;
//	colorTable.clear();
}

void indexed_bitmap::OpenPallet(std::string filename) {
	std::ifstream bitmapFile(filename.c_str(), std::ios::in | std::ios::binary);

	if (!bitmapFile) {
		char msg[] = "Can't open ";
		strncat(msg, filename.c_str(), 80);
		throw catch_all_exception(msg);
	}
	
	// Load header
	basic_bitmap::LoadHeader(bitmapFile);
	int offset      = bmiHeader.offset;
	int compression = bmiInfoHeader.compression;    
    if (offset != 0x36 || compression) return;
    basic_bitmap::LoadColorTable(bitmapFile);
    
    bitmapFile.close();
}
