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
******
******  - support for reference counted elements
******  - eliminated unecessary casts
******  - use std::string for renaming
******  - changes to header file to reduce file coupling 
******
******  Dec 2007 ostiguy@fnal.gov
******  - new typesafe propagator architecture
****** 
******  February 20, 2008  michelotti@fnal.gov
******  - bug fix: instantiated attribute propagator_
******    in the Solenoid constructors
****** 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <beamline/Solenoid.h>
#include <beamline/SolenoidPropagators.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


///////////////////////////////////////////////////
// 
// Constructors (and the destructor)
// 
///////////////////////////////////////////////////

Solenoid::Solenoid()
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::Solenoid( const char* n, double const& l, double const& s )
:   bmlnElmnt(n,l,s)
  , inEdge_(true)
  , outEdge_(true)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::Solenoid( Solenoid const& x )
:   bmlnElmnt(x)
  , inEdge_(x.inEdge_)
  , outEdge_(x.outEdge_)
  , propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid* Solenoid::Clone() const 
{ 
  return new Solenoid( *this ); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Solenoid::~Solenoid()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


///////////////////////////////////////////////////
// 
// Other member function(s) (except localPropagate)
// 
///////////////////////////////////////////////////

void Solenoid::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void Solenoid::Split( double const&, bmlnElmnt**, bmlnElmnt** ) const", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length_.
  // --------------------------------------------------
 
   SolenoidPtr s_a( (Clone()) );
   s_a->setLength( pc  *length_ );

   s_a->inEdge_   = inEdge_;    
   s_a->outEdge_  = false;      

   s_a->rename(  Name() + std::string("_1") );
  
   SolenoidPtr s_b( (Clone()) ); 

   s_b->setLength( ( 1.0 - pc )*length_ );

   s_b->inEdge_   = false;      
   s_b->outEdge_  = outEdge_;   
   s_b->rename( Name() + std::string( "_2" ) );

   a = s_a;
   b = s_b;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Solenoid::isMagnet() const
{
  return true;
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

void Solenoid::localPropagate( Particle &p ) 
{ 
  (*propagator_)(*this,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Solenoid::localPropagate( JetParticle &p ) 
{ 
  (*propagator_)(*this,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

