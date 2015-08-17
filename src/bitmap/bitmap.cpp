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

#include "bitmap.h"

#include <math.h>     // For abs
#include <fstream>    // For file i/o
#include <assert.h>   // For assert
#include <limits.h>   // For INT_MAX

bool bitmap::IsPassible (const location& loc) { 
	return IsPassible(loc.x,loc.y);
}

bool bitmap::IsPassible (int x, int y) { 
	if (x <= max().x && y <= max().y && x >= 0 && y >= 0)
        return Mono(x,y); 
	return false;
}

bool bitmap::IsConnectedLatticeEdge (int x, int y) {    
    if (y%2) 
        return (IsPassible(x-1,(y-1)/2) && !IsPassible(x,(y-1)/2)) ||
               (!IsPassible(x-1,(y-1)/2) && IsPassible(x,(y-1)/2));
    else
        return (IsPassible(x,(y-2)/2) && !IsPassible(x,y/2)) ||
               (!IsPassible(x,(y-2)/2) && IsPassible(x,y/2));
}


location bitmap::ExternalCellLocation(location l) {
    return ExternalCellLocation(l.x, l.y);
}


location bitmap::ExternalCellLocation(int x, int y) {
    if (y%2) return (IsPassible(x-1,(y-1)/2)) ? 
        location(x-1,(y-1)/2) : location(x,(y-1)/2);
    return (IsPassible(x,(y-2)/2)) ? 
        location(x,(y-2)/2) : location(x,y/2);
}


location bitmap::LatticeToCell(location loc) {
    if (loc.y%2) return location(loc.x,(loc.y-1)/2);
    return location(loc.x,loc.y/2);
}    
    

location::Vector bitmap::GetNonBorderLocs (location loc) 
{
    int x = loc.x;
    int y = loc.y;
    location::Vector borders;

	// Is it passible?
	if (IsPassible(x,y) == false) return borders;

	// Is it in-bounds?
//	if (x == 0 || y == 0) return true;
//	if (x == max().x || y == max().y) return true;

	int right  = (x < max().x) ? x + 1 : x;
	int bottom = (y < max().y) ? y + 1 : y;

	// Is it next to an impassible location?
	for (int check_x =  right-2; check_x <=  right; check_x ++) {
	for (int check_y = bottom-2; check_y <= bottom; check_y ++) {

		if (check_x != x || check_y != y) {
			if (IsPassible(check_x,check_y) == true) {
				borders.push_back(location(check_x,check_y));
			}
		}

	}	}

	return borders;
}


///////////////////////////////////////////////////////////////////////
/** Writes an ascii representation of a bitmap's mono mask.
	@param string& filename
	@param basic_bitmap* bbmp
*/
void bitmap::WriteLogFile (const std::string &filename) 
		throw (catch_all_exception) {

	std::ofstream outfile(filename.c_str());

	int width = bmiInfoHeader.width;
	int height = bmiInfoHeader.height;

	for (int y = 0; y < height; y ++) {
	for (int x = 0; x < width; x++) {

		location loc(x,y);
		outfile << (char)('`' + Mono(loc));
	}

		outfile << " : " << y << "\n";
	}

	char* runner = new char[2*width+4];
	
	MakeRunner(runner, 2*width+4);

	outfile << runner;

	outfile.close();	
	delete[] runner;
}


///////////////////////////////////////////////////////////////////////
// Makes an ascii line of numerals. Used when representing a map in ascii.
void bitmap::MakeRunner(char* runner, int width)
{
	width /= 2;
	width -= 2;
	
	for (int x = 0; x < width; x ++) {
		runner[x]       = '0' + (x%10);
		runner[x+width+2] = (x%10) ? ' ' : '0' + ((x/10)%10);
	}
	
	runner[width]   = ' ';
	runner[2*width+2] = '\0';

	runner[width+1]     = '\n';
	runner[2*width+1] = '\n';
}


///////////////////////////////////////////////////////////////////////
/* Write a bitmap's information to file 'fileName' */
void bitmap::WriteRawFile (const std::string &fileName) 
		throw (catch_all_exception) {

	int width  = GetWidth();
	int height = GetHeight();

	// Open the bitmap file
	std::ofstream 
		bitmapFile(fileName.c_str(), std::ios::out | std::ios::binary);

	if (!bitmapFile) {
		char msg[] = "Error opening file.\n";
		throw catch_all_exception(msg);
	}

	// Write header
	basic_bitmap::WriteHeader(bitmapFile);

	// Write color table
	if (palletSize > 0) basic_bitmap::WriteColorTable(bitmapFile);
	
	// Skip to the pixel data
	const int beginPixelData = bmiHeader.offset;
	bitmapFile.seekp ( beginPixelData );

	int bytesPerSample = bmiInfoHeader.bitCount/8;
	char* outputBuffer = new char[ paddedByteWidth*height ];
	char* pixelBuffer;

	for (int x = 0; x <  width; x++) {
	for (int y = 0; y < height; y++) {
		
		int bufferOffset = (x*bytesPerSample)+(y*paddedByteWidth);
		pixelBuffer = &outputBuffer[bufferOffset];
		ByteCast(x,y, pixelBuffer);

	}	}

	bitmapFile.write(outputBuffer, paddedByteWidth*height);
	bitmapFile.close();

	delete[] outputBuffer;
}


