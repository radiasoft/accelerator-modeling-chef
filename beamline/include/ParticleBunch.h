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
****** REVISION HISTORY
******
****** Mar 2007:          ostiguy@fnal.gov
******
****** - introduced bunch iterators
****** - use STL containers for implementation
**************************************************************************
*************************************************************************/

#ifndef PARTBUNCH_H
#define PARTBUNCH_H

#include <stdlib.h>
#include <list>
#include <algorithm>

#include <basic_toolkit/globaldefs.h>
#include <beamline/Particle.h>
#include <basic_toolkit/Distribution.h>

#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<Particle>       ParticlePtr;
typedef boost::shared_ptr<Particle const> ConstParticlePtr;

typedef boost::shared_ptr<Proton>         ProtonPtr;
typedef boost::shared_ptr<Proton const>   ConstProtonPtr;
typedef boost::shared_ptr<Positron>       PositronPtr;
typedef boost::shared_ptr<Positron const> ConstPositronPtr;


class ParticleBunch 
{

friend std::istream& operator >>( std::istream &is, ParticleBunch& bunch);

public:
  class Discriminator : public std::unary_function<ParticlePtr, bool>
  {
    public:
      Discriminator( const ParticleBunch* );
      Discriminator( const Discriminator& );
      virtual ~Discriminator() {}

      virtual bool operator()( ParticlePtr ) { return true; }
    protected:

      const ParticleBunch* _within;  // not owned
  };

  
  typedef std::list<ParticlePtr >::iterator       iterator;  
  typedef std::list<ParticlePtr >::const_iterator const_iterator;


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

  void append( const Particle& );  //  The Particle is cloned   
  void append( ParticlePtr p);     //  Particle is NOT cloned. Shared ownership with caller; 
                                   // caller can safely delete.

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end()   const;

  // void remove( const Particle* );

  std::list<ParticlePtr> remove( Discriminator& );

  void clear();
   
  // I'll have to think carefully about this one.
  // Returns a list of removed particles and hands
  // their ownership to invoking program.


  // Query functions
  int size()   const;
  bool empty() const;

protected:

  virtual Particle*  makeParticle(double energy, Vector const& state) = 0;

  std::list<ParticlePtr> bunch_;

};




// stream operators

std::ostream& operator<<( std::ostream &os, const ParticleBunch& bunch);
std::istream& operator>>( std::istream &is, ParticleBunch& bunch);



class DLLEXPORT ProtonBunch : public ParticleBunch
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
  void append( ProtonPtr  );

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
 protected:

 Proton*  makeParticle(double energy, Vector const& state);

};


class DLLEXPORT PositronBunch : public ParticleBunch
{
public:
  PositronBunch();
  PositronBunch( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );

  PositronBunch( int,                // number of particles
                 double,             // particle energy
                 double*,            // array of widths (size = 6)
                 double*,            // array of offsets (size = 6)
                 Distribution&,      // distribution function
                 Discriminator* = 0  // constraints predicate
                 );
  
  ~PositronBunch();
  
  void append( const Positron& );
  void append( PositronPtr );

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

 protected:
 
  Positron*  makeParticle(double energy, Vector const& state);

};

#endif // PARTBUNCH_H
