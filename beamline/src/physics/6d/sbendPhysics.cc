#include "beamline.inc"

void sbend::P_Exact( bmlnElmnt* p_be, Particle& p )
{
 sbend* pbe = (sbend*) p_be;
 static double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 double TrState[6];

 // Preliminary filter from state coordinates
 double psq = 1.0 + p.state[5];
        psq = psq*psq;
 double E_factor = 1.0 / sqrt( psq + p.pni2 );
 double beta_1 = E_factor*p.state[3];
 double beta_2 = E_factor*p.state[4];
 double beta_3 = E_factor*sqrt( psq - p.state[3]*p.state[3]
                                    - p.state[4]*p.state[4] );

 // REMOVE ??? double Rho    = p.bRho / pbe->strength;        // Fiducial parameters
 // REMOVE ??? double dphi   = - 2.0*asin( length / (2.0*Rho) );

 double dphi   = - pbe->angle;
 double Rho    = - pbe->length / dphi;
 double Omega  = csq_red * pbe->strength / p.E;

 Complex ui  ( 0.0, p.state[0] );
 Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double omega  = csq_red * pbe->strength / p.Energy();
 Complex bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 Complex expFactor( cos(dphi), - sin(dphi) );
 Complex bf = bi*expFactor +
              complex_i * Rho * ( 1.0 - expFactor );

 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 double dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );

 // Step 4.
 expFactor = cos(dthmdphi) + complex_i*sin(dthmdphi);
 Complex vuf = vui*expFactor;
 Complex uf  = ( ui + bi )*expFactor - bf;


 // Final filter back to state coordinates
 double dtheta = dthmdphi + dphi;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT    = - PH_MKS_c * dphi   / Omega;

 p.state[0] = imag( uf );
 p.state[1] = p.state[1] + beta_2*cdt;
 p.state[2] = p.state[2] + ( cdt - CDT );
 p.state[3] = imag( vuf )/( E_factor * PH_MKS_c );

}

void sbend::J_Exact( bmlnElmnt* p_be, JetParticle& p ) 
{
 sbend* pbe = (sbend*) p_be;
 static double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 Jet TrState[6];
 int i;

 // Preliminary filter from state coordinates
 static Jet psq;
 psq = 1.0 + p.state(5);
 psq = psq*psq;

 static Jet E_factor;
 E_factor = 1.0 / sqrt( psq + p.pni2 );

 static Jet beta_1, beta_2, beta_3;
 beta_1 = E_factor*p.state(3);
 beta_2 = E_factor*p.state(4);
 beta_3 = E_factor*sqrt( psq - p.state(3)*p.state(3)
                             - p.state(4)*p.state(4) );

 // REMOVE ??? double Rho    = p.bRho / pbe->strength;        // Fiducial parameters
 // REMOVE ??? double dphi   = - 2.0*asin( length / (2.0*Rho) );

 double dphi   = - pbe->angle;
 double Rho    = - pbe->length / dphi;
 double Omega  = csq_red * pbe->strength / p.E;

 static JetC ui, vui;
 ui  = complex_i * p.state(0);
 vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );

 // Step 1.
 static Jet omega;
 static JetC bi;
 omega  = csq_red * pbe->strength / p.Energy();
 bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 Complex expFactor( cos(dphi), - sin(dphi) );
 static JetC bf;
 bf = bi*expFactor +
      complex_i * Rho * ( 1.0 - expFactor );

 // Step 3.
 static Jet rho, dthmdphi;
 rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );

 // Step 4.
 static JetC expF, vuf, uf;
 expF = cos(dthmdphi) + complex_i*sin(dthmdphi);
 vuf  = vui*expF;
 uf   = ( ui + bi )*expF - bf;


 // Final filter back to state coordinates
 static Jet dtheta, cdt, CDT;
 dtheta = dthmdphi + dphi;
 cdt    = - PH_MKS_c * dtheta / omega;
 CDT.Reconstruct( cdt.Env() );
 CDT    = - PH_MKS_c * dphi   / Omega;

 ( p.state ).SetComponent( 0, imag( uf )                          );
 ( p.state ).SetComponent( 1, p.state(1) + beta_2*cdt             );
 ( p.state ).SetComponent( 2, p.state(2) + ( cdt - CDT )          );
 ( p.state ).SetComponent( 3, imag( vuf )/( E_factor * PH_MKS_c ) );

}
