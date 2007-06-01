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
******
**************************************************************************
*************************************************************************/

#ifndef WAKEKICKPROPAGATOR_H
#define WAKEKICKPROPAGATOR_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/ConvolutionFunctor.h>

class ParticleBunch;

class WakeKickPropagator {

public:

  WakeKickPropagator( int nsamples, double const& interval);

  WakeKickPropagator( WakeKickPropagator const& other);

  void operator()( Particle&      particle    ) {} 
  void operator()( JetParticle&   jetparticle ) {} 

  void operator()( ParticleBunch& bunch       );

  void debug(ParticleBunch&);

private:

  int                        nsamples_;
  double                     interval_;

  ConvolutionFunctor<double> lwake_;
  ConvolutionFunctor<double> twake_;
 
};

#endif // WAKEKICKPROPAGATOR_H 
