#include "VectorD.h"
#include "beamline.inc"

void rbend::P_Exact( bmlnElmnt* p_be, Particle& p )
{
  rbend::P_Face(p_be,p);
  rbend::P_NoEdge(p_be,p);
  rbend::P_Face(p_be,p);
}

void rbend::J_Exact( bmlnElmnt* p_be, JetParticle& p )
{
  rbend::J_Face(p_be,p);
  rbend::J_NoEdge(p_be,p);
  rbend::J_Face(p_be,p);
}

void rbend::P_InEdge( bmlnElmnt* p_be, Particle& p )
{
  rbend::P_Face(p_be,p);
  rbend::P_NoEdge(p_be,p);
}

void rbend::J_InEdge( bmlnElmnt* p_be, JetParticle& p )
{
  rbend::J_Face(p_be,p);
  rbend::J_NoEdge(p_be,p);
}

void rbend::P_OutEdge( bmlnElmnt* p_be, Particle& p )
{
  rbend::P_NoEdge(p_be,p);
  rbend::P_Face(p_be,p);
}

void rbend::J_OutEdge( bmlnElmnt* p_be, JetParticle& p )
{
  rbend::J_NoEdge(p_be,p);
  rbend::J_Face(p_be,p);
}




void rbend::P_Face( bmlnElmnt* p_be, Particle& p )
{
  static const enum { x = 0, y, cdt, xp, yp, dpop };
  static rbend* pbe;

  pbe = (rbend*) p_be;  // ??? Why isn't the argument an rbend* ?

  // Calculate the transformation angle.
  double psi;
  if( pbe->poleFaceAngle == M_TWOPI ) {
    psi =   asin(   ( pbe->strength * pbe->Length() )
                  / ( 2.0*p.ReferenceBRho() )
                );
    // i.e., sin( psi ) = (l/2) / rho
    //                  = Bl/(2*Brho)
    pbe->poleFaceAngle = psi;
  }
  else {
    psi = pbe->poleFaceAngle;
  }

  double cs( cos(psi) ); 
  double sn( sin(psi) ); 

  // Drift frame represented in the rbend frame.
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



void rbend::J_Face( bmlnElmnt* p_be, JetParticle& p )
{
  static const enum { x = 0, y, cdt, xp, yp, dpop };
  static rbend* pbe;

  pbe = (rbend*) p_be;  // ??? Why isn't the argument an rbend* ?

  // Calculate the transformation angle.
  double psi;
  if( pbe->poleFaceAngle == M_TWOPI ) {
    psi =   asin(   ( pbe->strength * pbe->Length() )
                  / ( 2.0*p.ReferenceBRho() )
                );
    // i.e., sin( psi ) = (l/2) / rho
    //                  = Bl/(2*Brho)
    pbe->poleFaceAngle = psi;
  }
  else {
    psi = pbe->poleFaceAngle;
  }

  double cs( cos(psi) ); 
  double sn( sin(psi) ); 

  // Drift frame represented in the rbend frame.
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



void rbend::P_NoEdge( bmlnElmnt* p_be, Particle& p )
{
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 static rbend* pbe;
 pbe = (rbend*) p_be;

 // Adjust for misalignment ...
 if( pbe->align ) {
   (pbe->align)->misalign( p.state );
 }


 // Put in a kludge for the vertical focusing upon entrance.
 double Rho  = p.bRho / pbe->strength;
 double psq = 1.0 + p.state[5];
 psq = psq*psq;
 // double p3divpbar = sqrt( psq - p.state[3]*p.state[3] - p.state[4]*p.state[4] );
 // p.state[4] -= ( (p.state[3]/p3divpbar) / Rho ) * p.state[1];
 if( pbe->poleFaceAngle != 0.0 && pbe->poleFaceAngle != M_TWOPI ) {
   p.state[4] -= ( tan(pbe->poleFaceAngle) / Rho ) * p.state[1];
 }


 // Preliminary filter from state coordinates
 double E_factor = 1.0 / sqrt( psq + p.pni2 );
 double beta_1 = E_factor*p.state[3];
 double beta_2 = E_factor*p.state[4];
 double beta_3 = E_factor*sqrt( psq - p.state[3]*p.state[3]
                                    - p.state[4]*p.state[4] );

 double dphi   = 0.0;
 double Omega  = csq_red * pbe->strength / p.E;

 Complex ui  ( 0.0, p.state[0] );
 Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double omega  = csq_red * pbe->strength / p.Energy();
 Complex bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 Complex bf = bi + pbe->length;
 
 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 double dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );

 // Step 4.
 Complex expFactor( cos(dthmdphi), sin(dthmdphi) );
 Complex vuf = vui*expFactor;
 Complex uf  = ( ui + bi )*expFactor - bf;


 // Final filter back to state coordinates
 double dtheta = dthmdphi + dphi;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT    = - PH_MKS_c * ( - 2.0*asin( pbe->length / (2.0*Rho) ) ) / Omega;

 p.state[0] = imag( uf );
 p.state[1] = p.state[1] + beta_2*cdt;
 p.state[2] = p.state[2] + ( cdt - CDT );
 p.state[3] = imag( vuf )/( E_factor * PH_MKS_c );


 // Put in a kludge for the vertical focusing upon exit.
 // p3divpbar = sqrt( psq - p.state[3]*p.state[3] - p.state[4]*p.state[4] );
 // p.state[4] -= ( (p.state[3]/p3divpbar) / Rho ) * p.state[1];
 if( pbe->poleFaceAngle != 0.0 && pbe->poleFaceAngle != M_TWOPI ) {
   p.state[4] -= ( tan(pbe->poleFaceAngle) / Rho ) * p.state[1];
 }


 // Adjust for misalignment ...
 if( pbe->align ) {
   (pbe->align)->align( p.state );
 }

}

void rbend::J_NoEdge( bmlnElmnt* p_be, JetParticle& p ) 
{
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 static rbend* pbe;
 pbe = (rbend*) p_be;

 // Adjust for misalignment ...
 if( pbe->align ) {
   (pbe->align)->misalign( p.state );
 }


 // Put in a kludge for the vertical focusing upon entrance.
 double Rho  = p.bRho / pbe->strength;
 Jet psq( 1.0 + p.state(5) );
 psq = psq*psq;
 // Jet p3divpbar( sqrt( psq - p.state(3)*p.state(3) - p.state(4)*p.state(4) ) );
 // p.state(4) -= ( (p.state(3)/p3divpbar) / Rho ) * p.state(1);
 if( pbe->poleFaceAngle != 0.0 && pbe->poleFaceAngle != M_TWOPI ) {
   p.state(4) -= ( tan(pbe->poleFaceAngle) / Rho ) * p.state(1);
 }


 // Preliminary filter from state coordinates
 Jet E_factor;
 E_factor = 1.0 / sqrt( psq + p.pni2 );

 Jet beta_1, beta_2, beta_3;
 beta_1 = E_factor*p.state(3);
 beta_2 = E_factor*p.state(4);
 beta_3 = E_factor*sqrt( psq - p.state(3)*p.state(3)
                             - p.state(4)*p.state(4) );

 double dphi   = 0.0;
 double Omega  = csq_red * pbe->strength / p.E;

 JetC ui, vui;
 ui  = complex_i * p.state(0);
 vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );

 // Step 1.
 Jet omega;
 JetC bi;
 omega  = csq_red * pbe->strength / p.Energy();
 bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 JetC bf;
 bf = bi + pbe->length;
 // Step 3.
 Jet rho, dthmdphi;
 rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );

 // Step 4.
 JetC expF, vuf, uf;
 expF = cos(dthmdphi) + complex_i*sin(dthmdphi);
 vuf  = vui*expF;
 uf   = ( ui + bi )*expF - bf;


 // Final filter back to state coordinates
 Jet dtheta, cdt;
 double CDT; 
 dtheta = dthmdphi + dphi;
 cdt    = - PH_MKS_c * dtheta / omega;
 CDT    = - PH_MKS_c * ( - 2.0*asin( pbe->length / (2.0*Rho) ) ) / Omega;

 ( p.state ).SetComponent( 0, imag( uf )                           );
 ( p.state ).SetComponent( 1, p.state(1) + beta_2*cdt              );
 ( p.state ).SetComponent( 2, p.state(2) + ( cdt - CDT )           );
 ( p.state ).SetComponent( 3, imag( vuf )/( E_factor * PH_MKS_c )  );


 // Put in a kludge for the vertical focusing upon exit.
 // p3divpbar = sqrt( psq - p.state(3)*p.state(3) - p.state(4)*p.state(4) );
 // p.state(4) -= ( (p.state(3)/p3divpbar) / Rho ) * p.state(1);
 if( pbe->poleFaceAngle != 0.0 && pbe->poleFaceAngle != M_TWOPI ) {
   p.state(4) -= ( tan(pbe->poleFaceAngle) / Rho ) * p.state(1);
 }


 // Adjust for misalignment ...
 if( pbe->align ) {
   (pbe->align)->align( p.state );
 }

}
