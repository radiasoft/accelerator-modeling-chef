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
******
******
**************************************************************************
*************************************************************************/
#ifndef CFRBENDPROPAGATORS_H
#define CFRBENDPROPAGATORS_H

#include <beamline/CF_rbend.h>

class CF_rbend::Propagator: public BasePropagator<CF_rbend> {

public:

  Propagator ( int n=1 ) : n_(n) {}

  Propagator* Clone() const { return new Propagator(*this); }

  void  setup( CF_rbend& elm ); 

  void  operator()(  CF_rbend& elm,       Particle& p);
  void  operator()(  CF_rbend& elm,    JetParticle& p);

 private:

  int n_; // number of slices  
 
};


#endif // CFRBENDPROPAGATORS_H

