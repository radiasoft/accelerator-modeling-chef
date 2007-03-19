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
******  Date: January 4, 2007   (Tuesday)
******  Stored in cvs repository
******  after major reorganization of files.
******  - lpjm
******
******  Date: January 13, 2006  (Friday)
******  Upgrades: preparation for use in ILC studies.
******  - lpjm
******
******  Date: December 14, 2006 (Thursday)
******  Elimination of raw pointers; Particles passed by reference
******  for consistency and safety (no test for null ptr required)
******  -jfo
******  
******  Date: February 2, 2007  (Friday)
******  Fixed edge focusing following recommendation of Paul Lebrun
******  to use actual electric field upon entry.
******  In addition, included change to "effective length_" calculation.
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

  double oldRefP, newRefP;
  double referenceEnergyGain, onaxisEnergyGain;
  double w;
  double k1, k2;

  Vector& state = p.getState();

  referenceEnergyGain = strength_*sin_phi_s;
  onaxisEnergyGain    = strength_*sin( phi_s + state[2] * w_rf / PH_MKS_c );

  // Assign focal lengths for effective kicks that
  // model first order edge focussing.
  k1 = - 0.5*(onaxisEnergyGain/length_)/ E;
  k2 =   0.5*(onaxisEnergyGain/length_)/(E + referenceEnergyGain);
  // ??? Are the denominators correct ???

  // Thin lens kick upon entry
  state[3] += k1*state[0];
  state[4] += k1*state[1];

  // Propagate through the inner structures
  double x_in = state[0];
  double y_in = state[1];
  w = (onaxisEnergyGain/2.0) / p.Energy();
  if( std::abs(w) > 1.0e-8 ) { w = (log(1.+w)/w); }
  else                       { w = 1.0;           }

  bmlnElmnt** x = _u;
  while( x <= _v ) {
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

  double oldRefP, newRefP;
  double referenceEnergyGain;

  Mapping& state = p.getState();

  referenceEnergyGain = strength_*sin_phi_s;
  Jet onaxisEnergyGain( strength_*sin( phi_s + state[2] * w_rf / PH_MKS_c ) );

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

  bmlnElmnt** x = _u;
  while( x <= _v ) {
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
  double denom, px, py, cs; 
  double oldRefP, newRefP;
  
  Vector& state = p.getState(); 
 
  if( 0.0 != this->strength_ ) {
    px = p.get_npx();
    py = p.get_npy();
    denom = 1.0 + p.get_ndp();
    cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
    double E = p.Energy() + ((strength_*cs)*(sin( phi_s + state[2] * w_rf / PH_MKS_c )));

    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength_*sin_phi_s );
    newRefP = p.ReferenceMomentum();

    state[3] *= ( oldRefP / newRefP );
    state[4] *= ( oldRefP / newRefP );

    double   m  = p.Mass();
    double   pr = p.ReferenceMomentum();

    state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinrfcavity::localPropagate( JetParticle& p ) 
{
  Jet__environment_ptr myEnv( p.State().Env() );
  Jet denom(myEnv), px(myEnv), py(myEnv), cs(myEnv);
  double oldRefP, newRefP;

  Mapping& state = p.getState(); 

  if( 0.0 != this->strength_ ) {
    px = p.get_npx();
    py = p.get_npy();
    denom = 1.0 + p.get_ndp();
    cs = sqrt( 1.0 - ( ( px*px + py*py ) / ( denom*denom ) ) );
    Jet E = p.Energy() + ((strength_*cs)*(sin( phi_s + state(2) * w_rf / PH_MKS_c )));

    oldRefP = p.ReferenceMomentum();
    p.SetReferenceEnergy( p.ReferenceEnergy() + strength_*sin_phi_s );
    newRefP = p.ReferenceMomentum();
    state[3] = ( oldRefP / newRefP )*state[3];
    state[4] = ( oldRefP / newRefP )*state[4];

     double   m  = p.Mass();
     double   pr = p.ReferenceMomentum();

     state[5] = ( sqrt((E - m)*(E + m))/pr ) - 1.0 ;
  }
}
