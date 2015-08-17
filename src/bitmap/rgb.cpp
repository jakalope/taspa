#include "rgb.h"

std::ostream& operator<< (std::ostream& out, rgb& urgb)
{
    out << std::dec << (int)urgb.r << " " << (int)urgb.g << " " << (int)urgb.b;
    return out;
}


std::istream& operator>> (std::istream& in, rgb& urgb)
{
    in >> std::skipws >> urgb.r >> urgb.g >> urgb.b;
    return in;
}
