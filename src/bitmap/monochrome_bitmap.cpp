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

#include <fstream>
#include <assert.h>
#include "monochrome_bitmap.h"

/* Create an empty monochrome bitmap information container */
monochrome_bitmap::monochrome_bitmap() { }

//monochrome_bitmap::mono_iterator monochrome_bitmap::operator[] 
//		(const location& loc) {
//	assert((int)loc.x <= data.max_x() && (int)loc.y <= data.max_y());
//	return data[loc];
//}

monochrome_bitmap::mono_iterator 
		monochrome_bitmap::operator() (int x, int y) {
	assert(x <= data.max_x() && y <= data.max_y());
	return data(x,y);
}

monochrome_bitmap::mono_iterator 
		monochrome_bitmap::iterator (int x, int y) {
	assert(x <= data.max_x() && y <= data.max_y());
	return data(x,y);
}

basic_bitmap::mono monochrome_bitmap::Mono (const location& loc) {
	assert((int)loc.x <= data.max_x() && (int)loc.y <= data.max_y());
	return *data(loc);
}

basic_bitmap::mono monochrome_bitmap::Mono (int x, int y) {
	assert(x <= data.max_x() && y <= data.max_y());
	return *data(x,y);
}

unsigned char monochrome_bitmap::Luminance(location loc) { return Mono(loc); }
unsigned char monochrome_bitmap::Luminance(int x, int y) { return Mono(x,y); }

/* Load a bitmap's information into this containter */
void monochrome_bitmap::ReadBitmapFile (const std::string &fileName) 
		throw (catch_all_exception) {

	char* pixelBuffer = basic_bitmap::ReadRawFile(fileName);
	int width  = GetWidth();
	int height = GetHeight();

	// Collect pixel data and store them under an incrimented index
	data.resize ( width, height );

	// Collect pixel data and store them under an incrimented index
	for (int y = 0; y < height; y ++) {
	for (int x = 0; x <  width; x ++) {

		// to do: check the math on these; switched from 
		// paddedWidth to paddedByteWidth
		int bufferOffset = (x+(y*paddedByteWidth))/8;
		int onebitOffset = (x+(y*paddedByteWidth))%8;

		*data(x,y) = (pixelBuffer[bufferOffset] >> (7-onebitOffset)) & 0x01;

	}	}

	delete[] pixelBuffer;
}

/* Load a bitmap's information into this containter */
void monochrome_bitmap::WriteBitmapFile (const std::string &fileName) 
		throw (catch_all_exception) {

	// Open the bitmap file
	std::ofstream 
		bitmapFile(fileName.c_str(), std::ios::out | std::ios::binary);

	if (!bitmapFile) {
		char msg[] = "Error opening file.\n";
		throw catch_all_exception(msg);
	}

	basic_bitmap::WriteHeader     (bitmapFile);
	basic_bitmap::WriteColorTable (bitmapFile);

	int width  = GetWidth();
	int height = GetHeight();

	int byteSize = (paddedByteWidth * height);

	char* outputBuffer = new char[ byteSize ];
	
	for(int index=0; index < byteSize; index++) 
		outputBuffer[index]=0;
	
	for (int y = 0; y < height; y++) {
	for (int x = 0; x <  width; x++) {

		// to do: check the math on these; 
		// switched from paddedWidth to paddedByteWidth		
		int bufferOffset = (x/8)+(y*paddedByteWidth);
		int onebitOffset = (x%8);

		outputBuffer[bufferOffset] |= *data(x,y)*(1<<(7-onebitOffset));

	}	}

	bitmapFile.write(outputBuffer, byteSize);
	bitmapFile.close();
	delete[] outputBuffer;
}

void monochrome_bitmap::MarkVertices ( 
			const location::Vector& concaveVector,
			const location::Vector& convexVector 
		) {

	for (location::ConstVectorIter itr = concaveVector.begin();
				itr != concaveVector.end(); itr ++) {

		SetPixel(itr->x,itr->y,basic_bitmap::black);
	}

	for (location::ConstVectorIter itr = convexVector.begin();
				itr != convexVector.end(); itr ++) {

		SetPixel(itr->x,itr->y,basic_bitmap::black);
	}
}

void monochrome_bitmap::MarkVertices ( const location::Vector& vertices, rgba color ) {

	for (location::ConstVectorIter itr = vertices.begin();
				itr != vertices.end(); itr ++) {

		SetPixel(itr->x,itr->y,basic_bitmap::black);
	}
}

