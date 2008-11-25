/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      TBunch.h
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
#ifndef TBUNCH_H
#define TBUNCH_H

#include <stdlib.h>
#include <vector>
#include <algorithm>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Distribution.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Particle.h>
#include <boost/pool/pool.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/indirect_fun.hpp>

// stream operators

template <typename Particle_t> 
std::ostream& operator<<( std::ostream &os, TBunch<Particle_t> const& bunch);

template <typename Particle_t> 
std::istream& operator>>( std::istream &is, TBunch<Particle_t>&       bunch);


template <typename Particle_t> 
class TBunch {

public:
  
  typedef  typename boost::ptr_vector<Particle_t>::iterator                iterator;  
  typedef  typename boost::ptr_vector<Particle_t>::const_iterator    const_iterator;

  typedef  typename boost::ptr_vector<Particle_t>::reverse_iterator                reverse_iterator;  
  typedef  typename boost::ptr_vector<Particle_t>::const_reverse_iterator    const_reverse_iterator;

  enum PhaseSpaceProjection { x_npx=0, y_npy, cdt_ndp };

  TBunch( Particle_t const& reference, int nparticles=0, double const& intensity=0.0);
 ~TBunch();
  
  void append( Particle_t    const& p );  

  void              setReferenceParticle ( Particle_t const& p);
  Particle_t const& getReferenceParticle () const;

  double  Intensity() const;      // actual population i.e. no of particles, as opposed to no of pseudo-particles
  void setIntensity( double const& value);      

  void populateGaussian ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0, unsigned int sd1=117, unsigned int sd2=137 );
  void populateWaterBag ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0, unsigned int sd1=117, unsigned int sd2=137 );
  void populateParabolic( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0, unsigned int sd1=117, unsigned int sd2=137 );
  void populateKV       ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0, unsigned int sd1=117, unsigned int sd2=137 );

  void populateBinomial ( PhaseSpaceProjection psid, double M, double x_lim, double px_lim, double r_12=0.0, unsigned int sd1=117, unsigned int sd2=137 );

  std::vector<double> emittances() const;
  std::vector<double> emittances(std::vector<double> const& dispersion ) const;

  template <typename UnaryPredicate_t>
  inline void remove( UnaryPredicate_t );          

  template <typename LessThanOperator_t>
  inline void sort( LessThanOperator_t );          

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

  TBunch ( TBunch const&);

  Particle_t*                    reference_;       // use pointer to preserve dynamic type 
  double                         intensity_;       // actual population (as opposed to no of pseudo-particles)

  boost::ptr_vector<Particle_t,  boost::view_clone_allocator>  bunch_;
  boost::ptr_vector<Particle_t,  boost::view_clone_allocator>  removed_;

  boost::pool<>                                                 pool_;


};


template <typename Particle_t>
template <typename LessThanOperator_t>
void TBunch<Particle_t>::sort( LessThanOperator_t op ) 
{  
  bunch_.sort( op ); 
}          


template <typename Particle_t>
template <typename UnaryPredicate_t>
void TBunch<Particle_t>::remove( UnaryPredicate_t predicate) 
{
  typename TBunch<Particle_t>::iterator it = std::remove_if(begin(), end(), predicate);
  removed_.transfer( removed_.end(), it, end(), bunch_ );
}


//------------------------------------------------------------------------------------
// specializations
//------------------------------------------------------------------------------------

#include <beamline/ParticleFwd.h> 
//class Particle;

template<>
void TBunch<Particle>::populateGaussian ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12, unsigned int, unsigned int );

template<>
void TBunch<Particle>::populateWaterBag ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12, unsigned int, unsigned int );

template<>
void TBunch<Particle>::populateParabolic( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12, unsigned int, unsigned int );

template<>
void TBunch<Particle>::populateKV       ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12, unsigned int, unsigned int );

template<>
void TBunch<Particle>::populateBinomial ( PhaseSpaceProjection psid, double M, double x_lim, double px_lim, double r_12, unsigned int, unsigned int);

template<>
std::vector<double> TBunch<Particle>::emittances() const;

template<>
std::vector<double> TBunch<Particle>::emittances(std::vector<double> const& dispersion ) const;


#ifndef BEAMLINE_EXPLICIT_TEMPLATES
#include <beamline/TBunch.tcc>
#endif

#endif // TBUNCH_H
