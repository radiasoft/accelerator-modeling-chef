#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "beamline.inc"

// **************************************************
//   struct sbendData 
// **************************************************

sbendData::sbendData() : bmlnElmntData() {
 more   = 1;
 angle  = 0.0;
}

sbendData::sbendData( sbendData& x ) : bmlnElmntData( (bmlnElmntData&) x ) {
 more   = x.more;
 angle  = x.angle;
}

sbendData::~sbendData() {
}

void sbendData::eliminate() {
 delete this;
}

void* sbendData::clone() {
 void* p = new sbendData( *this );
 return p;
}

// ??? REMOVE void sbendData::writeTo( FILE* f ) {
// ??? REMOVE  int sz = strlen( name );
// ??? REMOVE  if( sz == 0 ) {
// ??? REMOVE   cerr << "\n";
// ??? REMOVE   cerr << "*** ERROR ***                                        \n";
// ??? REMOVE   cerr << "*** ERROR *** void sbendData::writeTo( FILE* )       \n";
// ??? REMOVE   cerr << "*** ERROR *** Anomoly in ident.  Quitting.           \n";
// ??? REMOVE   cerr << "*** ERROR ***                                        \n";
// ??? REMOVE   cerr << "\n";
// ??? REMOVE   exit(0);
// ??? REMOVE  }
// ??? REMOVE 
// ??? REMOVE  fwrite( this, sizeof( sbendData ), 1, f );
// ??? REMOVE  fwrite( &sz, sizeof( int ), 1, f );
// ??? REMOVE  fprintf( f, "%s ", name );
// ??? REMOVE }
// ??? REMOVE 

// **************************************************
//   class sbend
// **************************************************

sbend::sbend( double l, double s, double alpha, 
              PROPFUNC pf, JETPROPFUNC jpf ) : bmlnElmnt( l, s, pf, jpf ) {
 if ( fabs( alpha ) < 1.0e-9 ) {
   cerr << "\n";
   cerr << "*** ERROR ** sbend::sbend( double l, double s, double alpha )\n";
   cerr << "             Bend angle < 1 nanoradian\n";
   cerr << "             Program aborted.\n";
   exit(0);
 }
 angle    = alpha;
 if(pf == 0) {
   Propagator = sbend::P_Exact;
   JetPropagator = sbend::J_Exact;
 }
}


sbend::sbend( char* n, double l, double s, double alpha, 
              PROPFUNC pf, JETPROPFUNC jpf ) : bmlnElmnt( n, l, s, pf, jpf ) {

 if ( fabs( alpha ) < 1.0e-9 ) {
   cerr << "\n";
   cerr << "*** ERROR ** sbend::sbend( char* n, double l, double s, double alpha )\n";
   cerr << "             Bend angle < 1 nanoradian\n";
   cerr << "             Program aborted.\n";
   exit(0);
 }
 angle    = alpha;
 if(pf == 0) {
   Propagator = sbend::P_Exact;
   JetPropagator = sbend::J_Exact;
 }
}


sbend::sbend( sbendData& x ) : bmlnElmnt( (bmlnElmntData&) x ) {
 angle = x.angle;
}

sbend::sbend( const sbend& x )
: bmlnElmnt( (bmlnElmnt&) x )
{
 angle = x.angle;
}

sbend::~sbend() {
}

void sbend::geomToEnd( BMLN_posInfo& g ) {
 double cs, sn;
 double rho = length/angle;
 cs = cos( angle );
 sn = sin( angle );
 for( int i = 0; i < 3; i++ ) geometry .inAxes[i] = g.outAxes[i];
 geometry.outAxes[0] =  cs*geometry.inAxes[0] + sn*geometry.inAxes[2];
 geometry.outAxes[1] =  geometry.inAxes[1];
 geometry.outAxes[2] = -sn*geometry.inAxes[0] + cs*geometry.inAxes[2];
 geometry.inPoint = g.outPoint;
 geometry.outPoint =
       geometry.inPoint +
       rho * ( ( cs - 1.0 )*geometry.inAxes[0] +
                 sn        *geometry.inAxes[2] );
}

