#include "beamline.h"

sbend::Exact_Prop    sbend::Exact;
sbend::Approx_Prop   sbend::Approx;

// ********************************************************

int sbend::Exact_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static sbend* pbe;
 pbe = (sbend*) p_be;
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 // Preliminary filter from state coordinates
 double psq = 1.0 + p.get_ndp();
        psq = psq*psq;
 double E_factor = 1.0 / sqrt( psq + p.PNI2() );
 double beta_1 = E_factor*p.get_npx();
 double beta_2 = E_factor*p.get_npy();
 double beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                                    - p.get_npy()*p.get_npy() );

 // REMOVE ??? double Rho    = p.bRho / pbe->strength;        // Fiducial parameters
 // REMOVE ??? double dphi   = - 2.0*asin( length / (2.0*Rho) );

 double dphi   = - pbe->getAngle();
 double Rho    = - pbe->Length() / dphi;
 double Omega  = csq_red * pbe->Strength() / p.ReferenceEnergy();

 Complex ui  ( 0.0, p.get_x() );
 Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double omega  = csq_red * pbe->Strength() / p.Energy();
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

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}

int sbend::Exact_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
 static sbend* pbe;
 pbe = (sbend*) p_be;
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 int i;

 // Preliminary filter from state coordinates
 static Jet psq;
 psq = 1.0 + p.get_ndp();
 psq = psq*psq;

 static Jet E_factor;
 E_factor = 1.0 / sqrt( psq + p.PNI2() );

 static Jet beta_1, beta_2, beta_3;
 beta_1 = E_factor*p.get_npx();
 beta_2 = E_factor*p.get_npy();
 beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                             - p.get_npy()*p.get_npy() );

 // REMOVE ??? double Rho    = p.bRho / pbe->strength;        // Fiducial parameters
 // REMOVE ??? double dphi   = - 2.0*asin( length / (2.0*Rho) );

 double dphi   = - pbe->getAngle();
 double Rho    = - pbe->Length() / dphi;
 double Omega  = csq_red * pbe->Strength() / p.ReferenceEnergy();

 static JetC ui, vui;
 ui  = complex_i * p.get_x();
 vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );

 // Step 1.
 static Jet omega;
 static JetC bi;
 omega  = csq_red * pbe->Strength() / p.Energy();
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

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}

// ********************************************************

double sbend::Approx_Prop::fastArcsin( double x ) const
{
  return x;
  /*
  double u = x*x;
  return x*( 1.0 +
             u*( 0.16666666666667 +
                 u*( 0.075 +
                     u*( 0.044642857142857 +
                         u*( 0.030381944444444 +
                             u*( 0.022372159090909 +
                                 u*( 0.017352764423077 +
                                     u*( 0.01396484375 +
                                         u*( 0.01155180089614 +
                                             u*( 0.0097616095291941 +
                                                 u*( 0.0083903358096168 +
                                                     u*( 0.0073125258735988 +
                                                         u*( 0.0064472103118896 )))))))))))));
 */
}


Jet sbend::Approx_Prop::fastArcsin( const Jet& x ) const
{
  return x;
  /*
  Jet u( x*x );
  return x*( 1.0 +
             u*( 0.16666666666667 +
                 u*( 0.075 +
                     u*( 0.044642857142857 +
                         u*( 0.030381944444444 +
                             u*( 0.022372159090909 +
                                 u*( 0.017352764423077 +
                                     u*( 0.01396484375 +
                                         u*( 0.01155180089614 +
                                             u*( 0.0097616095291941 +
                                                 u*( 0.0083903358096168 +
                                                     u*( 0.0073125258735988 +
                                                         u*( 0.0064472103118896 )))))))))))));
 */
}


int sbend::Approx_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static sbend* pbe;
 pbe = (sbend*) p_be;
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 // Preliminary filter from state coordinates
 double psq = 1.0 + p.get_ndp();
        psq = psq*psq;
 double E_factor = 1.0 / sqrt( psq + p.PNI2() );
 double beta_1 = E_factor*p.get_npx();
 double beta_2 = E_factor*p.get_npy();
 double beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                                    - p.get_npy()*p.get_npy() );

 double dphi   = - pbe->getAngle();
 double Rho    = - pbe->Length() / dphi;
 double Omega  = csq_red * pbe->Strength() / p.ReferenceEnergy();

 Complex ui  ( 0.0, p.get_x() );
 Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double omega  = csq_red * pbe->Strength() / p.Energy();
 Complex bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 Complex expFactor( cos(dphi), - sin(dphi) );
 Complex bf = bi*expFactor +
              complex_i * Rho * ( 1.0 - expFactor );

 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 double dthmdphi = fastArcsin( real(bi)/rho ) - fastArcsin( real(bf)/rho );

 // Step 4.
 expFactor = cos(dthmdphi) + complex_i*sin(dthmdphi);
 Complex vuf = vui*expFactor;
 Complex uf  = ( ui + bi )*expFactor - bf;


 // Final filter back to state coordinates
 double dtheta = dthmdphi + dphi;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT    = - PH_MKS_c * dphi   / Omega;

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}

int sbend::Approx_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
 static sbend* pbe;
 pbe = (sbend*) p_be;
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 int i;

 // Preliminary filter from state coordinates
 static Jet psq;
 psq = 1.0 + p.get_ndp();
 psq = psq*psq;

 static Jet E_factor;
 E_factor = 1.0 / sqrt( psq + p.PNI2() );

 static Jet beta_1, beta_2, beta_3;
 beta_1 = E_factor*p.get_npx();
 beta_2 = E_factor*p.get_npy();
 beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                             - p.get_npy()*p.get_npy() );

 double dphi   = - pbe->getAngle();
 double Rho    = - pbe->Length() / dphi;
 double Omega  = csq_red * pbe->Strength() / p.ReferenceEnergy();

 static JetC ui, vui;
 ui  = complex_i * p.get_x();
 vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );

 // Step 1.
 static Jet omega;
 static JetC bi;
 omega  = csq_red * pbe->Strength() / p.Energy();
 bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 Complex expFactor( cos(dphi), - sin(dphi) );
 static JetC bf;
 bf = bi*expFactor +
      complex_i * Rho * ( 1.0 - expFactor );

 // Step 3.
 static Jet rho, dthmdphi;
 rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 dthmdphi = fastArcsin( real(bi)/rho ) - fastArcsin( real(bf)/rho );

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

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}
