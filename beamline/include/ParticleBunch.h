/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      ParticleBunch.h
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
