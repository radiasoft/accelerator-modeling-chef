#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "beamline.inc"

// **************************************************
//   struct rbendData 
// **************************************************

rbendData::rbendData() : bmlnElmntData() {
 more   = 1;
 angle  = 0.0;
}

rbendData::rbendData( rbendData& x ) : bmlnElmntData( (bmlnElmntData&) x ) {
 more   = x.more;
 angle  = x.angle;
}

rbendData::~rbendData() {
}

void rbendData::eliminate() {
 delete this;
}

void* rbendData::clone() {
 void* p = new rbendData( *this );
 return p;
}

// **************************************************
//   class rbend
// **************************************************

rbend::rbend( double l, double s,
	     PROPFUNC pf, JETPROPFUNC jpf ) : bmlnElmnt( l, s, pf, jpf ) {
 if(pf == 0) {
   Propagator = rbend::P_Exact;
   JetPropagator = rbend::J_Exact;
 }
 poleFaceAngle = M_TWOPI;
 tanPFAngle = 0.0;
}

rbend::rbend( double l, double s, double angle,
	     PROPFUNC pf, JETPROPFUNC jpf ) : bmlnElmnt( l, s, pf, jpf ) {
 if(pf == 0) {
   Propagator = rbend::P_Exact;
   JetPropagator = rbend::J_Exact;
 }
 poleFaceAngle = angle;
 tanPFAngle    = tan( poleFaceAngle );
}


rbend::rbend( char* n, double l, double s,
	     PROPFUNC pf, JETPROPFUNC jpf ) : bmlnElmnt( n, l, s, pf, jpf ) {
 if(pf == 0) {
   Propagator = rbend::P_Exact;
   JetPropagator = rbend::J_Exact;
 }
 poleFaceAngle = M_TWOPI;
 tanPFAngle = 0.0;
}

rbend::rbend( char* n, double l, double s, double angle,
	     PROPFUNC pf, JETPROPFUNC jpf ) : bmlnElmnt( n, l, s, pf, jpf ) {
 if(pf == 0) {
   Propagator = rbend::P_Exact;
   JetPropagator = rbend::J_Exact;
 }
 poleFaceAngle = angle;
 tanPFAngle    = tan( poleFaceAngle );
}


