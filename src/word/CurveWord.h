#ifndef CURVEWORD_H
#define CURVEWORD_H

#include "IntermediateCurveWord.h"
#include "CellularWord.h"
#include "Stair.h"
#include "../bitmap/bitmap.h"

#include <vector>
#include <string>

class CurveWord : public std::string
{
    private:

    public:
    
    CurveWord(bitmap&, const location::Vector&, const std::string&);
    static std::string GetCurveWord
        (bitmap&, const location::Vector&, const std::string&);
    
};

#endif
