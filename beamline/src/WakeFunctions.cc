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
 
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double ShortRangeLWakeFunction::operator()( double const& s) const 
{

  return  A_L*exp( -sqrt(s/s_0) ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double ShortRangeTWakeFunction::operator()( double const& s) const
{

  return  A_T * s_1 * (1.0 - ( 1.0 + sqrt(s/s_1) ) *exp( -sqrt(s/s_1) ) );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
