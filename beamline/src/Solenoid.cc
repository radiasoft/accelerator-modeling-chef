/*************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Solenoid.cc
******  Author:    Leo Michelotti
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved
******
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
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
******  REVISION HISTORY:
******  ----------------
******  Date   : April 10, 2007
******  Author : Leo Michelotti
******  Changes: Original version.
******                                                                
******  Apr 2007 ostiguy@fnal.gov
******  - support for reference counted elements
******  - eliminated unnecessary casts
******  - use std::string for renaming
******  - changes to header file to reduce file coupling 
******  Dec 2007 ostiguy@fnal.gov
******  - new typesafe propagator architecture
******  February 20, 2008  michelotti@fnal.gov
******  - bug fix: instantiated attribute propagator_
******    in the Solenoid constructors
******  May 2008 ostiguy@fnal.gov
******  - setStrength() now dispatched to propagator by base class
******    (no longer virtual)
******  - added explicit implementation for assignment operator
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/Solenoid.h>
#include <beamline/SolenoidPropagators.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

Solenoid::Solenoid()
:  BmlnElmnt("", 1.0, 0.0), 
   inEdge_(true), outEdge_(true)
{
  propagator_ = PropagatorPtr( new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::Solenoid( const char* n, double const& l, double const& s )
:   BmlnElmnt(n,l,s),
    inEdge_(true), outEdge_(true)
{
  propagator_ = PropagatorPtr( new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::Solenoid( Solenoid const& x )
:   BmlnElmnt(x),
    inEdge_(x.inEdge_), outEdge_(x.outEdge_)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid* Solenoid::clone() const 
{ 
  return new Solenoid( *this ); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::~Solenoid()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid& Solenoid::operator=( Solenoid const& rhs)
{
  if ( &rhs == this ) return *this; 
  BmlnElmnt::operator=(rhs);
  inEdge_  = rhs.inEdge_;
  outEdge_ = rhs.outEdge_;
  return *this; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> Solenoid::split( double const& pc ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           "void Solenoid::Split( double const& pc) const", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length_.
  // --------------------------------------------------
 
   SolenoidPtr s_a( (clone()) );
   s_a->setLength( pc  *length_ );

   s_a->inEdge_   = inEdge_;    
   s_a->outEdge_  = false;      

   s_a->rename(  Name() + std::string("_1") );
  
   SolenoidPtr s_b( (clone()) ); 

   s_b->setLength( ( 1.0 - pc )*length_ );

   s_b->inEdge_   = false;      
   s_b->outEdge_  = outEdge_;   
   s_b->rename( Name() + std::string( "_2" ) );

   return std::make_pair(s_a, s_b);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Solenoid::isMagnet() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Solenoid::isThin() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Solenoid::isPassive() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Solenoid::isDriftSpace() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* Solenoid::Type() const
{
  return "Solenoid";
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Solenoid::hasInEdge()   const
{
  
  return inEdge_;
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Solenoid::hasOutEdge()  const
{
  return outEdge_;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::accept( BmlVisitor& v )            
{  
   v.visit( *this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

