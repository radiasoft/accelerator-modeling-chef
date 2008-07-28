/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT
******                                    
******  File:      NewtonSolver.h
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

#ifndef NEWTONSOLVER_H
#define NEWTONSOLVER_H

#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>

class NewtonSolver: boost::noncopyable {

 public:

  NewtonSolver( boost::function<Vector( Vector const&) > const& map,  
                boost::function<double( Vector const&) > const& norm,
		boost::function<Matrix( Vector const&) > const& jacobian ); 

 ~NewtonSolver();

  Vector operator()( Vector const& initial );
 
 private:

  boost::function<Vector( Vector const& ) >  map_;           // map(x_0) = 0  is the equation to solve    
  boost::function<double( Vector const& ) >  norm_;          // a function object to compute the vector norm  
  boost::function<Matrix( Vector const& ) >  invjacobian_;   // a function object to compute the _inverse_ jacobian  

}; 

#endif //  NewtonSolver 

