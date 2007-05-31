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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <beamline/rfcavity.h>
#include <beamline/drift.h>
#include <beamline/BmlVisitor.h>
#include <beamline/RefRegVisitor.h>

using namespace std;

// **************************************************
//   class rfcavity 
// **************************************************
rfcavity::rfcavity( const char* name_arg)
:   bmlnElmnt(name_arg, 1.0, 0.0)
  , w_rf_(0.0)
  , phi_s_(0.0)
  , sin_phi_s_(0.0)
  , Q_(0.0)
  , R_(0.0)
  , h_(-1.0)
  , u_(0)
  , v_(0)
{
  finishConstructor();
}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::rfcavity(double const& lng_arg,    // length [m]
                   double const& f_arg,      // rf frequency [Hz]
                   double const& eV_arg,     // rf voltage   [eV]
                   double const& phi_s_arg,  // synchronous phase 
                   double const& Q_arg,      // Quality factor 
                   double const& R_arg       // shunt impedance 
                   ) 
:   bmlnElmnt( lng_arg, eV_arg*1.0e-9 )
  , w_rf_(MATH_TWOPI*f_arg)
  , phi_s_(phi_s_arg)
  , sin_phi_s_(sin(phi_s_arg))
  , Q_(Q_arg)
  , R_(R_arg)
  , h_(-1.0)
  , u_(0)
  , v_(0)
{
  finishConstructor();
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
:   bmlnElmnt( name_arg, lng_arg, eV_arg*1.0e-9 ) 
  , w_rf_(MATH_TWOPI*f_arg)
  , phi_s_(phi_s_arg)
  , sin_phi_s_(sin(phi_s_arg))
  , Q_(Q_arg)
  , R_(R_arg)
  , h_(-1.0)
  , u_(0)
  , v_(0)
{
  finishConstructor();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::rfcavity( rfcavity const& x ) 
:   bmlnElmnt( x ) 
  , w_rf_(x.w_rf_)
  , phi_s_(x.phi_s_)
  , sin_phi_s_(x.sin_phi_s_)
  , Q_(x.Q_)
  , R_(x.R_)
  , h_(x.h_)
  , u_(0)
  , v_(0)
{
  int m = 1 + ( ( int(x.v_) - int(x.u_) )/sizeof( bmlnElmnt* ) );
  u_ = new bmlnElmnt* [ m ];
  v_ = &( u_[m-1] );
  
  for( int k=0; k < m; ++k) {
    u_[k] = ( x.u_[k] )->Clone();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::finishConstructor()
{
  // NOTE: If this code is ever modified, you 
  // must also modify rfcavity::readFrom and rfcavity::writeTo

  thinrfcavity* q = 0; 

  if( u_ ) { 

    // rfcavity has been constructed before;
    // it is now being modified
    // ------------------------------------
    bmlnElmnt* holder[3];
    v_ = u_;
    for( int i=0; i<3; ++i) { holder[i] = *(v_++); }

    v_ = u_;
    *(v_++) = holder[0]->Clone();    // Could be: "*(v_++) = holder[0];"
    *(v_)   = holder[1]->Clone();

    q = dynamic_cast<thinrfcavity*>(*v_);
    q->setHarmonicNumber(h_);
    q->setRadialFrequency(w_rf_);
    q->setPhi(phi_s_);
    q->setStrength( Strength() );  // bmlnElmnt version invoked
    v_++;
    *(v_)   = holder[2]->Clone();

    for( int i=0; i<3; ++i) { delete holder[i]; }
  }
  else { 

    // initial construction of rfcavity
    // -----------------------------------
    u_ = new bmlnElmnt* [3];

    v_ = u_;

    *(v_++) = new drift( this->Length()/2.0 );

    q = new thinrfcavity( 0.0, 1.0e9*strength_, phi_s_, Q_, R_ );
    q->setHarmonicNumber(h_);
    q->setRadialFrequency(w_rf_);

    *(v_++)   = q;
 
    *(v_)   = new drift( this->Length()/2.0 );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

rfcavity::~rfcavity()
{
  while( v_ >= u_ ) { delete (*(v_--)); }
  delete [] u_;
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
  finishConstructor();
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


void rfcavity::setStrength( double const& strength)
{
  bmlnElmnt::setStrength(strength);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void rfcavity::setHarmonicNumber( double const& n )
{
  thinrfcavity* q = 0;

  if( 0 < n ) {
    h_ = n;
  }
  for( bmlnElmnt** i = u_; i <= v_; ++i ) {
    if( (q =dynamic_cast<thinrfcavity*>(*i) )) q->setHarmonicNumber( n );
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

  thinrfcavity* q = 0;

  if( f > 0.0 ) {
    w_rf_ = MATH_TWOPI*f;
  }
  for( bmlnElmnt** i = u_; i <= v_; ++i ) {
    if( (q=dynamic_cast<thinrfcavity*>(*i)) ) q->setFrequency( f );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setFrequencyRelativeTo( double const& f )
{
  thinrfcavity* q = 0;

  if( (0 < f) && (0 < h_) ) {
    w_rf_ = MATH_TWOPI*h_*f;
  }

  for( bmlnElmnt** i = u_; i <= v_; ++i ) {
    if( (q=dynamic_cast<thinrfcavity*>(*i) ) ) 
             q->setFrequencyRelativeTo( f );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setRadialFrequency( double const& omega )
{
  thinrfcavity* q = 0;

  if( omega > 0 ) {
    w_rf_ = omega;
  }
  for( bmlnElmnt** i = u_; i <= v_; ++i) {
    if( (q=dynamic_cast<thinrfcavity*>(*i))) q->setRadialFrequency( omega );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setRadialFrequencyRelativeTo( double const& omega )
{

  thinrfcavity* q = 0;

  if( (omega >0 ) && (h_ > 0) ) {
    w_rf_ = h_*omega;
  }
  for( bmlnElmnt** i = u_; i <= v_; ++i) {
    if( (q=dynamic_cast<thinrfcavity*>(*i))) q->setRadialFrequencyRelativeTo( omega );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void rfcavity::setPhi( double const& angle )
{
  phi_s_     = angle;
  sin_phi_s_ = sin(angle);
  finishConstructor();
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void rfcavity::setQ( double const& Q )
{
  Q_     = Q;
  finishConstructor();
};


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::setR( double const& R )
{
  R_ = R;
  finishConstructor();
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

void rfcavity::acceptInner( BmlVisitor& v ) {

  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*x)->accept( v );
    ++x;
  }
}  


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::acceptInner( ConstBmlVisitor& v ) const {

  bmlnElmnt const* const* x = u_;

  while( x <= v_ ) {
    (*x)->accept( v );
    ++x;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::acceptInner( RefRegVisitor& v )
{
  double cdt = 0;
  ctRef_ = 0.0;
  thinrfcavity* q=0;
  
  bmlnElmnt** x = u_;

  while( x <= v_ ) {
    if( (q=dynamic_cast<thinrfcavity*>(*x) ))  
    {
      cdt = v.getCdt();
      v.setCdt(0.0);
      q->accept(v);
      v.setCdt(cdt);
    }
    else {
      (*x)->accept( v );
      ctRef_ += (*x)->getReferenceTime();
    }
    ++x;
  }
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
{}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::thinrfcavity(double const& f_arg,      // rf frequency [Hz]
                   	   double const& eV_arg,     // rf voltage   [eV]
                   	   double const& phi_s_arg,  // synchronous phase 
                   	   double const& Q_arg,      // Quality factor 
                   	   double const& R_arg       // shunt impedance 
                   	   ) 
:   bmlnElmnt( 0.0, eV_arg*1.0e-9 )
  , w_rf_( MATH_TWOPI*f_arg )
  , phi_s_( phi_s_arg )
  , sin_phi_s_( sin(phi_s_) )
  , Q_( Q_arg )
  , R_( R_arg )
  , h_( -1.0 )
{}

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
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinrfcavity::thinrfcavity( const thinrfcavity& x ) 
:   bmlnElmnt( x ) 
  , w_rf_( x.w_rf_ )
  , phi_s_( x.phi_s_ )
  , sin_phi_s_( x.sin_phi_s_ )
  , Q_( x.Q_ )
  , R_( x.R_ )
  , h_( x.h_ )
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

