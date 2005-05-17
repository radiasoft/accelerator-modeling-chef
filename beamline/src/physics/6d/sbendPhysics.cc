/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sbendPhysics.cc
******  Version:   2.2
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


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "GenericException.h"
#include "PhysicsConstants.h"
#include "Jet.h"
#include "Particle.h"
#include "sbend.h"

sbend::NoEdge_Prop  sbend::NoEdge;
sbend::Exact_Prop   sbend::Exact;
sbend::InEdge_Prop  sbend::InEdge;
sbend::OutEdge_Prop sbend::OutEdge;
// REMOVE: sbend::Null_Prop    sbend::AbortProp;
// REMOVE: sbend::Approx_Prop  sbend::Approx;


// REMOVE: // ********************************************************
// REMOVE: 
// REMOVE: int sbend::Null_Prop::operator()( bmlnElmnt* p_be, Particle& p )
// REMOVE: {
// REMOVE:   throw( GenericException( __FILE__, __LINE__,
// REMOVE:          "int sbend::Null_Prop::operator()( bmlnElmnt* p_be, Particle& p )",
// REMOVE:          "Must use a RefRegVisitor to register a reference proton." ) );
// REMOVE: }
// REMOVE: 
// REMOVE: int sbend::Null_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
// REMOVE: {
// REMOVE:   throw( GenericException( __FILE__, __LINE__,
// REMOVE:          "int sbend::Null_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )",
// REMOVE:          "Must use a RefRegVisitor to register a reference proton." ) );
// REMOVE: }
// REMOVE: 
// ********************************************************

int sbend::Exact_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static sbend* pbe;
 pbe = dynamic_cast<sbend*>(p_be);
 double edgeCoeff;

 // Put in a kludge for the vertical focusing upon entrance.
 if( 0.0 != pbe->_usTan ) {
   edgeCoeff = ( pbe->_usTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
 }

 // Propagate through the constant magnetic field.
 (*_myPropagator)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( 0.0 != pbe->_dsTan ) {
   edgeCoeff = ( pbe->_dsTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
 }

 return 0;
}


int sbend::Exact_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
 static sbend* pbe;
 pbe = dynamic_cast<sbend*>(p_be);
 double edgeCoeff;

 // Put in a kludge for the vertical focusing upon entrance.
 if( 0.0 != pbe->_usTan ) {
   edgeCoeff = ( pbe->_usTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
 }

 // Propagate through constant magnetic field.
 (*_myPropagator)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( 0.0 != pbe->_dsTan ) {
   edgeCoeff = ( pbe->_dsTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
 }

 return 0;
}


int sbend::InEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static sbend* pbe;
 pbe = dynamic_cast<sbend*>(p_be);
 double edgeCoeff;

 // Put in a kludge for the vertical focusing upon entrance.
 if( 0.0 != pbe->_usTan ) {
   edgeCoeff = ( pbe->_usTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
 }

 // Propagate through the constant magnetic field.
 (*_myPropagator)(pbe,p);

  return 0;
}


int sbend::InEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
 static sbend* pbe;
 pbe = dynamic_cast<sbend*>(p_be);
 double edgeCoeff;

 // Put in a kludge for the vertical focusing upon entrance.
 if( 0.0 != pbe->_usTan ) {
   edgeCoeff = ( pbe->_usTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
 }

 // Propagate through constant magnetic field.
 (*_myPropagator)(pbe,p);

 return 0;
}


int sbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static sbend* pbe;
 pbe = dynamic_cast<sbend*>(p_be);
 double edgeCoeff;

 // Propagate through the constant magnetic field.
 (*_myPropagator)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( 0.0 != pbe->_dsTan ) {
   edgeCoeff = ( pbe->_dsTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
 }

 return 0;
}


int sbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
 static sbend* pbe;
 pbe = dynamic_cast<sbend*>(p_be);
 double edgeCoeff;

 (*_myPropagator)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( 0.0 != pbe->_dsTan ) {
   edgeCoeff = ( pbe->_dsTan / ( p.ReferenceBRho() / pbe->Strength() ) );
   if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
     p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
   }
   else {
     p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
   }
 }

 return 0;
}


int sbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 sbend* pbe = dynamic_cast<sbend*>(p_be);
 double edgeCoeff;

 // Preliminary filter from state coordinates
 double psq = 1.0 + p.get_ndp();
        psq = psq*psq;
 double E_factor = 1.0 / sqrt( psq + p.PNI2() );
 double beta_1 = E_factor*p.get_npx();
 double beta_2 = E_factor*p.get_npy();
 double beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                                    - p.get_npy()*p.get_npy() );

 FNAL::Complex ui  ( 0.0, p.get_x() );
 FNAL::Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double omega  = csq_red * pbe->Strength() / p.Energy();
 FNAL::Complex bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 FNAL::Complex bf = bi*pbe->_propPhase + pbe->_propTerm;

 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 double dthmdphi =   (pbe->_myArcsin)( real(bi)/rho ) 
                   - (pbe->_myArcsin)( real(bf)/rho );

 // Step 4.
 FNAL::Complex expFactor( cos(dthmdphi), sin(dthmdphi) );
 FNAL::Complex vuf = vui*expFactor;
 FNAL::Complex uf  = ( ui + bi )*expFactor - bf;

 // Final filter back to state coordinates
 double dtheta = dthmdphi + pbe->_dphi;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT    = pbe->getReferenceTime();

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}


int sbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) 
{
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 sbend* pbe = dynamic_cast<sbend*>(p_be);

 // Preliminary filter from state coordinates
 Jet psq;
 psq = 1.0 + p.get_ndp();
 psq = psq*psq;

 Jet E_factor;
 E_factor = 1.0 / sqrt( psq + p.PNI2() );

 Jet beta_1, beta_2, beta_3;
 beta_1 = E_factor*p.get_npx();
 beta_2 = E_factor*p.get_npy();
 beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                             - p.get_npy()*p.get_npy() );

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
 bf = bi*pbe->_propPhase + pbe->_propTerm;

 // Step 3.
 Jet rho, dthmdphi;
 rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 dthmdphi =   (pbe->_myArcsin)( real(bi)/rho ) 
            - (pbe->_myArcsin)( real(bf)/rho );

 // Step 4.
 JetC expF, vuf, uf;
 expF = cos(dthmdphi) + complex_i*sin(dthmdphi);
 vuf  = vui*expF;
 uf   = ( ui + bi )*expF - bf;


 // Final filter back to state coordinates
 Jet dtheta, cdt;
 // REMOVE: double dphi   = - pbe->getAngle();
 dtheta = dthmdphi + pbe->_dphi;
 cdt    = - PH_MKS_c * dtheta / omega;
 double CDT = pbe->getReferenceTime();

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}

// ********************************************************

// REMOVE: double sbend::Approx_Prop::_fastArcsin( double x ) const
// REMOVE: {
// REMOVE:   return x;
// REMOVE:   /*
// REMOVE:   double u = x*x;
// REMOVE:   return x*( 1.0 +
// REMOVE:              u*( 0.16666666666667 +
// REMOVE:                  u*( 0.075 +
// REMOVE:                      u*( 0.044642857142857 +
// REMOVE:                          u*( 0.030381944444444 +
// REMOVE:                              u*( 0.022372159090909 +
// REMOVE:                                  u*( 0.017352764423077 +
// REMOVE:                                      u*( 0.01396484375 +
// REMOVE:                                          u*( 0.01155180089614 +
// REMOVE:                                              u*( 0.0097616095291941 +
// REMOVE:                                                  u*( 0.0083903358096168 +
// REMOVE:                                                      u*( 0.0073125258735988 +
// REMOVE:                                                          u*( 0.0064472103118896 )))))))))))));
// REMOVE:  */
// REMOVE: }


