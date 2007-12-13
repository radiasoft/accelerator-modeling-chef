/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
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
****** Dec 2007           ostiguy@fnal
****** - modified implementation to make momentum dependence
******   consistent with other magnetic elements 
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

bool vkick::isMagnet() const 
{ 
  return true; 
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

bool hkick::isMagnet() const 
{ 
  return true; 
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
  : bmlnElmnt(),vh_ratio_(0.0) 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( const char* s ) 
  : bmlnElmnt(s), vh_ratio_(0.0) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick(double const& hStrength, double const& vStrength ) 
  : bmlnElmnt(0.0, hStrength), vh_ratio_( vStrength / hStrength )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick(const char* s, double const& hStrength, double const& vStrength) 
  : bmlnElmnt(s, 0.0, hStrength), vh_ratio_( vStrength / hStrength )
{}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( double const& lng, double const& hStrength, double const& vStrength ) 
  : bmlnElmnt( "NONAME", lng , hStrength), vh_ratio_( vStrength / hStrength )
{ }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( const char* s, double const& lng, double const& hStrength, double const& vStrength ) 
: bmlnElmnt( s, lng, hStrength ), vh_ratio_( vStrength / hStrength )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( kick const& x )
  : bmlnElmnt (x),  vh_ratio_( x.vh_ratio_ )
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

  vh_ratio_   = rhs.vh_ratio_; 

  return *this; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  kick::setHorStrength(double const& value)
{
  double vstrength = strength_* vh_ratio_; 
  strength_  = value;
  vh_ratio_  = (strength_ != 0.0) ? vstrength/strength_ : vstrength;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   kick::setVerStrength(double const& value)
{
  vh_ratio_ = (strength_ != 0.0) ? value/strength_ : value;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  kick::HorStrength() const
{
  return   strength_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double kick::VerStrength() const
{
  return   vh_ratio_*strength_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& kick::writeTo(ostream& os) {
	os << OSTREAM_DOUBLE_PREC << strength_ << "  " << strength_*vh_ratio_;
	os << "\n";
	return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& kick::readFrom(istream& is) {
	double a,b;
	is >> a >> b;
	strength_ = a;
	vh_ratio_  = b/a;
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

bool kick::isMagnet() const 
{ 
  return true; 
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
