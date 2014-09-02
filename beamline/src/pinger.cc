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
**************************************************************************
**************************************************************************
*************************************************************************/



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

Pinger::Pinger(char const* n) 
  : bmlnElmnt(n ,0.0, 0.0 ),  kick_direction_(0), counter_(-1)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::Pinger(char const* n, double const& k, double const& r, int c) 
  : bmlnElmnt(n, 0.0, k), kick_direction_(r), counter_(c)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::Pinger( Pinger const& o) 
  : bmlnElmnt(o),  kick_direction_(o.kick_direction_), counter_(o. counter_), 
    propagator_(PropagatorPtr(o.propagator_->Clone()))
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Pinger::~Pinger() 
{}

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

istream& Pinger::readFrom(istream& is) {
  is >> kick_direction_ >> counter_;
  return is;
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& Pinger::writeTo(ostream& os) {
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void Pinger::usePropagator( PropagatorPtr& x )
{
  propagator_ = PropagatorPtr( x->Clone() );
  propagator_->setup( *this );
}


// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::localPropagate( Particle& p) 
{ 
  (*propagator_)(*this,p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::localPropagate( JetParticle& p) 
{ 
  (*propagator_)(*this,p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::localPropagate( ParticleBunch& p) 
{ 
  (*propagator_)(*this,p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Pinger::localPropagate( JetParticleBunch& p) 
{ 
  (*propagator_)(*this,p);
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

/*********  HPinger Constructors *********/

HPinger::HPinger() 
 : Pinger("", 0.0, 0.0, -1) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HPinger::HPinger(char const* name) 
 : Pinger(name, 0.0, 0.0, -1) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HPinger::HPinger(char const* name, double const& kick, int count) 
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

char const* HPinger::Type() const 
{ 
  return "HPinger"; 
}
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  HPinger::isMagnet() const
{
  return false;
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

VPinger::VPinger(char const* name) 
: Pinger(name, 0.0, M_PI_2, -1) 
{}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VPinger::VPinger(char const* name, double const& kick, int c) 
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

const char* VPinger::Type() const 
{ 
  return "VPinger"; 
}

// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  VPinger::isMagnet() const
{
  return false;
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
