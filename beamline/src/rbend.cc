#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "rbend.h"
#include "MathConstants.h"
#include "PhysicsConstants.h"

// **************************************************
//   class rbend
// **************************************************

rbend::rbend( double l, double s, PropFunc* pf ) 
: bmlnElmnt( l, s, pf ) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = M_TWOPI;
 tanPFAngle = 0.0;
}

rbend::rbend( double l, double s, double angle, PropFunc* pf ) 
: bmlnElmnt( l, s, pf ) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = angle;
 tanPFAngle    = tan( poleFaceAngle );
}


rbend::rbend( char* n, double l, double s, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf ) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = M_TWOPI;
 tanPFAngle = 0.0;
}

rbend::rbend( char* n, double l, double s, double angle, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf ) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = angle;
 tanPFAngle    = tan( poleFaceAngle );
}


rbend::rbend( const rbend& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
  poleFaceAngle = x.poleFaceAngle;
  tanPFAngle    = tan( poleFaceAngle );
}

rbend::~rbend() {
}

void rbend::eliminate() {
 delete this;
}


const char* rbend::Type() const { 
  return "rbend"; 
}


double rbend::OrbitLength( const Particle& x )
{
  static double tworho;
  tworho  = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength;
  return tworho * asin( length / tworho );
}


void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** rbend::Split                       \n"
            "*** ERROR *** pc = " << pc << 
                               "and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  *a = new rbend( pc*length, strength );
  *b = new rbend( (1.0 - pc)*length, strength );

  // Rename
  char* newname;
  newname = new char [ strlen(ident) + 6 ];

  strcpy( newname, ident );
  strcat( newname, "_CL_A" );
  (*a)->Rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_CL_B" );
  (*b)->Rename( newname );

  delete [] newname;
}


double rbend::setPoleFaceAngle( const Particle& p )
{
  this->poleFaceAngle =   
     asin(   ( this->strength * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );

  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)

  this->tanPFAngle = tan( this->poleFaceAngle );
  return this->poleFaceAngle;
}


double rbend::setPoleFaceAngle( const JetParticle& p )
{
  this->poleFaceAngle =   
     asin(   ( this->strength * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );

  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)

  this->tanPFAngle = tan( this->poleFaceAngle );
  return this->poleFaceAngle;
}


ostream& rbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << poleFaceAngle << " ";
  // Determine which propogators are being used, and make a note of it.
  if ( Propagator == 		&rbend::Exact )
    os << "rbend::P_Exact    rbend::J_Exact";
  else if ( Propagator == 	&rbend::LikeMAD )
    os << "rbend::P_LikeMAD  rbend::J_LikeMAD";
  else if ( Propagator == 	&rbend::NoEdge )
    os << "rbend::P_NoEdge   rbend::J_NoEdge";
  else if ( Propagator == 	&rbend::InEdge )
    os << "rbend::P_InEdge   rbend::J_InEdge";
  else if ( Propagator == 	&rbend::OutEdge )
    os << "rbend::P_OutEdge  rbend::J_OutEdge";
  else
    os << "UNKNOWN  UNKNOWN";
  
  os << "\n";
  return os;
}

istream& rbend::readFrom(istream& is)
{
  double a;
  char prop_fun[100], jet_prop_fun[100];

  is >> a >> prop_fun >> jet_prop_fun;
  setAngle(a);

  if ( strcasecmp(prop_fun, 		"rbend::P_Exact" ) == 0 )
    setPropFunction(&rbend::Exact );
  else if ( strcasecmp(prop_fun, 	"rbend::P_LikeMAD" ) == 0 )
    setPropFunction(&rbend::LikeMAD );
  else if ( strcasecmp(prop_fun, 	"rbend::P_NoEdge" ) == 0 )
    setPropFunction(&rbend::NoEdge );
  else if ( strcasecmp(prop_fun, 	"rbend::P_InEdge" ) == 0 )
    setPropFunction(&rbend::InEdge );
  else if ( strcasecmp(prop_fun, 	"rbend::P_OutEdge" ) == 0 )
    setPropFunction(&rbend::OutEdge );
  else
    {
      cerr << " **** WARNING **** rbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting rbend::P_Exact\n";
      setPropFunction(&rbend::Exact);
    }
  
  return is;
}


// **************************************************
//   classes rbend::XXXEdge_Prop
// **************************************************

// --- rbend::NoEdge_Prop -----------------------

rbend::NoEdge_Prop::NoEdge_Prop()
: _approx(0)
{
}
 
rbend::NoEdge_Prop::~NoEdge_Prop()
{
}
 
char rbend::NoEdge_Prop::isApproximate()
{
  return _approx;
}

void rbend::NoEdge_Prop::makeApproximate()
{
  _approx = 1;
}

void rbend::NoEdge_Prop::makeExact()
{
  _approx = 0;
}


// --- rbend::Exact_Prop -----------------------

rbend::Exact_Prop::Exact_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::Exact_Prop::~Exact_Prop()
{
}
 
char rbend::Exact_Prop::isApproximate()
{
  return _myPropagator->isApproximate();
}

void rbend::Exact_Prop::makeApproximate()
{
  _myPropagator->makeApproximate();
}

void rbend::Exact_Prop::makeExact()
{
  _myPropagator->makeExact();
}

void rbend::Exact_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- rbend::InEdge_Prop -----------------------

rbend::InEdge_Prop::InEdge_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::InEdge_Prop::~InEdge_Prop()
{
}
 
char rbend::InEdge_Prop::isApproximate()
{
  return _myPropagator->isApproximate();
}

void rbend::InEdge_Prop::makeApproximate()
{
  _myPropagator->makeApproximate();
}

void rbend::InEdge_Prop::makeExact()
{
  _myPropagator->makeExact();
}

void rbend::InEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- rbend::OutEdge_Prop -----------------------

rbend::OutEdge_Prop::OutEdge_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::OutEdge_Prop::~OutEdge_Prop()
{
}
 
char rbend::OutEdge_Prop::isApproximate()
{
  return _myPropagator->isApproximate();
}

void rbend::OutEdge_Prop::makeApproximate()
{
  _myPropagator->makeApproximate();
}

void rbend::OutEdge_Prop::makeExact()
{
  _myPropagator->makeExact();
}

void rbend::OutEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


