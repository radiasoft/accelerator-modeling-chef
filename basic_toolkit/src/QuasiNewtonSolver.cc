/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASICTOOLKIT
******                                    
******  File:         QuasiNewtonSolver.cc
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
******
******  A C++ implementation of the classical BFGS quasi-Newton 
******  algorithm.
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/QuasiNewtonSolver.h>
#include <boost/ref.hpp>

using FNAL::pcerr;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QuasiNewtonSolver::QuasiNewtonSolver( boost::function<double  ( Vector const&) > const& f,
                                      boost::function<Vector  ( Vector const&) > const& gradf,  
                                      boost::function<double  ( Vector const&) > const& norm )
  : f_(f), gradf_(gradf), norm_(norm), H_(), linesearch_( boost::cref(f), boost::cref(gradf) ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

QuasiNewtonSolver::~QuasiNewtonSolver()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector QuasiNewtonSolver::operator()( Vector const& initial )
{

  int const max_iter = 100;
 
  Vector x = initial;

  H_ = Matrix::Imatrix( x.Dim()); 
 
  int niter = 0;

  do { 

     Vector gf =  gradf_( x );
     Vector dx =  H_ * gf; 
     Vector xn =  linesearch_(x , dx ); 

    //------------------------------------------
    // update H_ using the BFGS updating formula 
    //------------------------------------------

     dx            = ( xn         - x   );
     Vector dgradf = ( gradf_(xn) - gf  );

     Vector  H_dgf     =  (H_*dgradf);  
     double  dgf_H_dgf =  H_dgf*H_dgf;
     double  dx_dgradf =  dx*dgradf;  

     Vector  u = (dx /dx_dgradf) - H_dgf / dgf_H_dgf; 


     // note: the % operator denotes an outer (cartesian) product

     H_ += (dx%dx)/(dx_dgradf) -  (H_dgf % H_dgf) / dgf_H_dgf  +  dgf_H_dgf * ( u%u );

     // if converged break;  

  } while (  ++niter < max_iter );

  return x;
}

