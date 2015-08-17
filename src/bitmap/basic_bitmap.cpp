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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <assert.h>
#include <math.h>
#include "basic_bitmap.h"

basic_bitmap::basic_bitmap() { initialized = false; palletSize=0; }

void basic_bitmap::LoadHeader(std::ifstream& bitmapFile) {
			
	assert(bitmapFile);
	
	// Collect lead-in data from the file
	const int beginLeadin = 0x00;
	bitmapFile.seekg( beginLeadin );
	bitmapFile.read(reinterpret_cast<char *>(&bmiHeader.type),2);
	bitmapFile.read(reinterpret_cast<char *>(&bmiHeader.size),4);
	bitmapFile.read(reinterpret_cast<char *>(&bmiHeader.reserved1),2);
	bitmapFile.read(reinterpret_cast<char *>(&bmiHeader.reserved2),2);
	bitmapFile.read(reinterpret_cast<char *>(&bmiHeader.offset),4);

	// Collect header data from the file
	bitmapFile.read(
		reinterpret_cast<char *>(&bmiInfoHeader),sizeof(info_header));

	if (bmiHeader.offset == 0) {
		bmiHeader.offset = 0x36;
	}

	if (bmiInfoHeader.compression != 0) {
		char msg[] = "Unknown bitmap compression.";
		throw catch_all_exception(msg);
	}

	int height = bmiInfoHeader.height;
	int width  = bmiInfoHeader.width;
	pixelCount = width*height;

//	double samplesPerByte = 8.000 / bmiInfoHeader.bitCount;
	paddedByteWidth = bmiInfoHeader.sizeImage/height;
//	while(paddedWidth%(int)(4*ceill(samplesPerByte))!=0) { paddedWidth++; }
}

void basic_bitmap::LoadColorTable(std::ifstream& bitmapFile) {
	
	assert(bmiHeader.offset-0x35 >= 0);
//	assert(bmiInfoHeader.clrUsed > 0);

	short palletBytes = bmiHeader.offset-0x35;
	palletSize = palletBytes/4;
	colorTable.resize(palletSize);
	
	// Read all bitmap pixels from file
	char *pixelBuffer = new char [palletBytes];
	bitmapFile.read(pixelBuffer, palletBytes);

	for (int index=0; index < palletSize; index++) {
		colorTable[index].b = pixelBuffer[(index*4)+0];
		colorTable[index].g = pixelBuffer[(index*4)+1];
		colorTable[index].r = pixelBuffer[(index*4)+2];
		colorTable[index].a = pixelBuffer[(index*4)+3];
	}
	
	delete[] pixelBuffer;
}

void basic_bitmap::WriteColorTable(std::ofstream& bitmapFile) {
	
	bitmapFile.seekp ( 0x36 );
	unsigned short palletBytes = palletSize*4;
	char *pixelBuffer = new char [palletBytes];

	for (int index=0; index < palletSize; index++) {
		pixelBuffer[index*4+0] = colorTable[index].b;
		pixelBuffer[index*4+1] = colorTable[index].g;
		pixelBuffer[index*4+2] = colorTable[index].r;
		pixelBuffer[index*4+3] = colorTable[index].a;
	}
	
	bitmapFile.write(pixelBuffer, palletBytes);
	delete[] pixelBuffer;
}

void basic_bitmap::WriteHeader(std::ofstream& bitmapFile) {
	// Write lead-in data
	bitmapFile.write(reinterpret_cast<char *>(&bmiHeader.type),2);
	bitmapFile.write(reinterpret_cast<char *>(&bmiHeader.size),4);
	bitmapFile.write(reinterpret_cast<char *>(&bmiHeader.reserved1),2);
	bitmapFile.write(reinterpret_cast<char *>(&bmiHeader.reserved2),2);
	bitmapFile.write(reinterpret_cast<char *>(&bmiHeader.offset),4);

	// Write header data
	bitmapFile.write(
		reinterpret_cast<char *>(&bmiInfoHeader),sizeof(info_header));
}

/* Load a bitmap's information into this containter */
char* basic_bitmap::ReadRawFile (const std::string &fileName) 
		throw (catch_all_exception) {

	// Open the bitmap file
	std::ifstream bitmapFile(fileName.c_str(), std::ios::in | std::ios::binary);

	if (!bitmapFile) {
		char msg[] = "Error opening file.\n";
		throw catch_all_exception(msg);
	}
	
	// Load header
	basic_bitmap::LoadHeader(bitmapFile);
	
	// Load color table
	if (bmiHeader.offset != 0x36) basic_bitmap::LoadColorTable(bitmapFile);

	// Skip to the pixel data
	const int beginPixelData = bmiHeader.offset;
	bitmapFile.seekg ( beginPixelData );

	// Read all bitmap pixels from file
	// to do: is this causing a memory leak?
	char* pixelBuffer = new char [ bmiInfoHeader.sizeImage ];
	bitmapFile.read(pixelBuffer, bmiInfoHeader.sizeImage);
	bitmapFile.close();
		
	return pixelBuffer;
}

image_format basic_bitmap::GetFormat(const std::string &fileName) 
		throw (catch_all_exception) {
	
    std::ifstream checkExistance(fileName.c_str());
    if (!checkExistance) {
        char msg[] = "File does not exist.";
        throw catch_all_exception(msg);
    }
	
    #ifdef HAVE_SDL_IMAGE	
    return SDL;
    #endif

	std::string extension = fileName.substr(fileName.size()-4);
    for (size_t i = 0; i < extension.length(); i++)
        extension[i]=toupper(extension[i]);

	if (extension == "JPEG" || extension == ".JPG") return JPEG;
	if (extension == ".GIF") return GIF;
	if (extension == ".PNG") return PNG;

	std::ifstream bitmapFile(fileName.c_str(), std::ios::in | std::ios::binary);

	if (!bitmapFile) {
		char msg[] = "Can't open ";
		strncat(msg, fileName.c_str(), 80);
		throw catch_all_exception(msg);
	}
	
	// Load header
	basic_bitmap::LoadHeader(bitmapFile);
	
	int bitCount    = bmiInfoHeader.bitCount;
	int offset      = bmiHeader.offset;
	int compression = bmiInfoHeader.compression;
	
	if (compression) {
		if (bitCount ==  8) return BMP_RLE_08;
		if (bitCount ==  4) return BMP_RLE_04;
	}

	if (offset == 0x36) {
		if (bitCount == 24)	return BMP_RGB_24;
		if (bitCount == 16)	return BMP_RGB_16;
		if (bitCount ==  8)	return BMP_RGB_08;
	}
	
	if (bitCount == 8) return BMP_IDX_08;
	if (bitCount == 4) return BMP_IDX_04;
	//if (bitCount == 1) return BMP_MON_01;    This one isn't working yet.
	
	char msg[] = "Unsupported bitmap format.";
	throw catch_all_exception(msg);
}
