/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      lambertson.cc
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
******                                                               
**************************************************************************
*************************************************************************/



#include <iomanip>
#include <beamline/Particle.h>
#include <beamline/lambertson.h>
#include <beamline/LambertsonPropagators.h>
#include <beamline/BmlVisitor.h>

using namespace std;

// **************************************************
//   class thinLamb
// **************************************************

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinLamb::thinLamb() 
 : bmlnElmnt(), turnNumber_(0), xSeptum_ (0.0), ExtBeamline_()
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);

  for (int i =0; i < 6 ; ++i) RefState_[i] = 0.0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinLamb::thinLamb( char const* n)
  : bmlnElmnt( n ), turnNumber_(0), xSeptum_(0.0), ExtBeamline_()
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);

  for (int i =0; i < 6 ; ++i) RefState_[i] = 0.0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinLamb::thinLamb( char const* n, double const&x, BmlPtr& b, double* s)
  : bmlnElmnt(n), turnNumber_(0), xSeptum_ (x), ExtBeamline_(b)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);

  for (int i =0; i < 6 ; ++i) RefState_[i] = s[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinLamb::thinLamb( double const& x, BmlPtr& b, double* s)
  : bmlnElmnt( ), turnNumber_(0), xSeptum_(x), ExtBeamline_(b)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);

  for (int i =0; i < 6 ; i++ ) RefState_[i] = s[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinLamb::thinLamb( thinLamb const& x ) 
  : bmlnElmnt( x ), turnNumber_(x.turnNumber_), xSeptum_(x.xSeptum_), 
    propagator_(x.propagator_->Clone()), ExtBeamline_(x.ExtBeamline_)
{
  for (int i =0; i < 6 ; ++i) { RefState_[i] = x.RefState_[i]; }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinLamb& thinLamb::operator=( thinLamb const& rhs )
{
  if ( &rhs == this ) return *this;

  bmlnElmnt::operator=(rhs);

  turnNumber_  = rhs.turnNumber_;
  xSeptum_     = rhs.xSeptum_;
  ExtBeamline_ = rhs.ExtBeamline_;
  propagator_  = PropagatorPtr( rhs.propagator_->Clone() );

  for (int i =0; i < 6 ; ++i) { RefState_[i] = rhs.RefState_[i]; }

  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinLamb::~thinLamb() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinLamb::toGo( Particle const& p )
{
  if( std::string("KILL") == p.getTag() ) {
    cout << "DGN: " << __FILE__ << "," << __LINE__
         << ": LAMBERTSON: Am now eliminating particle with state = "
         << p.State()
         << endl;
  }
  return ( std::string("KILL") == p.getTag() );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::setSeptum( double const& x) {
 xSeptum_ = x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::setBeamline( BmlPtr& b) {
 ExtBeamline_ = b;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::setRefState( const double* x) {
  for (int i =0; i < 6 ; i++ ) RefState_[i] = x[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::getRefState( double* x) {
  for (int i =0; i < 6 ; i++ ) x[i] = RefState_[i];
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinLamb::Type() const 
{ 
  return "thinLamb"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int thinLamb::passages() const
{
  return turnNumber_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::resetPassNumber( int n )
{
  turnNumber_ = n;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinLamb::isMagnet() const
{
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& thinLamb::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << xSeptum_;
  for ( int i = 0; i < 6; i++) {
    os  << " " << RefState_[i];
  }
  os << "\n";
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& thinLamb::readFrom(istream& is) 
{
  is >> xSeptum_;
  for ( int i = 0; i < 6; i++) {
    is >> RefState_[i];
  }
  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::accept( BmlVisitor& v )            
{ 
  v.visit( *this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::accept( ConstBmlVisitor& v ) const 
{  
   v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinLamb::usePropagator( PropagatorPtr& x )
{
  propagator_ = PropagatorPtr( x->Clone() );
  propagator_->setup( *this );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::localPropagate( Particle& p ) 
{  
   (*propagator_)(*this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::localPropagate( JetParticle& p ) 
{  
   (*propagator_)(*this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::localPropagate( ParticleBunch& b ) 
{  
   (*propagator_)(*this, b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::localPropagate( JetParticleBunch& b ) 
{  
   (*propagator_)(*this, b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

