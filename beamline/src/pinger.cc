/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      pinger.cc
******                                                                
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
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** May 2008           ostiguy@fnal.gov
****** - added explicit assignment operator implementations.
******
**************************************************************************
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <beamline/pinger.h>
#include <beamline/PingerPropagators.h>
#include <beamline/BmlVisitor.h>

using namespace std;

/** Implements the classes Pinger, HPinger and VPinger.  This bmlnElmt kicks
    the beam the same as an hkick and vkick, but only the first time a
    particle passes through the device after it is armed.

    The user must explicitly arm the pinger for it to work.  Note also that
    having the pinger armed before a closed-orbit calculation is performed
    won't work!

    E. McCrory, November, 1997
    **/

/* baseclass Pinger Constructors */

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::Pinger() 
  : bmlnElmnt("", 0.0, 0.0 ), kick_direction_(0), counter_(-1)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::Pinger(std::string const& n) 
  : bmlnElmnt(n ,0.0, 0.0 ),  kick_direction_(0), counter_(-1)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::Pinger(std::string const& n, double const& k, double const& r, int c) 
  : bmlnElmnt(n, 0.0, k), kick_direction_(r), counter_(c)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::Pinger( Pinger const& o) 
  : bmlnElmnt(o),  kick_direction_(o.kick_direction_), counter_(o. counter_) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::~Pinger() 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger& Pinger::operator=( Pinger const& rhs)
{
  if ( this == &rhs ) return *this;
  bmlnElmnt::operator=(rhs);

  kick_direction_  = rhs.kick_direction_;
  counter_         = rhs.counter_;        

  return *this;
}
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* Pinger::Type() const 
{ 
  return "Pinger"; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  Pinger::isMagnet() const
{
  return false;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  Pinger::isThin() const
{
  return true;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  Pinger::isPassive() const
{
  return false;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  Pinger::isDriftSpace() const
{
  return false;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::arm( int n )  const 
{  
  counter_ = n; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Pinger::countdown() const 
{ 
   counter_ = (counter_ > -1 ) ? --counter_ : -1 ; 
   return (counter_ == -1);
} 

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::disarm()   const 
{ 
  counter_ = -1; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Pinger::isArmed() const 
{ 
  return ( counter_ >= 0 ); 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& Pinger::getKickDirection() const          
{ 
  return kick_direction_; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::setKickDirection(double const& k) 
{ 
  kick_direction_ = k; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& Pinger::readFrom(istream& is) 
{
  is >> kick_direction_ >> counter_;
  return is;
}
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& Pinger::writeTo(ostream& os) 
{
  os << kick_direction_ << "  " << counter_ << "\n";
  return os;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Pinger::accept( BmlVisitor& v ) 
{ 
    v.visit( *this ); 
}
  
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Pinger::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}


/*********  HPinger Constructors *********/

HPinger::HPinger() 
 : Pinger("", 0.0, 0.0, -1) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HPinger::HPinger(std::string const& name) 
 : Pinger(name, 0.0, 0.0, -1) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HPinger::HPinger(std::string const& name, double const& kick, int count) 
: Pinger(name, kick, 0.0, count) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HPinger::HPinger( HPinger const& p) 
 : Pinger(p) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HPinger::~HPinger() 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HPinger& HPinger::operator=( HPinger const& rhs)
{
  if ( this == &rhs ) return *this;
  Pinger::operator=(rhs);
  return *this;

}
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* HPinger::Type() const 
{ 
  return "HPinger"; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HPinger::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HPinger::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

/********** VPinger *********/


VPinger::VPinger() 
: Pinger("", 0.0, M_PI_2, -1) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VPinger::VPinger(std::string const& name) 
: Pinger(name, 0.0, M_PI_2, -1) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VPinger::VPinger(std::string const& name, double const& kick, int c) 
: Pinger(name, kick, M_PI_2, c) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VPinger::VPinger( VPinger const& p) 
 : Pinger(p) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VPinger::~VPinger() 
 {}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VPinger& VPinger::operator=( VPinger const& rhs)
{
  if ( this == &rhs ) return *this;
  Pinger::operator=(rhs);
  return *this;

}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* VPinger::Type() const 
{ 
  return "VPinger"; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VPinger::accept( BmlVisitor& v ) { 
  v.visit( *this ); 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VPinger::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}
