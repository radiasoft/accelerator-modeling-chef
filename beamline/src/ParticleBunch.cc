#include "ParticleBunch.h"

#include <math.h>
#ifdef __VISUAL_CPP__
#include "MathConstants.h"
#else
#include <string.h>
#endif
// #include "beamline.inc"

extern "C" long lrand48( void ); // added by O.K.

#define    MAX_RANDOM    2147483647.0 

ParticleBunch::ParticleBunch() : slist(), iNOfTracked_( 0 ), iNOfTotal_( 0 ) {
}

ParticleBunch::~ParticleBunch() {
 slist_iterator getNext( *(slist*) this );
 Particle* p;
 while( (p = (Particle*) getNext()) ) delete p;
}

void ParticleBunch::recreate(int,double,double*,Distribution&, BunchPredicate*) {
  cerr << "ParticleBunch::recreate -- This method doesn't know what to do!" << endl;
}

void ParticleBunch::recreate(int,double,double*,double*,Distribution&,BunchPredicate*) {
  cerr << "ParticleBunch::recreate -- This method doesn't know what to do!" << endl;
}

ProtonBunch::ProtonBunch() : ParticleBunch() {
}
 
ProtonBunch::ProtonBunch(int nm, double energy, double* widths,
			 Distribution& distrib,
                         BunchPredicate* pBunchPredicate ): ParticleBunch() {
#if defined(__sparc) && !defined(__GNUG__)
 const int false = 0;
 const int true  = 1;
#endif
#if defined(__mips) && !defined(__GNUG__)
 const int false = 0;
 const int true  = 1;
#endif

 Proton* p;
 double x[ BMLN_dynDim ];

 int bProtonAllocated = false;

 while( iNOfTracked_ < nm ) {
   
   if ( !bProtonAllocated ) {  
     p = new Proton(energy);
   }
   x[0] = distrib.getValue()*widths[0];
   x[1] = distrib.getValue()*widths[1];
   x[2] = distrib.getValue()*widths[2];
   x[3] = distrib.getValue()*widths[3];
   x[4] = distrib.getValue()*widths[4];
   x[5] = distrib.getValue()*widths[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     bProtonAllocated = true;
   } else {
     append( p );
     bProtonAllocated = false;
     iNOfTracked_++;
   }
   iNOfTotal_++;
 }
}

ProtonBunch::ProtonBunch(int nm, double energy, double* widths, double* offsets,
                         Distribution& distrib,
                         BunchPredicate* pBunchPredicate ): ParticleBunch() {
 Proton* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Proton(energy);
   x[0] = distrib.getValue()*widths[0] + offsets[0];
   x[1] = distrib.getValue()*widths[1] + offsets[1];
   x[2] = distrib.getValue()*widths[2] + offsets[2];
   x[3] = distrib.getValue()*widths[3] + offsets[3];
   x[4] = distrib.getValue()*widths[4] + offsets[4];
   x[5] = distrib.getValue()*widths[5] + offsets[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;
 }
}

void ProtonBunch::recreate( int nm, double energy, double* widths,
			    Distribution& distrib,
                            BunchPredicate* pBunchPredicate ) {
 Proton* p;
 double x[ BMLN_dynDim ];
				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Proton*) getNext()) )delete p;
 clear();

 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {
   
   p = new Proton(energy);
   x[0] = distrib.getValue()*widths[0];
   x[1] = distrib.getValue()*widths[1];
   x[2] = distrib.getValue()*widths[2];
   x[3] = distrib.getValue()*widths[3];
   x[4] = distrib.getValue()*widths[4];
   x[5] = distrib.getValue()*widths[5];
   p->setState( x );

   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
}

void ProtonBunch::recreate(int nm, double energy, double* widths, double* offsets,
                           Distribution& distrib, BunchPredicate* pBunchPredicate ) {
 Proton* p;
 double x[ BMLN_dynDim ];

				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Proton*) getNext()) ) delete p;
 clear();

 iNOfTracked_ = 0;
 iNOfTotal_   = 0;
 
 while( iNOfTracked_ < nm ) {
   
   p = new Proton(energy);
   x[0] = distrib.getValue()*widths[0] + offsets[0];
   x[1] = distrib.getValue()*widths[1] + offsets[1];
   x[2] = distrib.getValue()*widths[2] + offsets[2];
   x[3] = distrib.getValue()*widths[3] + offsets[3];
   x[4] = distrib.getValue()*widths[4] + offsets[4];
   x[5] = distrib.getValue()*widths[5] + offsets[5];
   p->setState( x );

   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
}

