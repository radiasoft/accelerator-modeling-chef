#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      sbend.cc
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

#include "sbend.h"


using namespace std;

// **************************************************
//   class sbend
// **************************************************

sbend::sbend( double l, double s, double alpha, PropFunc* pf ) 
: bmlnElmnt( l, s, pf ) {
 if ( fabs( alpha ) < 1.0e-9 ) {
   cerr << "\n";
   cerr << "*** ERROR ** sbend::sbend( double l, double s, double alpha )\n";
   cerr << "             Bend angle = "
        << alpha
        << " < 1 nanoradian\n";
   cerr << "             Program aborted.\n";
   exit(0);
 }
 angle    = alpha;
 if(pf == 0) {
   Propagator = &sbend::Exact;
 }
}


sbend::sbend( char* n, double l, double s, double alpha, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf ) {

 if ( fabs( alpha ) < 1.0e-9 ) {
   cerr << "\n";
   cerr << "*** ERROR ** sbend::sbend( "
        << n
        << ", double l, double s, double alpha )\n";
   cerr << "             Bend angle = "
        << alpha
        << " < 1 nanoradian\n";
   cerr << "             Program aborted.\n";
   exit(0);
 }
 angle    = alpha;
 if(pf == 0) {
   Propagator = &sbend::Exact;
 }
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


void sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "*** ERROR ***                                    \n"
            "*** ERROR *** sbend::Split                       \n"
            "*** ERROR *** pc = " << pc << 
                               " and is out of bounds.         \n"
            "*** ERROR ***                                    \n"
         << endl;
    exit (1);
  }

  // We assume "strength" means field, not field*length.
  *a = new sbend( pc*length, strength, pc*angle, Propagator );
  *b = new sbend( (1.0 - pc)*length, strength, (1.0 - pc)*angle, Propagator );

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
    setPropFunction(&sbend::Exact); 
  else if ( strcasecmp(prop_fun,        "sbend::P_LikeMAD") == 0 )
    setPropFunction(&sbend::LikeMAD); 
  else
    {
      cerr << " **** WARNING **** sbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting sbend::P_Exact\n";
      setPropFunction(&sbend::Exact);
    }

  setAngle(a);
  return is;
}


ostream& sbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << angle << " ";
  // Determine which propogators are being used, and make a note of it.
  if ( Propagator == 		&sbend::Exact )
    os << "sbend::P_Exact  sbend::J_Exact";
  else if ( Propagator == 	&sbend::LikeMAD )
    os << "sbend::P_LikeMAD  sbend::J_LikeMAD";
  else 
    os << "UNKNOWN  UNKNOWN";
  
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



const char* sbend::Type() const 
{ 
  return "sbend"; 
}
