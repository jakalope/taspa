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

#include "distillers.h"
#include "bitmap.h"
#include "../location/location.hpp"
#include "rgb_float.h"

bool Luminance(location loc, bitmap* bmp) {
	return bmp->Luminance(loc) > 170;
}


bool AvgLuminance(location loc, bitmap* bmp) {

	unsigned avgLuminance = 0;
	const unsigned THRESHOLD = 128;
	const int OFFSET = 1;
	
	// Get a box of colors (center and its 8 neighbors),
	// using the center value for pixels not inside the limits.
	
	int xCen = loc.x;
	int xMin = (loc.x-OFFSET >= 0) ? loc.x-OFFSET : loc.x+OFFSET;
	int xMax = (loc.x+OFFSET < bmp->GetWidth()) ? loc.x+OFFSET : loc.x-OFFSET;
	
	int yCen = loc.y;
	int yMin = (loc.y-OFFSET >= 0) ? loc.y-OFFSET : loc.y+OFFSET;
	int yMax = (loc.y+OFFSET < bmp->GetHeight()) ? loc.y+OFFSET : loc.y-OFFSET;

	unsigned char neighbors[8] = {
			bmp->Luminance(xMin, yMin),
			bmp->Luminance(xCen, yMin),
			bmp->Luminance(xMax, yMin), 
			bmp->Luminance(xMin, yCen),
			bmp->Luminance(xMax, yCen), 
			bmp->Luminance(xMin, yMax),
			bmp->Luminance(xCen, yMax),
			bmp->Luminance(xMax, yMax) 
		};

	for (int i = 0; i < 8; i ++)
		avgLuminance += neighbors[i];
		
	avgLuminance /= 8;
	
	return avgLuminance > THRESHOLD;
}


bool ColorVariation(location loc, bitmap* bmp) {
	
	// minimum color variance (in radians, 0 <= THRESHOLD <= pi/2) 
	// before returning a "black" pixel value: 0.
	//  const float MAX_VARIATION = 2.2214396;
	//	const float LUMINANCE_SCALER = 0.5000;
	//	const float CHAR_SCALE_FACTOR = 0.008711528*LUMINANCE_SCALER;
	//	const float THRESHOLD_SCALER = 0.3000;
	const float THRESHOLD = 0.25;
		//2*(MAX_VARIATION+1)*THRESHOLD_SCALER*LUMINANCE_SCALER;
	
	const int OFFSET = 6;
	
	// Get a box of colors (center and its 8 neighbors),
	// using the center value for pixels not inside the limits.
	
	int xCen = loc.x;
	int xMin = (loc.x-OFFSET >= 0) ? loc.x-OFFSET : loc.x+OFFSET;
	int xMax = (loc.x+OFFSET < bmp->GetWidth()) ? loc.x+OFFSET : loc.x-OFFSET;
	
	int yCen = loc.y;
	int yMin = (loc.y-OFFSET >= 0) ? loc.y-OFFSET : loc.y+OFFSET;
	int yMax = (loc.y+OFFSET < bmp->GetHeight()) ? loc.y+OFFSET : loc.y-OFFSET;

	rgb_float neighbors[8] = {
			rgb_float::unit(bmp->GetPixel(xMin, yMin)),
			rgb_float::unit(bmp->GetPixel(xCen, yMin)),
			rgb_float::unit(bmp->GetPixel(xMax, yMin)), 
			rgb_float::unit(bmp->GetPixel(xMin, yCen)),
			rgb_float::unit(bmp->GetPixel(xMax, yCen)), 
			rgb_float::unit(bmp->GetPixel(xMin, yMax)),
			rgb_float::unit(bmp->GetPixel(xCen, yMax)),
			rgb_float::unit(bmp->GetPixel(xMax, yMax)) 
		};

	// To find the variation of color, we want to percieve RGB as the
	// first octant in the chart of a Z^3 vector with R,G,and B as axes.
	// The variation is the magnitude of the vector marking the
	// change in direction between two unit RGB vectors, namely the vector
	// whose entries are ( phi_i - phi_[n-1-i], theta_i - theta_[n-1-i] ), 
	// in spherical coordinates.
	
	float phi[8], theta[8];
	float variation = 0.;
	
	for (int i = 0; i < 8; i ++) {
		// phi_i = arctan(B_i/G_i), theta_i = arctan(R_i/G_i)
		phi[i]    =  atanf(neighbors[i].b / neighbors[i].g);
		theta[i]  =  atanf(neighbors[i].r / neighbors[i].g);
	}
	
	for (int i = 0; i < 8; i ++) {
		// Color variation is:
		//		-the arc length along the unit sphere's first octant
		//		-the geometric mean of delta phi and delta theta
		// Thus: ColorVariation_i = sqrt(DeltaPhi^2 + DeltaTheta^2)
		float delta_phi    = phi[i]    - phi[7-i];
		float delta_theta  = theta[i]  - theta[7-i];
		
		variation += 
			sqrtf(delta_phi*delta_phi + delta_theta*delta_theta);
	}
	
	// TotalVariation = Sum(ColorVariation_i, 1 to n)/n
	//variation /= 8.; // in radians, 0 <= variance <= pi/2
	//variation += (bmp->Luminance(loc)*CHAR_SCALE_FACTOR + 1.00000);

	// If the color varies too much, return black.
	if (variation >= THRESHOLD) return false;
	return true;
}


bool LumDiff(location loc, bitmap* bmp) {
    
    const int OFFSET = 1;
	int xMin = (loc.x-OFFSET >= 0) ? loc.x-OFFSET : loc.x;

    return abs(bmp->Luminance(loc.x,loc.y)-bmp->Luminance(xMin,loc.y)) <= 32;
}


bool Footprint(location loc, bitmap* bmp) {
    
    const int footprint = 5; // 9x9 footprint
    const int offset = footprint - 1;
    const rgb rgbWhite(255,255,255);
    
    if (loc.x - offset < 0 || loc.x + offset > bmp->max().x) return false;
    if (loc.y - offset < 0 || loc.y + offset > bmp->max().y) return false;
    
    int bottom_y = loc.y - offset;
    int top_y    = loc.y + offset;

    int left_x   = loc.x - offset;
    int right_x  = loc.x + offset;

    for (int x = left_x; x <= right_x; x ++)
    for (int y = bottom_y; y <= top_y; y ++)
        if (Luminance(location(x,y),bmp) == false) return false;
        
    return true;
}
