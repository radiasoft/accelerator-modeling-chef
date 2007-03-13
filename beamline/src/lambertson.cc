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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <iomanip>
#include <beamline/lambertson.h>
#include <beamline/BmlVisitor.h>

using namespace std;

// **************************************************
//   class thinLamb
// **************************************************

thinLamb::thinLamb() 
 : bmlnElmnt(), xSeptum_ (0.0), ExtBeamline_()
{
 for (int i =0; i < 6 ; ++i) RefState_[i] = 0.0;
}

thinLamb::thinLamb( char const* n)
  : bmlnElmnt( n ), xSeptum_(0.0), ExtBeamline_()
{

  for (int i =0; i < 6 ; ++i) RefState_[i] = 0.0;
}

thinLamb::thinLamb( char const* n, double const&x, BmlPtr& b, double* s)
  : bmlnElmnt( n ), xSeptum_ (x), ExtBeamline_(b)
{
  for (int i =0; i < 6 ; ++i) RefState_[i] = s[i];
}

thinLamb::thinLamb( double const& x, BmlPtr& b, double* s)
  : bmlnElmnt( ), xSeptum_(x), ExtBeamline_(b)
{
  for (int i =0; i < 6 ; i++ ) RefState_[i] = s[i];
}

thinLamb::thinLamb( thinLamb const& x ) 
  : bmlnElmnt( x ), xSeptum_(x.xSeptum_), ExtBeamline_(x.ExtBeamline_)
{
  for (int i =0; i < 6 ; ++i) RefState_[i] = x.RefState_[i];
}

thinLamb::~thinLamb() 
{}

void thinLamb::setSeptum( double const& x) {
 xSeptum_ = x;
}

void thinLamb::setBeamline( BmlPtr& b) {
 ExtBeamline_ = b;
}

void thinLamb::setRefState( const double* x) {
  for (int i =0; i < 6 ; i++ ) RefState_[i] = x[i];
}

void thinLamb::getRefState( double* x) {
  for (int i =0; i < 6 ; i++ ) x[i] = RefState_[i];
}


const char* thinLamb::Type() const 
{ 
  return "thinLamb"; 
}


ostream& thinLamb::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << xSeptum_;
  for ( int i = 0; i < 6; i++) {
    os  << " " << RefState_[i];
  }
  os << "\n";
  return os;
}

istream& thinLamb::readFrom(istream& is) 
{
  is >> xSeptum_;
  for ( int i = 0; i < 6; i++) {
    is >> RefState_[i];
  }
  return is;
}

void thinLamb::accept( BmlVisitor& v )            
{ 
  v.visit( *this ); 
}


void thinLamb::accept( ConstBmlVisitor& v ) const 
{  
   v.visit( *this ); 
}
