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

#include <fstream>
#include <vector>
#include <map>

template<typename key_type, typename value_type>
bool Write
(std::ofstream& outfile, std::map<key_type,value_type>& m)
{
    std::vector< std::pair<key_type,value_type> > v(m.begin(), m.end());
    size_t size = sizeof(v[0])*v.size();
    outfile.write(dynamic_cast<char*>(size), sizeof(size));
    outfile.write(dynamic_cast<char*>(&v[0]), size);
}

template<typename key_type, typename value_type>
bool Read
(std::ifstream& infile, std::map<key_type,value_type>& m)
{
    size_t size;

    infile.read(dynamic_cast<char*>(size), sizeof(size));

    size_t elements = size/sizeof(std::pair<key_type,value_type>);
    
    std::vector< std::pair<key_type,value_type> > v(elements) );
    infile.read(dynamic_cast<char*>(&v[0]), size);
}

