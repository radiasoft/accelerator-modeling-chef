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
******   REVISION HISTORY
******                                     
******   August 2007  ostiguy@fnal.gov
******  - efficiency improvmnents: 
******        - access particle state by refeoence                                                                  
******        - eliminate slow and uncessary dynamic casts  
******
**************************************************************************
*************************************************************************/


#include <beamline/sbend.h>

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <mxyzptlk/Jet.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

static const std::complex<double> complex_i(0.0, 1.0);


sbend::NoEdge_Prop  sbend::NoEdge;
sbend::Exact_Prop   sbend::Exact;
sbend::InEdge_Prop  sbend::InEdge;
sbend::OutEdge_Prop sbend::OutEdge;


namespace {
  Particle::PhaseSpaceIndex const& i_x    =  Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y    =  Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt  =  Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx  =  Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy  =  Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp  =  Particle::ndpIndex;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::Exact_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{

 sbend * pbe = static_cast<sbend*>(p_be);
 double edgeCoeff = 0.0;

 Vector&  state = p.State();

 // Put in a kludge for the vertical focusing upon entrance.

 if( pbe->usTan_ != 0.0 ) {

   edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
 }

 // Propagate through the constant magnetic field.
 (*myPropagator_)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( pbe->dsTan_  != 0.0) {

   edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
 }

 return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::Exact_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{

 sbend* pbe       = static_cast<sbend*>(p_be);
 double edgeCoeff = 0.0;

 Mapping&  state = p.State();

 // Put in a kludge for the vertical focusing upon entrance.

 if( pbe->usTan_ != 0.0 ) {

   edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
 }

 // Propagate through the constant magnetic field.
 (*myPropagator_)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( pbe->dsTan_  != 0.0) {

   edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
 }

 return 0;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int sbend::InEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 sbend* pbe = static_cast<sbend*>(p_be);
 double edgeCoeff = 0.0;

 Vector&  state = p.State();

 // Put in a kludge for the vertical focusing upon entrance.
 if( 0.0 != pbe->usTan_ ) {

   edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
 }

 // Propagate through the constant magnetic field.
 (*myPropagator_)(pbe,p);

  return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::InEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
 sbend* pbe       = static_cast<sbend*>(p_be);
 double edgeCoeff = 0.0;

 Mapping&  state = p.State();

 // Put in a kludge for the vertical focusing upon entrance.

 if( pbe->usTan_  != 0.0 ) {
   edgeCoeff = ( pbe->usTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
 }

 // Propagate through constant magnetic field.
 (*myPropagator_)(pbe,p);

 return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 sbend* pbe       = static_cast<sbend*>(p_be);
 double edgeCoeff = 0.0;

 Vector&  state = p.State();

 // Propagate through the constant magnetic field.
 (*myPropagator_)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( 0.0 != pbe->dsTan_ ) {
   edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
 }

 return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::OutEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p )
{
 sbend* pbe       = static_cast<sbend*>(p_be);
 double edgeCoeff = 0.0;

 Mapping&  state = p.State();

 (*myPropagator_)(pbe,p);

 // Put in a kludge for the vertical focusing upon exit.
 if( pbe->dsTan_ != 0.0 ) {

   edgeCoeff = ( pbe->dsTan_ / ( p.ReferenceBRho() / pbe->Strength() ) );

   if( p.Charge() > 0.0 ) {
     state[i_npy] +=  edgeCoeff * state[i_y];
   }
   else {
     state[i_npy] -=  edgeCoeff * state[i_y];
   }
 }

 return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, Particle& p )
{
 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 sbend* pbe = static_cast<sbend*>(p_be);

 Vector&  state = p.State();

 // Preliminary filter from state coordinates

 double psq = 1.0 + p.get_ndp();
        psq = psq*psq;

 double E_factor = 1.0 / sqrt( psq + p.PNI2() );
 double beta_1 = E_factor*state[i_npx];
 double beta_2 = E_factor*state[i_npy];
 double beta_3 = E_factor*sqrt( psq - state[i_npx]*state[i_npx]
                                    - state[i_npy]*state[i_npy] );

 std::complex<double>  ui  ( 0.0, p.get_x() );
 std::complex<double>  vui ( PH_MKS_c*beta_3, PH_MKS_c*beta_1 );

 // Step 1.

 double            omega  = csq_red * pbe->Strength() / p.Energy();
 std::complex<double>  bi = ( complex_i*vui / omega ) - ui;

 // Step 2.
 std::complex<double>  bf = bi*pbe->propPhase_ + pbe->propTerm_;

 // Step 3.
 double rho = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;

 double dthmdphi =    asin( real(bi)/rho ) 
                   -  asin( real(bf)/rho );

 // Step 4.

 std::complex<double>  expFactor( cos(dthmdphi), sin(dthmdphi) );
 std::complex<double>  vuf = vui*expFactor;
 std::complex<double>  uf  = ( ui + bi )*expFactor - bf;

 // Final filter back to state coordinates

 double dtheta =   dthmdphi + pbe->dphi_;
 double cdt    = - PH_MKS_c * dtheta / omega;
 double CDT    =   pbe->getReferenceTime();

 state[i_x]    =    imag( uf );
 state[i_y]   +=    beta_2*cdt;
 state[i_cdt] +=   (cdt - CDT );
 state[i_npx]  =   ( imag( vuf )/( E_factor * PH_MKS_c ) );

 return 0;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int sbend::NoEdge_Prop::operator()( bmlnElmnt* p_be, JetParticle& p ) 
{

 static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;

 sbend* pbe = static_cast<sbend*>(p_be);

 Mapping&  state = p.State();

 // Preliminary filter from state coordinates

 Jet psq = 1.0 + p.get_ndp();
 psq = psq*psq;

 Jet E_factor = 1.0 / sqrt( psq + p.PNI2() );

 Jet beta_1 = E_factor*state[i_npx];
 Jet beta_2 = E_factor*state[i_npy];
 Jet beta_3 = E_factor*sqrt( psq - state[i_npx]*state[i_npx]
                                 - state[i_npy]*state[i_npy] );

 JetC ui  = complex_i * p.get_x();
 JetC vui = PH_MKS_c*( beta_3 + complex_i * beta_1 );

 // Step 1.

 Jet omega  = csq_red * pbe->Strength() / p.Energy();
 JetC  bi   = ( complex_i*vui / omega ) - ui;

 // Step 2.
 JetC bf = bi*pbe->propPhase_ + pbe->propTerm_;

 // Step 3.

 Jet rho      = PH_MKS_c * sqrt( beta_1*beta_1 + beta_3*beta_3 ) / omega;
 Jet dthmdphi =   asin( real(bi)/rho ) 
                - asin( real(bf)/rho );

 // Step 4.

 JetC expF = cos(dthmdphi) + complex_i*sin(dthmdphi);
 JetC vuf  = vui*expF;
 JetC uf   = ( ui + bi )*expF - bf;


 Jet dtheta = dthmdphi + pbe->dphi_;
 Jet cdt    = - PH_MKS_c * dtheta / omega;

 double CDT = pbe->getReferenceTime();

 state[i_x]    =    imag( uf );
 state[i_y]   +=    beta_2*cdt;
 state[i_cdt] +=   (cdt - CDT );
 state[i_npx]  =   ( imag( vuf )/( E_factor * PH_MKS_c ) );


 return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
