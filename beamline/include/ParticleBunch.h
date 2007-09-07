/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      ParticleBunch.h
******                                                                
******  Copyright (c) 2007 Fermi Research Alliance, Inc.    
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******   
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov
******
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef PARTICLEBUNCH_H
#define PARTICLEBUNCH_H

#include <stdlib.h>
#include <vector>
#include <algorithm>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Distribution.h>
#include <beamline/Particle.h>
#include <boost/pool/pool.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/indirect_fun.hpp>

class ParticleBunch;

// stream operators

std::ostream& operator<<( std::ostream &os, ParticleBunch const& bunch);
std::istream& operator>>( std::istream &is, ParticleBunch&       bunch);


class ParticleBunch {


 friend std::istream& operator >>( std::istream &is, ParticleBunch& bunch);


public:
  
  typedef  boost::ptr_vector<Particle>::iterator                iterator;  
  typedef  boost::ptr_vector<Particle>::const_iterator    const_iterator;

  typedef  boost::ptr_vector<Particle>::reverse_iterator                reverse_iterator;  
  typedef  boost::ptr_vector<Particle>::const_reverse_iterator    const_reverse_iterator;

  enum PhaseSpaceProjection { x_npx=0, y_npy, ct_dpp };

  ParticleBunch( Particle const& reference, int nparticles=0, double const& population=0.0);
 ~ParticleBunch();
  
  void append( Particle    const& p );  

  void            setReferenceParticle ( Particle const& p);
  Particle const& getReferenceParticle () const;

  double Population() const;

  void populateGaussian ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);
  void populateWaterBag ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);
  void populateParabolic( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);
  void populateKV       ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);

  void populateBinomial ( PhaseSpaceProjection psid, double M, double x_lim, double px_lim, double r_12=0.0);


  template <typename UnaryPredicate_t>
  void remove( UnaryPredicate_t );          

  template <typename BinaryPredicate_t>
  void sort( BinaryPredicate_t );          

  void clear(); 
  int  size()           const;
  int  removed_size()   const;
  bool empty()          const;

  // iterators 
 
  iterator                   begin();
  const_iterator             begin()          const;

  iterator                   end();
  const_iterator             end()            const;

  const_iterator             removed_begin()  const;
  const_iterator             removed_end()    const;

  reverse_iterator           rbegin();
  const_reverse_iterator     rbegin()         const;

  reverse_iterator           rend();
  const_reverse_iterator     rend()           const;

  const_reverse_iterator     removed_rbegin()  const;
  const_reverse_iterator     removed_rend()    const;


private:

  ParticleBunch (ParticleBunch const&);

  Particle*                    reference_;       // use pointer to preserve dynamic type 
  double                       population_;      // actual population (as opposed to no of pseudo-particles)

  boost::ptr_vector<Particle,  boost::view_clone_allocator>  bunch_;
  boost::ptr_vector<Particle,  boost::view_clone_allocator>  removed_;

  boost::pool<>                                              pool_;

};


template <typename BinaryPredicate_t>
void ParticleBunch::sort( BinaryPredicate_t predicate ) 
{  
  bunch_.sort( predicate ); 
}          


template <typename UnaryPredicate_t>
void ParticleBunch::remove( UnaryPredicate_t predicate) 
{
  ParticleBunch::iterator it = std::remove_if(begin(), end(), predicate);
  removed_.transfer( removed_.end(), it, end(), bunch_ );
}






#endif // PARTICLEBUNCH_H
