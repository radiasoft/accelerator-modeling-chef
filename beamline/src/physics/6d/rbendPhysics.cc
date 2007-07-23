/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rbendPhysics.cc
******  Version:   2.4
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
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
**************************************************************************
*************************************************************************/

#ifdef HAVE_CONFIG
#include <config.h>
#endif

#include <beamline/rbend.h>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/VectorD.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <mxyzptlk/Jet.h>


static const std::complex<double> complex_i(0.0, 1.0);

rbend::NoEdge_Prop       rbend::NoEdge;
rbend::Exact_Prop        rbend::Exact;
rbend::InEdge_Prop       rbend::InEdge;
rbend::OutEdge_Prop      rbend::OutEdge;
rbend::Real_Exact_Prop   rbend::RealExact;
rbend::Real_InEdge_Prop  rbend::RealInEdge;
rbend::Real_OutEdge_Prop rbend::RealOutEdge;

// ********************************************************


int rbend::Exact_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  if( 0.0 != pbe->usTan_ ) {
    edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
  }

  // Propagate through the constant magnetic field.
  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  if( 0.0 != pbe->dsTan_ ) {
    edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}


int rbend::Exact_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  if( 0.0 != pbe->usTan_ ) {
    edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
  }

  // Propagate through constant magnetic field.
  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  if( 0.0 != pbe->dsTan_ ) {
    edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}


int rbend::InEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  if( 0.0 != pbe->usTan_ ) {
    edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
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


int rbend::InEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  if( 0.0 != pbe->usTan_ ) {
    edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
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


int rbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Propagate through the constant magnetic field.
  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  if( 0.0 != pbe->dsTan_ ) {
    edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}


int rbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  if( 0.0 != pbe->dsTan_ ) {
    edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}


int rbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 rbend* pbe = dynamic_cast<rbend*>(p_be);
 double edgeCoeff;

 // Preliminary filter from state coordinates
 double psq = 1.0 + p.get_ndp();
        psq = psq*psq;
 double E_factor = 1.0 / sqrt( psq + p.PNI2() );
 double beta_1 = E_factor*p.get_npx();
 double beta_2 = E_factor*p.get_npy();
 double beta_3 = E_factor*sqrt( psq - p.get_npx()*p.get_npx()
                                    - p.get_npy()*p.get_npy() );

 std::complex<double>  ui  ( 0.0, p.get_x() );
 std::complex<double>  vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.
 double omega  = csq_red * pbe->Strength() / p.Energy();
 std::complex<double>  bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 std::complex<double>  bf = bi*pbe->propPhase_ + pbe->propTerm_;
 
 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 double dthmdphi =   asin( real(bi)/rho ) 
                   - asin( real(bf)/rho );

 // Step 4.
 std::complex<double>  expFactor( cos(dthmdphi), sin(dthmdphi) );
 std::complex<double>  vuf = vui*expFactor;
 std::complex<double>  uf  = ( ui + bi )*expFactor - bf;

 // Final filter back to state coordinates
 // REMOVE: double dphi   = 0.0;
 double dtheta = dthmdphi + pbe->dphi_;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT    = pbe->getReferenceTime();

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}


int rbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) 
{
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
 rbend* pbe = dynamic_cast<rbend*>(p_be);

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
 bf = bi*pbe->propPhase_ + pbe->propTerm_;

 // Step 3.
 Jet rho, dthmdphi;
 rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 dthmdphi =   asin( real(bi)/rho ) 
            - asin( real(bf)/rho );

 // Step 4.
 JetC expF, vuf, uf;
 expF = cos(dthmdphi) + complex_i*sin(dthmdphi);
 vuf  = vui*expF;
 uf   = ( ui + bi )*expF - bf;


 // Final filter back to state coordinates
 Jet dtheta, cdt;
 // REMOVE: double dphi   = 0.0;
 dtheta = dthmdphi + pbe->dphi_;
 cdt    = - PH_MKS_c * dtheta / omega;
 double CDT = pbe->getReferenceTime();

 p.set_x    ( imag( uf ) );
 p.set_y    ( p.get_y() + beta_2*cdt );
 p.set_cdt  ( p.get_cdt() + ( cdt - CDT ) );
 p.set_npx  ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}

// ********************************************************

int rbend::Real_Exact_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  double xpr = p.get_npx()/p.get_npz();
  if( 0.0 != xpr ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
  }

  // Propagate through the constant magnetic field.
  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  xpr = p.get_npx()/p.get_npz();
  if( 0.0 != xpr ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}


int rbend::Real_Exact_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  Jet edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  Jet xpr;
  xpr = p.get_npx()/p.get_npz();
  if( 0.0 != xpr.standardPart() ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
  }

  // Propagate through constant magnetic field.
  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  xpr = p.get_npx()/p.get_npz();
  if( 0.0 != xpr.standardPart() ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}


int rbend::Real_InEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  double xpr = p.get_npx()/p.get_npz();
  if( 0.0 != xpr ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
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


int rbend::Real_InEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  Jet edgeCoeff;

  // Put in a kludge for the vertical focusing upon entrance.
  Jet xpr;
  xpr = p.get_npx() / p.get_npz();
  if( 0.0 != xpr.standardPart() ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
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


int rbend::Real_OutEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  double edgeCoeff;

  // Propagate through the constant magnetic field.
  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  double xpr = p.get_npx() / p.get_npz();
  if( 0.0 != xpr ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}


int rbend::Real_OutEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
  static rbend* pbe;
  pbe = dynamic_cast<rbend*>(p_be);
  Jet edgeCoeff;

  (*_myPropagator)(pbe,p);

  // Put in a kludge for the vertical focusing upon exit.
  Jet xpr; 
  xpr = p.get_npx() / p.get_npz();
  if( 0.0 != xpr.standardPart() ) {
    edgeCoeff = ( xpr / ( p.ReferenceBRho() / pbe->Strength() ) );
    #if 0
    if( ( p.Charge() > 0.0 ) == ( pbe->Strength() > 0.0 ) ) {
    #endif
    #if 1
    if( p.Charge() > 0.0 ) {
    #endif
      p.set_npy( p.get_npy() + edgeCoeff* p.get_y() );
    }
    else {
      p.set_npy( p.get_npy() - edgeCoeff* p.get_y() );
    }
  }

  return 0;
}
