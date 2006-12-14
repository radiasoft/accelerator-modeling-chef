/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rfcavity.cc
******  Version:   2.1
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <beamline/rfcavity.h>
#include <beamline/drift.h>

using namespace std;

// **************************************************
//   class rfcavity 
// **************************************************
rfcavity::rfcavity( const char* name_arg)
:   bmlnElmnt(name_arg, 1.0, 0.0)
  , w_rf(0.0)
  , phi_s(0.0)
  , sin_phi_s(0.0)
  , Q(0.0)
  , R(0.0)
  , h(-1.0)
  , _u(0)
  , _v(0)
{
  _finishConstructor();
}
  

rfcavity::rfcavity(double const& lng_arg,    // length [m]
                   double const& f_arg,      // rf frequency [Hz]
                   double const& eV_arg,     // rf voltage   [eV]
                   double const& phi_s_arg,  // synchronous phase 
                   double const& Q_arg,      // Quality factor 
                   double const& R_arg       // shunt impedance 
                   ) 
:   bmlnElmnt( lng_arg, eV_arg*1.0e-9 )
  , w_rf(MATH_TWOPI*f_arg)
  , phi_s(phi_s_arg)
  , sin_phi_s(sin(phi_s_arg))
  , Q(Q_arg)
  , R(R_arg)
  , h(-1.0)
  , _u(0)
  , _v(0)
{
  _finishConstructor();
}


rfcavity::rfcavity( const char* name_arg, // name
                    double const& lng_arg,    // length [m]
                    double const& f_arg,      // rf frequency 
                    double const& eV_arg,     // rf voltage 
                    double const& phi_s_arg,  // synchronous phase 
                    double const& Q_arg,      // Quality factor 
                    double const& R_arg       // shunt impedance 
                  ) 
:   bmlnElmnt( name_arg, lng_arg, eV_arg*1.0e-9 ) 
  , w_rf(MATH_TWOPI*f_arg)
  , phi_s(phi_s_arg)
  , sin_phi_s(sin(phi_s_arg))
  , Q(Q_arg)
  , R(R_arg)
  , h(-1.0)
  , _u(0)
  , _v(0)
{
  _finishConstructor();
}


rfcavity::rfcavity( const rfcavity& x ) 
:   bmlnElmnt( (bmlnElmnt&) x ) 
  , w_rf(x.w_rf)
  , phi_s(x.phi_s)
  , sin_phi_s(x.sin_phi_s)
  , Q(x.Q)
  , R(x.R)
  , h(x.h)
  , _u(0)
  , _v(0)
{
  int m = 1 + ( ( int(x._v) - int(x._u) )/sizeof( bmlnElmnt* ) );
  _u = new bmlnElmnt* [ m ];
  _v = &( _u[m-1] );
  
  for( int k = 0; k < m; k++ ) {
    _u[k] = ( x._u[k] )->Clone();
  }
}


void rfcavity::_finishConstructor()
{
  // NOTE: If this code is ever modified, you 
  // must also modify rfcavity::readFrom and rfcavity::writeTo
  if( 0 != _u ) { 
    // rfcavity has been constructed before;
    // it is now being modified
    // ------------------------------------
    bmlnElmnt* holder[3];
    _v = _u;
    for( int i = 0; i < 3; i++ ) { holder[i] = *(_v++); }

    _v = _u;
    *(_v++) = holder[0]->Clone();    // Could be: "*(_v++) = holder[0];"
    *(_v)   = holder[1]->Clone();
    dynamic_cast<thinrfcavity*>(*_v)->setHarmonicNumber(h);
    dynamic_cast<thinrfcavity*>(*_v)->setRadialFrequency(w_rf);
    dynamic_cast<thinrfcavity*>(*_v)->setPhi(phi_s);
    (*_v)->setStrength( this->Strength() );  // bmlnElmnt version invoked
    _v++;
    *(_v)   = holder[2]->Clone();

    for( int i = 0; i < 3; i++ ) { delete holder[i]; }
  }
  else { 
    // initial construction of rfcavity
    // -----------------------------------
    _u = new bmlnElmnt* [3];

    _v = _u;
    *(_v++) = new drift( this->Length()/2.0 );
    *(_v)   = new thinrfcavity( 0.0, 1.0e9*(this->Strength()), phi_s, Q, R );
    dynamic_cast<thinrfcavity*>(*_v)->setHarmonicNumber(h);
    dynamic_cast<thinrfcavity*>(*_v)->setRadialFrequency(w_rf);
    _v++;
    *(_v)   = new drift( this->Length()/2.0 );
  }
}


