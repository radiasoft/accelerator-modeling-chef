/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      JetQuadrupolePhysics.cc
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
#include "JetQuadrupole.h"
#include "drift.h"

JetQuadrupole::TPOT_Prop::TPOT_Prop( int m )
: _n(m)
{
};

int JetQuadrupole::TPOT_Prop::get_n() const
{
  return _n;
};

void JetQuadrupole::TPOT_Prop::set_n( int m )
{
  _n = m;
};


int JetQuadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, Particle& p ) {
  ((JetQuadrupole*) pbe)->p_bml->propagate( p );
  return 0;
}

int JetQuadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, JetParticle& p ) {
  ((JetQuadrupole*) pbe)->p_bml->propagate( p );
  return 0;
}


void JetQuadrupole::TPOT_Prop::setup( JetQuadrupole* pqd ) const
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
    pqd->p_bml->append( pqd->p_bml_e = new JetthinQuad( str*lng, pqd->strengthIndex ) );
    pqd->p_bml->append( new drift( lng / 2.0 ) );
  }
  
  else if( _n == 4 ) {      // TEAPOT-like schema .........
    pqd->p_bml->append( new drift( frontLength ) );
    pqd->p_bml->append( new JetthinQuad( quarterStrength, pqd->strengthIndex ) );
    for( int i = 0; i < 3; i++ ) {
      pqd->p_bml->append( new drift( sepLength ) );
      pqd->p_bml->append( new JetthinQuad( quarterStrength, pqd->strengthIndex ) );
    }
    pqd->p_bml->append( new drift( frontLength ) );
  }
  
  else {                   // Equal spacing ...............
    pqd->p_bml->append( new drift( lng / ( 2.0*xm ) ) );
    pqd->p_bml->append( new JetthinQuad( str*lng/ xm, pqd->strengthIndex ) );
    for( int i = 0; i < _n-1; i++ ) {
      pqd->p_bml->append( new drift( lng / xm ) );
      pqd->p_bml->append( new JetthinQuad( str*lng/ xm, pqd->strengthIndex ) );
    }
    pqd->p_bml->append( new drift( lng / ( 2.0*xm ) ) );
  }
}



JetQuadrupole::TPOT_Prop  JetQuadrupole::LikeTPOT(4);



void JetthinQuad::localPropagate( Particle& p ) {
 // "Strength" is B'l in Tesla
 static double xpr, ypr, p3divpbar, state5p1;

 state5p1 = 1.0 + p.state[5];
 p3divpbar = sqrt( state5p1 * state5p1
                 - p.state[3]*p.state[3] 
                 - p.state[4]*p.state[4] );
 xpr = p.state[3] / p3divpbar; 
 ypr = p.state[4] / p3divpbar; 

 xpr += - strength * p.state[0] / p.BRho();
 ypr +=   strength * p.state[1] / p.BRho();

 p3divpbar = state5p1 / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 p.state[3] = xpr*p3divpbar;
 p.state[4] = ypr*p3divpbar;
}

void JetthinQuad::localPropagate( JetParticle& p ) {
 Jet dummy;
 Jet xpr, ypr, p3divpbar;

 p3divpbar = sqrt( pow( 1.0 + p.state(5), 2.0 )
                 - p.state(3)*p.state(3) 
                 - p.state(4)*p.state(4) );
 xpr = p.state(3) / p3divpbar; 
 ypr = p.state(4) / p3divpbar; 

 xpr += - KL * p.state(0) / p.BRho();
 ypr +=   KL * p.state(1) / p.BRho();

 p3divpbar = ( 1.0 + p.state(5) ) / sqrt( 1.0 + xpr*xpr + ypr*ypr );

 dummy = xpr*p3divpbar;
 ( p.state ).SetComponent( 3, dummy );
 dummy = ypr*p3divpbar;
 ( p.state ).SetComponent( 4, dummy );
}

