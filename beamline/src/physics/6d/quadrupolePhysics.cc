/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      quadrupolePhysics.cc
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

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/beamline.h>
#include <beamline/quadrupole.h>
#include <beamline/drift.h>
#include <iostream>


quadrupole::TPOT_Prop::TPOT_Prop( int m )
: _n(m) {}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int quadrupole::TPOT_Prop::get_n() const
{
  return _n;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::TPOT_Prop::set_n( int m )
{
  _n = m;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int quadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, Particle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - pbe->getReferenceTime() );
  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int quadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, JetParticle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - pbe->getReferenceTime() );
  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::TPOT_Prop::setup( quadrupole* pqd ) const
{
  double xm   = _n;
  double lng  = pqd->Length();
  double str  = pqd->Strength();
 
  double frontLength     = 6.0*(lng/4.0)/15.0;
  double sepLength       = ( lng - 2.0*frontLength ) / 3.0;
  double quarterStrength = pqd->Strength()*lng/4.0;
 

  if( pqd->p_bml ) { pqd->p_bml->zap(); delete pqd->p_bml;  pqd->p_bml=0; }

  pqd->p_bml = new beamline;
  
  if( _n == 1 ) {           // One thin element ..........
    pqd->p_bml->append( new drift( lng / 2.0 ) );
    pqd->p_bml->append( pqd->p_bml_e = new thinQuad( str*lng ) );
    pqd->p_bml->append( new drift( lng / 2.0 ) );
  }
  
  else if( _n == 4 ) {      // TEAPOT-like schema .........
    pqd->p_bml->append( new drift( frontLength ) );
    pqd->p_bml->append( new thinQuad( quarterStrength ) );
    for( int i = 0; i < 3; i++ ) {
      pqd->p_bml->append( new drift( sepLength ) );
      pqd->p_bml->append( new thinQuad( quarterStrength ) );
    }
    pqd->p_bml->append( new drift( frontLength ) );
  }
  
  else if( 0 == _n % 4 ) { // TEAPOT tandem ...............
    int    u         = _n/4;
    double xu        = u;
    frontLength     /= xu;
    sepLength       /= xu;
    quarterStrength /= xu;
    for( int i = 0; i < u; i++ ) {
      pqd->p_bml->append( new drift( frontLength ) );
      pqd->p_bml->append( new thinQuad( quarterStrength ) );
      for( int i = 0; i < 3; i++ ) {
        pqd->p_bml->append( new drift( sepLength ) );
        pqd->p_bml->append( new thinQuad( quarterStrength ) );
      }
      pqd->p_bml->append( new drift( frontLength ) );
    }
  }

  else {                   // Equal spacing ...............
    pqd->p_bml->append( new drift( lng / ( 2.0*xm ) ) );
    pqd->p_bml->append( new thinQuad( str*lng/ xm ) );
    for( int i = 0; i < _n-1; i++ ) {
      pqd->p_bml->append( new drift( lng / xm ) );
      pqd->p_bml->append( new thinQuad( str*lng/ xm ) );
    }
    pqd->p_bml->append( new drift( lng / ( 2.0*xm ) ) );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


quadrupole::TPOT_Prop  quadrupole::LikeTPOT(4);


void thinQuad::localPropagate( Particle& p ) {
 // "Strength" is B'l in Tesla

 double k      = 0;
 Vector& state = p.getState();

 if( strength != 0.0 ) 
 {
   k = strength / p.ReferenceBRho();
 
   state[3] += - k * state[0];
   state[4] +=   k * state[1];

 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::localPropagate( JetParticle& p ) {

 double k       = 0;
 Mapping& state = p.getState();

 if( strength != 0.0 ) 
 {

   k = strength / p.ReferenceBRho();

   state[3] += - k * state[0];
   state[4] +=   k * state[1];

 }

}
