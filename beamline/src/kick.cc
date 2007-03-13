/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      kick.cc
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <iomanip>
#include <beamline/kick.h>
#include <beamline/BmlVisitor.h>

using namespace std;

// **************************************************
//   class vkick 
// **************************************************

vkick::vkick() 
: bmlnElmnt() {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Unfortunately, this is a special case. Normally a single "double" argument
// indicates length.

vkick::vkick( double const& k ) 
  : bmlnElmnt(0,0, k) 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick::vkick( const char* n ) 
  : bmlnElmnt(n, 0.0, 0.0) 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Unfortunately, this is a special case. Normally a single "double" argument
// indicates length.

vkick::vkick( const char* n, double const& k ) 
  : bmlnElmnt(n, 0.0, k) 
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick::vkick( vkick const& x )
: bmlnElmnt( x )
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick::vkick( const char* n, double const& l, double const& s ) 
: bmlnElmnt(n,l,s) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick&  vkick::operator=( vkick const& rhs) {

  if ( &rhs == this ) return *this;  
  bmlnElmnt::operator=(rhs);

  return *this; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick::~vkick() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* vkick::Type() const 
{ 
  return "vkick"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::accept( BmlVisitor& v )
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::accept( ConstBmlVisitor& v ) const
{
   v.visit( *this ); 
}

// **************************************************
//   class hkick
// **************************************************

hkick::hkick() : bmlnElmnt() {
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::hkick( double const& k ) 
  : bmlnElmnt( 0.0, k )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::hkick( const char* n ) 
 :bmlnElmnt(n, 0.0, 0.0) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Unfortunately, this is a special case. Normally a single "double" argument
// indicates length.

hkick::hkick( const char* n, double const& k ) 
: bmlnElmnt( n, 0.0, k)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::hkick( hkick const& x ) 
: bmlnElmnt( x )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::hkick( const char* n, double const& l, double const& s ) 
: bmlnElmnt(n,l,s) 
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick&  hkick::operator=( hkick const& rhs) 
{
  if ( &rhs == this ) return *this;  
  bmlnElmnt::operator=(rhs);

  return *this; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::~hkick() 
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* hkick::Type() const 
{ 
  return "hkick"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::accept( ConstBmlVisitor& v ) const 
{
  v.visit( *this ); 
}

// ************************************************
//   class kick
// ************************************************

kick::kick() 
  : bmlnElmnt(), horizontalKick_(0.0), verticalKick_(0.0)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( const char* s ) 
 : bmlnElmnt(s) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick(double const& hStrength, double const& vStrength ) 
  : bmlnElmnt(), horizontalKick_(hStrength), verticalKick_(vStrength)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick(const char* s, double const& hStrength, double const& vStrength) 
  : bmlnElmnt(s), horizontalKick_(hStrength), verticalKick_(vStrength)
{}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( double const& lng, double const& hStrength, double const& vStrength ) 
  : bmlnElmnt( "NONAME", lng , hStrength + vStrength ),   // strength is arbitrary
    horizontalKick_( hStrength ), verticalKick_ ( vStrength )
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( const char* s, double const& lng, double const& hStrength, double const& vStrength ) 
: bmlnElmnt( s, lng, hStrength + vStrength ),  // strength is arbitrary
  horizontalKick_( hStrength ), verticalKick_( vStrength )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( kick const& x )
  : bmlnElmnt (x),  horizontalKick_( x.horizontalKick_ ), verticalKick_(x.verticalKick_)
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::~kick() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick&  kick::operator=( kick const& rhs) {

  if ( &rhs == this ) return *this;  

  bmlnElmnt::operator=(rhs);

  horizontalKick_ = rhs.horizontalKick_; 
  verticalKick_   = rhs.verticalKick_; 

  return *this; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& kick::writeTo(ostream& os) {
	os << OSTREAM_DOUBLE_PREC << horizontalKick_ << "  " << verticalKick_;
	os << "\n";
	return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& kick::readFrom(istream& is) {
	double a,b;
	is >> a >> b;
	horizontalStrength() = a;
	verticalStrength() = b;
	return is;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* kick::Type() const 
{ 
  return "kick"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::accept(BmlVisitor& v)            
{  
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::accept(ConstBmlVisitor& v) const 
{ 
  v.visit( *this ); 
}
