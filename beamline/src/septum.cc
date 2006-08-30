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
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
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
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <beamline/septum.h>


using namespace std;

// **************************************************
//   class thinSeptum
// **************************************************

thinSeptum::thinSeptum() : bmlnElmnt() {
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "thinSeptum::thinSeptum() : bmlnElmnt() {", 
         "Default construction not permitted in this version." ) );
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