rfcavity::~rfcavity()
{
  while( _v >= _u ) { delete (*(_v--)); }
  delete [] _u;
}


void rfcavity::acceptInner( RefRegVisitor& v )
{
  double cdt = 0;
  _ctRef = 0.0;
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    if( 0 == strcmp( "thinrfcavity", (*x)->Type() ) ) {
      cdt = v.getCdt();
      v.setCdt(0.0);
      (*x)->accept(v);
      v.setCdt(cdt);
    }
    else {
      (*x)->accept( v );
      _ctRef += (*x)->getReferenceTime();
    }
    x++;
  }
}


void rfcavity::acceptInner( BmlVisitor& v )
{
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*x)->accept( v );
    x++;
  }
}


void rfcavity::acceptInner( ConstBmlVisitor& v )
{
  bmlnElmnt** x = _u;
  while( x <= _v ) {
    (*x)->accept( v );
    x++;
  }
}


ostream& rfcavity::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC 
     << (w_rf/MATH_TWOPI)
     << " " << phi_s
     << " " << Q
     << " " << R
     << " " << h
     << '\n';
  return os;
}

istream& rfcavity::readFrom(istream& is) 
{
  double w;
  is >> w 
     >> phi_s 
     >> Q 
     >> R
     >> h;
  w_rf = w*MATH_TWOPI;
  sin_phi_s = sin(phi_s);
  _finishConstructor();
  return is;
}


const char* rfcavity::Type() const 
{
  return "rfcavity"; 
}


void rfcavity::setHarmonicNumber( double const& n )
{
  if( 0 < n ) {
    h = n;
  }
  for( bmlnElmnt** i = _u; i <= _v; i++ ) {
    if( 0 == strcmp( "thinrfcavity", (*i)->Type() ) ) {
      dynamic_cast<thinrfcavity*>(*i)->setHarmonicNumber( n );
    }
  }
}


void rfcavity::setHarmonicNumber( int n )
{
  this->setHarmonicNumber( static_cast<double>(n) );
}


void rfcavity::setFrequency( double const& f )
{
  if( 0 < f ) {
    w_rf = MATH_TWOPI*f;
  }
  for( bmlnElmnt** i = _u; i <= _v; i++ ) {
    if( 0 == strcmp( "thinrfcavity", (*i)->Type() ) ) {
      dynamic_cast<thinrfcavity*>(*i)->setFrequency( f );
    }
  }
}


void rfcavity::setFrequencyRelativeTo( double const& f )
{
  if( (0 < f) && (0 < h) ) {
    MATH_TWOPI*( ((double) h)*f );
  }
  for( bmlnElmnt** i = _u; i <= _v; i++ ) {
    if( 0 == strcmp( "thinrfcavity", (*i)->Type() ) ) {
      dynamic_cast<thinrfcavity*>(*i)->setFrequencyRelativeTo( f );
    }
  }
}


void rfcavity::setRadialFrequency( double const& omega )
{
  if( 0 < omega ) {
    w_rf = omega;
  }
  for( bmlnElmnt** i = _u; i <= _v; i++ ) {
    if( 0 == strcmp( "thinrfcavity", (*i)->Type() ) ) {
      dynamic_cast<thinrfcavity*>(*i)->setRadialFrequency( omega );
    }
  }
}


void rfcavity::setRadialFrequencyRelativeTo( double const& omega )
{
  if( (0 < omega) && (0 < h) ) {
    w_rf = ((double) h)*omega;
  }
  for( bmlnElmnt** i = _u; i <= _v; i++ ) {
    if( 0 == strcmp( "thinrfcavity", (*i)->Type() ) ) {
      dynamic_cast<thinrfcavity*>(*i)->setRadialFrequencyRelativeTo( omega );
    }
  }
}


