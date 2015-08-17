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

#include "bmp.h"

bitmap* OpenBitmap(std::string inFilename, 
		Distiller _distiller)  throw(catch_all_exception) {
			
	// Check input file type
	image_format bmf=BMP_FMT_ERROR;
	basic_bitmap checkHeader;
	
	char msg[50];

	try { bmf = checkHeader.GetFormat(inFilename); }
	catch (catch_all_exception e) { throw e; }
	
	bitmap* bmp;
	
	switch(bmf) {

	#ifdef RGB_BITMAP_H
	// Load a 24 bit rgb bitmap
	case BMP_RGB_24 : 	bmp = new rgb_bitmap(_distiller);
						bmp->ReadBitmapFile( inFilename );
						return bmp;
	#endif

	#ifdef INDEXED_BITMAP_H
	// Load an 8 bit indexed rgb bitmap
	case BMP_IDX_08 :	bmp = new indexed_bitmap(_distiller);
						bmp->ReadBitmapFile( inFilename );
						return bmp;
	#endif

	#ifdef MONOCHROME_BITMAP_H
	// Load a 1 bit monochrome bitmap
	case BMP_MON_01 :	bmp = new monochrome_bitmap();
						bmp->ReadBitmapFile( inFilename );
						return bmp;
	#endif

	// Load an image file supported by SDL into a 24 bit rgb bitmap object
	case LBM  :
	case GIF  :
	case JPEG :
	case PCX  :
	case PNG  :
    case PNM  :
	case TGA  :
	case TIFF :
	case XCF  :
	case XPM  :
    case SDL  :
	#ifdef _SDL_IMAGE_H
                        bmp = new rgb_bitmap(_distiller); 
						bmp = ReadSdlFile(bmp, inFilename, _distiller); 
						return bmp;
	#endif
                        strncpy(msg, 
"Non-bitmap image format. Install the \
SDL-image library, re-compile Taspa, then try again.\n", 50);
                        break;
	                        

	// Exit with error for unsupported image type
	case BMP_RGB_16 : 	strncpy(msg, "5-6-5 bitmaps not yet supported.\n", 50); 
						break;

	case BMP_RGB_08 : 	strncpy(msg, "3-3-2 bitmaps not yet supported.\n", 50);
						break;

	case BMP_IDX_04 : 	strncpy(msg, "RGB4 bitmaps not yet supported.\n", 50);
						break;

	case BMP_RLE_08 : 	strncpy(msg, "RLE8 not yet supported.\n", 50);
						break;

	case BMP_RLE_04 : 	strncpy(msg, "RLE4 not yet supported.\n", 50);
						break;

	default : 			strncpy(msg, "Unsupported image format.\n", 50);
						break;
	}	
	
	throw catch_all_exception(msg);
}