rbend::rbend( rbendData& x ) : bmlnElmnt( (bmlnElmntData&) x ) {
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


char* rbend::Type() const { 
  return "rbend"; 
}


double rbend::OrbitLength( const Particle& x )
{
  static double tworho;
  tworho  = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength;
  return tworho * asin( length / tworho );
}


rbendData* rbend::image() {
 rbendData* p = new rbendData;
 bmlnElmnt::image( (bmlnElmntData*) p );
 p->more      = 1;
 p->angle     = poleFaceAngle;
 return p;
}
 
void rbend::image( int d, slist* s, BMLN_posInfo* cg ) {
  int j;
  rbendData* p = new rbendData;
  bmlnElmnt::image( (bmlnElmntData*) p );
 
  p->address   = this;
  p->depth     = d;
  geomToEnd( *cg );
  cg->outPoint = geometry.outPoint;
  for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];
 
      p->geometry.inPoint  = geometry.inPoint;
      p->geometry.outPoint = geometry.outPoint;
      for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
      for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];
 
  s->append( p );
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
  if ( Propagator == 		&rbend::P_Exact )
    os << "rbend::P_Exact ";
  // ??? REMOVE: else if ( Propagator == 	&rbend::P_InFace )
  // ??? REMOVE:   os << "rbend::P_InFace ";
  // ??? REMOVE: else if ( Propagator == 	&rbend::P_OutFace )
  // ??? REMOVE:   os << "rbend::P_OutFace ";
  else if ( Propagator == 	&rbend::P_LikeMAD )
    os << "rbend::P_LikeMAD ";
  else if ( Propagator == 	&rbend::P_Exact )
    os << "rbend::P_Exact ";
  else if ( Propagator == 	&rbend::P_NoEdge )
    os << "rbend::P_NoEdge ";
  else if ( Propagator == 	&rbend::P_InEdge )
    os << "rbend::P_InEdge ";
  else if ( Propagator == 	&rbend::P_OutEdge )
    os << "rbend::P_OutEdge ";
  
  if ( JetPropagator == 	&rbend::J_Exact )
    os << "rbend::J_Exact";
  // ??? REMOVE: else if ( JetPropagator == 	&rbend::J_InFace )
  // ??? REMOVE:   os << "rbend::J_InFace";
  // ??? REMOVE: else if ( JetPropagator == 	&rbend::J_OutFace )
  // ??? REMOVE:   os << "rbend::J_OutFace";
  else if ( JetPropagator == 	&rbend::J_LikeMAD )
    os << "rbend::J_LikeMAD";
  else if ( JetPropagator == 	&rbend::J_Exact )
    os << "rbend::J_Exact";
  else if ( JetPropagator == 	&rbend::J_NoEdge )
    os << "rbend::J_NoEdge";
  else if ( JetPropagator == 	&rbend::J_InEdge )
    os << "rbend::J_InEdge";
  else if ( JetPropagator == 	&rbend::J_OutEdge )
    os << "rbend::J_OutEdge";
  
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
    setPropFunction(&rbend::P_Exact );
  // ??? REMOVE: else if ( strcasecmp(prop_fun, 	"rbend::P_InFace" ) == 0 )
  // ??? REMOVE:   setPropFunction(&rbend::P_InFace );
  // ??? REMOVE: else if ( strcasecmp(prop_fun, 	"rbend::P_OutFace" ) == 0 )
  // ??? REMOVE:   setPropFunction(&rbend::P_OutFace );
  else if ( strcasecmp(prop_fun, 	"rbend::P_LikeMAD" ) == 0 )
    setPropFunction(&rbend::P_LikeMAD );
  else if ( strcasecmp(prop_fun, 	"rbend::P_Exact" ) == 0 )
    setPropFunction(&rbend::P_Exact );
  else if ( strcasecmp(prop_fun, 	"rbend::P_NoEdge" ) == 0 )
    setPropFunction(&rbend::P_NoEdge );
  else if ( strcasecmp(prop_fun, 	"rbend::P_InEdge" ) == 0 )
    setPropFunction(&rbend::P_InEdge );
  else if ( strcasecmp(prop_fun, 	"rbend::P_OutEdge" ) == 0 )
    setPropFunction(&rbend::P_OutEdge );
  else
    {
      cerr << " **** WARNING **** rbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting rbend::P_Exact\n";
      setPropFunction(&rbend::P_Exact);
    }
  
  if ( strcasecmp(jet_prop_fun, 	"rbend::J_Exact" ) == 0 )
    setJPropFunction(&rbend::J_Exact);
  // ??? REMOVE: else if ( strcasecmp(jet_prop_fun, 	"rbend::J_InFace" ) == 0 )
  // ??? REMOVE:   setJPropFunction(&rbend::J_InFace);
  // ??? REMOVE: else if ( strcasecmp(jet_prop_fun, 	"rbend::J_OutFace" ) == 0 )
  // ??? REMOVE:   setJPropFunction(&rbend::J_OutFace);
  else if ( strcasecmp(jet_prop_fun, 	"rbend::J_LikeMAD" ) == 0 )
    setJPropFunction(&rbend::J_LikeMAD);
  else if ( strcasecmp(jet_prop_fun, 	"rbend::J_Exact" ) == 0 )
    setJPropFunction(&rbend::J_Exact);
  else if ( strcasecmp(jet_prop_fun, 	"rbend::J_NoEdge" ) == 0 )
    setJPropFunction(&rbend::J_NoEdge);
  else if ( strcasecmp(jet_prop_fun, 	"rbend::J_InEdge" ) == 0 )
    setJPropFunction(&rbend::J_InEdge);
  else if ( strcasecmp(jet_prop_fun, 	"rbend::J_OutEdge" ) == 0 )
    setJPropFunction(&rbend::J_OutEdge);
  else
    {
      cerr << " **** WARNING **** rbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown jet propagator function specified:\n";
      cerr << " **** WARNING **** " << jet_prop_fun << "\n";
      cerr << " **** WARNING **** Substituting rbend::J_Exact\n";
      setJPropFunction(&rbend::J_Exact);
    }

  return is;
}


// **************************************************
//   Frame functions
// **************************************************


