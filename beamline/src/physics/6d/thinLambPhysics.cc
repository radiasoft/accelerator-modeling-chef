#include "beamline.inc"
#include "ParticleBunch.h"

void thinLamb::propagate( Particle& p ) { }

void thinLamb::propagate( JetParticle& p ) { }

void thinLamb::propagate( ParticleBunch& PB ) {
  static int i;
  static double TrState[6], dummy[6];
  int ExtFlag;
  int ExtCount = 0;
  ParticleBunch* ExtPB;

  if (ExtBeamline == 0 ) return; 
 
  ExtPB = new ParticleBunch();
  slist_iterator getNext((slist&)PB);
  Particle *pCur, *pNxt;
  
  if ( !(pCur = (Particle*)getNext()) ) return; // no more particles

  do {
    
    ExtFlag = 0;

    if( align ) align->misalign( *pCur, geometry /* ??? */, TrState );
    else        for( i = 0; i < 6; i++  ) TrState[i] = pCur->state[i];
    
    if (TrState[0] > xSeptum ) {
      for ( i = 0; i < 6; i++ ) TrState[i] -= RefState[i];
      ExtFlag = 1;
      ++ExtCount;
    }

    if( align ) {
      for( i = 0; i < 6; i++  ) dummy[i] = TrState[i];
      align->align( dummy, geometry /* ??? */, TrState );
    }

    for ( i = 0; i < 6; i++ )  pCur->state[i] = TrState[i];

    pNxt =  (Particle*)getNext();
    if (ExtFlag ) {
      ExtPB->append(pCur);
      PB.remove(pCur);
    }

  } while ( (pCur = pNxt) );

  if (ExtCount) ExtBeamline->propagate(*ExtPB);
  delete ExtPB;

}

