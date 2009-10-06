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
****** - support for reference counted elements 
****** - visitor interface takes advantage of compiler dynamic typing
****** Dec 2007    ostiguy@fnal.gov
****** - new typesafe propagators
****** Apr 2008    michelotti@fnal.gov
****** - added placeholder rfcavity::setLength method
****** May 2008 ostiguy@fnal.gov
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <basic_toolkit/GenericException.h>
#include <beamline/RFCavityPropagators.h>
#include <beamline/rfcavity.h>
#include <beamline/beamline.h>
#include <beamline/Drift.h>
#include <beamline/BmlVisitor.h>
#include <beamline/marker.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

// **************************************************
//   class rfcavity 
// **************************************************

rfcavity::rfcavity( std::string const& name)
  :   BmlnElmnt(name, 1.0, 0.0),
    f_(0.0),
    phi_s_(0.0),
    sin_phi_s_(0.0),
    Q_(0.0),
    R_(0.0),
    h_(-1.0)

{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::rfcavity( std::string const& name,
                    double const& l,      // length [m]
                    double const& f,      // rf frequency 
                    double const& GeV,    // rf voltage 
                    double const& phis,   // synchronous phase 
                    double const& Q,      // Quality factor 
                    double const& R       // shunt impedance 
                  ) 
:   BmlnElmnt( name, l, GeV*1.0e-9 ) ,
    f_( f),
    phi_s_(phis),
    sin_phi_s_(sin(phis)),
    Q_(Q), 
    R_(R),
    h_(-1.0)
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::rfcavity( rfcavity const& o ) 
:   BmlnElmnt( o ),
    f_(o.f_),
    phi_s_(o.phi_s_),
    sin_phi_s_(o.sin_phi_s_),
    Q_(o.Q_),
    R_(o.R_),
    h_(o.h_)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::~rfcavity()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity& rfcavity::operator=( rfcavity const& rhs) 
{
  if ( this == &rhs) return *this;
  BmlnElmnt::operator=(rhs);
  f_         = rhs.f_;
  phi_s_     = rhs.phi_s_;
  sin_phi_s_ = rhs.sin_phi_s_;
  Q_         = rhs.Q_;
  R_         = rhs.R_;
  h_         = rhs.h_;
  return *this;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> rfcavity::split( double const& pct) const
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: "  << __FILE__ << "," << __LINE__
           << "\n*** WARNING ****: void " << Type() << "::split( double const&) const"
              "\n*** WARNING ****: Splitting a " << Type() << " is forbidden in this version."
              "\n*** WARNING ****: " 
           << std::endl;
  ostringstream uic;
  uic  <<   "Splitting a " << Type() << " is forbidden in this version.";
  throw( GenericException( __FILE__, __LINE__, 
         "rfcavity::split( double const& ) const", 
         uic.str().c_str() ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& rfcavity::writeTo(ostream& os) 
{
#if 0
  os << OSTREAM_DOUBLE_PREC 
     << f_
     << " " << phi_s_
     << " " << Q_
     << " " << R_
     << " " << h_
     << '\n';
#endif
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& rfcavity::readFrom(istream& is) 
{
  double w;
  is >> f_
     >> phi_s_ 
     >> Q_ 
     >> R_
     >> h_;
  sin_phi_s_ = sin(phi_s_);
  propagator_->ctor(*this);
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

bool    rfcavity::isThin() const 
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    rfcavity::isPassive() const 
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    rfcavity::isDriftSpace() const 
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setHarmon( double const& h )
{
  h_ = h;

  propagator_->setAttribute(*this, "HARMONIC_NUMBER", h );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setFrequency( double const& f )
{
  f_ = f;
  propagator_->setAttribute(*this, "FREQUENCY", f_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setFrequencyRelativeTo( double const& f )
{
  f_ *= h_;  
  propagator_->setAttribute(*this, "FREQUENCY", f_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setPhi( double const& angle )
{
  phi_s_     =  angle;
  sin_phi_s_ =  sin(angle);

  propagator_->setAttribute(*this, "PHIS", phi_s_ );

};

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setQ( double const& Q )
{
  Q_     = Q;
  propagator_->setAttribute(*this, "QFACTOR", Q_ );

};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setR( double const& R )
{
  R_ = R;
  propagator_->setAttribute(*this, "SHUNTR", f_ );
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

double const& rfcavity::phi() const 
{ 
  return phi_s_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rfcavity::frequency() const 
{ 
 return f_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  rfcavity::Q()  const 
{ 
  return Q_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rfcavity::R() const 
{ 
  return R_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& rfcavity::harmon() const 
{ 
  return h_; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::propagateReference( Particle& particle, double initialBRho, bool scaling ) 
{               
  propagator_->propagateReference(particle, initialBRho, scaling );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class thinrfcavity 
// **************************************************

thinrfcavity::thinrfcavity(std::string const& name) 
 :   BmlnElmnt(name, 0.0, 0.0),
    f_( 0.0 ), 
    phi_s_( 0.0 ),
    sin_phi_s_( 0.0 ),
    Q_( 0.0 ),
    R_( 0.0 ),
    h_( -1.0 )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::thinrfcavity(std::string const& name,
                   	   double const& f,          // rf frequency 
                   	   double const& gevs,        // rf voltage 
                   	   double const& phis,       // synchronous phase 
                   	   double const& Q,          // Quality factor 
                   	   double const& R           // shunt impedance 
                   	   ) 
    :   BmlnElmnt( name, 0.0, gevs*1.0e-9 ), 
	f_(f),
	phi_s_( phis),
	sin_phi_s_( sin(phis) ),
	Q_( Q ),
	R_( R ),
        h_( -1.0 )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::thinrfcavity( thinrfcavity const& o ) 
:   BmlnElmnt( o ), 
    f_( o.f_ ),
    phi_s_( o.phi_s_ ),
    sin_phi_s_( o.sin_phi_s_ ),
    Q_( o.Q_ ),
    R_( o.R_ ),
    h_( o.h_ )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::~thinrfcavity()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity& thinrfcavity::operator=( thinrfcavity const& rhs) 
{
  if ( this == &rhs) return *this;
  BmlnElmnt::operator=(rhs);
  f_         = rhs.f_;
  phi_s_     = rhs.phi_s_;
  sin_phi_s_ = rhs.sin_phi_s_;
  Q_         = rhs.Q_;
  R_         = rhs.R_;
  h_         = rhs.h_;

  return *this;

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& thinrfcavity::writeTo(ostream& os) 
{
#if 0
  os << OSTREAM_DOUBLE_PREC
     << w_rf_/MATH_TWOPI
     << " " <<  phi_s_
     << " " << Q_
     << " " << R_
     << " " << h_
     << '\n';
#endif
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


istream& thinrfcavity::readFrom(istream& is) 
{

  is >> f_ 
     >> phi_s_ 
     >> Q_ 
     >> R_
     >> h_;
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

bool    thinrfcavity::isThin() const 
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    thinrfcavity::isPassive() const 
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    thinrfcavity::isDriftSpace() const 
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::setHarmon( double const& h )
{
  h_ = ( h > 0 ) ? h : -1; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::setFrequency( double const& f )
{
  f_ = f;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void thinrfcavity::setFrequencyRelativeTo( double const& f )
{
  if( (f > 0) && (h_ > 0) ) {
    f_ = h_* f;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinrfcavity::setQ( double const& Q )
{
  Q_ = Q;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::setR( double const& R )
{
  R_ = R;
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

double const& thinrfcavity::phi()               const 
{ 
 return phi_s_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::Q()                 const 
{ 
  return Q_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::R()                 const 
{ 
  return R_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::harmon()    const 
{ 
  return h_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& thinrfcavity::frequency()    const 
{ 
  return f_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

