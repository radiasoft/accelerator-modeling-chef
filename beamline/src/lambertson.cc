/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      lambertson.cc
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <iomanip>
#include <beamline/lambertson.h>

using namespace std;

// **************************************************
//   class thinLamb
// **************************************************

thinLamb::thinLamb() : bmlnElmnt() {
  xSeptum = 0.0;
  ExtBeamline = 0;
  for (int i =0; i < 6 ; i++ ) RefState[i] = 0.0;
}

thinLamb::thinLamb( char* n)
: bmlnElmnt( n ) {
  xSeptum = 0.0;
  ExtBeamline = 0;
  for (int i =0; i < 6 ; i++ ) RefState[i] = 0.0;
}

thinLamb::thinLamb( char* n, double x, beamline* b, double* s)
: bmlnElmnt( n ) {
  xSeptum = x;
  ExtBeamline = b;
  for (int i =0; i < 6 ; i++ ) RefState[i] = s[i];
}

thinLamb::thinLamb( double x, beamline* b, double* s)
: bmlnElmnt( ) {
  xSeptum = x;
  ExtBeamline = b;
  for (int i =0; i < 6 ; i++ ) RefState[i] = s[i];
}

thinLamb::thinLamb( const thinLamb& x ) : bmlnElmnt( (bmlnElmnt&) x )
{
  xSeptum = x.xSeptum;
  ExtBeamline = x.ExtBeamline;
  for (int i =0; i < 6 ; i++ ) RefState[i] = x.RefState[i];
}

thinLamb::~thinLamb() {
}

void thinLamb::setSeptum( double x) {
 xSeptum = x;
}

void thinLamb::setBeamline( beamline* b) {
 ExtBeamline = b;
}

void thinLamb::setRefState( const double* x) {
  for (int i =0; i < 6 ; i++ ) RefState[i] = x[i];
}

void thinLamb::getRefState( double* x) {
  for (int i =0; i < 6 ; i++ ) x[i] = RefState[i];
}


void thinLamb::eliminate() {
 delete this;
}


const char* thinLamb::Type() const 
{ 
  return "thinLamb"; 
}


ostream& thinLamb::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << xSeptum;
  for ( int i = 0; i < 6; i++) {
    os  << " " << RefState[i];
  }
  os << "\n";
  return os;
}

istream& thinLamb::readFrom(istream& is) 
{
  is >> xSeptum;
  for ( int i = 0; i < 6; i++) {
    is >> RefState[i];
  }
  return is;
}