ProtonBunch::ProtonBunch( int, int nm, char t, 
                          double time_h, double energy_h ) : ParticleBunch() {
 int i;
 Proton* p;
 double x[ BMLN_dynDim ];
 x[0] = x[1] = x[3] = x[4] = 0.0;

 srand48(0);     // has to be before the loop

 double random, rad_fract, theta;
 for( i = 0; i < nm; i++ ) {
  p = new Proton;
  
  switch (t){
  case 'R':
    random =  lrand48()/MAX_RANDOM;
    rad_fract = sqrt(1. - pow(1. - random, 2./3.) );
    theta = M_TWOPI * lrand48()/MAX_RANDOM;

    x[2] = time_h * rad_fract * cos(theta);
    x[5] = energy_h * rad_fract * sin(theta);

    break;

  default: printf("No implementation yet for types other than R" );
    break;
  }

  p->setState( x );
  append( p );
 }
}

ProtonBunch::~ProtonBunch() {
}

ElectronBunch::ElectronBunch() : ParticleBunch() {
}
 
ElectronBunch::ElectronBunch(int nm, double energy, double* widths,
                             Distribution& distrib,
                             BunchPredicate* pBunchPredicate ) : ParticleBunch() {
                               
 Electron* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Electron(energy);
   x[0] = distrib.getValue()*widths[0];
   x[1] = distrib.getValue()*widths[1];
   x[2] = distrib.getValue()*widths[2];
   x[3] = distrib.getValue()*widths[3];
   x[4] = distrib.getValue()*widths[4];
   x[5] = distrib.getValue()*widths[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
}

ElectronBunch::ElectronBunch(int nm, double energy, double* widths,
			     double* offsets,Distribution& distrib,
                             BunchPredicate* pBunchPredicate ) : ParticleBunch() {
 Electron* p;
 double x[ BMLN_dynDim ];

 while( iNOfTracked_ < nm ) {
   
   p = new Electron(energy);
   x[0] = distrib.getValue()*widths[0] + offsets[0];
   x[1] = distrib.getValue()*widths[1] + offsets[1];
   x[2] = distrib.getValue()*widths[2] + offsets[2];
   x[3] = distrib.getValue()*widths[3] + offsets[3];
   x[4] = distrib.getValue()*widths[4] + offsets[4];
   x[5] = distrib.getValue()*widths[5] + offsets[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
}

ElectronBunch::~ElectronBunch() {
}

void ElectronBunch::recreate(int nm, double energy, double* widths,
			     Distribution& distrib, BunchPredicate* pBunchPredicate ) {
 Electron* p;
 double x[ BMLN_dynDim ];
				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Electron*) getNext()) ) delete p;

 clear();
 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {
   
   p = new Electron(energy);
   x[0] = distrib.getValue()*widths[0];
   x[1] = distrib.getValue()*widths[1];
   x[2] = distrib.getValue()*widths[2];
   x[3] = distrib.getValue()*widths[3];
   x[4] = distrib.getValue()*widths[4];
   x[5] = distrib.getValue()*widths[5];   
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;   
 }
}

void ElectronBunch::recreate(int nm, double energy, double* widths, double* offsets,
                             Distribution& distrib, BunchPredicate* pBunchPredicate ) {
 Electron* p;
 double x[ BMLN_dynDim ];

				// First get rid of the old particles
 slist_iterator getNext( *(slist*) this );
 while( (p = (Electron*) getNext()) ) delete p;
 clear();

 iNOfTracked_ = 0;
 iNOfTotal_   = 0;

 while( iNOfTracked_ < nm ) {

   p = new Electron(energy);
   x[0] = distrib.getValue()*widths[0] + offsets[0];
   x[1] = distrib.getValue()*widths[1] + offsets[1];
   x[2] = distrib.getValue()*widths[2] + offsets[2];
   x[3] = distrib.getValue()*widths[3] + offsets[3];
   x[4] = distrib.getValue()*widths[4] + offsets[4];
   x[5] = distrib.getValue()*widths[5] + offsets[5];
   p->setState( x );
   
   if (( pBunchPredicate != 0 ) && ( (*pBunchPredicate)( *p ) == 0 )) {
     delete p;
   } else {
     append( p );
     iNOfTracked_++;
   }
   iNOfTotal_++;
 }
}
