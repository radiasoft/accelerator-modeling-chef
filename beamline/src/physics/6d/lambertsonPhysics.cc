#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      lambertsonPhysics.cc
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


#include "lambertson.h"
#include "ParticleBunch.h"

void thinLamb::localPropagate( Particle& p ) { }

void thinLamb::localPropagate( JetParticle& p ) { }

void thinLamb::localPropagate( ParticleBunch& PB ) {
  static int i;
  static double TrState[6], dummy[6];
  int ExtFlag;
  ParticleBunch* ExtPB;

  if (ExtBeamline == 0 ) return; 
 
  ExtPB = new ParticleBunch();
  slist_iterator getNext((slist&)PB);
  Particle *pCur, *pNxt;
  
  if ( !(pCur = (Particle*)getNext()) ) return; // no more particles

  do {
    
    ExtFlag = 0;

    for( i = 0; i < 6; i++ ) { 
      TrState[i] = pCur->state[i];
    }

    // Now figure out if the particle is really over the magnet septum.
    if (TrState[0] > xSeptum ) {
      for ( i = 0; i < 6; i++ ) TrState[i] -= RefState[i];
      ExtFlag = 1;
    }

    for ( i = 0; i < 6; i++ )  pCur->state[i] = TrState[i];

    pNxt =  (Particle*)getNext();

    // Tranfer the particle in the extraction region into a different
    // ParticleBunch and remove it from the main bunch.
    if (ExtFlag) {
      if (align) this->leaveLocalFrame( *pCur );
      ExtPB->append(pCur);
      PB.remove(pCur);
    }
  } while ( (pCur = pNxt) );

  if (ExtPB->size() != 0) {
    ExtBeamline->propagate(*ExtPB);
  }

  delete ExtPB;
}

