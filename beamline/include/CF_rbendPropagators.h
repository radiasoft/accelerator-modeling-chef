/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      CF_rbendPropagators.h
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
******  Author: Jean-Francois Ostiguy  ostiguy@fnal.gov
******
******  May 2008 ostiguy@fnal.gov
******  - propagator moved (back) to base class
******  - generic type bmlnElmnt used as function argument 
******
******
**************************************************************************
*************************************************************************/
#ifndef CFRBENDPROPAGATORS_H
#define CFRBENDPROPAGATORS_H

#include <beamline/CF_rbend.h>

template<typename Particle_t>
class TBunch;

class Particle;
class JetParticle;


class CF_rbend::Propagator: public BasePropagator {

public:

  Propagator ( int n=1 ) : n_(n) {}

  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( bmlnElmnt& elm ); 
   
  void  setAttribute ( bmlnElmnt& elm, std::string const& name, boost::any const& value );

  void  operator()(  bmlnElmnt const& elm,            Particle& p);
  void  operator()(  bmlnElmnt const& elm,         JetParticle& p);

 private:

  int n_; // number of slices  
 
};


#endif // CFRBENDPROPAGATORS_H


