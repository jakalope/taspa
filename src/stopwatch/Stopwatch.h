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

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <stdlib.h>
#include <sys/time.h>
#include <string>

std::string TimeStamp();

// Define number of seconds since program start.
#define SECONDS ((Tpf->tv_sec - Tps->tv_sec) + \
				 (Tpf->tv_usec - Tps->tv_usec)/1000000.)


class Stopwatch {
	
	private:
	timeval *Tps, *Tpf;

	public:
	Stopwatch() : 
		Tps((struct timeval*) malloc(sizeof(struct timeval))),
		Tpf((struct timeval*) malloc(sizeof(struct timeval))) { }
		
	void Start() {
		gettimeofday (Tps, 0);
	}
	
	float Lap() { gettimeofday (Tpf, 0); return SECONDS; }
	
	~Stopwatch() { free(Tps); free(Tpf); }
};

#endif
