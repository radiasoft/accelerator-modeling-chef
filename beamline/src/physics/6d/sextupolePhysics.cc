/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      sextupolePhysics.cc
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

#include <beamline/beamline.h>
#include <beamline/sextupole.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

void sextupole::localPropagate( Particle& p ) {
  p_bml_->propagate( p );
  p.set_cdt( p.get_cdt() - ctRef_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::localPropagate( JetParticle& p ) {
  p_bml_->propagate( p );
  p.set_cdt( p.get_cdt() - ctRef_ );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::localPropagate( Particle& p ) {

 Vector& state = p.getState(); 
 
 double k = strength_ / p.ReferenceBRho();
 double x = state[0];
 double y = state[1];
 
 if( strength_ != 0.0 ) 
 {

   state[3] -= k * ( x*x - y*y );
   state[4] += 2.0 * k * x*y;

 }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::localPropagate( JetParticle& p ) {

 Mapping& state = p.getState(); 

 double k = strength_ / p.ReferenceBRho();
 Jet x = state[0];
 Jet y = state[1];
 
 if( strength_ != 0.0 ) 
 {
   state[3] -= k * ( x*x - y*y );
   state[4] += 2.0 * k * x*y ;

 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 class thinSextupole;

 class ThinSextupolePropagator: public bmlnElmnt_core_access,
                                public particle_core_access,
                                public jetparticle_core_access {

 public:

  ThinSextupolePropagator(); 
 ~ThinSextupolePropagator();
 
 void  operator()(  bmlnElmnt& elm,    Particle&    );  
  void  operator()( bmlnElmnt& elm, JetParticle&    );  

  const char* Type()  const;

 };

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ThinSextupolePropagator::ThinSextupolePropagator()
: bmlnElmnt_core_access(), particle_core_access(), jetparticle_core_access() 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ThinSextupolePropagator::~ThinSextupolePropagator() {


}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinSextupolePropagator::operator()( bmlnElmnt& elm,  Particle& p ) 
{ 

  Vector& state    = p.getState(); 
  double  strength_ = elm.Strength(); 

  double k = strength_ / p.ReferenceBRho();
  double x = state[0];
  double y = state[1];
 

  state[3] -= k * ( x*x - y*y );
  state[4] += 2.0 * k * x*y;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ThinSextupolePropagator::operator()( bmlnElmnt& elm, JetParticle& p)
{ 

 Mapping& state   = p.getState(); 
 double  strength_ = elm.Strength(); 

 double k = strength_ / p.ReferenceBRho();
 Jet x = state[0];
 Jet y = state[1];
 
 if( strength_ != 0.0 ) 
 {
   state[3] -= k * ( x*x - y*y );
   state[4] += 2.0 * k * x*y ;

 }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* ThinSextupolePropagator::Type()       const
{ 
   return "ThinSextupolePropagator"; 
}

