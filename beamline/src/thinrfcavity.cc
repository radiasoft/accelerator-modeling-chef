#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "beamline.inc"

// **************************************************
//   struct thinrfcavityData
// **************************************************

thinrfcavityData::thinrfcavityData() : bmlnElmntData() {
 more = 1;
}

thinrfcavityData::thinrfcavityData( thinrfcavityData& x  ) 
: bmlnElmntData( (bmlnElmntData&) x ) 
{
 more      = x.more;
 w_rf      = x.w_rf;
 phi_s     = x.phi_s;
 sin_phi_s = x.sin_phi_s;
 eV        = x.eV;
 Q         = x.Q;
 R         = x.R;
}

thinrfcavityData::~thinrfcavityData() {
}


void thinrfcavityData::eliminate() {
 delete this;
}

void* thinrfcavityData::clone() {
 void* p = new thinrfcavityData( *this );
 return p;
}

// ??? REMOVE void thinrfcavityData::writeTo( FILE* f ) {
// ??? REMOVE  int sz = strlen( name );
// ??? REMOVE  if( sz == 0 ) {
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "*** ERROR *** void thinrfcavityData::writeTo( FILE* )\n" );
// ??? REMOVE   printf( "*** ERROR *** Anomoly in ident.  Quitting.           \n" );
// ??? REMOVE   printf( "*** ERROR ***                                        \n" );
// ??? REMOVE   printf( "\n" );
// ??? REMOVE   exit(0);
// ??? REMOVE  }
// ??? REMOVE 
// ??? REMOVE  fwrite( this, sizeof( thinrfcavityData ), 1, f );
// ??? REMOVE  fwrite( &sz, sizeof( int ), 1, f );
// ??? REMOVE  fprintf( f, "%s ", name );
// ??? REMOVE }
// ??? REMOVE 


// **************************************************
//   class thinrfcavity 
// **************************************************
thinrfcavity::thinrfcavity(char *name_arg) : bmlnElmnt(name_arg, 0.0, 0.0)
{
  w_rf = 0;
  eV = 0;
  phi_s = 0;
  sin_phi_s = 0;
  Q = 0;
  R = 0;
}
  
thinrfcavity::thinrfcavity(double w_rf_arg,   // rf frequency [Hz]
                   	   double eV_arg,     // rf voltage   [eV]
                   	   double phi_s_arg,  // synchronous phase 
                   	   double Q_arg,      // Quality factor 
                   	   double R_arg       // shunt impedance 
                   	   ) 
: bmlnElmnt( 0.0, eV_arg ) 
{
  w_rf = MATH_TWOPI*w_rf_arg;
  eV = eV_arg*1.0e-9;    // Conversion from eV to GeV
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
: bmlnElmnt( name_arg, 0.0, eV_arg ) 
{
  w_rf = MATH_TWOPI*w_rf_arg;
  eV = eV_arg*1.0e-9;    // Conversion from eV to GeV
  phi_s = phi_s_arg;
  sin_phi_s = sin(phi_s);
  Q = Q_arg;
  R = R_arg;
}

thinrfcavity::thinrfcavity( thinrfcavity& x ) 
: bmlnElmnt( (bmlnElmnt&) x ) 
{
 w_rf      = x.w_rf;
 phi_s     = x.phi_s;
 sin_phi_s = x.sin_phi_s;
 eV        = x.eV;
 Q         = x.Q;
 R         = x.R;
}


thinrfcavity::thinrfcavity( thinrfcavityData& x ) 
: bmlnElmnt( (bmlnElmntData&) x ) 
{
 w_rf      = x.w_rf;
 phi_s     = x.phi_s;
 sin_phi_s = x.sin_phi_s;
 eV        = x.eV;
 Q         = x.Q;
 R         = x.R;

}
thinrfcavity::~thinrfcavity(){
}

thinrfcavityData* thinrfcavity::image() {
 thinrfcavityData* p = new thinrfcavityData;
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->more      = 1;
 p->w_rf      = w_rf;
 p->phi_s     = phi_s;
 p->sin_phi_s = sin_phi_s;
 p->eV        = eV;
 p->Q         = Q;
 p->R         = R;

 return p;
}

void thinrfcavity::image( int d, slist* s, BMLN_posInfo* cg ) {
 int       j;
 thinrfcavityData* p = new thinrfcavityData;
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->address   = this;
 p->depth     = d;
 p->more      = 1;
 p->w_rf      = w_rf;
 p->phi_s     = phi_s;
 p->sin_phi_s = sin_phi_s;
 p->eV        = eV;
 p->Q         = Q;
 p->R         = R;

 geomToEnd( *cg );
 cg->outPoint = geometry.outPoint;
 for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];

     p->geometry.inPoint  = geometry.inPoint;
     p->geometry.outPoint = geometry.outPoint;
     for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
     for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];

 s->append( p );
}

void thinrfcavity::eliminate() {
 delete this;
}

ostream& thinrfcavity::writeTo(ostream& os) 
{
  // Note that w_rf is returned also through bmlnElmnt::Strength()
  os << OSTREAM_DOUBLE_PREC << w_rf/MATH_TWOPI <<
    " " << eV*1.0e9 << " " <<  phi_s << " " << Q << " " << R << "\n";
  return os;
}

istream& thinrfcavity::readFrom(istream& is) 
{
  double w, e;
  is >> w >> e >> phi_s >> Q >> R;
  w_rf = w*MATH_TWOPI;
  eV   = e/1.0e9;
  sin_phi_s = sin(phi_s);
  return is;
}