// REMOVE: Jet sbend::Approx_Prop::_fastArcsin( const Jet& x ) const
// REMOVE: {
// REMOVE:   return x;
// REMOVE:   /*
// REMOVE:   Jet u( x*x );
// REMOVE:   return x*( 1.0 +
// REMOVE:              u*( 0.16666666666667 +
// REMOVE:                  u*( 0.075 +
// REMOVE:                      u*( 0.044642857142857 +
// REMOVE:                          u*( 0.030381944444444 +
// REMOVE:                              u*( 0.022372159090909 +
// REMOVE:                                  u*( 0.017352764423077 +
// REMOVE:                                      u*( 0.01396484375 +
// REMOVE:                                          u*( 0.01155180089614 +
// REMOVE:                                              u*( 0.0097616095291941 +
// REMOVE:                                                  u*( 0.0083903358096168 +
// REMOVE:                                                      u*( 0.0073125258735988 +
// REMOVE:                                                          u*( 0.0064472103118896 )))))))))))));
// REMOVE:  */
// REMOVE: }


// REMOVE: int sbend::Approx_Prop::operator()( bmlnElmnt* p_be, Particle& p )
// REMOVE: {
// REMOVE:  static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
// REMOVE:  sbend* pbe = dynamic_cast<sbend*>(p_be);
// REMOVE:  double edgeCoeff;
// REMOVE: 
// REMOVE:  // Put in a kludge for the vertical focusing upon entrance.
// REMOVE:  if( 0.0 != pbe->_usTan ) {
// REMOVE:    edgeCoeff = ( pbe->_usTan / ( p.ReferenceBRho() / pbe->Strength() ) );
// REMOVE:    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
// REMOVE:  }
// REMOVE: 
// REMOVE:  // Preliminary filter from state coordinates
// REMOVE:  double psq = 1.0 + p.get_ndp();
// REMOVE:         psq = psq*psq;
// REMOVE:  double E_factor = 1.0 / sqrt( psq + p.PNI2() );
// REMOVE:  double beta_1 = E_factor*p.get_npx();
// REMOVE:  double beta_2 = E_factor*p.get_npy();
// REMOVE:  double beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
// REMOVE:                                     - p.get_npy()*p.get_npy() );
// REMOVE: 
// REMOVE:  FNAL::Complex ui  ( 0.0, p.get_x() );
// REMOVE:  FNAL::Complex vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );
// REMOVE: 
// REMOVE:  // Step 1.
// REMOVE:  double omega  = csq_red * pbe->Strength() / p.Energy();
// REMOVE:  FNAL::Complex bi = ( complex_i*vui / omega ) - ui;
// REMOVE: 
// REMOVE:  // Step 2.
// REMOVE:  FNAL::Complex bf = bi*pbe->_propPhase + pbe->_propTerm;
// REMOVE: 
// REMOVE:  // Step 3.
// REMOVE:  double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
// REMOVE:  double dthmdphi = _fastArcsin( real(bi)/rho ) - _fastArcsin( real(bf)/rho );
// REMOVE: 
// REMOVE:  // Step 4.
// REMOVE:  FNAL::Complex expFactor( cos(dthmdphi), sin(dthmdphi) );
// REMOVE:  FNAL::Complex vuf = vui*expFactor;
// REMOVE:  FNAL::Complex uf  = ( ui + bi )*expFactor - bf;
// REMOVE: 
// REMOVE: 
// REMOVE:  // Final filter back to state coordinates
// REMOVE:  double dphi   = - pbe->getAngle();
// REMOVE:  double dtheta = dthmdphi + dphi;
// REMOVE:  double cdt    = - PH_MKS_c * dtheta / omega;
// REMOVE:  double CDT = pbe->getReferenceTime();
// REMOVE: 
// REMOVE:  p.set_x    ( imag( uf ) );
// REMOVE:  p.set_y    ( p.get_y() + beta_2*cdt );
// REMOVE:  p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
// REMOVE:  p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );
// REMOVE: 
// REMOVE:  // Put in a kludge for the vertical focusing upon exit.
// REMOVE:  if( 0.0 != pbe->_dsTan ) {
// REMOVE:    edgeCoeff = ( pbe->_dsTan / ( p.ReferenceBRho() / pbe->Strength() ) );
// REMOVE:    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
// REMOVE:  }
// REMOVE: 
// REMOVE:  return 0;
// REMOVE: }


