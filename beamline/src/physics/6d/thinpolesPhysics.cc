/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      thinpolesPhysics.cc
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
****** REVISION HISTORY
******
****** Apr 2007  ostiguy@fnal.gov
******
****** - fixed propagator for thin2pole which previously did not alter
******   the reference frame.
****** - efficiency improvements: get Particle/JetParticle states by reference. 
******   avoid empty initializations.                                                              
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/thinpoles.h>
#include <mxyzptlk/Jet.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

static const std::complex<double> complex_i(0.0,1.0);

void thin2pole::localPropagate( Particle& p ) 
{

 if (strength_ == 0.0 ) return;

 Vector& state = p.getState();

 const double angle =  strength_/p.ReferenceBRho();

 state[3]  -= angle;

 //----------------------------------------
 // thin2pole rotates the reference frame 
 //----------------------------------------
 
 state[3]   = cos( angle )*state[3] + sin( angle ) * p.get_npz();


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thin2pole::localPropagate( JetParticle& p ) {

 if (strength_ == 0.0 ) return;

 Mapping& state = p.getState();

 const double angle =  strength_/p.ReferenceBRho();

 state[3] -= angle;

 //----------------------------------------
 // thin2pole rotates the reference frame 
 //----------------------------------------

 state[3]  = cos( angle )*state[3] + sin(angle ) * p.get_npz();

}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thin12pole::localPropagate( Particle& p ) {
 
 if(strength_ == 0.0) return;

 Vector& state = p.getState();


 double k = strength_/p.ReferenceBRho();
   
 std::complex<double>  z( state[0], state[1] );
 std::complex<double>  z2 = z*z;
 
 z *= z2*z2;
   
 state[3] -= k * real(z);
 state[4] += k * imag(z);
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thin12pole::localPropagate( JetParticle& p ) {

  if(strength_ == 0.0) return;

  Mapping& state = p.getState();

  Jet k = strength_/p.ReferenceBRho();

  JetC  z = state(0) + complex_i*state(1);
  JetC z2  = z*z;
        z *= z2*z2;

   state[3] -= k * real(z);
   state[4] += k * imag(z);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void thin14pole::localPropagate( Particle& p ) 
{
  if(strength_ == 0.0)  return;

  Vector& state = p.getState();
 
  double k = strength_/p.ReferenceBRho();
 
  std::complex<double>  z( state[0], state[1] );
  z *= z*z;
  z *= z;

  state[3] -= k * real(z);
  state[4] += k * imag(z);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thin14pole::localPropagate( JetParticle& p ) {

  if(strength_ == 0.0 ) return;

  Mapping& state = p.getState();
  
  Jet k = strength_/p.ReferenceBRho();
 
  JetC z = state(0) + complex_i*state(1);
       z *= z*z;
       z *= z;

  state[3] -= k * real(z);
  state[4] += k * imag(z);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



void thin16pole::localPropagate( Particle& p ) 
{
 
  if(strength_ == 0.0) return;

  Vector& state = p.getState();

  double k = strength_/p.ReferenceBRho();
 
  std::complex<double>  z( state[0], state[1] );
  std::complex<double>  u = z;
  z *= z*z;
  z *= z*u;
    
  state[3] -= k * real(z);
  state[4] += k * imag(z);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thin16pole::localPropagate( JetParticle& p ) 
{

  if(strength_ == 0.0) return;

  Mapping& state = p.getState();

  Jet k = strength_/p.ReferenceBRho();
 
  JetC z = state(0) + complex_i* state(1);
  JetC u = z;

  z *= z*z;
  z *= z*u;

  state[3] -= k * real(z);
  state[4] += k * imag(z);

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thin18pole::localPropagate( Particle& p ) 
{
 
  if(strength_ == 0.0) return;

  Vector& state = p.getState();

  double k = strength_/p.ReferenceBRho();

  std::complex<double> z( state[0], state[1]);

  z *= z;
  z *= z;
  z *= z;

  state[3] -= k * real(z);
  state[4] += k * imag(z);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thin18pole::localPropagate( JetParticle& p ) 
{

  if(strength_ == 0.0) return;

  Mapping& state = p.getState();

  Jet k = strength_/p.ReferenceBRho();
  Jet x = state(0);
  Jet y = state(1);
 
  JetC z = state(0) + complex_i* state(1);

  z *= z; // z**2
  z *= z; // z**4
  z *= z; // z**8

  state[3] -= k * real(z);
  state[4] += k * imag(z);

}


