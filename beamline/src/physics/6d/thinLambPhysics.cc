#include "beamline.inc"
#include "ParticleBunch.h"

void thinLamb::propagate( Particle& ) { }

void thinLamb::propagate( JetParticle& ) { }

void thinLamb::propagate( ParticleBunch& PB ) {
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

    // Correct the particle `State' based on the alignment data of the
    // thinLamb (if there is any).  Otherwise just grab the particle
    // coordinates.
    if( align ) {
      align->misalign( *pCur, geometry /* ??? */, TrState );
    } else {
      for( i = 0; i < 6; i++ ) TrState[i] = pCur->state[i];
    }

    // Now figure out if the particle is really over the magnet septum.
    if (TrState[0] > xSeptum ) {
      for ( i = 0; i < 6; i++ ) TrState[i] -= RefState[i];
      ExtFlag = 1;
    }

    if( align ) {
      for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
      align->align( dummy, geometry /* ??? */, TrState );
    }

    for ( i = 0; i < 6; i++ )  pCur->state[i] = TrState[i];

    pNxt =  (Particle*)getNext();

    // Tranfer the particle in the extraction region into a different
    // ParticleBunch and remove it from the main bunch.
    if (ExtFlag) {
      ExtPB->append(pCur);
      PB.remove(pCur);
    }
  } while ( (pCur = pNxt) );

  if (ExtPB->size() != 0) ExtBeamline->propagate(*ExtPB);
  delete ExtPB;
}

