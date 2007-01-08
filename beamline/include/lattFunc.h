/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      beamline.h
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
******                All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                 
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#ifndef LATTFUNC_H
#define LATTFUNC_H


struct lattRing  {
  struct {
    double hor;
    double ver;
  } tune;
  struct {
    double hor;
    double ver;
  } chromaticity;

} ;


std::ostream& operator<<(std::ostream&, const lattRing&);


struct lattFunc {

  double arcLength;
  struct {
    double hor;
    double ver;
  } dispersion;
  struct {
    double hor;
    double ver;
  } dPrime;
  struct {
    double hor;
    double ver;
  } beta;
  struct {
    double hor;
    double ver;
  } alpha;
  struct {
    double hor;
    double ver;
  } psi;

} ;

std::ostream& operator<<(std::ostream&, const lattFunc&);
std::istream& operator>>(std::istream&, lattFunc&);

#endif // LATTFUNC_H
