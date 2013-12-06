/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      septum.cc
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
******  REVISION HISTORY
******
******  Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations.
******
******  Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
******
******  Sep 2012           cspark@fnal.gov
****** - new class septum added for finite length
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>

#include <beamline/SeptumPropagators.h>
#include <beamline/septum.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>


using namespace std;

// **************************************************
//   class septum
// **************************************************


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

septum::septum( char const* n, double const& l )
  : bmlnElmnt( n, l ), voltage_(0.0), gap_(0.0), xWire_(0.0), wireWidth_(0.0),
    numberKicked_(0), numberBadHits_(0), numberBackHits_(0), numberOutGap_(0),
    turnNumber_(0), reportNumber_(0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

septum::septum( char const* n, double const& l, double const& v, double const& g )
  : bmlnElmnt( n, l ), voltage_(v), gap_(g), xWire_(0.0), wireWidth_(0.0),
    numberKicked_(0), numberBadHits_(0), numberBackHits_(0), numberOutGap_(0),
    turnNumber_(0), reportNumber_(0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

septum::septum( char const* n, double const& l, double const& v, double const& g, double const& xw )
  : bmlnElmnt( n, l ), voltage_(v), gap_(g), xWire_(xw), wireWidth_(0.0),
    numberKicked_(0), numberBadHits_(0), numberBackHits_(0), numberOutGap_(0),
    turnNumber_(0), reportNumber_(0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

septum::septum( char const* n, double const& l, double const& v, double const& g, double const& xw, double const& ww )
  : bmlnElmnt( n, l ), voltage_(v), gap_(g), xWire_(xw), wireWidth_(ww),
    numberKicked_(0), numberBadHits_(0), numberBackHits_(0), numberOutGap_(0),
    turnNumber_(0), reportNumber_(0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

septum::septum( septum const& x )
  : bmlnElmnt( x ), voltage_(x.voltage_), gap_(x.gap_), xWire_(x.xWire_),
    wireWidth_(x.wireWidth_), propagator_(x.propagator_->Clone() ),
    numberKicked_(x.numberKicked_), numberBadHits_(x.numberBadHits_),
    numberBackHits_(x.numberBackHits_), numberOutGap_(x.numberOutGap_),
    turnNumber_(x.turnNumber_), reportNumber_(x.reportNumber_)
{
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

septum& septum::operator=( septum const& rhs )
{
  if ( &rhs == this ) return *this;

  bmlnElmnt::operator=(rhs);

  //length_         = rhs.length_;
  voltage_        = rhs.voltage_;
  gap_            = rhs.gap_;
  xWire_          = rhs.xWire_;
  wireWidth_      = rhs.wireWidth_;
  numberKicked_   = rhs.numberKicked_;
  numberBadHits_  = rhs.numberBadHits_;
  numberBackHits_ = rhs.numberBackHits_;
  numberOutGap_   = rhs.numberOutGap_;
  turnNumber_     = rhs.turnNumber_;
  propagator_     = PropagatorPtr( rhs.propagator_->Clone() );

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

septum::~septum()
{
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::setVoltage( double const& x) {
  voltage_ = x;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::setGap( double const& x ) {
  if( 0.0 < x ) { gap_ = x; }
  if( gap_ < wireWidth_ ) { wireWidth_ = gap_; }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::setWire( double const& x) {
  xWire_ = x;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::setWireWidth( double const& x) {
  if( 0.0 < x ) { wireWidth_ = x; }
  if( gap_ < wireWidth_ ) { gap_ = wireWidth_; }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::setReportNumber( int const& x ) {
  if( 0 <= x ) { reportNumber_ = x; }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* septum::Type() const
{
  return "e_septum";
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool septum::isMagnet() const
{
  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& septum::readFrom(istream& is)
{
  // is; // >> voltage_ >> gap_ >> xWire_ >> wireWidth_;
  return is;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& septum::writeTo(ostream& os)
{
  //os; // << OSTREAM_DOUBLE_PREC << voltage_ << " " << gap_ << " " << xWire_ << " " << wireWidth_ << "\n";
  return os;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::accept( BmlVisitor& v )
{
   v.visit(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::accept( ConstBmlVisitor& v ) const
{
   v.visit(*this);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::localPropagate( Particle& p )
{
   (*propagator_)(*this, p);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::localPropagate( JetParticle& p )
{
   (*propagator_)(*this, p);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::localPropagate( ParticleBunch& b )
{
  turnNumber_++;
  if( 0 < reportNumber_ ) {
    if( turnNumber_ == reportNumber_ ) {
      time_t encodedTime;
      time( &encodedTime );
      tm* timestamp = localtime( &encodedTime );
      ostringstream namebuffer;
      namebuffer << "septum_report_"
                 << timestamp->tm_year + 1900
                 << timestamp->tm_mon + 1
                 << timestamp->tm_mday << '_'
                 << timestamp->tm_hour << '_'
                 << timestamp->tm_min  << ".dat";

      ofstream dataStream( namebuffer.str().c_str() );
      for( ParticleBunch::iterator it = b.begin();
                                   it != b.end();
                                   ++it                ) {
        dataStream << (*it).get_x() << "  " << (*it).get_npx() << endl;
      }
      dataStream.close();
    }
  }

  (*propagator_)(*this, b);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void septum::localPropagate( JetParticleBunch& b )
{
   (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class thinSeptum
// **************************************************


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( char const* n )
  : bmlnElmnt( n ), strengthPos_(0.0), strengthNeg_(0.0), xWire_(0.0),
    numberKicked_(0), numberBadHits_(0), numberBackHits_(0), numberOutGap_(0),
    wireWidth_(0.0), gap_(0.0), turnNumber_(0), reportNumber_(0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( char const* n, double const& sP, double const& sN, double const& xw)
  : bmlnElmnt( n ), strengthPos_(sP), strengthNeg_(sN), xWire_(xw),
    numberKicked_(0), numberBadHits_(0), numberBackHits_(0), numberOutGap_(0),
    wireWidth_(0.0), gap_(0.0), turnNumber_(0), reportNumber_(0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( double const& sP, double const& sN, double const& xw)
  : bmlnElmnt(), strengthPos_(sP), strengthNeg_(sN), xWire_(xw),
    numberKicked_(0), numberBadHits_(0), numberBackHits_(0), numberOutGap_(0),
    wireWidth_(0.0), gap_(0.0), turnNumber_(0), reportNumber_(0)
{
  propagator_ = PropagatorPtr( new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::thinSeptum( thinSeptum const& x ) 
  : bmlnElmnt( x ), strengthPos_(x.strengthPos_), strengthNeg_(x.strengthNeg_),
    xWire_(x.xWire_), propagator_(x.propagator_->Clone() ),
    numberKicked_(x.numberKicked_), numberBadHits_(x.numberBadHits_), numberBackHits_(x.numberBackHits_), numberOutGap_(x.numberOutGap_),
    wireWidth_(x.wireWidth_), gap_(x.gap_), turnNumber_(x.turnNumber_), reportNumber_(x.reportNumber_)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum& thinSeptum::operator=( thinSeptum const& rhs )
{
  if ( &rhs == this ) return *this;

  bmlnElmnt::operator=(rhs);

  strengthPos_    = rhs.strengthPos_;
  strengthNeg_    = rhs.strengthNeg_;
  xWire_          = rhs.xWire_;
  numberKicked_   = rhs.numberKicked_;
  numberBadHits_  = rhs.numberBadHits_;
  numberBackHits_ = rhs.numberBackHits_;
  numberOutGap_   = rhs.numberOutGap_;
  wireWidth_      = rhs.wireWidth_;
  gap_            = rhs.gap_;
  turnNumber_     = rhs.turnNumber_;
  propagator_     = PropagatorPtr( rhs.propagator_->Clone() );

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSeptum::~thinSeptum() 
{
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::setStrengths( double const& sPos, double const& sNeg ) {
  strengthPos_ = sPos;
  strengthNeg_ = sNeg;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::setWire( double const& x) {
  xWire_ = x;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::setWireWidth( double const& x) {
  if( 0.0 < x ) { wireWidth_ = x; }
  if( gap_ < wireWidth_ ) { gap_ = wireWidth_; }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::setGap( double const& x ) {
  if( 0.0 < x ) { gap_ = x; }
  if( gap_ < wireWidth_ ) { wireWidth_ = gap_; }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::setReportNumber( int const& x ) {
  if( 0 <= x ) { reportNumber_ = x; }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinSeptum::Type() const 
{ 
  return "thinSeptum"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinSeptum::isMagnet() const 
{ 
  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& thinSeptum::readFrom(istream& is) 
{
  is >> strengthPos_ >> strengthNeg_ >> xWire_;
  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& thinSeptum::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC << strengthPos_ << " " << strengthNeg_ << " " << xWire_ << "\n";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::accept( BmlVisitor& v )
{ 
   v.visit(*this); 
}
 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::accept( ConstBmlVisitor& v ) const 
{ 
   v.visit(*this); 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::localPropagate( Particle& p ) 
{ 
   (*propagator_)(*this, p); 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::localPropagate( JetParticle& p ) 
{ 
   (*propagator_)(*this, p); 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::localPropagate( ParticleBunch& b ) 
{ 
  turnNumber_++;
  if( 0 < reportNumber_ ) {
    if( turnNumber_ == reportNumber_ ) {
      time_t encodedTime;
      time( &encodedTime );
      tm* timestamp = localtime( &encodedTime );
      ostringstream namebuffer;
      namebuffer << "septum_report_"
                 << timestamp->tm_year + 1900
                 << timestamp->tm_mon + 1
                 << timestamp->tm_mday << '_'
                 << timestamp->tm_hour << '_'
                 << timestamp->tm_min  << ".dat";

      ofstream dataStream( namebuffer.str().c_str() );
      for( ParticleBunch::iterator it = b.begin();
                                   it != b.end();
                                   ++it                ) {
        dataStream << (*it).get_x() << "  " << (*it).get_npx() << endl;
      }
      dataStream.close();
    }
  }

  (*propagator_)(*this, b); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSeptum::localPropagate( JetParticleBunch& b ) 
{ 
   (*propagator_)(*this, b); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
