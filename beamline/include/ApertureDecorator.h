/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      ApertureDecorator.h
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

#ifndef APERTUREDECORATOR_H
#define APERTUREDECORATOR_H

#include <beamline/PropagatorDecorator.h>

class ApertureDecorator: public PropagatorDecorator {
 
public:

  ApertureDecorator( PropagatorPtr p );   
  ApertureDecorator( ApertureDecorator const& o );   
 ~ApertureDecorator();

  ApertureDecorator* Clone() const;

  void  operator()(  bmlnElmnt const& elm,         Particle& p);
  void  operator()(  bmlnElmnt const& elm,      JetParticle& p); 

  bool hasAperture() const;
  void setAperture( bmlnElmnt::aperture_t const, double const& hor, double const& ver );

 private:
  
  bool  lost ( double const& x, double const& y) const;

  bmlnElmnt::aperture_t type_;
  double                hor_;
  double                ver_;

};

#endif // APERTUREDECORATOR_H



