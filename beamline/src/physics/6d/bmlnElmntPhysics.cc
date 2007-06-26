/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      bmlnElmntPhysics.cc
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
****** REVISION HISTORY 
******                                         
****** June 2007 ostiguy@fnal.gov
****** - templated propagator 
******
**************************************************************************
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/bmlnElmnt.h>
#include <iostream>

using FNAL::pcerr;
using FNAL::pcout;

#ifdef MADPHYSICS

void bmlnElmnt::localPropagate( Particle& p ) {

 static bool firstCall = true;
 if( firstCall ) {
  (*pcout) << "*** WARNING ***  bmlnElmnt::localPropagate( Particle& ): This\n"
            << "*** WARNING ***  routine was written using a linear\n"
            << "*** WARNING ***  approximation." << std::endl;
  firstCall = false;
 }

 Vector& state = p.getState();

 state[0] += length_ * state[3];
 state[1] += length_ * state[4];
 state[2] += length_/(p.Beta()*p.Gamma()*p.Gamma() ) * state[5];

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( JetParticle& p ) {

 static bool firstCall = true;
 if( firstCall ) {
  (*pcout) << "*** WARNING ***  bmlnElmnt::localPropagate( JetParticle& ): This\n"
            << "*** WARNING ***  routine was written using a linear\n"
            << "*** WARNING ***  approximation." << std::endl;
  firstCall = false;
 }

 Mapping & state = p.getState();

 Jet  dummy = state[0] + length_ * state[3];
 state.SetComponent( 0, dummy );
 dummy = state[1] + length_ * state[4];
 state.SetComponent( 1, dummy );
 dummy = state[2] + length_/(p.Beta()*p.Gamma()*p.Gamma()) * state[5];
 state.SetComponent( 2, dummy );

}

#endif

/* ------------------------------------------------------ */

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifndef MADPHYSICS


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace { 

template <typename Particle_t>
void propagate( bmlnElmnt& elm, Particle_t& p ) 
{

 typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
 typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;
 
 State_t& state = p.getState();

 Component_t npz = p.get_npz();

 Component_t xpr = state[3] / npz;
 Component_t ypr = state[4] / npz;

 state[0] += ( elm.Length() * xpr);
 state[1] += ( elm.Length() * ypr);

 Component_t D = elm.Length()*sqrt( 1.0 + xpr*xpr + ypr*ypr ); 

 state[2] += ( D / p.Beta() ) - elm.getReferenceTime();
}

}// anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void bmlnElmnt::localPropagate( Particle& p ) 
{
   ::propagate(*this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( JetParticle& p ) 
{
   ::propagate(*this, p);
}


#endif

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( ParticleBunch& b ) 
{

 for (  ParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  
 {  localPropagate( *it ); }

}


