#include "sextupole.h"

// **************************************************
//   class JetthinSext
// **************************************************

JetthinSext::JetthinSext ( double s, int index ) 
: bmlnElmnt() {
 // The strength is to be interpreted as
 // B''l in  Tesla / meter
 K2L.setVariable( s, index );
 K2L += s;
 strength = s;
 strengthIndex = index;
}

JetthinSext::JetthinSext ( char* n, 
			   double s,
			   int index) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // B''l in  Tesla / meter
 K2L.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}


JetthinSext::JetthinSext( bmlnElmntData& x ) : bmlnElmnt( x ) {
 cerr << "*** ERROR *** \n"
      << "*** ERROR *** JetthinSext::JetthinSext( bmlnElmntData& x ) \n"
      << "*** ERROR *** Sorry, Jack!  You can't do this.\n"
      << endl;
 exit (1);
}

JetthinSext::JetthinSext( const JetthinSext& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  K2L = x.K2L;
  strengthIndex = x.strengthIndex;
}

JetthinSext::~JetthinSext() {
}

void JetthinSext::setStrength( double s ) {
 K2L.setVariable( s, strengthIndex );
 strength = s;
}


void JetthinSext::setStrength( double s, int index ) {
 K2L.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}

void JetthinSext::eliminate() {
 delete this;
}


const char* JetthinSext::Type() const 
{ 
  return "JetthinSext"; 
}


ostream& JetthinSext::writeTo(ostream& os)
{
  os << strengthIndex << "\n";
  return os;
}

istream& JetthinSext::readFrom(istream& is)
{
  is >> strengthIndex;
  return is;
}
