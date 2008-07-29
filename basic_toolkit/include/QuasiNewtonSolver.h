/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT
******                                    
******  File:      QuasiNewtonSolver.h
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
#ifndef QUASINEWTONSOLVER_H
#define QUASINEWTONSOLVER_H

#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/LineSearch.h>

class QuasiNewtonSolver: boost::noncopyable {

 public:

  QuasiNewtonSolver( boost::function<double( Vector const&) > const& f,  
                     boost::function<Vector( Vector const&) > const& gradient,  
                     boost::function<double( Vector const&) > const& norm );

 ~QuasiNewtonSolver();

  Vector operator()( Vector const& initial );
 
 private:

  bool converged() const;  
  
  boost::function<double( Vector const& ) >  f_;             // objective function    
  boost::function<Vector( Vector const& ) >  gradf_;            
  boost::function<double( Vector const& ) >  norm_;          // a function object to compute the vector norm  
  Matrix                                     H_;             // inverse Hessian approximate  
  LineSearch                                 linesearch_;     

}; 

#endif //  QUASINEWTONSOLVER_H

