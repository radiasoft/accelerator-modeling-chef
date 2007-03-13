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
#include <beamline/septum.h>
#include <beamline/BmlVisitor.h>


using namespace std;

// **************************************************
//   class thinSeptum
// **************************************************


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( char const* n )
: bmlnElmnt( n ), strengthPos_(0.0), strengthNeg_(0.0), xWire_(0.0)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( char const* n, double const& sP, double const& sN, double const& xw)
  : bmlnElmnt( n ), strengthPos_(sP), strengthNeg_(sN), xWire_(xw)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( double const& sP, double const& sN, double const& xw)
  : bmlnElmnt(), strengthPos_(sP), strengthNeg_(sN), xWire_(xw)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( thinSeptum const& x ) 
  : bmlnElmnt( x ), strengthPos_(x.strengthPos_), strengthNeg_(x.strengthNeg_),
    xWire_(x.xWire_)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum& thinSeptum::operator=( thinSeptum const& rhs) {

  if ( &rhs == this ) return *this;

  bmlnElmnt::operator=(rhs);

  strengthPos_ =  rhs.strengthPos_;
  strengthNeg_ =  rhs.strengthNeg_;
  xWire_       =  rhs.xWire_;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::~thinSeptum() {
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::setStrengths( double const& sPos, double const& sNeg ) {
 strengthPos_ = sPos;
 strengthNeg_ = sNeg;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::setWire( double const& x) {
 xWire_ = x;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinSeptum::Type() const 
{ 
  return "thinSeptum"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& thinSeptum::readFrom(istream& is) 
{
  is >> strengthPos_ >> strengthNeg_ >> xWire_;
  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& thinSeptum::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << strengthPos_ << " " << strengthNeg_ << " " << xWire_ << "\n";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::accept( BmlVisitor& v )
{ 
   v.visit(*this); 
}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::accept( ConstBmlVisitor& v ) const 
{ 
   v.visit(*this); 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
