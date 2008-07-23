/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      erf.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>

using FNAL::pcerr;
using FNAL::pcout;

/*
** Complex error function.
**
** Author: Leo Michelotti
** Date: July 7, 1995
** 
*/


std::complex<double>  w( std::complex<double> const& z );

//////////////////////////////////////////////////////////////

std::complex<double>  erfSeries( const std::complex<double> & z ) 
{
  static std::complex<double> series        = 1.0;
  static std::complex<double> oldseries     = 0.0;
  static std::complex<double> arg           = 2.0*z*z;
  double                      den           = 1.0;
  std::complex<double>        term          = 1.0;

  while( series != oldseries ) {
    oldseries = series;
    den      += 2.0;
    term     *= ( arg/den );
    series   += term;
  }  

  return z*series;
}

//////////////////////////////////////////////////////////////

std::complex<double>  erf( const std::complex<double> & z ) 
{
  if( ( fabs(imag(z)) > 3.9 ) || ( fabs(real(z)) > 3.0 ) ) {
    std::complex<double>  u( - imag(z), real(z) );
    return ( 1.0 - std::exp(u*u)*w(u) );
  }

  std::complex<double>  series        = 1.0;
  std::complex<double>  oldseries     = 0.0;
  std::complex<double>  arg           = - z*z;
  double                den           = 1.0;
  std::complex<double>  term          = 1.0;
  double                fctr_x        = 0.0;

  while( series != oldseries ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return (2.0/MATH_SQRTPI)*z*series;
}

//////////////////////////////////////////////////////////////

std::complex<double>  erfc( const std::complex<double> & z ) 
{
  static const std::complex<double>  one( 1.0, 0.0 );
  return ( one - erf( z ) );
}

//////////////////////////////////////////////////////////////

std::complex<double>  w( std::complex<double> const& z ) 
{
  static const std::complex<double>  mi( 0., -1. );
  
  double x = real(z);
  double y = imag(z);

  if( y < 0.0 ) return 2.0*std::exp( -z*z ) - w( -z );

  if( x < 0.0 ) return conj( w( std::complex<double> ( - x, y ) ) );

  if( ( x > 6.0 ) || ( y > 6.0 ) ) {

    std::complex<double> z2 = z*z;

    return ( - mi * z * (
                        ( 0.5124242  /( z2 - 0.2752551 )) + 
                        ( 0.05176536 /( z2 - 2.724745  ))
                        ) 
           );
  }

  if( ( x > 3.9 ) || ( y > 3.0 ) ) {
    std::complex<double> z2 = z*z;
    return ( - mi * z * (
                        ( 0.4613135   /( z2 - 0.1901635 )) + 
                        ( 0.09999216  /( z2 - 1.7844927 )) + 
                        ( 0.002883894 /( z2 - 5.5253437 ))
                        ) 
           );
  }

  return std::exp( -z*z )*( 1.0 - erf( mi*z ) );

}

