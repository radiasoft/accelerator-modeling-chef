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

void thin2pole::localPropagate( Particle& p ) {

 Vector& state = p.getState();
 
 if(strength != 0) {
   state[3] -= strength/p.ReferenceBRho();
 }
}

void thin2pole::localPropagate( JetParticle& p ) {

 Mapping& state = p.getState();

  if(strength != 0) {
    state[3] -= strength/p.ReferenceBRho();
  }
}



void thin12pole::localPropagate( Particle& p ) {
 double x, y, k;
 
 Vector& state = p.getState();

 if(strength != 0) {
   k = strength/p.ReferenceBRho();
   x = state[0];
   y = state[1];
   
   std::complex<double>  z( x, y );
   std::complex<double>  z2 = z*z;
   z = z2*z2*z;
   
   state[3] -= k * real(z);
   state[4] += k * imag(z);
 }
}

void thin12pole::localPropagate( JetParticle& p ) {

  Mapping& state = p.getState();

  if(strength != 0) {
    Jet  k, x, y, s;
    JetC z, z2;
    
    k = strength/p.ReferenceBRho();
    x = state(0);
    y = state(1);
    
    z = x + complex_i*y;
    z2 = z*z;
    z = z2*z2*z;

   state[3] -= k * real(z);
   state[4] += k * imag(z);

  }
}



void thin14pole::localPropagate( Particle& p ) {
 double x, y, k;
 
 Vector& state = p.getState();

  if(strength != 0) {
    k = strength/p.ReferenceBRho();
    x = state[0];
    y = state[1];
 
    std::complex<double>  z( x, y );
    z = z*z*z;
    z = z*z;

    state[3] -= k * real(z);
    state[4] += k * imag(z);
  }
}

void thin14pole::localPropagate( JetParticle& p ) {

  Mapping& state = p.getState();

  if(strength != 0) {
    Jet  k, x, y, s;
    JetC z;
 
    k = strength/p.ReferenceBRho();
    x = state(0);
    y = state(1);
 
    z = x + complex_i*y;
    z = z*z*z;
    z = z*z;

    state[3] -= k * real(z);
    state[4] += k * imag(z);


  }
}



void thin16pole::localPropagate( Particle& p ) {
 double x, y, k;
 
 Vector& state = p.getState();

  if(strength != 0) {
    k = strength/p.ReferenceBRho();
    x = state[0];
    y = state[1];
 
    std::complex<double>  z( x, y );
    std::complex<double>  u = z;
    z = z*z*z;
    z = z*z*u;
    
    state[3] -= k * real(z);
    state[4] += k * imag(z);
  }
}

void thin16pole::localPropagate( JetParticle& p ) {

  Mapping& state = p.getState();

  if(strength != 0) {
    Jet  k, x, y, s;
    JetC z, u;
 
    k = strength/p.ReferenceBRho();
    x = state(0);
    y = state(1);
 
    z = x + complex_i*y;
    u = z;
    z = z*z*z;
    z = z*z*u;

    state[3] -= k * real(z);
    state[4] += k * imag(z);

  }
}



void thin18pole::localPropagate( Particle& p ) {
 double x, y, k;
 
 Vector& state = p.getState();

  if(strength != 0) {
    k = strength/p.ReferenceBRho();
    x = state[0];
    y = state[1];
 
    std::complex<double>  z( x, y );
    z = z*z;
    z = z*z;
    z = z*z;

    state[3] -= k * real(z);
    state[4] += k * imag(z);
  }
}

void thin18pole::localPropagate( JetParticle& p ) {

 Mapping& state = p.getState();

  if(strength != 0) {
    Jet  k, x, y, s;
    JetC z;
 
    k = strength/p.ReferenceBRho();
    x = state(0);
    y = state(1);
 
    z = x + complex_i*y;
    z = z*z;
    z = z*z;
    z = z*z;

    state[3] -= k * real(z);
    state[4] += k * imag(z);

  }
}


void thinMultipole::localPropagate( Particle& p ) {
 static char firstCall = 1;
 if( firstCall ) {
  printf( "\n" );
  printf( "*** SORRY ***                               \n" );
  printf( "*** SORRY *** thinMultipole::localPropagate \n" );
  printf( "*** SORRY *** Thin multipoles are being     \n" );
  printf( "*** SORRY *** treated as identity elements. \n" );
  printf( "*** SORRY ***                               \n" );
  firstCall = 0;
 }
}

void thinMultipole::localPropagate( JetParticle& p ) {
 static char firstCall = 1;
 if( firstCall ) {
  printf( "\n" );
  printf( "*** SORRY ***                               \n" );
  printf( "*** SORRY *** thinMultipole::localPropagate \n" );
  printf( "*** SORRY *** Thin multipoles are being     \n" );
  printf( "*** SORRY *** treated as identity elements. \n" );
  printf( "*** SORRY ***                               \n" );
  firstCall = 0;
 }
}
