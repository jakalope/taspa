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

#ifdef HAVE_SDL_IMAGE
#include "jpeg.h"

rgba GetPixel ( SDL_Surface* pSurface , int x , int y ) 
{
  rgba color;//(color.b , color.g , color.r);
  //SDL_Color color;
  Uint32 col = 0 ;

  //determine position
  char* pPosition = ( char* ) pSurface->pixels ;

  //offset by y
  pPosition += ( pSurface->pitch * y ) ;

  //offset by x
  pPosition += ( pSurface->format->BytesPerPixel * x ) ;

  //copy pixel data
  memcpy ( &col , pPosition , pSurface->format->BytesPerPixel ) ;

  //convert color
  SDL_GetRGB ( col , pSurface->format , &color.r , &color.g , &color.b ) ;
  return ( color ) ;
}

bitmap* ReadSdlFile( bitmap* bm, std::string filename, Distiller _distiller ) 
		throw (catch_all_exception) {

	SDL_Surface *image = NULL;

	try {
		image = IMG_Load( filename.c_str() );
        if (!image) throw "Problem loading Jpeg file.";
	} CATCH_ALL("Problem loading Jpeg file.");
	
    SDL_LockSurface(image);
	
	bm->resize(image->w,image->h);
	bm->FillHeaders();

	int width  = bm->GetWidth();
	int height = bm->GetHeight();
	
	for(int x = 0; x < width;  x++) {
	for(int y = 0; y < height; y++) {
		
		rgba rgbq(GetPixel(image, x, y));
		bm->SetPixel(x,height-y-1,rgbq);
			
	}	}
	
	for(int x = 0; x < width;  x++) {
	for(int y = 0; y < height; y++) {
		
		rgba rgbq(bm->GetPixel(x,y));
		location loc(x,y);
		rgbq.a = _distiller(loc,bm);
		bm->SetPixel(x,y,rgbq);

	}	}

	SDL_UnlockSurface( image );
	SDL_FreeSurface( image );
	SDL_Quit();

	return bm;
}

#endif
