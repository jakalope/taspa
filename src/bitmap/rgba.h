#ifndef RGBA_H
#define RGBA_H

#include <vector>
#include "rgb.h"

/** @name Four byte RGB pixel object
	Defines a standard 24-bit bitmap pixel.
*/
class rgba {

	public:	
        typedef std::vector<rgba> Vector;
        
		unsigned char  b;			/* Blue value */
		unsigned char  g;		/* Green value */
		unsigned char  r;			/* Red value */
		unsigned char  a;		/* Reserved */

		bool operator != (const rgba &temp) {
			return	!((temp.b     == b)   &&
					  (temp.g    == g)  &&
					  (temp.r      == r));
		}

		bool operator == (const rgba &temp) {
			return	 ((temp.b     == b)   &&
					  (temp.g    == g)  &&
					  (temp.r      == r));
		}
		
		operator rgb () {
			rgb rgbt(b, g, r);
			return rgbt;
		}
		
		rgba operator- (rgba rgbq) {
			rgbq.b = abs(rgbq.b-b);
			rgbq.g = abs(rgbq.g-g);
			rgbq.r = abs(rgbq.r-r);
			return rgbq;
		}
		
		long l2norm() {
			return (long)(sqrtl(b*b+r*r+g*g));
		}

		rgba (	unsigned char blue, unsigned char green,
					unsigned char red ) :
			b(blue), g(green),	
			r(red),   a(0) {	}

		rgba (	unsigned char blue, unsigned char green,
					unsigned char red,  unsigned char alpha ) :
			b(blue), g(green),	
			r(red),   a(alpha) {	}
		
		rgba (rgb color) :
            b(color.b), g(color.g), r(color.r), a(0) { }
		
		rgba () { }
};

rgba RgbToRgba(rgb rgbt);

#endif
