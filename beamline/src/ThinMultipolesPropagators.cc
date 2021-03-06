/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******
******  File:      ThinMultipolePropagators.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
******  Author: ostiguy@fnal.gov
****** 
****** REVISION HISTORY 
****** May 2008 ostiguy@fnal.gov
******  - propagator moved backed to base class. Use static downcast 
******    in operator()() implementation.
******                                                                
***************************************************************************
***************************************************************************
**************************************************************************/

#include <beamline/thinMultipole.h>
#include <beamline/ThinMultipolePropagators.h>
#include <mxyzptlk/Jet.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <complex>
#include <numeric>

namespace {

  std::complex<double> const complex_i(0.0,1.0);

  std::complex<double> toComplex( double const& re, double const& im )
  { return std::complex<double>(re,im); }

  inline JetC toComplex( Jet const& re, Jet const& im )
  { return  re + complex_i*im; }

  template<typename T>
  inline T local_pow(T const& value, int n );

  template<>
  inline JetC local_pow( JetC const& value, int n ) 
  { return pow(value, n); }

  template<>
  inline std::complex<double> local_pow( std::complex<double> const& value, int n ) 
  { return std::pow(value, n); }
  
  typedef PhaseSpaceIndexing::index index;
 
  index const i_x   = Particle::i_x;
  index const i_y   = Particle::i_y;
  index const i_cdt = Particle::i_cdt;
  index const i_npx = Particle::i_npx;
  index const i_npy = Particle::i_npy;
  index const i_ndp = Particle::i_ndp;

  template<typename Particle_t>
  void propagate( thinMultipole const& elm, Particle_t & p ) 
  {
   // "Strength" is (b_n + i a_n) * length  in Tesla

   // Field =  Sum_n c_n  z^n = Sum_n (b_n + i a_n ) (x_n + i y_n) ^ n 

   typedef typename PropagatorTraits<Particle_t>::State_t              State_t;
   typedef typename PropagatorTraits<Particle_t>::Component_t          Component_t;
   typedef typename PropagatorTraits<Particle_t>::ComplexComponent_t   ComplexComponent_t;

   State_t& state = p.state();

   if( elm.Strength() == 0.0 ) return; 

   for ( thinMultipole::const_iterator it  = elm.begin(); 
	                               it != elm.end(); ++it ) {


     int                  const n =    it->first;
     std::complex<double> const k =  - it->second / p.refBrho();
 


     ComplexComponent_t z( state[i_x] +  complex_i*state[i_y] );
 
     z = ::local_pow(z,n);

     state[i_npx] += real( k * z );
     state[i_npy] += imag( k * z );


   }
  }

} // namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::Propagator::Propagator()
  : BasePropagator()
{}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::Propagator::Propagator( thinMultipole const& elm) 
  : BasePropagator(elm)
{}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::Propagator::Propagator( Propagator const& p) 
  : BasePropagator(p)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::Propagator::ctor( BmlnElmnt const& elm) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<thinMultipole const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::Propagator::operator()(  BmlnElmnt const& elm, JetParticle&     p ) 
{
  ::propagate( static_cast<thinMultipole const&>(elm), p);
}