// REMOVE: int sbend::Approx_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
// REMOVE: {
// REMOVE:  static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
// REMOVE:  sbend* pbe = dynamic_cast<sbend*>(p_be);
// REMOVE:  double edgeCoeff;
// REMOVE: 
// REMOVE:  // Put in a kludge for the vertical focusing upon entrance.
// REMOVE:  if( 0.0 != pbe->_usTan ) {
// REMOVE:    edgeCoeff = ( pbe->_usTan / ( p.ReferenceBRho() / pbe->Strength() ) );
// REMOVE:    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
// REMOVE:  }
// REMOVE: 
// REMOVE:  // Preliminary filter from state coordinates
// REMOVE:  Jet psq;
// REMOVE:  psq = 1.0 + p.get_ndp();
// REMOVE:  psq = psq*psq;
// REMOVE: 
// REMOVE:  Jet E_factor;
// REMOVE:  E_factor = 1.0 / sqrt( psq + p.PNI2() );
// REMOVE: 
// REMOVE:  Jet beta_1, beta_2, beta_3;
// REMOVE:  beta_1 = E_factor*p.get_npx();
// REMOVE:  beta_2 = E_factor*p.get_npy();
// REMOVE:  beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
// REMOVE:                              - p.get_npy()*p.get_npy() );
// REMOVE: 
// REMOVE:  JetC ui, vui;
// REMOVE:  ui  = complex_i * p.get_x();
// REMOVE:  vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );
// REMOVE: 
// REMOVE:  // Step 1.
// REMOVE:  Jet omega;
// REMOVE:  JetC bi;
// REMOVE:  omega  = csq_red * pbe->Strength() / p.Energy();
// REMOVE:  bi = ( complex_i*vui / omega ) - ui;
// REMOVE: 
// REMOVE:  // Step 2.
// REMOVE:  JetC bf;
// REMOVE:  bf = bi*pbe->_propPhase + pbe->_propTerm;
// REMOVE: 
// REMOVE:  // Step 3.
// REMOVE:  Jet rho, dthmdphi;
// REMOVE:  rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
// REMOVE:  dthmdphi = _fastArcsin( real(bi)/rho ) - _fastArcsin( real(bf)/rho );
// REMOVE: 
// REMOVE:  // Step 4.
// REMOVE:  JetC expF, vuf, uf;
// REMOVE:  expF = cos(dthmdphi) + complex_i*sin(dthmdphi);
// REMOVE:  vuf  = vui*expF;
// REMOVE:  uf   = ( ui + bi )*expF - bf;
// REMOVE: 
// REMOVE: 
// REMOVE:  // Final filter back to state coordinates
// REMOVE:  Jet dtheta, cdt;
// REMOVE:  double dphi   = - pbe->getAngle();
// REMOVE:  dtheta = dthmdphi + dphi;
// REMOVE:  cdt    = - PH_MKS_c * dtheta / omega;
// REMOVE:  double CDT = pbe->getReferenceTime();
// REMOVE: 
// REMOVE:  p.set_x    ( imag( uf ) );
// REMOVE:  p.set_y    ( p.get_y() + beta_2*cdt );
// REMOVE:  p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
// REMOVE:  p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );
// REMOVE: 
// REMOVE:  // Put in a kludge for the vertical focusing upon exit.
// REMOVE:  if( 0.0 != pbe->_dsTan ) {
// REMOVE:    edgeCoeff = ( pbe->_dsTan / ( p.ReferenceBRho() / pbe->Strength() ) );
// REMOVE:    p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
// REMOVE:  }
// REMOVE: 
// REMOVE:  return 0;
// REMOVE: }
