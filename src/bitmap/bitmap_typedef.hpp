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

#ifndef BITMAP_TYPEDEF_HPP
#define BITMAP_TYPEDEF_HPP

#include <vector>
#include <math.h>
#include <stdint.h>
#include <algorithm>
#include <set>

#include "rgb.h"
#include "rgba.h"

/**@name RLE compression type enumeration */
//@{

/// No compression - straight RGB data
const int BI_RGB       = 0;

/// 8-bit run-length compression
const int BI_RLE8      = 1;

/// 4-bit run-length compression
const int BI_RLE4      = 2;

/// RGB bitmap with RGB masks
const int BI_BITFIELDS = 3;

//@}

/**@name Supported bitmap formats */
//@{

/// No compression - straight RGB data
enum image_format {
	BMP_RLE_08,
	BMP_RLE_04,
	BMP_RGB_24,
	BMP_RGB_16,
	BMP_RGB_08,
	BMP_IDX_08,
	BMP_IDX_04,
	BMP_MON_01,
    LBM,  
    GIF,  
    JPEG, 
    PCX,  
    PNG,  
    PNM,  
    TGA,  
    TIFF, 
    XCF,  
    XPM,
    SDL,
	BMP_FMT_ERROR
};

//@}

typedef unsigned char color_index;

struct header {

		short	type;// 			: 16;
		int	size;//			: 32;
		short	reserved1;//		: 16;
		short	reserved2;//		: 16;
		int	offset;//			: 16;
};

struct info_header {

		int	size;//			: 32;	/* Size of info header */
		int	width;//			: 32;	/* Width of image */
		int	height;//			: 32;	/* Height of image */
		short	planes;//			: 16;	/* Number of color planes */
		short	bitCount;//		: 16;	/* Number of bits per pixel */
		int	compression;//		: 32;	/* Type of compression to use */
		int	sizeImage;//		: 32;	/* Size of image data */
		int	xPelsPerMeter;//	: 32;	/* X pixels per meter */
		int	yPelsPerMeter;//	: 32;	/* Y pixels per meter */
		int	clrUsed;//			: 32;	/* Number of colors used */
		int	clrImportant;//	: 32;	/* Number of important colors */
};

enum bmi_error_code {
	BMI_SUCCESS = 0,
	BMI_INVALID_HEADER=1,
	BMI_INVALID_INFOHEADER=2,
	BMI_OPENING_FILE=3,
	BMI_INVALID_BITCOUNT=4,
	BMI_INVALID_COMPRESSION=5,
	BMI_UNOPENED=6
};

#endif