// ??? REMOVE: void rbend::enterLocalFrame( Particle& p ) const
// ??? REMOVE: {
// ??? REMOVE:   P_Face( this, p );
// ??? REMOVE:   bmlnElmnt::enterLocalFrame( p );
// ??? REMOVE: }
// ??? REMOVE: 
// ??? REMOVE: 
// ??? REMOVE: void rbend::enterLocalFrame( JetParticle& p ) const
// ??? REMOVE: {
// ??? REMOVE:   J_Face( this, p );
// ??? REMOVE:   bmlnElmnt::enterLocalFrame( p );
// ??? REMOVE: }
// ??? REMOVE: 
// ??? REMOVE: 
// ??? REMOVE: void rbend::leaveLocalFrame( Particle& p ) const
// ??? REMOVE: {
// ??? REMOVE:   bmlnElmnt::leaveLocalFrame( p );
// ??? REMOVE:   P_Face( this, p );
// ??? REMOVE: }
// ??? REMOVE: 
// ??? REMOVE: 
// ??? REMOVE: void rbend::leaveLocalFrame( JetParticle& p ) const
// ??? REMOVE: {
// ??? REMOVE:   bmlnElmnt::leaveLocalFrame( p );
// ??? REMOVE:   J_Face( this, p );
// ??? REMOVE: }
// ??? REMOVE: 
// ??? REMOVE: 
// ??? REMOVE: void rbend::P_Face( const bmlnElmnt* p_be, Particle& p )
// ??? REMOVE: {
// ??? REMOVE:   static int x;
// ??? REMOVE:   static int y;
// ??? REMOVE:   static int cdt;
// ??? REMOVE:   static int xp;
// ??? REMOVE:   static int yp;
// ??? REMOVE:   static int dpop;
// ??? REMOVE: 
// ??? REMOVE:   x    = p.xIndex();
// ??? REMOVE:   y    = p.yIndex();
// ??? REMOVE:   cdt  = p.cdtIndex();
// ??? REMOVE:   xp   = p.npxIndex();
// ??? REMOVE:   yp   = p.npyIndex();
// ??? REMOVE:   dpop = p.ndpIndex();
// ??? REMOVE: 
// ??? REMOVE: 
// ??? REMOVE:   static rbend* pbe;
// ??? REMOVE: 
// ??? REMOVE:   pbe = (rbend*) p_be;  // ??? Why isn't the argument an rbend* ?
// ??? REMOVE: 
// ??? REMOVE:   // Calculate the transformation angle.
// ??? REMOVE:   double psi;
// ??? REMOVE:   if( pbe->poleFaceAngle == M_TWOPI ) {
// ??? REMOVE:     psi =   asin(   ( pbe->strength * pbe->Length() )
// ??? REMOVE:                   / ( 2.0*p.ReferenceBRho() )
// ??? REMOVE:                 );
// ??? REMOVE:     // i.e., sin( psi ) = (l/2) / rho
// ??? REMOVE:     //                  = Bl/(2*Brho)
// ??? REMOVE:     pbe->poleFaceAngle = psi;
// ??? REMOVE:     pbe->tanPFAngle    = tan( psi );
// ??? REMOVE:   }
// ??? REMOVE:   else {
// ??? REMOVE:     psi = pbe->poleFaceAngle;
// ??? REMOVE:   }
// ??? REMOVE: 
// ??? REMOVE:   double cs( cos(psi) ); 
// ??? REMOVE:   double sn( sin(psi) ); 
// ??? REMOVE: 
// ??? REMOVE:   // Drift frame represented in the rbend frame.
// ??? REMOVE:   Vector e_1(3), e_2(3), e_3(3);
// ??? REMOVE:   e_1(0) = cs;  e_1(1) = 0.0;  e_1(2) = -sn; 
// ??? REMOVE:   e_2(0) = 0.0; e_2(1) = 1.0;  e_2(2) = 0.0; 
// ??? REMOVE:   e_3(0) = sn;  e_3(1) = 0.0;  e_3(2) = cs; 
// ??? REMOVE: 
// ??? REMOVE:   // Coordinate transformation.
// ??? REMOVE:   Vector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
// ??? REMOVE:   Vector dummy    ( p.VectorBeta() );
// ??? REMOVE:   Vector beta     ( dummy(0)*e_1 +
// ??? REMOVE:                     dummy(1)*e_2 +
// ??? REMOVE:                     dummy(2)*e_3 );
// ??? REMOVE: 
// ??? REMOVE:   double tau      ( - r(2) / beta(2) );
// ??? REMOVE: 
// ??? REMOVE:   p.state[x]    = r(0) + tau*beta(0);
// ??? REMOVE:   p.state[y]    = r(1) + tau*beta(1);
// ??? REMOVE:   p.state[cdt] += tau;
// ??? REMOVE: 
// ??? REMOVE:   // Momentum transformation
// ??? REMOVE:   double p1( p.State( xp ) );
// ??? REMOVE:   double p2( p.State( yp ) );
// ??? REMOVE:   double p3divpbar = sqrt( ( 1.0 + p.state[dpop] ) * ( 1.0 + p.state[dpop] )
// ??? REMOVE:                             - p1*p1 - p2*p2 );
// ??? REMOVE: 
// ??? REMOVE:   p.state[xp] = cs*p.State( xp ) + sn*p3divpbar;
// ??? REMOVE: }
// ??? REMOVE: 
// ??? REMOVE: 
// ??? REMOVE: void rbend::J_Face( const bmlnElmnt* p_be, JetParticle& p )
// ??? REMOVE: {
// ??? REMOVE:   static int x;
// ??? REMOVE:   static int y;
// ??? REMOVE:   static int cdt;
// ??? REMOVE:   static int xp;
// ??? REMOVE:   static int yp;
// ??? REMOVE:   static int dpop;
// ??? REMOVE: 
// ??? REMOVE:   x    = p.xIndex();
// ??? REMOVE:   y    = p.yIndex();
// ??? REMOVE:   cdt  = p.cdtIndex();
// ??? REMOVE:   xp   = p.npxIndex();
// ??? REMOVE:   yp   = p.npyIndex();
// ??? REMOVE:   dpop = p.ndpIndex();
// ??? REMOVE: 
// ??? REMOVE: 
// ??? REMOVE:   static rbend* pbe;
// ??? REMOVE: 
// ??? REMOVE:   pbe = (rbend*) p_be;  // ??? Why isn't the argument an rbend* ?
// ??? REMOVE: 
// ??? REMOVE:   // Calculate the transformation angle.
// ??? REMOVE:   double psi;
// ??? REMOVE:   if( pbe->poleFaceAngle == M_TWOPI ) {
// ??? REMOVE:     psi =   asin(   ( pbe->strength * pbe->Length() )
// ??? REMOVE:                   / ( 2.0*p.ReferenceBRho() )
// ??? REMOVE:                 );
// ??? REMOVE:     // i.e., sin( psi ) = (l/2) / rho
// ??? REMOVE:     //                  = Bl/(2*Brho)
// ??? REMOVE:     pbe->poleFaceAngle = psi;
// ??? REMOVE:     pbe->tanPFAngle    = tan( psi );
// ??? REMOVE:   }
// ??? REMOVE:   else {
// ??? REMOVE:     psi = pbe->poleFaceAngle;
// ??? REMOVE:   }
// ??? REMOVE: 
// ??? REMOVE:   double cs( cos(psi) ); 
// ??? REMOVE:   double sn( sin(psi) ); 
// ??? REMOVE: 
// ??? REMOVE:   // Drift frame represented in the rbend frame.
// ??? REMOVE:   Vector e_1(3), e_2(3), e_3(3);
// ??? REMOVE:   e_1(0) = cs;  e_1(1) = 0.0;  e_1(2) = -sn; 
// ??? REMOVE:   e_2(0) = 0.0; e_2(1) = 1.0;  e_2(2) = 0.0; 
// ??? REMOVE:   e_3(0) = sn;  e_3(1) = 0.0;  e_3(2) = cs; 
// ??? REMOVE: 
// ??? REMOVE:   // Coordinate transformation.
// ??? REMOVE:   JetVector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
// ??? REMOVE:   JetVector dummy    ( p.VectorBeta() );
// ??? REMOVE:   JetVector beta     ( dummy(0)*e_1 +
// ??? REMOVE:                        dummy(1)*e_2 +
// ??? REMOVE:                        dummy(2)*e_3 );
// ??? REMOVE: 
// ??? REMOVE:   Jet tau            ( - r(2) / beta(2) );
// ??? REMOVE: 
// ??? REMOVE:   ( p.state ).SetComponent( x,   r(0) + tau*beta(0) );
// ??? REMOVE:   ( p.state ).SetComponent( y,   r(1) + tau*beta(1) );
// ??? REMOVE:   ( p.state ).SetComponent( cdt, p.state(cdt) + tau );
// ??? REMOVE: 
// ??? REMOVE:   // Momentum transformation
// ??? REMOVE:   Jet p1( p.State( xp ) );
// ??? REMOVE:   Jet p2( p.State( yp ) );
// ??? REMOVE:   Jet p3divpbar = sqrt( ( 1.0 + p.state(dpop) ) * ( 1.0 + p.state(dpop) )
// ??? REMOVE:                             - p1*p1 - p2*p2 );
// ??? REMOVE: 
// ??? REMOVE:   ( p.state ).SetComponent( xp, cs*p.State( xp ) + sn*p3divpbar );
// ??? REMOVE: }
