/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rfcavity.cc
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
****** REVISION HISTORY:
******                   
****** April 2007  ostiguy@fnal.gov
******
****** - support for reference counted elements 
****** - visitor interface takes advantage of compiler dynamic typing
******                                                                 
****** Dec 2007    ostiguy@fnal.gov
****** - new typesafe propagators
******
****** Apr 2008    michelotti@fnal.gov
****** - added placeholder rfcavity::setLength method
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <beamline/RFCavityPropagators.h>
#include <beamline/rfcavity.h>
#include <beamline/beamline.h>
#include <beamline/drift.h>
#include <beamline/BmlVisitor.h>
#include <beamline/RefRegVisitor.h>
#include <beamline/marker.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

// **************************************************
//   class rfcavity 
// **************************************************

rfcavity::rfcavity( const char* name_arg)
  :   bmlnElmnt(name_arg, 1.0, 0.0),
  w_rf_(0.0),
  phi_s_(0.0),
  sin_phi_s_(0.0),
  Q_(0.0),
  R_(0.0),
  h_(-1.0)

{
  propagator_ = PropagatorPtr(new Propagator() );
  propagator_->setup(*this);
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::rfcavity( const char* name_arg, // name
                    double const& lng_arg,    // length [m]
                    double const& f_arg,      // rf frequency 
                    double const& eV_arg,     // rf voltage 
                    double const& phi_s_arg,  // synchronous phase 
                    double const& Q_arg,      // Quality factor 
                    double const& R_arg       // shunt impedance 
                  ) 
:   bmlnElmnt( name_arg, lng_arg, eV_arg*1.0e-9 ) ,
    w_rf_(MATH_TWOPI*f_arg),
    phi_s_(phi_s_arg),
    sin_phi_s_(sin(phi_s_arg)),
    Q_(Q_arg), 
    R_(R_arg),
    h_(-1.0)
{
  propagator_ = PropagatorPtr(new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::rfcavity( rfcavity const& x ) 
:   bmlnElmnt( x ),
    w_rf_(x.w_rf_),
    phi_s_(x.phi_s_),
    sin_phi_s_(x.sin_phi_s_),
    Q_(x.Q_),
    R_(x.R_),
    h_(x.h_),
    propagator_ (x.propagator_->Clone()) 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::~rfcavity()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::Split( double const&, ElmPtr& a, ElmPtr& b ) const
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: "  << __FILE__ << "," << __LINE__
           << "\n*** WARNING ****: void rfcavity::Split( double const&, ElmPtr&, ElmPtr& ) const"
              "\n*** WARNING ****: Splitting a " << Type() << " is forbidden in this version."
              "\n*** WARNING ****: A marker and a clone of the original object will be returned."
              "\n*** WARNING ****: " 
           << std::endl;
  a = ElmPtr( new marker("PLACEHOLDER") );
  b = ElmPtr( Clone() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& rfcavity::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC 
     << (w_rf_/MATH_TWOPI)
     << " " << phi_s_
     << " " << Q_
     << " " << R_
     << " " << h_
     << '\n';
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& rfcavity::readFrom(istream& is) 
{
  double w;
  is >> w 
     >> phi_s_ 
     >> Q_ 
     >> R_
     >> h_;
  w_rf_ = w*MATH_TWOPI;
  sin_phi_s_ = sin(phi_s_);
  propagator_->setup(*this);
  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


const char* rfcavity::Type() const 
{
  return "rfcavity"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool    rfcavity::isMagnet() const 
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rfcavity::getReferenceTime()    const 
{
  double value = 0;

  for( beamline::iterator it = bml_->begin(); it != bml_->end();  ++it ) {
    value += (*it)->getReferenceTime();
  }

  return value;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setStrength( double const& strength)
{
  bmlnElmnt::setStrength(strength);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setLength( double const& strength)
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: Attempt to explicitly set the length of a rfcavity. "
              "\n*** WARNING ****: This is most likely an error.  Will not comply."
              "\n*** WARNING ****: Continuing, nonetheless... "
              "\n*** WARNING ****: " 
           << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setHarmonicNumber( double const& n )
{
  ThinRFCavityPtr q;

  for( beamline::iterator it = bml_->begin(); it != bml_->end();  ++it ) {
    if( (q = boost::dynamic_pointer_cast<thinrfcavity>(*it) )) q->setHarmonicNumber( n );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void rfcavity::setHarmonicNumber( int n )
{
  setHarmonicNumber( static_cast<double>(n) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setFrequency( double const& f )
{
  ThinRFCavityPtr q;

  for( beamline::iterator it = bml_->begin(); it != bml_->end();  ++it ) {
    if( (q = boost::dynamic_pointer_cast<thinrfcavity>(*it) )) q->setHarmonicNumber( h_ );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setFrequencyRelativeTo( double const& f )
{
  ThinRFCavityPtr q;

  if( (0 < f) && (0 < h_) ) {
    w_rf_ = MATH_TWOPI*h_*f;
  }

  for( beamline::iterator it = bml_->begin(); it != bml_->end();  ++it ) {
    if( (q =boost::dynamic_pointer_cast<thinrfcavity>(*it) )) q->setFrequencyRelativeTo( f );  
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setRadialFrequency( double const& omega )
{
  ThinRFCavityPtr q;

  if( omega > 0 ) {
    w_rf_ = omega;
  }
  for( beamline::iterator it = bml_->begin(); it != bml_->end();  ++it ) {
    if( (q = boost::dynamic_pointer_cast<thinrfcavity>(*it) )) q->setRadialFrequency( w_rf_ );  
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setRadialFrequencyRelativeTo( double const& omega )
{
  ThinRFCavityPtr q;

  if( (omega >0 ) && (h_ > 0) ) {
    w_rf_ = h_*omega;
  }

  for( beamline::iterator it = bml_->begin(); it != bml_->end();  ++it ) {
    if( (q=boost::dynamic_pointer_cast<thinrfcavity>(*it))) q->setRadialFrequency( w_rf_);
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setPhi( double const& angle )
{
  phi_s_     = angle;
  sin_phi_s_ = sin(angle);
  propagator_->setup(*this);
};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void rfcavity::setQ( double const& Q )
{
  Q_     = Q;
  propagator_->setup(*this);
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setR( double const& R )
{
  R_ = R;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rfcavity::getPhi() const 
{ 
  return phi_s_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rfcavity::getRadialFrequency() const 
{ 
 return w_rf_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double rfcavity::getDesignEnergyGain()   const
{
    return strength_*sin_phi_s_; 
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  rfcavity::getQ()  const 
{ 
  return Q_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rfcavity::getR() const 
{ 
  return R_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rfcavity::getHarmonicNumber() const 
{ 
  return h_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( Particle& p) 
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( JetParticle& p) 
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( ParticleBunch& b) 
{
  (*propagator_)(*this, b);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( JetParticleBunch& b) 
{
  (*propagator_)(*this, b);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class thinrfcavity 
// **************************************************

thinrfcavity::thinrfcavity(const char *name_arg) 
:   bmlnElmnt(name_arg, 0.0, 0.0)
  , w_rf_( 0.0 )
  , phi_s_( 0.0 )
  , sin_phi_s_( 0.0 )
  , Q_( 0.0 )
  , R_( 0.0 )
  , h_( -1.0 )
{
  propagator_ = PropagatorPtr(new Propagator() );
  propagator_->setup(*this);
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::thinrfcavity(const char * name_arg, // name
                   	   double const& f_arg,      // rf frequency 
                   	   double const& eV_arg,     // rf voltage 
                   	   double const& phi_s_arg,  // synchronous phase 
                   	   double const& Q_arg,      // Quality factor 
                   	   double const& R_arg       // shunt impedance 
                   	   ) 
:   bmlnElmnt( name_arg, 0.0, eV_arg*1.0e-9 ) 
  , w_rf_( MATH_TWOPI*f_arg )
  , phi_s_( phi_s_arg )
  , sin_phi_s_( sin(phi_s_) )
  , Q_( Q_arg )
  , R_( R_arg )
  , h_( -1.0 )
{
  propagator_ = PropagatorPtr(new Propagator() );
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::thinrfcavity( const thinrfcavity& x ) 
:   bmlnElmnt( x ) 
  , w_rf_( x.w_rf_ )
  , phi_s_( x.phi_s_ )
  , sin_phi_s_( x.sin_phi_s_ )
  , Q_( x.Q_ )
  , R_( x.R_ )
  , h_( x.h_ ), propagator_ (x.propagator_->Clone() )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


thinrfcavity::~thinrfcavity()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& thinrfcavity::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC
     << w_rf_/MATH_TWOPI
     << " " <<  phi_s_
     << " " << Q_
     << " " << R_
     << " " << h_
     << '\n';
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


istream& thinrfcavity::readFrom(istream& is) 
{
  double w;
  is >> w 
     >> phi_s_ 
     >> Q_ 
     >> R_
     >> h_;
  w_rf_      = w*MATH_TWOPI;
  sin_phi_s_ = sin(phi_s_);
  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinrfcavity::Type() const 
{
  return "thinrfcavity"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    thinrfcavity::isMagnet() const 
{

  return false;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::setHarmonicNumber( double const& n )
{
  if( 0 < n ) {
    h_ = n;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinrfcavity::setHarmonicNumber( int n )
{
  setHarmonicNumber( static_cast<double>(n) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinrfcavity::setFrequency( double const& f )
{
  if( f > 0) {
    w_rf_ = MATH_TWOPI*f;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void thinrfcavity::setFrequencyRelativeTo( double const& f )
{
  if( (f > 0) && (h_ > 0) ) {
    w_rf_ = MATH_TWOPI*h_*f;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::setRadialFrequency( double const& omega )
{
  if( omega > 0 ) {
    w_rf_ = omega;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::setRadialFrequencyRelativeTo( double const& omega )
{
  if( (omega > 0) && (h_>0) ) {
    w_rf_ = h_*omega;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::setPhi( double const& angle )
{
  phi_s_     = angle;
  sin_phi_s_ = sin(angle);
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  thinrfcavity::accept( BmlVisitor& v )
{
 
  v.visit(*this); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  thinrfcavity::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::getPhi()              const 
{ 
 return phi_s_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::getRadialFrequency()        const 
{ 
  return w_rf_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::getQ()                const 
{ 
  return Q_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::getR()                const 
{ 
  return R_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::getHarmonicNumber()   const 
{ 
  return h_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( Particle& p) 
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( JetParticle& p) 
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( ParticleBunch& b) 
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( JetParticleBunch& b) 
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

