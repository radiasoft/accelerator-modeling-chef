/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
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
******  Date: December 14, 2006 (Thursday)
******  Elimination of raw pointers; usage of reference to 
******  particle state to eliminate one level of indirection.
******  Changes made by Jean-Francois Ostiguy.
******  - lpjm
******  
******  Date: February 2, 2007  (Friday)
******  Fixed edge focusing following recommendation of Paul Lebrun
******  to use actual electric field upon entry.
******  In addition, included change to "effective length" calculation.
******  !! WARNING: THESE CHANGES ARE TENTATIVE AND BEING REVIEWED !!
******  - lpjm
******  
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/rfcavity.h>

#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using FNAL::pcerr;
using FNAL::pcout;


void rfcavity::localPropagate( Particle& p ) 
{
  double E( p.ReferenceEnergy() );

  Vector& state = p.getState();

  double referenceEnergyGain = strength_*sin_phi_s_;
  double onaxisEnergyGain    = strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c );

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.
  double k1 = - 0.5*(onaxisEnergyGain/length_)/ E;
  double k2 =   0.5*(onaxisEnergyGain/length_)/(E + referenceEnergyGain);
  // ??? Are the denominators correct ???

  // Thin lens kick upon entry
  state[3] += k1*state[0];
  state[4] += k1*state[1];

  // Propagate through the inner structures
  double x_in = state[0];
  double y_in = state[1];

  double w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*(x++))->localPropagate( p );
  }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];

  // Thin lens kick upon exit

  state[3] += k2*state[0];
  state[4] += k2*state[1];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void rfcavity::localPropagate( JetParticle& p ) 
{
  double E( p.ReferenceEnergy() ); 

  Mapping& state = p.getState();

  double referenceEnergyGain = strength_*sin_phi_s_;
  Jet    onaxisEnergyGain( strength_*sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c ) );

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.

  Jet k1( - 0.5*(onaxisEnergyGain/length_)/ E );
  Jet k2(   0.5*(onaxisEnergyGain/length_)/(E + referenceEnergyGain) );

  // ??? Are the denominators correct ???

  // Thin lens kick upon entry
  state[3] = state[3] + k1*state[0];
  state[4] = state[4] + k1*state[1];

  // Propagate through the inner structures
  Jet x_in( state[0] );
  Jet y_in( state[1] );
  Jet w( (onaxisEnergyGain/2.0) / p.Energy() );

  if( std::abs(w.standardPart()) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                                      { w = 1.0;           }

  bmlnElmnt** x = u_;
  while( x <= v_ ) {
    (*(x++))->localPropagate( p );
  }

  state[0] = ( 1.0 - w )*x_in + w*state[0];
  state[1] = ( 1.0 - w )*y_in + w*state[1];

  // Thin lens kick upon exit

  state[3] =  state[3] + k2*state[0];
  state[4] =  state[4] + k2*state[1];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinrfcavity::localPropagate( Particle& p ) 
{
  if( strength_ == 0.0)  return;

  Vector& state = p.getState(); 
 
  double px = p.get_npx();
  double py = p.get_npy();
  double denom = 1.0 + p.get_ndp();
  double cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
  double E = p.Energy() + ((strength_*cs)*(sin( phi_s_ + state[2] * w_rf_ / PH_MKS_c )));

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + strength_*sin_phi_s_ );
  double newRefP = p.ReferenceMomentum();

  state[3] *= ( oldRefP / newRefP );
  state[4] *= ( oldRefP / newRefP );

  double   m  = p.Mass();
  double   pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinrfcavity::localPropagate( JetParticle& p ) 
{
  if( strength_ == 0.0 ) return;

  Mapping& state = p.getState(); 

  Jet px = p.get_npx();
  Jet py = p.get_npy();
  Jet denom = 1.0 + p.get_ndp();
  Jet cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
  Jet E = p.Energy() + ((strength_*cs)*(sin( phi_s_ + state(2) * w_rf_ / PH_MKS_c )));

  double oldRefP = p.ReferenceMomentum();
  p.SetReferenceEnergy( p.ReferenceEnergy() + strength_ *sin_phi_s_ );
  double newRefP = p.ReferenceMomentum();
  
  state[3] = ( oldRefP / newRefP )*state[3];
  state[4] = ( oldRefP / newRefP )*state[4];

  double   m  = p.Mass();
  double   pr = p.ReferenceMomentum();

  state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0 ;

}
