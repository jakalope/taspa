#include "CurveWord.h"

CurveWord::CurveWord
(bitmap& bmp, const location::Vector& boundary, const std::string& relWord)
{
    std::string dblRel = relWord + 'F' + relWord;
    InterCurveWord intrCrvWrd(bmp,boundary,dblRel);
    assign((std::string)intrCrvWrd);

//    std::cerr << *this << std::endl;
}

std::string CurveWord::GetCurveWord
(bitmap& bmp, const location::Vector& boundary, const std::string& relWord)
{
    std::string dblRel = relWord + 'F' + relWord;
    InterCurveWord intrCrvWrd(bmp,boundary,dblRel);
    return (std::string)intrCrvWrd;

//    std::cerr << *this << std::endl;
}
