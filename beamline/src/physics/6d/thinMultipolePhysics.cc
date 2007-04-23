/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  File:      thinMultipolePhysics.cc
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
******  Author:    Jean-Francois Ostiguy                                     
******             Fermilab                                           
******             ostiguy@fnal.gov                                                   
******                                                                
**************************************************************************
*************************************************************************/

#include <beamline/thinMultipole.h>
#include <mxyzptlk/Jet.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinMultipole::localPropagate( Particle& p ) 
{

  Vector& state = p.getState();

#if  0
  if(strength_ == 0.0 ) return;


   k = strength_/p.ReferenceBRho();
   x = state[0];
   y = state[1];
   
   std::complex<double>  z( x, y );
   std::complex<double>  z2 = z*z;
   
   z = z2*z2*z;
   
   state[3] -= k * real(z);
   state[4] += k * imag(z);
 }

#endif
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::localPropagate( JetParticle& p ) 
{

#if 0

  Mapping& state = p.getState();


  if(strength_ != 0) {
    Jet  k, x, y, s;
    JetC z, z2;
    
    k = strength_/p.ReferenceBRho();
    x = state(0);
    y = state(1);
    
    z = x + complex_i*y;
    z2 = z*z;
    z = z2*z2*z;

   state[3] -= k * real(z);
   state[4] += k * imag(z);

  }

#endif
}

