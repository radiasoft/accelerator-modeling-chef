/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      quadrupole.cc
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
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******  REVISION HISTORY
******
******  Apr 2008           michelotti@fnal.gov
******  - added quadrupole::setLength(..) method to override
******    the base class implementation.
******
******  Mar 2007           ostiguy@fnal.gov
******  - support for reference counted elements
******  - reduced src file coupling due to visitor interface.
******    visit() takes advantage of (reference) dynamic type.
******  - use std::string for string operations.
******
******  Dec 2007           ostiguy@fnal.gov
******  - new typesafe propagators
******
******  Jun 2013           michelotti@fnal.gov
******  - restored ability to change the number of thin
******    quadrupole kicks within the propagator.
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/quadrupole.h>
#include <beamline/drift.h>
#include <beamline/beamline.h>
#include <beamline/BmlVisitor.h>
#include <beamline/QuadrupolePropagators.h>
#include <beamline/Alignment.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;



// **************************************************
//   class quadrupole
// **************************************************

quadrupole::quadrupole()
  : bmlnElmnt( "", 1.0, 0.0 )
{
     propagator_ = PropagatorPtr( new Propagator( 4 ) );
     propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::quadrupole( const char* id, double const& l, double const& s )
  : bmlnElmnt( id, l, s )
{
     propagator_ = PropagatorPtr( new Propagator(4) );     
     propagator_->setup(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::quadrupole( quadrupole const& x ) 
  : bmlnElmnt(x), propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole* quadrupole::Clone() const 
{ 
  return new quadrupole( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::~quadrupole() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::setStrength( double const& s ) 
{
  bmlnElmnt::setStrength(s);
  double integratedStrength = strength_*length_;

  if( bml_) 
  {
    int counter = 0;
    for ( beamline::iterator it  = bml_->begin();
                             it != bml_->end(); ++it ) {
      if( typeid(**it) == typeid(thinQuad ) )  ++counter;
    }

    if( counter <= 0 ) {
      throw( GenericException( __FILE__, __LINE__, 
             "void quadrupole::setStrength( double const& s ) {", 
             "No thin quads in the internal beamline." ) );
    }
    else if( counter == 1) {
      if(elm_) 
      {
        elm_->setStrength( integratedStrength );
      }
      else 
      {
        throw( GenericException( __FILE__, __LINE__, 
               "void quadrupole::setStrength( double const& s ) {", 
               "elm_ not set." ) );
      }
    }
    else {
      for ( beamline::iterator it  = bml_->begin(); it != bml_->end(); ++it ) {
        if( typeid(**it) == typeid(thinQuad) ) {
          (*it)->setStrength( integratedStrength/counter );
        }
      }
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::setLength( double const& l ) 
{
  double ratio = length_;
  bmlnElmnt::setLength( l );
  ratio = length_ / ratio;

  if( bml_) 
  {
    int counter = 0;
    for ( beamline::iterator it  = bml_->begin(); it != bml_->end(); ++it ) {
      if( typeid(**it) == typeid(thinQuad ) )  ++counter;
    }

    if( counter <= 0 ) {
      throw( GenericException( __FILE__, __LINE__, 
             "void quadrupole::setLength( double const& s )", 
             "No thin quads in the internal beamline." ) );
    }
    else if( counter == 1) {
      if(elm_) 
      {
        elm_->setStrength( ratio*elm_->Strength() );
      }
      else 
      {
        throw( GenericException( __FILE__, __LINE__, 
               "void quadrupole::setLength( double const& s )", 
               "elm_ not set." ) );
      }
    }
    else {
      for ( beamline::iterator it  = bml_->begin(); it != bml_->end(); ++it ) {
        if( typeid(**it) == typeid(thinQuad) ) {
          (*it)->setStrength( ratio*((*it)->Strength()) );
        }
        else if( typeid(**it) == typeid(drift) ) {
          (*it)->setLength( ratio*((*it)->Length()) );
        }
        else {
          ostringstream uic;
          uic  << "Unrecognized element type "
               << (*it)->Type()
               << " in quadrupole "
               << Name()
               << "'s internal beamline.";
          throw( GenericException( __FILE__, __LINE__, 
                 "void quadrupole::setLength( double const& s )", 
                 uic.str().c_str() ) );
        }
      }
    }
  }
}

///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void quadrupole::setNumberOfKicks( int n )
{
  // This piece of code must mimic the body of the quadrupole constructor.
  propagator_ = PropagatorPtr( new Propagator(n) );
  propagator_->setup(*this);
}



///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int quadrupole::numberOfKicks() const
{
  quadrupole::Propagator const* the_pointer = dynamic_cast<quadrupole::Propagator*>(propagator_.get());
  return the_pointer->numberOfKicks();
}


///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* quadrupole::Type() const 
{ 
  return "quadrupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool quadrupole::isMagnet() const
{
  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           "void quadrupole::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length_.
  a = QuadrupolePtr( Clone() );
  b = QuadrupolePtr( Clone()  );

  a->setLength( pc        * length_ );
  b->setLength( (1.0- pc) * length_ );

  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  // -----------------------------------------------------------------
  a->setAlignment( Alignment() );
  b->setAlignment( Alignment() );

  // Rename
  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& quadrupole::readFrom(istream& is)
{
  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& quadrupole::writeTo(ostream& os)
{
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::accept( BmlVisitor& v )            
{  
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::accept( ConstBmlVisitor& v ) const { 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::localPropagate( Particle&    p )   
{ 
  (*propagator_)(*this, p);        
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  quadrupole::localPropagate( JetParticle& p )   
{ 
  (*propagator_)(*this, p);        
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::localPropagate( ParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);        
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::localPropagate( JetParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);        
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class thinQuad
// **************************************************

thinQuad::thinQuad() 
 : bmlnElmnt( "", 0.0, 0.0 ) // length, strength
{
  propagator_ = PropagatorPtr( new Propagator() ); 
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::thinQuad( const char* n, double const& s ) 
  : bmlnElmnt(n, 0.0, s) 
{
  propagator_ = PropagatorPtr( new Propagator() ); 
  propagator_->setup(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::thinQuad( thinQuad const& x ) 
  : bmlnElmnt( x ), propagator_(PropagatorPtr(x.propagator_->Clone()))
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad* thinQuad::Clone() const 
{  
  return new thinQuad( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::~thinQuad() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinQuad::Type() const 
{ 
  return "thinQuad"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinQuad::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::accept( BmlVisitor& v )            
{  
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::accept( ConstBmlVisitor& v ) const { 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::localPropagate( Particle&    p )   
{ 
  (*propagator_)(*this, p);        
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  thinQuad::localPropagate( JetParticle& p )   
{ 
  (*propagator_)(*this, p);        
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::localPropagate( ParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);        
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::localPropagate( JetParticleBunch& b ) 
{ 
  (*propagator_)(*this, b);        
}
