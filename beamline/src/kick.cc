/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
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
****** May 2008           ostiguy@fnal.gov
****** - attribute changes now dispatched to propagator
****** - added explicit implementation for assigment operator.
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** - strength_ is now B*L (used to be kick angle) 
******   so that behavior is consistent with other magnets 
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
******
******  
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <iomanip>
#include <beamline/kick.h>
#include <beamline/KickPropagators.h>
#include <beamline/BmlVisitor.h>

using namespace std;

// **************************************************
//   class vkick 
// **************************************************

vkick::vkick() 
: bmlnElmnt()
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick::vkick( std::string const& n ) 
  : bmlnElmnt(n, 0.0, 0.0) 
{   
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Unfortunately, this is a special case. Normally a single "double" argument
// indicates length.

vkick::vkick( std::string const& n, double const& k ) 
  : bmlnElmnt(n, 0.0, k) 
{ 
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick::vkick( vkick const& x )
  : bmlnElmnt( x )
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vkick::vkick( std::string const& n, double const& l, double const& s ) 
: bmlnElmnt(n,l,s) 
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}


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

bool vkick::isThin() const 
{ 
  //--------------------------------------------------------------------------- 
  // NOTE: even though a kicker is allowed to have a finite length 
  //       is is a thin element because strength_ is the integrated strength
  //       and changing its length_ attribute will *not* change this value.  
  //---------------------------------------------------------------------------
  return true; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool vkick::isPassive() const 
{ 
  return false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool vkick::isDriftSpace() const 
{ 
  return false; 
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

hkick::hkick() : bmlnElmnt()
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::hkick( std::string const& n ) 
 :bmlnElmnt(n, 0.0, 0.0) 
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Unfortunately, this is a special case. Normally a single "double" argument
// indicates length.

hkick::hkick( std::string const& n, double const& k ) 
: bmlnElmnt( n, 0.0, k)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::hkick( hkick const& x ) 
: bmlnElmnt( x )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hkick::hkick( std::string const& n, double const& l, double const& s ) 
: bmlnElmnt(n,l,s) 
{ 
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}


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

bool hkick::isThin() const 
{ 
  //--------------------------------------------------------------------------- 
  // NOTE: even though a kicker is allowed to have a finite length 
  //       is is a thin element because strength_ is the integrated strength
  //       and changing its length_ attribute will *not* change this value.  
  //---------------------------------------------------------------------------
  return true; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool hkick::isPassive() const 
{ 
  return false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool hkick::isDriftSpace() const 
{ 
  return false; 
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
  : bmlnElmnt("",0.0, 0.0), vh_ratio_(0.0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( std::string const& s ) 
  : bmlnElmnt(s), vh_ratio_(0.0) 
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick(std::string const& s, double const& hStrength, double const& vStrength) 
  : bmlnElmnt(s, 0.0, hStrength ),  vh_ratio_( (hStrength != 0.0) ? vStrength/hStrength: vStrength )
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( std::string const& s, double const& lng, double const& hStrength, double const& vStrength ) 
: bmlnElmnt( s, lng, hStrength ), vh_ratio_( ( hStrength != 0.0) ? vStrength/ hStrength : vStrength )
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

kick::kick( kick const& x )
  : bmlnElmnt (x), vh_ratio_(x.vh_ratio_)
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

  vh_ratio_ = rhs.vh_ratio_;

  return *this; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& kick::writeTo(ostream& os) 
{
#if 0
	os << OSTREAM_DOUBLE_PREC << Strength() << "  " << vh_ratio_*Strength();
	os << "\n";
#endif
	return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& kick::readFrom(istream& is) {
	double a,b;
	is >> a >> b;
	setStrength(a );
	vh_ratio_  = (a !=0) ? b/a : b;
	return is;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  kick::setHorStrength(double const& value)
{
  double vstrength = Strength() * vh_ratio_; 
  setStrength( value);
  vh_ratio_  = (Strength() != 0.0) ? vstrength/Strength() : vstrength;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   kick::setVerStrength(double const& value)
{
  vh_ratio_ = ( Strength() != 0.0) ? value/Strength() : value;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  kick::getHorStrength() const
{
  return   Strength(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double kick::getVerStrength() const
{
  return   vh_ratio_*Strength(); 
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

bool kick::isThin() const 
{ 
  //--------------------------------------------------------------------------- 
  // NOTE: even though a kicker is allowed to have a finite length 
  //       is is a thin element because strength_ is the integrated strength
  //       and changing its length_ attribute will *not* change this value.  
  //---------------------------------------------------------------------------
  return true; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool kick::isPassive() const 
{ 
  return false; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool kick::isDriftSpace() const 
{ 
  return false; 
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

