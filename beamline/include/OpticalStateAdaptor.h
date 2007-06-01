/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      OpticalStateAdaptor.h
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
**************************************************************************
*************************************************************************/
#ifndef OPTICALSTATEADAPTOR_H
#define OPTICALSTATEADAPTOR_H


#include <beamline/Particle.h>
#include <beamline/JetParticle.h>


class  OpticalStateAdaptor {

 public:

 
  OpticalStateAdaptor( Particle&       p);
 ~OpticalStateAdaptor();

  double const&  operator[](int i)  const; 
  double&        operator[](int i);

  void sync();  

 private:

  bool           state_is_valid_;
  Particle&      particle_;
  Vector         optical_state_;
 
};

class  JetOpticalStateAdaptor {

 public:

 
  JetOpticalStateAdaptor( JetParticle&       p);
 ~JetOpticalStateAdaptor();

  Jet const&  operator[](int i)  const; 
  Jet&        operator[](int i);

  void sync();  

 private:

  bool           state_is_valid_;
  JetParticle&   particle_;
  Mapping        optical_state_;
 
};

#endif // OPTICALSTATEADAPTOR

