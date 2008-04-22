/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sextupole.cc
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
****** Apr 2008           michelotti@fnal.gov
****** - added sextupole::setLength(..) method to override
******   the base class implementation.
****** 
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** 
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** 
****** Apr 2008           michelotti@fnal.gov
****** - changed signature of setStrength(..) to
******   match that of the virtual bmlnElmnt method
****** 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <beamline/beamline.h>
#include <beamline/sextupole.h>
#include <beamline/drift.h>
#include <beamline/BmlVisitor.h>
#include <beamline/SextupolePropagators.h>
#include <beamline/Alignment.h>

using namespace std;


// **************************************************
//   class sextupole 
// **************************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::sextupole ()
: bmlnElmnt( "", 1.0, 0.0 ) 
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::sextupole ( const char* n, double l, double s ) 
: bmlnElmnt( n, l, s ) 
{
  propagator_ =  PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::sextupole( sextupole const& x ) 
  : bmlnElmnt( x ), propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole* sextupole::Clone() const 
{ 
  return new sextupole( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::~sextupole() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::setStrength( double const& s ) 
{
  bmlnElmnt::setStrength(s);
  elm_->setStrength( strength_*length_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::setLength( double const& l ) 
{
  double ratio = length_;
  bmlnElmnt::setLength( l );
  ratio = length_ / ratio;

  if( bml_) 
  {
    int counter = 0;
    for ( beamline::iterator it  = bml_->begin(); it != bml_->end(); ++it ) {
      if( typeid(**it) == typeid(thinSextupole) )  ++counter;
    }

    if( counter <= 0 ) {
      throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
             "void sextupole::setLength( double const& s )", 
             "No thin sextupoles in the internal beamline." ) );
    }
    else {
      for ( beamline::iterator it  = bml_->begin(); it != bml_->end(); ++it ) {
        if( typeid(**it) == typeid(thinSextupole) ) {
          (*it)->setStrength( ratio*((*it)->Strength()) );
        }
        else if( typeid(**it) == typeid(drift) ) {
          (*it)->setLength( ratio*((*it)->Length()) );
        }
        else {
          ostringstream uic;
          uic  << "Unrecognized element type "
               << (*it)->Type()
               << " in " << Type() << " " << Name()
               << "'s internal beamline.";
          throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
                 "void sextupole::setLength( double const& s )", 
                 uic.str().c_str() ) );
        }
      }
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::setCurrent( double s ) {
 elm_->setCurrent( s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* sextupole::Type() const 
{ 
  return "sextupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void sextupole::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const",
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length_.
  SextupolePtr s_a( (Clone()) );
  SextupolePtr s_b( (Clone()) );

  s_a->setLength(       pc  *length_ );
  s_b->setLength(  (1.0-pc )*length_ );

  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  a->setAlignment( Alignment() );
  b->setAlignment( Alignment() );

  // Rename
  s_a->rename( ident_ + string("_1") );
  s_b->rename( ident_ + string("_2") );

  a = s_a;
  b = s_b;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sextupole::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::localPropagate( Particle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::localPropagate( JetParticle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::localPropagate( ParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::localPropagate( JetParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}


// **************************************************
//   class thinSextupole
// **************************************************

thinSextupole::thinSextupole () 
: bmlnElmnt( "", 0.0, 0.0 ) {
 // The strength is to be interpreted as
 // (1/2)*B''l in  Tesla / meter
  propagator_ =  PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::thinSextupole ( const char* n, double s ) 
: bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // (1/2)*B''l in  Tesla / meter
  propagator_ =  PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::thinSextupole( thinSextupole const& x ) 
  : bmlnElmnt( x ), propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole* thinSextupole::Clone() const 
{ 
  return new thinSextupole( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::~thinSextupole() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinSextupole::Type() const 
{ 
  return "thinSextupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinSextupole::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::localPropagate( Particle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::localPropagate( JetParticle& p ) 
{ 
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::localPropagate( ParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::localPropagate( JetParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);
}
