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

#ifndef BMP_H
#define BMP_H

#include "rgb_bitmap.h"            // Supports 24-bit RGB .bmp files
#include "monochrome_bitmap.h"     // Supports 1-bit .bmp files
#include "indexed_bitmap.h"        // Supports (some) indexed .bmp files
#include "distillers.h"            // Bitmap object boundary detectors

/* If the SDL library has been included, allow Jpeg support */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SDL_IMAGE
#include "jpeg.h"	               // Jpeg to 24-bit bitmap conversion via SDL
#endif

bitmap* OpenBitmap(std::string inFilename, 
		Distiller _distiller)  throw(catch_all_exception);

#endif
