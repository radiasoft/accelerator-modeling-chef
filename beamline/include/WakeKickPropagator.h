/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      WakeKickPropagator.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******                                                                
****** REVISION HISTORY
******
****** May 2008 ostiguy@fnal.gov
****** - propagator moved (back) to base class
****** - generic type bmlnElmnt used as function argument 
******
**************************************************************************
*************************************************************************/

#ifndef WAKEKICKPROPAGATOR_H
#define WAKEKICKPROPAGATOR_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/ConvolutionFunctor.h>
#include <beamline/WakeKick.h>

class    Particle;
class JetParticle;

template <typename Particle_t>
class TBunch;

typedef TBunch<Particle>       ParticleBunch;
typedef TBunch<JetParticle> JetParticleBunch;

class WakeKick::Propagator: public BasePropagator {

public:

  Propagator( int nsamples, double const& interval);

  Propagator( WakeKick::Propagator const& other);

  Propagator* Clone() const; 

  void setup( bmlnElmnt& elm);

  void operator()(  bmlnElmnt const&  elm,          Particle& p );
  void operator()(  bmlnElmnt const&  elm,       JetParticle& p );
  void operator()(  bmlnElmnt const&  elm,     ParticleBunch& b );
  void operator()(  bmlnElmnt const&  elm,  JetParticleBunch& b );

  void debug( WakeKick&  elm, ParticleBunch&);

private:

  int                        nsamples_;
  double                     interval_;

  ConvolutionFunctor<double> lwake_;
  ConvolutionFunctor<double> twake_;
 
};

#endif // WAKEKICKPROPAGATOR_H 
