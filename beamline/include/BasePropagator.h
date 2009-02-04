/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BasePropagator.h
******
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
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
******                                                                
******  Author: Jean-Francois Ostiguy  
******          ostiguy@fnal.gov
******
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic bmlmElmnt type use in signatures
******
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef BASEPROPAGATOR_H
#define BASEPROPAGATOR_H

#include <complex>
#include <string>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <basic_toolkit/VectorFwd.h>
#include <mxyzptlk/JetVectorFwd.h>
#include <beamline/bmlnElmnt.h>

class BasePropagator;
typedef boost::shared_ptr<BasePropagator> PropagatorPtr; 

#include <beamline/ParticleFwd.h>
#include <beamline/ParticleBunchFwd.h>
#include <mxyzptlk/JetFwd.h>
#include <mxyzptlk/JetVectorFwd.h>
#include <mxyzptlk/MappingFwd.h>
#include <basic_toolkit/VectorFwd.h>

template<typename Particle_t>
  struct PropagatorTraits { 
  static double  norm( double const& comp);  
}; 


template<>
struct PropagatorTraits<Particle> {
  typedef Vector                                   State_t;
  typedef double                                   Component_t;
  typedef std::complex<double>                     ComplexComponent_t;
  typedef Vector                                   Vector_t;
  static double  norm(  PropagatorTraits<Particle>::Component_t const& comp);  
};  

template<>
struct PropagatorTraits<JetParticle> {
  typedef Mapping                 State_t;
  typedef Jet                 Component_t;
  typedef JetC         ComplexComponent_t;
  typedef JetVector              Vector_t;
  static double  norm( PropagatorTraits<JetParticle>::Component_t const& comp);  
};  
   

class BasePropagator {

 public:
  
   BasePropagator(); 
   BasePropagator( BasePropagator const& o); 

   BasePropagator& operator=( BasePropagator const& o); 

   virtual  BasePropagator* Clone() const    = 0;
   virtual ~BasePropagator();

   virtual void  setup( bmlnElmnt& );
  
   virtual void  setAttribute( bmlnElmnt& elm, std::string const& name, boost::any const& value );

   virtual void  operator()(  bmlnElmnt const& elm,         Particle& p) = 0; 
   virtual void  operator()(  bmlnElmnt const& elm,      JetParticle& p) = 0; 
   virtual void  operator()(  bmlnElmnt const& elm,    ParticleBunch& b);  
   virtual void  operator()(  bmlnElmnt const& elm, JetParticleBunch& b);  

   virtual  bool hasAlignment() const; 
   virtual  void setAlignment( Vector const& translation, Vector const& rotation, bool relative=false);

   virtual  bool hasAperture()   const; 
   virtual  void setAperture( bmlnElmnt::aperture_t, double const& hor, double const& ver );

   virtual  boost::tuple<bmlnElmnt::aperture_t, double, double>  aperture()   const; 
   virtual  boost::tuple<Vector,Vector>                         alignment()   const; 

};

#endif // BASEPROPAGATOR_H



