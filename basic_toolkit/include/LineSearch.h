/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT
******                                    
******  File:      LineSearch.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef LINESEARCH_H
#define LINESEARCH_H

#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>

class LineSearch: boost::noncopyable {

 public:

  LineSearch( boost::function<double( Vector const&) > const& f,
              boost::function<Vector( Vector const&) > const& gradf );


 ~LineSearch();

  Vector operator()( Vector const& initial, Vector const& direction );
 
 private:

  bool  armijo_rule()              const;
  bool  curvature_condition()      const;

  static double const c1_; 
  static double const c2_;

  boost::function<double( Vector const& ) >  f_;               // a function object to compute the objective function f    
  boost::function<Vector( Vector const& ) >  gradf_;           // a function object to compute the gradient of f    

  double fxp_;          // the value of f      at the previous step :     f( xp ); 
  Vector gradfxp_;      // the value of grad f at the previous step : gradf( xp ); 
  double fx_;           // the value of f      for the current step :     f( x  ); 
  Vector gradfx_;       // the value of f      for the current step : gradf( x  ); 

  Vector dx_;           


}; 

#endif //  LINESEARCH_H

