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
******
**************************************************************************
*************************************************************************/
#ifndef BASEPROPAGATOR_H
#define BASEPROPAGATOR_H

#include <complex>

class    Particle;
class JetParticle;

template<typename T>
class TVector;

typedef TVector<double> Vector;

template<typename T>
class TJet;

typedef TJet<double>                Jet;
typedef TJet<std::complex<double> > JetC;


template<typename T>
class TJetVector;

typedef TJetVector<double>                 JetVector;
typedef TJetVector<std::complex<double> > JetCVector;


template<typename T>
class TBunch;

typedef TBunch<Particle>            ParticleBunch;
typedef TBunch<JetParticle>      JetParticleBunch;

template<typename T>
class TMapping;

typedef TMapping<double>                 Mapping;
typedef TMapping<std::complex<double> >  MappingC;

template<typename Particle_t>
  struct PropagatorTraits { 
  static double  norm( double const& comp);  
}; 


template<>
struct PropagatorTraits<Particle> {
  typedef Vector                                   State_t;
  typedef double                               Component_t;
  typedef std::complex<double>                 ComplexComponent_t;
  typedef Vector                                  Vector_t;
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
   
 
template<typename Element_t>
class BasePropagator {

 public:
  
  virtual BasePropagator<Element_t>* Clone() const                = 0;

  virtual void  setup( Element_t&);

  virtual void  operator()(  Element_t& elm,         Particle& p) = 0; 
  virtual void  operator()(  Element_t& elm,      JetParticle& p) = 0; 
  virtual void  operator()(  Element_t& elm,    ParticleBunch& b);  
  virtual void  operator()(  Element_t& elm, JetParticleBunch& b);  

};

#ifndef  BEAMLINE_EXPLICIT_TEMPLATES 
#include <beamline/BasePropagator.tcc>
#endif

#endif // BASEPROPAGATOR_H



