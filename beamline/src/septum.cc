/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      septum.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include <iomanip>

#include "septum.h"


using namespace std;

// **************************************************
//   class thinSeptum
// **************************************************

thinSeptum::thinSeptum() : bmlnElmnt() {
  cerr << "\n*** ERROR *** Cannot define thinSeptum without parameters.\n" 
       << endl;
  exit(1);
}

thinSeptum::thinSeptum( char* n )
: bmlnElmnt( n ) {
  strengthPos = 0.0;
  strengthNeg = 0.0;
  xWire = 0.0;
}

thinSeptum::thinSeptum( char* n, double sP, double sN, double xw)
: bmlnElmnt( n ) {
  strengthPos = sP;
  strengthNeg = sN;
  xWire = xw;
}

thinSeptum::thinSeptum( double sP, double sN, double xw)
: bmlnElmnt( ) {
  strengthPos = sP;
  strengthNeg = sN;
  xWire = xw;
}

thinSeptum::thinSeptum( const thinSeptum& x ) : bmlnElmnt( (bmlnElmnt&) x )
{
  strengthPos = x.strengthPos;
  strengthNeg = x.strengthNeg;
  xWire = x.xWire;
}


thinSeptum::~thinSeptum() {
}


void thinSeptum::setStrengths( double sPos, double sNeg ) {
 strengthPos = sPos;
 strengthNeg = sNeg;
}

void thinSeptum::setWire( double x) {
 xWire = x;
}


void thinSeptum::eliminate() {
 delete this;
}


const char* thinSeptum::Type() const 
{ 
  return "thinSeptum"; 
}


istream& thinSeptum::readFrom(istream& is) 
{
  is >> strengthPos >> strengthNeg >> xWire;
  return is;
}

ostream& thinSeptum::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << strengthPos << " " << strengthNeg << " " << xWire << "\n";
  return os;
}