void rfcavity::setPhi( double const& angle )
{
  phi_s = angle;
  sin_phi_s = sin(angle);
  _finishConstructor();
};



void rfcavity::setStrength( double const& eV )
{
  bmlnElmnt::setStrength( eV*1.0e-9 );
  _finishConstructor();
}


// **************************************************
//   class thinrfcavity 
// **************************************************
thinrfcavity::thinrfcavity(const char *name_arg) 
:   bmlnElmnt(name_arg, 0.0, 0.0)
  , w_rf( 0.0 )
  , phi_s( 0.0 )
  , sin_phi_s( 0.0 )
  , Q( 0.0 )
  , R( 0.0 )
  , h( -1.0 )
{
}
  
thinrfcavity::thinrfcavity(double const& f_arg,      // rf frequency [Hz]
                   	   double const& eV_arg,     // rf voltage   [eV]
                   	   double const& phi_s_arg,  // synchronous phase 
                   	   double const& Q_arg,      // Quality factor 
                   	   double const& R_arg       // shunt impedance 
                   	   ) 
:   bmlnElmnt( 0.0, eV_arg*1.0e-9 )
  , w_rf( MATH_TWOPI*f_arg )
  , phi_s( phi_s_arg )
  , sin_phi_s( sin(phi_s) )
  , Q( Q_arg )
  , R( R_arg )
  , h( -1.0 )
{
}

thinrfcavity::thinrfcavity(const char * name_arg, // name
                   	   double const& f_arg,      // rf frequency 
                   	   double const& eV_arg,     // rf voltage 
                   	   double const& phi_s_arg,  // synchronous phase 
                   	   double const& Q_arg,      // Quality factor 
                   	   double const& R_arg       // shunt impedance 
                   	   ) 
:   bmlnElmnt( name_arg, 0.0, eV_arg*1.0e-9 ) 
  , w_rf( MATH_TWOPI*f_arg )
  , phi_s( phi_s_arg )
  , sin_phi_s( sin(phi_s) )
  , Q( Q_arg )
  , R( R_arg )
  , h( -1.0 )
{
}

thinrfcavity::thinrfcavity( const thinrfcavity& x ) 
:   bmlnElmnt( (bmlnElmnt&) x ) 
  , w_rf( x.w_rf )
  , phi_s( x.phi_s )
  , sin_phi_s( x.sin_phi_s )
  , Q( x.Q )
  , R( x.R )
  , h( x.h )
{
}


thinrfcavity::~thinrfcavity(){
}

ostream& thinrfcavity::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC
     << w_rf/MATH_TWOPI
     << " " <<  phi_s
     << " " << Q
     << " " << R
     << " " << h
     << '\n';
  return os;
}

istream& thinrfcavity::readFrom(istream& is) 
{
  double w;
  is >> w 
     >> phi_s 
     >> Q 
     >> R
     >> h;
  w_rf = w*MATH_TWOPI;
  sin_phi_s = sin(phi_s);
  return is;
}



const char* thinrfcavity::Type() const 
{
  return "thinrfcavity"; 
}


void thinrfcavity::setHarmonicNumber( double const& n )
{
  if( 0 < n ) {
    h = n;
  }
}


void thinrfcavity::setHarmonicNumber( int n )
{
  this->setHarmonicNumber( static_cast<double>(n) );
}


void thinrfcavity::setFrequency( double const& f )
{
  if( 0 < f ) {
    w_rf = MATH_TWOPI*f;
  }
}


void thinrfcavity::setFrequencyRelativeTo( double const& f )
{
  if( (0 < f) && (0 < h) ) {
    MATH_TWOPI*( ((double) h)*f );
  }
}


void thinrfcavity::setRadialFrequency( double const& omega )
{
  if( 0 < omega ) {
    w_rf = omega;
  }
}


void thinrfcavity::setRadialFrequencyRelativeTo( double const& omega )
{
  if( (0 < omega) && (0 < h) ) {
    w_rf = ((double) h)*omega;
  }
}


void thinrfcavity::setPhi( double const& angle )
{
  phi_s = angle;
  sin_phi_s = sin(angle);
};



