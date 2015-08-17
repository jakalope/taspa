#ifndef RGB_H
#define RGB_H

#include <set>
#include <iostream>

/** @name Three byte RGB pixel object
	Defines a standard 24-bit bitmap pixel.
*/
struct rgb {

    typedef std::set<rgb> Set;

    unsigned char  b;		/* Blue value */
    unsigned char  g;		/* Green value */
    unsigned char  r;		/* Red value */

    bool operator != (const rgb &temp) {
        return	!((temp.b    == b) &&
                  (temp.g    == g)  &&
                  (temp.r    == r));
    }

    bool operator == (const rgb &temp) {
        return	 ((temp.b    == b)  &&
                  (temp.g    == g)  &&
                  (temp.r    == r));
    }
    
    bool operator < (const rgb& temp) const {
        return	  (temp.b     > b)   ||
        
                 ((temp.b     == b) &&
                  (temp.g    > g))  ||

                 ((temp.b     == b)  &&
                  (temp.g    == g) &&
                  (temp.r      > r));
    }
    
    rgb& operator= (char* tmp)
        { b = tmp[0]; g = tmp[1]; r = tmp[2]; return *this; }

    rgb ( unsigned char blue, unsigned char green, unsigned char red ) :
        b(blue), g(green), r(red) { }

    rgb (unsigned char *t) : b(t[0]), g(t[1]), r(t[2]) { }
    rgb () { }		
};

std::istream& operator>> (std::istream& in, rgb& urgb);
std::ostream& operator<< (std::ostream& out, rgb& urgb);

#endif
