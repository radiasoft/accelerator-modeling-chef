#ifndef PARTBUNCH_H
#define PARTBUNCH_H

#include <stdlib.h>
#include "Particle.h"
#include "Distribution.h"
#include "slist.h"
#include "BunchPredicate.h"

class ParticleBunch : public slist
{
public:
  ParticleBunch();
  
  virtual ~ParticleBunch();
  
  virtual void recreate( int,	             // number of particles
			 double,             // particle energy
			 double*,            // array of widths (size = 6)
			 Distribution&,      // distribution function
                         BunchPredicate* = 0 // constraints predicate
			);
  virtual void recreate( int,	             // number of particles
			 double,             // particle energy
			 double*,            // array of widths (size = 6)
			 double*,            // array of offsets (size = 6)
			 Distribution&,      // distribution function
                         BunchPredicate* = 0 // constraints predicate
			);

  int iNOfTracked( void ) const {
    return iNOfTracked_;
  }
  
  int iNOfTotal( void ) const {
    return iNOfTotal_;
  }

  protected:
    int iNOfTracked_;
    int iNOfTotal_;
};

class ProtonBunch : public ParticleBunch
{
public:
  ProtonBunch();
  ProtonBunch( int,		   // number of particles
	       double,		   // particle energy
	       double*,		   // array of widths (size = 6)
	       Distribution&,	   // distribution function
               BunchPredicate* = 0 // constraints predicate
	       );
  ProtonBunch( int,		   // number of particles
	       double,		   // particle energy
	       double*,		   // array of widths (size = 6)
	       double*,		   // array of offsets (size = 6)
	       Distribution&,	   // distribution function
               BunchPredicate* = 0 // constraints predicate
	      );

  ProtonBunch( int, int, char, double, double );
  ~ProtonBunch();
  
  void recreate( int,	       	     // number of particles
		 double,	     // particle energy
		 double*,	     // array of widths (size = 6)
		 Distribution&,	     // distribution function
                 BunchPredicate* = 0 // constraints predicate
		 );
  void recreate( int,		     // number of particles
		 double,	     // particle energy
		 double*,	     // array of widths (size = 6)
		 double*,	     // array of offsets (size = 6)
		 Distribution&,	     // distribution function
                 BunchPredicate* = 0 // constraints predicate
		 );
};

class ElectronBunch : public ParticleBunch
{
public:
  ElectronBunch();
  ElectronBunch( int,		     // number of particles
		 double,	     // particle energy
		 double*,	     // array of widths (size = 6)
		 Distribution&,	     // distribution function
                 BunchPredicate* = 0 // constraints predicate
		 );

  ElectronBunch( int,		     // number of particles
		 double,	     // particle energy
		 double*,	     // array of widths (size = 6)
		 double*,	     // array of offsets (size = 6)
		 Distribution&,	     // distribution function
                 BunchPredicate* = 0 // constraints predicate
		 );
  
  ~ElectronBunch();
  
  void recreate( int,		     // number of particles
		 double,	     // particle energy
		 double*,	     // array of widths (size = 6)
		 Distribution&,	     // distribution function
                 BunchPredicate* = 0 // constraints predicate
		 );
  void recreate( int,		     // number of particles
		 double,	     // particle energy
		 double*,	     // array of widths (size = 6)
		 double*,	     // array of offsets (size = 6)
		 Distribution&,	     // distribution function
                 BunchPredicate* = 0 // constraints predicate
		 );
};

#endif // PARTBUNCH_H
