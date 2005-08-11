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
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "Particle.h"
#include "quadrupole.h"
#include "drift.h"

quadrupole::TPOT_Prop::TPOT_Prop( int m )
: _n(m)
{
};

int quadrupole::TPOT_Prop::get_n() const
{
  return _n;
};

void quadrupole::TPOT_Prop::set_n( int m )
{
  _n = m;
};


int quadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, Particle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - pbe->getReferenceTime() );
  return 0;
}

int quadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, JetParticle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - pbe->getReferenceTime() );
  return 0;
}


void quadrupole::TPOT_Prop::setup( quadrupole* pqd ) const
{
  double xm   = _n;
  double lng  = pqd->Length();
  double str  = pqd->Strength();
 
  double frontLength     = 6.0*(lng/4.0)/15.0;
  double sepLength       = ( lng - 2.0*frontLength ) / 3.0;
  double quarterStrength = pqd->Strength()*lng/4.0;
 
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



quadrupole::TPOT_Prop  quadrupole::LikeTPOT(4);



void thinQuad::localPropagate( Particle& p ) {
 // "Strength" is B'l in Tesla
 static double TrState[6];
 static double k;

 if( strength != 0.0 ) 
 {
   memcpy((void *)TrState, (const void *)p.state, 6 * sizeof(double));

   k = strength / p.ReferenceBRho();
   TrState[3] += - k * TrState[0];
   TrState[4] +=   k * TrState[1];

   p.state[3] = TrState[3];
   p.state[4] = TrState[4];

 }
}

void thinQuad::localPropagate( JetParticle& p ) {
 static Jet TrState[6];
 static int i;
 static double k;

 if( strength != 0.0 ) 
 {
   for( i = 0; i < 6; i++  ) {
     TrState[i] = p.state(i);
   }

   k = strength / p.ReferenceBRho();
   TrState[3] += - k * TrState[0];
   TrState[4] +=   k * TrState[1];

   ( p.state ).SetComponent( 3, TrState[3] );
   ( p.state ).SetComponent( 4, TrState[4] );
 }
}
