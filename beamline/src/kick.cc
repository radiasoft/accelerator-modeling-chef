#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "kick.h"

// ************************************************
//   class kick
// ************************************************

kick::kick() : bmlnElmnt() {
	horizontalKick = 0.0;
	verticalKick = 0.0;
}

kick::kick( char* s ) : bmlnElmnt(s) {
}

kick::kick(double hStrength, double vStrength ) : bmlnElmnt() {
	horizontalKick = hStrength;
	verticalKick = vStrength;
}

kick::kick(char* s, double hStrength, double vStrength) : bmlnElmnt(s) {
	horizontalKick = hStrength;
	verticalKick = vStrength;
}

kick::kick(const kick& x) : bmlnElmnt( (bmlnElmnt&) x ) {
	horizontalKick = x.horizontalKick;
	verticalKick = x.verticalKick;
}

kick::~kick() {
}

ostream& kick::writeTo(ostream& os) {
	os << OSTREAM_DOUBLE_PREC << horizontalKick << "  " << verticalKick;
	os << "\n";
	return os;
}

istream& kick::readFrom(istream& is) {
	double a,b;
	is >> a >> b;
	horizontalStrength() = a;
	verticalStrength() = b;
	return is;
}


char* kick::Type() const 
{ 
  return "kick"; 
}
