#include "VectorD.h"
#include "beamline.h"

rbend::Exact_Prop   rbend::Exact;
rbend::NoEdge_Prop  rbend::NoEdge;
rbend::InEdge_Prop  rbend::InEdge;
rbend::OutEdge_Prop rbend::OutEdge;

// ********************************************************

double rbend::NoEdge_Prop::_fastArcsin( double x ) const
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


Jet rbend::NoEdge_Prop::_fastArcsin( const Jet& x ) const
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


// ********************************************************

int rbend::Exact_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    double edgeCoeff = ( pbe->getTanPoleFaceAngle() / ( p.ReferenceBRho() / pbe->Strength() ) );
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through the constant magnetic field.
  (*_myPropagator)(pbe,p);


  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif

  return 0;
}

int rbend::Exact_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    double edgeCoeff = ( pbe->getTanPoleFaceAngle() / ( p.ReferenceBRho() / pbe->Strength() ) );
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through constant magnetic field.
  (*_myPropagator)(pbe,p);


  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif

  return 0;
}


int rbend::InEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    double edgeCoeff = ( pbe->getTanPoleFaceAngle() / ( p.ReferenceBRho() / pbe->Strength() ) );
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through the constant magnetic field.
  (*_myPropagator)(pbe,p);

  return 0;
}


int rbend::InEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Put in a kludge for the vertical focusing upon entrance.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    double edgeCoeff = ( pbe->getTanPoleFaceAngle() / ( p.ReferenceBRho() / pbe->Strength() ) );
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif


  // Propagate through constant magnetic field.
  (*_myPropagator)(pbe,p);

  return 0;
}


int rbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  // Propagate through the constant magnetic field.
  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    double edgeCoeff = ( pbe->getTanPoleFaceAngle() / ( p.ReferenceBRho() / pbe->Strength() ) );
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif

  return 0;
}


int rbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = (rbend*) p_be;

  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  #ifdef RBEND_POLEFACECHECK
  if( 0.0 != pbe->getTanPoleFaceAngle() ) {
  #endif
    double edgeCoeff = ( pbe->getTanPoleFaceAngle() / ( p.ReferenceBRho() / pbe->Strength() ) );
    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
  #ifdef RBEND_POLEFACECHECK
  }
  #endif

  return 0;
}




int rbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static rbend* pbe;
 pbe = (rbend*) p_be;

 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 double Rho  = p.ReferenceBRho() / pbe->Strength();
 double psq = 1.0 + p.get_ndp();
 psq = psq*psq;


 // Preliminary filter from state coordinates
 double E_factor = 1.0 / sqrt( psq + p.PNI2() );
 double beta_1 = E_factor*p.get_npx();
 double beta_2 = E_factor*p.get_npy();
 double beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                                    - p.get_npy()*p.get_npy() );

 double dphi   = 0.0;
 double Omega  = csq_red * pbe->Strength() / p.ReferenceEnergy();

 Complex ui  ( 0.0, p.get_x() );
 Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double omega  = csq_red * pbe->Strength() / p.Energy();
 Complex bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 Complex bf = bi + pbe->Length();
 
 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 double dthmdphi;
 if( _approx ) {
   dthmdphi = _fastArcsin( real(bi)/rho ) - _fastArcsin( real(bf)/rho );
 }
 else {
   dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );
 }

 // Step 4.
 Complex expFactor( cos(dthmdphi), sin(dthmdphi) );
 Complex vuf = vui*expFactor;
 Complex uf  = ( ui + bi )*expFactor - bf;


 // Final filter back to state coordinates
 double dtheta = dthmdphi + dphi;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT;
 if( _approx ) {
   CDT    = - PH_MKS_c * ( - 2.0*_fastArcsin( pbe->Length() / (2.0*Rho) ) ) / Omega;
 }
 else {
   CDT    = - PH_MKS_c * ( - 2.0*asin( pbe->Length() / (2.0*Rho) ) ) / Omega;
 }

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );


 return 0;
}

int rbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) 
{
 static rbend* pbe;
 pbe = (rbend*) p_be;

 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 double Rho  = p.ReferenceBRho() / pbe->Strength();
 Jet psq( 1.0 + p.get_ndp() );
 psq = psq*psq;

 // Preliminary filter from state coordinates
 Jet E_factor;
 E_factor = 1.0 / sqrt( psq + p.PNI2() );

 Jet beta_1, beta_2, beta_3;
 beta_1 = E_factor*p.get_npx();
 beta_2 = E_factor*p.get_npy();
 beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                             - p.get_npy()*p.get_npy() );

 double dphi   = 0.0;
 double Omega  = csq_red * pbe->Strength() / p.ReferenceEnergy();

 JetC ui, vui;
 ui  = complex_i * p.get_x();
 vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );

 // Step 1.
 Jet omega;
 JetC bi;
 omega  = csq_red * pbe->Strength() / p.Energy();
 bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 JetC bf;
 bf = bi + pbe->Length();

 // Step 3.
 Jet rho, dthmdphi;
 rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 if( _approx ) {
   dthmdphi = _fastArcsin( real(bi)/rho ) - _fastArcsin( real(bf)/rho );
 }
 else {
   dthmdphi = asin( real(bi)/rho ) - asin( real(bf)/rho );
 }

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
 if( _approx ) {
   CDT    = - PH_MKS_c * ( - 2.0*_fastArcsin( pbe->Length() / (2.0*Rho) ) ) / Omega;
 }
 else {
   CDT    = - PH_MKS_c * ( - 2.0*asin( pbe->Length() / (2.0*Rho) ) ) / Omega;
 }

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}
