/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      WakeFunctions.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
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
**************************************************************************
*************************************************************************/

#include <cmath>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/WakeFunctions.h>


#if 0 
namespace {

  double A       =  1.025;
  double a       =  0.035;    // Tesla cavity     
  double Z_0     =  377.0;    //
  double s_0     =  3.46e-3;  // 0.84 * 10.54 cm = 
  double s_1     =  1.4e-3;
  double const c =  PH_MKS_c;

  double A_L = ( A * Z_0 * c )/ ( (M_PI*M_PI)*a );
  double A_T =  2.0/(a*a) * A_L * 2;
}
 
#endif

// -------------------------------------------------------------------------------------------
// An approximate analytic form suitable to represent the short range wakefield in an n-cell
// linac cavity is derived in 
// K. Bane " Short Range Dipole Wakefields in accelerating structure for the NLC" LCC-0116 (SLAC)
//----------------------------------------------------------------------------------------------
//
// 
//  w_l(s) =   Z_0 c    u(s) exp (-sqrt(s/s_0)
//           ---------
//            pi a**2
//
//
//  w_t(s) =   4Z_0c s_0    u(s) [ 1- (1 + sqrt(s/s_0))* exp(s/s_0) ]
//             ----------
//              pi * a**4
//
//  where Z_0 = 377 Ohm (vacuum impedance)
//          a = aperture
//
//
//  Below, we assume the somewhat more general forms
// 
// 
//  w_l(s) =   A_L   u(s) exp (-sqrt(s/s_0)
//
//
//  w_t(s) =   A_T   u(s) [ 1- (1 + sqrt(s/s_1))* exp(s/s_1) ]

//           
// -------------------------------------------------------------------------------------------
// The parameter values below were extracted from
// I. Zagorodnov, N. Solyak "Wakefield Effects of New ILC Cavity Shapes"  EPAC 2006
//--------------------------------------------------------------------------------------------

// ILC Cavity high gradient (RE)

namespace RE {
  double A_L = -388.0;
  double A_T = 1300.0;
  double s_0 = 1.90e-3;
  double s_1 = 0.91e-3;
}
 
// ILC Cavity low loss (LL)

namespace LL {   
  double A_L = -459.0;
  double A_T = 1720.0;
  double s_0 = 1.85e-3;
  double s_1 = 0.84e-3;
}
// ILC Cavity TESLA (LL)

namespace TESLA {
  double A_L = -344.0;
  double A_T = 1000.0;
  double s_0 = 1.74e-3;
  double s_1 = 0.92e-3;
}



using ::TESLA::A_L;
using ::TESLA::A_T;
using ::TESLA::s_0;
using ::TESLA::s_1;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double ShortRangeLWakeFunction::operator()( int i, double const& ds, double const& cutoff ) const 
{

  const double s= i*ds;
  return  ( s > cutoff) ? 0.0 : A_L*exp( -sqrt(s/s_0) ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double ShortRangeTWakeFunction::operator()( int i, double const& ds, double const& cutoff ) const
{

  const double s = i*ds;
  
  return  ( s > cutoff) ? 0.0 : A_T * (1.0 - ( 1.0 + sqrt(s/s_1) )*exp( -sqrt(s/s_1) ) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
