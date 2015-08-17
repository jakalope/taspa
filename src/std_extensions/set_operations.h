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
 
#ifndef SET_OPERATIONS_H
#define SET_OPERATIONS_H

#include <vector>
#include <set>
#include <list>
#include <algorithm>

template<class value_type>
inline bool Contains ( const std::vector<value_type>& S, const value_type& t )
{
	return (std::find(S.begin(), S.end(), t) != S.end());
}

template<class value_type> 
inline bool Contains ( const std::set<value_type>& S, const value_type& t )
{
	return (std::find(S.begin(), S.end(), t) != S.end());
}

template<class value_type> 
inline bool Contains ( const std::list<value_type>& S, const value_type& t )
{
	return (std::find(S.begin(), S.end(), t) != S.end());
}

#endif
