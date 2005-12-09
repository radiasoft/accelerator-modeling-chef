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
#include <rfcavity.h>


using namespace std;

// **************************************************
//   class rfcavity 
// **************************************************
rfcavity::rfcavity( const char* name_arg) 
: bmlnElmnt(name_arg, 1.0, 0.0)
{
  w_rf = 0.0;
  phi_s = 0.0;
  sin_phi_s = 0.0;
  Q = 0.0;
  R = 0.0;
  h = -1.0;
}
  

rfcavity::rfcavity(double lng_arg,    // length [m]
                   double w_rf_arg,   // rf frequency [Hz]
                   double eV_arg,     // rf voltage   [eV]
                   double phi_s_arg,  // synchronous phase 
                   double Q_arg,      // Quality factor 
                   double R_arg       // shunt impedance 
                   ) 
: bmlnElmnt( lng_arg, eV_arg*1.0e-9 )
{
  w_rf = MATH_TWOPI*w_rf_arg;
  phi_s = phi_s_arg;
  sin_phi_s = sin(phi_s);
  Q = Q_arg;
  R = R_arg;
  h = -1.0;
}


rfcavity::rfcavity( char* name_arg,    // name
                    double lng_arg,    // length [m]
                    double w_rf_arg,   // rf frequency 
                    double eV_arg,     // rf voltage 
                    double phi_s_arg,  // synchronous phase 
                    double Q_arg,      // Quality factor 
                    double R_arg       // shunt impedance 
                  ) 
: bmlnElmnt( name_arg, lng_arg, eV_arg*1.0e-9 ) 
{
  w_rf = MATH_TWOPI*w_rf_arg;
  phi_s = phi_s_arg;
  sin_phi_s = sin(phi_s);
  Q = Q_arg;
  R = R_arg;
  h = -1.0;
}


rfcavity::rfcavity( const rfcavity& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
 w_rf      = x.w_rf;
 phi_s     = x.phi_s;
 sin_phi_s = x.sin_phi_s;
 Q         = x.Q;
 R         = x.R;
 h         = x.h;
}


rfcavity::~rfcavity(){
}

void rfcavity::eliminate() {
 delete this;
}

ostream& rfcavity::writeTo(ostream& os) 
{
  // Note that eV is returned also through bmlnElmnt::Strength()
  os << OSTREAM_DOUBLE_PREC << w_rf/MATH_TWOPI <<
    " " << length <<
    " " << strength << " " <<  phi_s << " " << Q << " " << R << "\n";
  return os;
}

istream& rfcavity::readFrom(istream& is) 
{
  double w, e, l;
  is >> w >> l >> e >> phi_s >> Q >> R;
  w_rf = w*MATH_TWOPI;
  sin_phi_s = sin(phi_s);
  setLength(l);
  setStrength(e);
  return is;
}


const char* rfcavity::Type() const 
{
  return "rfcavity"; 
}


void rfcavity::setHarmonicNumber( double n )
{
  if( 0 < n ) {
    h = n;
  }
}


void rfcavity::setHarmonicNumber( int n )
{
  this->setHarmonicNumber( (double) n );
}


void rfcavity::setFrequency( double f )
{
  if( 0 < f ) {
    w_rf = MATH_TWOPI*f;
  }
}


void rfcavity::setFrequencyRelativeTo( double f )
{
  if( (0 < f) && (0 < h) ) {
    MATH_TWOPI*( ((double) h)*f );
  }
}


void rfcavity::setRadialFrequency( double omega )
{
  if( 0 < omega ) {
    w_rf = omega;
  }
}


void rfcavity::setRadialFrequencyRelativeTo( double omega )
{
  if( (0 < omega) && (0 < h) ) {
    ((double) h)*omega;
  }
}


// **************************************************
//   class thinrfcavity 
// **************************************************
thinrfcavity::thinrfcavity(char *name_arg) : bmlnElmnt(name_arg, 0.0, 0.0)
{
  w_rf = 0.0;
  phi_s = 0.0;
  sin_phi_s = 0.0;
  Q = 0.0;
  R = 0.0;
  h = -1.0;
}
  
thinrfcavity::thinrfcavity(double w_rf_arg,   // rf frequency [Hz]
                   	   double eV_arg,     // rf voltage   [eV]
                   	   double phi_s_arg,  // synchronous phase 
                   	   double Q_arg,      // Quality factor 
                   	   double R_arg       // shunt impedance 
                   	   ) 
: bmlnElmnt( 0.0, eV_arg*1.0e-9 )
{
  w_rf = MATH_TWOPI*w_rf_arg;
  phi_s = phi_s_arg;
  sin_phi_s = sin(phi_s);
  Q = Q_arg;
  R = R_arg;
  h = -1.0;
}

thinrfcavity::thinrfcavity(char * name_arg,   // name
                   	   double w_rf_arg,   // rf frequency 
                   	   double eV_arg,     // rf voltage 
                   	   double phi_s_arg,  // synchronous phase 
                   	   double Q_arg,      // Quality factor 
                   	   double R_arg       // shunt impedance 
                   	   ) 
: bmlnElmnt( name_arg, 0.0, eV_arg*1.0e-9 ) 
{
  w_rf = MATH_TWOPI*w_rf_arg;
  phi_s = phi_s_arg;
  sin_phi_s = sin(phi_s);
  Q = Q_arg;
  R = R_arg;
  h = -1.0;
}

thinrfcavity::thinrfcavity( const thinrfcavity& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
 w_rf      = x.w_rf;
 phi_s     = x.phi_s;
 sin_phi_s = x.sin_phi_s;
 Q         = x.Q;
 R         = x.R;
 h         = x.h;
}


thinrfcavity::~thinrfcavity(){
}

void thinrfcavity::eliminate() {
 delete this;
}

ostream& thinrfcavity::writeTo(ostream& os) 
{
  // Note that eV is returned also through bmlnElmnt::Strength()
  os << OSTREAM_DOUBLE_PREC << w_rf/MATH_TWOPI <<
    " " << strength << " " <<  phi_s << " " << Q << " " << R << "\n";
  return os;
}

istream& thinrfcavity::readFrom(istream& is) 
{
  double w, e;
  is >> w >> e >> phi_s >> Q >> R;
  w_rf = w*MATH_TWOPI;
  sin_phi_s = sin(phi_s);
  setStrength(e);
  return is;
}



const char* thinrfcavity::Type() const 
{
  return "thinrfcavity"; 
}


void thinrfcavity::setHarmonicNumber( double n )
{
  if( 0 < n ) {
    h = n;
  }
}


void thinrfcavity::setHarmonicNumber( int n )
{
  this->setHarmonicNumber( (double) n );
}


void thinrfcavity::setFrequency( double f )
{
  if( 0 < f ) {
    w_rf = MATH_TWOPI*f;
  }
}


void thinrfcavity::setFrequencyRelativeTo( double f )
{
  if( (0 < f) && (0 < h) ) {
    MATH_TWOPI*( ((double) h)*f );
  }
}


void thinrfcavity::setRadialFrequency( double omega )
{
  if( 0 < omega ) {
    w_rf = omega;
  }
}


void thinrfcavity::setRadialFrequencyRelativeTo( double omega )
{
  if( (0 < omega) && (0 < h) ) {
    ((double) h)*omega;
  }
}