///////////////////////////////////////////////////////////////////////
// Returns an std::vector<location> object of the locations along
// a digital line from a to b.
location::Vector bitmap::GetLine (location a, location b)
{
	if (a > b) {
		location::Vector rev(GetLine(b,a));
		std::reverse(rev.begin(), rev.end());
		return rev;
	}

	location::Vector line;
	location::Vector null(0);
	line.push_back(a);

	if (a == b) return line;
	
	int	dx, dy, dx2, dy2, ix, iy, err, i;
	location pos(a);

	// difference between starting and ending points
	dx = b.x - a.x;
	dy = b.y - a.y;

	// calculate direction of the vector and store in ix and iy
	if (dx >= 0) ix = 1;
	if (dx < 0) {	ix = -1;	dx = abs(dx);	}

	if (dy >= 0) iy = 1;
	if (dy < 0) {	iy = -1;	dy = abs(dy);	}

	// scale deltas and store in dx2 and dy2
	dx2 = dx * 2;
	dy2 = dy * 2;

	// dx is the major axis
	if (dx > dy) {
		// initialize the error term
		err = dy2 - dx;

		for (i = 0; i < dx; i++) {
			if (err >= 0) {
				err -= dx2;
				pos.y += iy;
//				if (Mono(pos) == false) return null;
//				line.push_back(pos);
			}
			err += dy2;
			pos.x += ix;
			if (Mono(pos) == false) return null;			
			line.push_back(pos);
		}
	}
	
	// dy is the major axis
	else {
		// initialize the error term
		err = dx2 - dy;
		for (i = 0; i < dy; i++) {
			if (err >= 0) {
				err -= dy2;
				pos.x += ix;
//				if (Mono(pos) == false) return null;
//				line.push_back(pos);
			}
			err += dx2;
			pos.y += iy;
			if (Mono(pos) == false) return null;
			line.push_back(pos);
		}
	}

	assert(pos == b);
	return line;
} // end of void Line (...


///////////////////////////////////////////////////////////////////////
// Returns true if the locations along a digital line from a to b are
// traversable.
bool bitmap::HasLineOfSight(location a, location b) {

	if (a == b) return Mono(a);
	
	if (a > b) {
		return HasLineOfSight(b,a);
	}

	int	dx, dy, dx2, dy2, ix, iy, err, i;
	location pos(a);

	// difference between starting and ending points
	dx = b.x - a.x;
	dy = b.y - a.y;

	// calculate direction of the vector and store in ix and iy
	if (dx >= 0) ix = 1;
	else {	ix = -1;	dx = abs(dx);	}

	if (dy >= 0) iy = 1;
	else {	iy = -1;	dy = abs(dy);	}

	// scale deltas and store in dx2 and dy2
	dx2 = dx * 2;
	dy2 = dy * 2;

	// dx is the major axis
	if (dx > dy) {
		// initialize the error term
		err = dy2 - dx;
		for (i = 0; i < dx; i++) {
			if (err >= 0) {
				err -= dx2;
				pos.y += iy;
				if (Mono(pos) == false) return false;
			}
			err += dy2;
			pos.x += ix;
			if (Mono(pos) == false) return false;			
		}
	}
	
	// dy is the major axis
	else {
		// initialize the error term
		err = dx2 - dy;
		for (i = 0; i < dy; i++) {
			if (err >= 0) {
				err -= dy2;
				pos.x += ix;
				if (Mono(pos) == false) return false;
			}
			err += dx2;
			pos.y += iy;
			if (Mono(pos) == false) return false;
		}
	}

	assert(pos == b);
	return true;
} // end of void Line (...


///////////////////////////////////////////////////////////////////////
// Global function returning the L2 normal 
// distance between src and dest, multiplied by an integer.
// We're using integer weights and we need to discern
// between diagonal distances which in reality are less than one unit 
// of one another. So we multiply everything under the radical by
// some low power of two (like 2^16), take the integer approx. of the 
// square root, and our final weight is close to 2^8 times that of the
// actual real-valued distance.
undirectedLength GetWeight(location src, location dest, bitmap* bm) {
	if (!bm->HasLineOfSight(src,dest)) return INT_MAX;
	undirectedLength weight = src.L2scaled(dest);
	return weight;
}


