/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rfcavityPhysics.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
******  REVISION HISTORY
******  ------------------
******  Revisions (a few):
******  ------------------
******  Date: January 4, 2000   (Tuesday)
******  Stored in cvs repository
******  after major reorganization of files.
******  - lpjm
******
******  Date: January 13, 2006  (Friday)
******  Upgrades: preparation for use in ILC studies.
******  - lpjm
******
******  December 2006 ostiguy@fnal.gov 
******  -Efficiency improvements. 
******  -Particle state passed by reference.
******
******  Date: February 2, 2007  (Friday)
******  Fixed edge focusing following recommendation of Paul Lebrun
******  to use actual electric field upon entry.
******  In addition, included change to "effective length" calculation.
******  !! WARNING: THESE CHANGES ARE TENTATIVE AND BEING REVIEWED !!
******  - lpjm
******
******  Date: May 9, 2007
******  A minor bug fix; produces minimal effect.  Another major overhaul
******  will be coming soon anyway.
******  - lpjm
******
******  
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/rfcavity.h>
#include <beamline/OpticalStateAdaptor.h>

#include <basic_toolkit/TMatrix.h>

using FNAL::pcerr;
using FNAL::pcout;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void rfcavity::localPropagate( Particle& p ) 
{

  double const ctr       = ctRef_;
  if( 0 == strength_ ) { 
    ctRef_ = length_/ p.ReferenceBeta();
    bmlnElmnt::localPropagate( p ); 
    ctRef_ = ctr;
    return;
  }

  double const oldLength = length_;
  double const m         = p.Mass();
  double const p_i       = p.get_npz() * p.ReferenceMomentum();
  double const E_i       = p.Energy();
  double const cdt_i     = p.get_cdt();

  Vector& state   = p.State();

  double const referenceEnergyGain = strength_*sin_phi_s_;
  double const onaxisEnergyGain    = strength_ * sin ( phi_s_ + state[2] * w_rf_ / PH_MKS_c );  // ??? Improve!
  double const eE_z                = onaxisEnergyGain / oldLength;

  // Thin lens kick upon entry
  double k  = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );

  state[3] += k*state[0];  // ??? Improve!
  state[4] += k*state[1];

  // Free space propagation through "effective half length"
  // of the cavity.

  double x_in  = state[0];
  double y_in  = state[1];
  double ct_in = state[2];

  length_  = oldLength/2.0;
  bmlnElmnt::localPropagate( p );

  double w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];

  // Cavity increases energy and momentum of the particle
  double E  = p.Energy() + onaxisEnergyGain/2.0;

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  double newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5]  = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;


  //--------------------------------------
  // PUT TRANSVERSE WAKE FIELD KICK HERE...
  //--------------------------------------

  // Free space propagation through "effective half length"
  // of the cavity.

  x_in  = state[0];
  y_in  = state[1];
  ct_in = state[2];

  length_  = oldLength/2.0;
  bmlnElmnt::localPropagate( p );

  w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];
  state[2] = ct_in;


  // Cavity increases energy and momentum of the particle
  E = p.Energy() + onaxisEnergyGain/2.0;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2.0 );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;


  // Thin lens kick upon exit
  k         = ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );
  state[3] += k*state[0];
  state[4] += k*state[1];

  // Restore parameters
  length_  = oldLength;
  ctRef_   = ctr; 

  // Fix the time calculation
  double rfct =   length_
                * (   (p.get_npz() * p.ReferenceMomentum() - p_i)
                    / (p.Energy()                          - E_i) );

  state[2] = cdt_i + ( rfct - ctRef_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( JetParticle& p ) 
{
  double const ctr       = ctRef_;
  if( 0 == strength_ ) { 
    ctRef_ = length_/p.ReferenceBeta();
    bmlnElmnt::localPropagate( p ); 
    ctRef_ = ctr;
    return;
  }

  double const oldLength = length_;
  double const m         = p.Mass();
  Jet    const p_i       = p.get_npz() * p.ReferenceMomentum();
  Jet    const E_i       = p.Energy();
  Jet    const cdt_i     = p.get_cdt();

  Mapping& state = p.State();

  double const referenceEnergyGain = strength_*sin_phi_s_;
  Jet    const onaxisEnergyGain    = strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );
  Jet    const eE_z                = onaxisEnergyGain / oldLength;

  // Thin lens kick upon entry
  Jet k     = - ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );
  state[3] += k*state[0];  // ??? Improve!
  state[4] += k*state[1];


  // Free space propagation through "effective half length"
  // of the cavity.
  Jet x_in  = state[0];
  Jet y_in  = state[1];
  Jet ct_in = state[2];

  length_  = oldLength/2.0;
  bmlnElmnt::localPropagate( p );

  Jet w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w.standardPart()) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                                      { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];
  state[2] = ct_in;


  // Cavity increases energy and momentum of the particle
  Jet E  = p.Energy() + onaxisEnergyGain/2.0;

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2. );
  double newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
  

  //--------------------------------------
  // PUT TRANSVERSE WAKE FIELD KICK HERE...
  //--------------------------------------


  // Free space propagation through "effective half length"
  // of the cavity.
  x_in  = state[0];
  y_in  = state[1];
  ct_in = state[2];

  length_  = oldLength/2.0;
  bmlnElmnt::localPropagate( p );

  w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w.standardPart()) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                                      { w = 1.0;           }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];
  state[2] = ct_in;


  // Cavity increases energy and momentum of the particle
  E = p.Energy() + onaxisEnergyGain/2.0;

  oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + referenceEnergyGain/2.0 );
  newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  state[5] = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;


  // Thin lens kick upon exit
  k         = ( eE_z/p.ReferenceMomentum() ) / ( 2.0*p.BetaZ() );
  state[3] += k*state[0];
  state[4] += k*state[1];

  // Restore length
  length_ = oldLength;
  ctRef_ = ctr;

  // Fix the time calculation
  Jet rfct =   length_
             * (   (p.get_npz() * p.ReferenceMomentum() - p_i)
                 / (p.Energy()                          - E_i) );

  state[2] = cdt_i + ( rfct - ctRef_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( Particle& p ) 
{
  if( 0.0 != strength_ ) {
    double const m  = p.Mass();
    Vector& state = p.State(); 
    double E = p.Energy() + strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );

    double oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength_*sin_phi_s_ );
    double newRefP = p.ReferenceMomentum();

    state[3] *= ( oldRefP / newRefP );
    state[4] *= ( oldRefP / newRefP );

    state[5]  = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinrfcavity::localPropagate( JetParticle& p ) 
{
  if( 0.0 != strength_ ) {
    double const m  = p.Mass();
    Mapping& state = p.State(); 
    Jet E = p.Energy() + strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );

    double oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength_*sin_phi_s_ );
    double newRefP = p.ReferenceMomentum();

    state[3] *= ( oldRefP / newRefP );
    state[4] *= ( oldRefP / newRefP );

    state[5]  = ( sqrt((E - m)*(E + m))/newRefP ) - 1.0 ;
  }
}
