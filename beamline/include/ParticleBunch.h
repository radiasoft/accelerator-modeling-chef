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
#include <list>
#include <algorithm>

#include "Particle.h"
#include "Distribution.h"

class ParticleBunch 
{
// Subclasses: Iterator, Discriminator
public:
  class Discriminator : public std::unary_function<Particle*, bool>
  {
    public:
      Discriminator( const ParticleBunch* );
      Discriminator( const Discriminator& );
      virtual ~Discriminator() {}

      virtual bool operator()( Particle* ) { return true; }
    protected:
      const ParticleBunch* _within;  // not owned
  };


  class Iterator
  {
    public:
      Iterator( ParticleBunch& );
      Iterator( const Iterator& );
      ~Iterator();

      const Particle* next();
      void reset();
    private:
      std::list<Particle*>::iterator _cur;
      ParticleBunch* _ref;
  };
  friend class Iterator;

// Public member functions
public:
  ParticleBunch();
  virtual ~ParticleBunch();
  
  // virtual void recreate( int,                // number of particles
  //                        double,             // particle energy
  //                        double*,            // array of widths (size = 6)
  //                        Distribution&,      // distribution function
  //                        Discriminator* = 0  // constraints predicate
  //                      ) = 0;
  // virtual void recreate( int,                // number of particles
  //                        double,             // particle energy
  //                        double*,            // array of widths (size = 6)
  //                        double*,            // array of offsets (size = 6)
  //                        Distribution&,      // distribution function
  //                        Discriminator* = 0  // constraints predicate
  //                       ) = 0;


  // Modifier functions
  void append( const Particle& );  
  // The argument is cloned to assure ownership     THIS IS A MISTAKE
  // by the ParticleBunch. Responsibility for the
  // argument remains with the invoking program.

  // void remove( const Particle* );
  std::list<Particle*> remove( Discriminator& );
  // I'll have to think carefully about this one.
  // Returns a list of removed particles and hands
  // their ownership to invoking program.

  // Query functions
  int size() const;
  bool isEmpty() const;

// Protected data
protected:
  std::list<Particle*> _bag;
  // Particles in the _bag are owned by the ParticleBunch object.
};


class ProtonBunch : public ParticleBunch
{
public:
  ProtonBunch();
  ProtonBunch( int,                // number of particles
               double,             // particle energy
               double*,            // array of widths (size = 6)
               Distribution&,      // distribution function
               Discriminator* = 0  // constraints predicate
               );
  ProtonBunch( int,                // number of particles
               double,             // particle energy
               double*,            // array of widths (size = 6)
               double*,            // array of offsets (size = 6)
               Distribution&,      // distribution function
               Discriminator* = 0  // constraints predicate
              );

  ProtonBunch( int, int, char, double, double );
  ~ProtonBunch();
  
  void append( const Proton& );

  void recreate( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );
  void recreate( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 double*,            // array of offsets (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );
};


class ElectronBunch : public ParticleBunch
{
public:
  ElectronBunch();
  ElectronBunch( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );

  ElectronBunch( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 double*,            // array of offsets (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );
  
  ~ElectronBunch();
  
  void recreate( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );
  void recreate( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 double*,            // array of offsets (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );
};

#endif // PARTBUNCH_H