void sbend::geomToStart( BMLN_posInfo& g ) {
 double cs, sn;
 double rho = length/angle;
 cs = cos( angle );
 sn = sin( angle );
 for( int i = 0; i < 3; i++ ) geometry .outAxes[i] = g.inAxes[i];
 geometry.inAxes[0] =  cs*geometry.outAxes[0] - sn*geometry.outAxes[2];
 geometry.inAxes[1] =  geometry.outAxes[1];
 geometry.inAxes[2] =  sn*geometry.outAxes[0] + cs*geometry.outAxes[2];
 geometry.outPoint = g.inPoint;
 geometry.inPoint =
       geometry.outPoint -
       rho * ( ( cs - 1.0 )*geometry.inAxes[0] +
                 sn        *geometry.inAxes[2] );
}

void sbend::eliminate() {
 delete this;
}


sbendData* sbend::image() {
 sbendData* p = new sbendData;
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->more      = 1;
 p->angle     = angle;
 return p;
}

void sbend::image( int d, slist* s, BMLN_posInfo* cg ) {
 sbendData* p = new sbendData;
 bmlnElmnt::image( (bmlnElmntData*) p );

 p->address   = this;
 p->depth     = d;
 p->more      = 1;
 p->angle     = angle;

 geomToEnd( *cg );
 cg->outPoint = geometry.outPoint;
 int j; // O.K.
 for( j = 0; j < 3; j++ ) cg->outAxes[j] = geometry.outAxes[j];

     p->geometry.inPoint  = geometry.inPoint;
     p->geometry.outPoint = geometry.outPoint;
     for( j = 0; j < 3; j++ ) p->geometry.inAxes[j]  = geometry.inAxes[j];
     for( j = 0; j < 3; j++ ) p->geometry.outAxes[j] = geometry.outAxes[j];

 s->append( p );
}


void sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** sbend::Split                       \n"
            "*** ERROR *** pc = " << pc << 
                               "and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  // We assume "strength" means field, not field*length.
  *a = new sbend( pc*length, strength, pc*angle,
                  Propagator, JetPropagator );
  *b = new sbend( (1.0 - pc)*length, strength, (1.0 - pc)*angle,
                  Propagator, JetPropagator );

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

istream& sbend::readFrom(istream& is)
{
  double a;
  char prop_fun[100], jet_prop_fun[100];

  is >> a >> prop_fun >> jet_prop_fun;
  if ( strcasecmp(prop_fun,             "sbend::P_Exact") == 0 )
    setPropFunction(&sbend::P_Exact); 
  else if ( strcasecmp(prop_fun,        "sbend::P_LikeMAD") == 0 )
    setPropFunction(&sbend::P_LikeMAD); 
  else if ( strcasecmp(prop_fun,        "sbend::P_Exact") == 0 )
     setPropFunction(&sbend::P_Exact);
  else
    {
      cerr << " **** WARNING **** sbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting sbend::P_Exact\n";
      setPropFunction(&sbend::P_Exact);
    }

  if ( strcasecmp(jet_prop_fun,         "sbend::J_Exact" ) == 0 )
    setJPropFunction(&sbend::J_Exact);
  else if ( strcasecmp(jet_prop_fun,    "sbend::J_LikeMAD" ) == 0 )
    setJPropFunction(&sbend::J_LikeMAD);
  else if ( strcasecmp(jet_prop_fun,    "sbend::J_Exact" ) == 0 )
    setJPropFunction(&sbend::J_Exact);
  else
    {
      cerr << " **** WARNING **** sbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown jet propagator function specified:\n";
      cerr << " **** WARNING **** " << jet_prop_fun << "\n";
      cerr << " **** WARNING **** Substituting sbend::J_Exact\n";
      setJPropFunction(&sbend::J_Exact);
    }
  
  setAngle(a);
  return is;
}


ostream& sbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << angle << " ";
  // Determine which propogators are being used, and make a note of it.
  if ( Propagator == 		&sbend::P_Exact )
    os << "sbend::P_Exact ";
  else if ( Propagator == 	&sbend::P_LikeMAD )
    os << "sbend::P_LikeMAD ";
  else if ( Propagator == 	&sbend::P_Exact )
    os << "sbend::P_Exact ";
  
  if ( JetPropagator == 	&sbend::J_Exact )
    os << "sbend::J_Exact";
  else if ( JetPropagator == 	&sbend::J_LikeMAD )
    os << "sbend::J_LikeMAD";
  else if ( JetPropagator == 	&sbend::J_Exact )
    os << "sbend::J_Exact";
  
  os << "\n";
  return os;
}


// **************************************************
//   Frame functions
// **************************************************


