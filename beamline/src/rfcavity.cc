/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      rfcavity.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#include <iomanip>

#include "rfcavity.h"


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
}


rfcavity::rfcavity( const rfcavity& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
 w_rf      = x.w_rf;
 phi_s     = x.phi_s;
 sin_phi_s = x.sin_phi_s;
 Q         = x.Q;
 R         = x.R;
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
}

thinrfcavity::thinrfcavity( const thinrfcavity& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
 w_rf      = x.w_rf;
 phi_s     = x.phi_s;
 sin_phi_s = x.sin_phi_s;
 Q         = x.Q;
 R         = x.R;
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


