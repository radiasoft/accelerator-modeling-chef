/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC_TOOLKIT
******                                    
******  File:      LineSearch.cc
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

#include <basic_toolkit/LineSearch.h>
#include <algorithm>

double const LineSearch::c1_  = 0.9; 
double const LineSearch::c2_  = 0.9; 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LineSearch::LineSearch( boost::function<double( Vector const&) > const& f,
                        boost::function<Vector( Vector const&) > const& gradf )

: f_(f), gradf_(gradf)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LineSearch::~LineSearch()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  LineSearch::armijo_rule() const
{
    
  //-------------------------------------------------------------
  // f( x_k + alpha_k p_k ) <= f(x_k) + c_1 alpha_k p_k^T grad f( x_k )
  //-------------------------------------------------------------

  return  ( fx_ <= fxp_ + c1_ * ( dx_ * gradfx_) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  LineSearch::curvature_condition() const
{
  //-------------------------------------------------------
  // p_k^T grad f( x_k +\alpha_k p_k) >= c_2 p_k^T  grad f( x_k )  
  //--------------------------------------------------------


  return  ( std::abs( (dx_*gradfx_) )  >= c2_* std::abs( dx_*gradfxp_ ) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector LineSearch::operator()( Vector const& x0, Vector const& dx0 )
{
  //------------------------------------------------------------------- 
  // backtracking linesearch algorithm
  // dx0:  initial correction, 
  //       typically computed by Quasi-Newton algorithm   
  //-------------------------------------------------------------------

  int const istep_max = 100;    
  double const tau = 0.75;

  Vector dx   = dx0;
  Vector x    = x0;

  fxp_     =     f_(x0); 
  gradfxp_ = gradf_(x0); 
 
  int istep = 0;

   while ( istep < istep_max ) {

    x   = x0 + dx;    

    fx_      =     f_(x); 
    gradfx_  = gradf_(x); 
 
   if ( armijo_rule() && curvature_condition())  break;

    dx *= tau; 
 
 }

  
  return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
