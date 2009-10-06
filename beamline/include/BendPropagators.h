/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BendPropagators.h
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
******  - generic type BmlnElmnt used as function argument 
******
******
**************************************************************************
*************************************************************************/

#ifndef BENDPROPAGATORS_H
#define BENDPROPAGATORS_H

#include <beamline/BasePropagator.h>
#include <beamline/Bend.h>

class Bend::Propagator: public BasePropagator {

 public:

  Propagator();
  Propagator(Bend const& elm);
  Propagator(Bend::Propagator const& p);

  Propagator* clone() const;
 
  void  ctor( BmlnElmnt const& elm ); 

  void  setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value ); 

  void  operator()(  BmlnElmnt const& elm,            Particle& p);
  void  operator()(  BmlnElmnt const& elm,         JetParticle& p);

 private:
  
   double                     dphi_;    
   std::complex<double>       propPhase_;   
   std::complex<double>       propTerm_;    

};

#endif // BENDPROPAGATORS_H

