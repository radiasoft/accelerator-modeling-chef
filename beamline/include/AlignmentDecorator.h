/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      AlignmentPropagatorDecorator.h
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

#ifndef ALIGNMENTDECORATOR_H
#define ALIGNMENTDECORATOR_H

#include <beamline/PropagatorDecorator.h>
#include <basic_toolkit/Matrix.h>

class AlignmentDecorator: public PropagatorDecorator {
 
public:

  AlignmentDecorator( PropagatorPtr p );   
  AlignmentDecorator( AlignmentDecorator const& o );   
 ~AlignmentDecorator();

  AlignmentDecorator* clone() const;

  void  operator()(  BmlnElmnt const& elm,         Particle& p);
  void  operator()(  BmlnElmnt const& elm,      JetParticle& p); 

  bool hasAlignment() const; 
  void setAlignment( Vector const& offset, Vector const& angles, bool relative=false);

 private:
  
  Matrix rotation( double const& roll, double const& yaw,  double const& pitch ); 

  Vector offsets_;
  Vector angles_;
  
  Matrix rotation_;
};

#endif // ALIGNMENTDECORATOR_H



