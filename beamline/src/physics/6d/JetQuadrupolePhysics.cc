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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <Particle.h>
#include <JetQuadrupole.h>
#include <drift.h>

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


int JetQuadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, Particle& p ) 
{
  ((JetQuadrupole*) pbe)->p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - pbe->getReferenceTime() );
  return 0;
}


int JetQuadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, JetParticle& p ) 
{
  ((JetQuadrupole*) pbe)->p_bml->propagate( p );
  p.set_cdt( p.get_cdt() - pbe->getReferenceTime() );
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
 double TrState[6];
 double k = 0;

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

void JetthinQuad::localPropagate( JetParticle& p ) {
 Jet TrState[6];

 int i = 0;
 if( strength != 0.0 ) 
 {
   for( i = 0; i < 6; i++  ) {
     TrState[i] = p.state(i);
   }

   Jet k( (1.0/ p.ReferenceBRho()) * JetStrength  );
   TrState[3] += - k * TrState[0];
   TrState[4] +=   k * TrState[1];

   ( p.state ).SetComponent( 3, TrState[3] );
   ( p.state ).SetComponent( 4, TrState[4] );
 }
}

