#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "rfcavity.h"


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
