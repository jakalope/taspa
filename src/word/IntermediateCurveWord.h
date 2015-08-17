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

#ifndef INTERMEDIATECURVEWORD_H
#define INTERMEDIATECURVEWORD_H

#include <vector>
#include <string>

#include "Stair.h"
#include "../bitmap/bitmap.h"

class InterCurveWord : public std::string {
    
    public:
    
    InterCurveWord
    (bitmap& bmp, const location::Vector& boundary, const std::string& relWord); 
    
    static std::string GetCurvatureWord
    (bitmap& bmp, const location::Vector& boundary, const std::string& relWord); 

    static Stair::Case Find_CX_Staircases(const std::string& curveWord);
    static Stair::Case Find_XC_Staircases(const std::string& curveWord);

    std::string SetConcaveCurvature(std::string curveWord);
    std::string SetLinearCurvature(std::string curveWord);

};

#endif