void sbend::enterLocalFrame( Particle& p ) const
{
  static double halfAngle;
  halfAngle = this->angle / 2.0;
  P_Face( p,   halfAngle );
  bmlnElmnt::enterLocalFrame( p );
  P_Face( p, - halfAngle );
}


void sbend::enterLocalFrame( JetParticle& p ) const
{
  static double halfAngle;
  halfAngle = this->angle / 2.0;
  J_Face( p,   halfAngle );
  bmlnElmnt::enterLocalFrame( p );
  J_Face( p, - halfAngle );
}


void sbend::leaveLocalFrame( Particle& p ) const
{
  static double halfAngle;
  halfAngle = this->angle / 2.0;
  P_Face( p, - halfAngle );
  bmlnElmnt::leaveLocalFrame( p );
  P_Face( p,   halfAngle );
}


void sbend::leaveLocalFrame( JetParticle& p ) const
{
  static double halfAngle;
  halfAngle = this->angle / 2.0;
  J_Face( p, - halfAngle );
  bmlnElmnt::leaveLocalFrame( p );
  J_Face( p,   halfAngle );
}


void sbend::P_Face( Particle& p, const double& psi ) const
{
  static int x;
  static int y;
  static int cdt;
  static int xp;
  static int yp;
  static int dpop;

  x    = p.xIndex();
  y    = p.yIndex();
  cdt  = p.cdtIndex();
  xp   = p.npxIndex();
  yp   = p.npyIndex();
  dpop = p.ndpIndex();


  double cs( cos(psi) ); 
  double sn( sin(psi) ); 

  // Drift frame represented in the sbend frame.
  Vector e_1(3), e_2(3), e_3(3);
  e_1(0) = cs;  e_1(1) = 0.0;  e_1(2) = -sn; 
  e_2(0) = 0.0; e_2(1) = 1.0;  e_2(2) = 0.0; 
  e_3(0) = sn;  e_3(1) = 0.0;  e_3(2) = cs; 

  // Coordinate transformation.
  Vector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
  Vector dummy    ( p.VectorBeta() );
  Vector beta     ( dummy(0)*e_1 +
                    dummy(1)*e_2 +
                    dummy(2)*e_3 );

  double tau      ( - r(2) / beta(2) );

  p.state[x]    = r(0) + tau*beta(0);
  p.state[y]    = r(1) + tau*beta(1);
  p.state[cdt] += tau;

  // Momentum transformation
  double p1( p.State( xp ) );
  double p2( p.State( yp ) );
  double p3divpbar = sqrt( ( 1.0 + p.state[dpop] ) * ( 1.0 + p.state[dpop] )
                            - p1*p1 - p2*p2 );

  p.state[xp] = cs*p.State( xp ) + sn*p3divpbar;
}


void sbend::J_Face( JetParticle& p, const double& psi ) const
{
  static int x;
  static int y;
  static int cdt;
  static int xp;
  static int yp;
  static int dpop;

  x    = p.xIndex();
  y    = p.yIndex();
  cdt  = p.cdtIndex();
  xp   = p.npxIndex();
  yp   = p.npyIndex();
  dpop = p.ndpIndex();


  double cs( cos(psi) ); 
  double sn( sin(psi) ); 

  // Drift frame represented in the sbend frame.
  Vector e_1(3), e_2(3), e_3(3);
  e_1(0) = cs;  e_1(1) = 0.0;  e_1(2) = -sn; 
  e_2(0) = 0.0; e_2(1) = 1.0;  e_2(2) = 0.0; 
  e_3(0) = sn;  e_3(1) = 0.0;  e_3(2) = cs; 

  // Coordinate transformation.
  JetVector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
  JetVector dummy    ( p.VectorBeta() );
  JetVector beta     ( dummy(0)*e_1 +
                       dummy(1)*e_2 +
                       dummy(2)*e_3 );

  Jet tau            ( - r(2) / beta(2) );

 ( p.state ).SetComponent( x,   r(0) + tau*beta(0) );
 ( p.state ).SetComponent( y,   r(1) + tau*beta(1) );
 ( p.state ).SetComponent( cdt, p.state(cdt) + tau );

  // Momentum transformation
  Jet p1( p.State( xp ) );
  Jet p2( p.State( yp ) );
  Jet p3divpbar = sqrt( ( 1.0 + p.state(dpop) ) * ( 1.0 + p.state(dpop) )
                            - p1*p1 - p2*p2 );

  ( p.state ).SetComponent( xp, cs*p.State( xp ) + sn*p3divpbar );
}
