/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      PropagatorDecorator.h
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
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef PROPAGATORDECORATOR_H
#define PROPAGATORDECORATOR_H

#include <basic_toolkit/VectorFwd.h>
#include <beamline/BasePropagator.h>

class PropagatorDecorator : public BasePropagator {
 
public:

  PropagatorDecorator( PropagatorPtr p );   
  PropagatorDecorator( PropagatorDecorator const& o);   
 ~PropagatorDecorator();

  PropagatorDecorator* clone() const;

  void  ctor ( BmlnElmnt const& );
 
  void  setAttribute( BmlnElmnt& elm, std::string const& name, boost::any const& value );

  void  operator()(  BmlnElmnt const& elm,         Particle& p);
  void  operator()(  BmlnElmnt const& elm,      JetParticle& p); 
  void  operator()(  BmlnElmnt const& elm,    ParticleBunch& b);  
  void  operator()(  BmlnElmnt const& elm, JetParticleBunch& b);  

  bool hasAlignment() const;
  bool hasAperture()  const;

  void  setAlignment( Vector const& translation, Vector const& rotation);
  void   setAperture( BmlnElmnt::aperture_t type, double const& hor, double const& ver );

  boost::tuple<BmlnElmnt::aperture_t, double, double>  aperture()   const; 
  boost::tuple<Vector,Vector>                         alignment()   const; 

 protected:

   PropagatorPtr propagator_;

 private:

   PropagatorDecorator& operator=(  PropagatorDecorator const&); // forbidden 

};

#endif // PROPAGATORDECORATOR_H



