#include "VectorD.h"
#include "beamline.inc"

void rbend::P_Exact( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    double edgeCoeff = ( pbe->tanPFAngle / ( p.bRho / pbe->strength ) );
    p.state[4] -= edgeCoeff* p.state[1];
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through the constant magnetic field.
  rbend::P_NoEdge(pbe,p);


  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    p.state[4] -= edgeCoeff* p.state[1];
  #ifdef RBEND_POLEFACECHECK
  }
  #endif

}

void rbend::J_Exact( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    double edgeCoeff = ( pbe->tanPFAngle / ( p.bRho / pbe->strength ) );
    p.state(4) -= edgeCoeff * p.state(1);
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through constant magnetic field.
  rbend::J_NoEdge(pbe,p);


  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    p.state(4) -= edgeCoeff * p.state(1);
  #ifdef RBEND_POLEFACECHECK
  }
  #endif

}


void rbend::P_InEdge( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    double edgeCoeff = ( pbe->tanPFAngle / ( p.bRho / pbe->strength ) );
    p.state[4] -= edgeCoeff* p.state[1];
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through the constant magnetic field.
  rbend::P_NoEdge(pbe,p);
}


void rbend::J_InEdge( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    double edgeCoeff = ( pbe->tanPFAngle / ( p.bRho / pbe->strength ) );
    p.state(4) -= edgeCoeff * p.state(1);
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through constant magnetic field.
  rbend::J_NoEdge(pbe,p);
}


void rbend::P_OutEdge( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Propagate through the constant magnetic field.
  rbend::P_NoEdge(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    double edgeCoeff = ( pbe->tanPFAngle / ( p.bRho / pbe->strength ) );
    p.state[4] -= edgeCoeff* p.state[1];
  #ifdef RBEND_POLEFACECHECK
  }
  #endif
}


void rbend::J_OutEdge( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  rbend::J_NoEdge(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->tanPFAngle ) {
  #endif
    double edgeCoeff = ( pbe->tanPFAngle / ( p.bRho / pbe->strength ) );
    p.state(4) -= edgeCoeff * p.state(1);
  #ifdef RBEND_POLEFACECHECK
  }
  #endif
}




void rbend::P_NoEdge( bmlnElmnt* p_be, Particle& p )
{
 static rbend* pbe;
 pbe = (rbend*) p_be;

 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 double Rho  = p.bRho / pbe->strength;
 double psq = 1.0 + p.state[5];
 psq = psq*psq;


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

}

void rbend::J_NoEdge( bmlnElmnt* p_be, JetParticle& p ) 
{
 static rbend* pbe;
 pbe = (rbend*) p_be;

 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 double Rho  = p.bRho / pbe->strength;
 Jet psq( 1.0 + p.state(5) );
 psq = psq*psq;

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

}
