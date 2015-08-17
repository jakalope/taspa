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

#ifndef MATRIX_H
#define MATRIX_H

#include <sstream>
#include <vector>
#include "../location/location.hpp"

template<typename T>
class matrix {

	public:
		typedef std::vector<std::vector<T> > t_matrix;
		typedef typename std::vector<T>::iterator y_iterator;
		typedef typename std::vector<std::vector<T> >::iterator x_iterator;
			
	protected:
		bool initialized;
		location _max;
	
		t_matrix myMatrix;
		
	public:
		matrix() { initialized = false; }
		
		matrix(matrix *m) {
			*this = *m;
		}
	
		matrix(int width, int height) {
			resize ( width, height );
		}
		
		matrix(int width, int height, T initial_value) {
			resize ( width, height, initial_value );
		}

		matrix& operator= (matrix *m) {
			myMatrix = m->myMatrix;
			initialized = m->initialized;
			_max = m->max;
		}			

//		y_iterator operator[] (const location& loc) {
//			y_iterator yit = myMatrix[loc.x].begin() + loc.y;
//			return yit;
//		}

        std::vector<T>& operator[] (size_t x) { return myMatrix[x]; }
        
        bool operator==(matrix<T>& Q) {
            for (int i = 0; i <= _max.x; i ++)
                if (!(myMatrix[i] == Q.myMatrix[i])) return false;
            
            return true;
        }

		// to do: can't extract data from the matrix with this; can only set
		// data.
		y_iterator operator() (int x, int y) {
			y_iterator yit = myMatrix[x].begin()+y; 
			return yit;
		}
			
		y_iterator operator() (location loc) {
			y_iterator yit = myMatrix[loc.x].begin()+loc.y; 
			return yit;
		}

		T GetValue(int x, int y) throw (std::out_of_range) { 
			try {return myMatrix.at(x).at(y);}
			catch (std::out_of_range e) { throw e; }
		}
		
		void SetValue(int x, int y, T _val) throw (std::out_of_range) { 
			try {myMatrix.at(x).at(y) = _val;}
			catch (std::out_of_range e) { throw e; }
		}

		// to do: how to I make this appropriately mutable?
		void resize (int width, int height) {
			myMatrix.resize(width, std::vector<T>(height));
			_max.x=width-1;
			_max.y=height-1;
			initialized = true;
		}
		
		void resize (int width, int height, T initial_value) {
			myMatrix.resize(width, std::vector<T>(height, initial_value));
			_max.x=width-1;
			_max.y=height-1;
			initialized = true;
		}
		
		int max_x() { return _max.x; }
		int max_y() { return _max.y; }
		location max() { return _max; }

		x_iterator begin() { return myMatrix.begin(); }
		x_iterator end()   { return myMatrix.end();   }
		
		y_iterator begin(int _x) { return myMatrix.at(_x).begin(); }
		y_iterator end  (int _x) { return myMatrix.at(_x).end();   }
		
		int Height() { return _max.y+1; }
		
        char* CharCast() {
            char* chars = new char[byte_size()];
            for (int r=0; r < Height(); r ++)
                memcpy(&chars[r*row_size()], &myMatrix[r][0], row_size());
            return chars;
        }
        
        size_t byte_size() { return Height()*Height()*sizeof(T); }
        size_t row_size() { return Height()*sizeof(T); }
};

#endif
