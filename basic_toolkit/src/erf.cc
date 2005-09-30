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

#include <iosetup.h>
#include <complex>
#include <MathConstants.h>

using FNAL::pcerr;
using FNAL::pcout;

/*
** Complex error function.
**
** Author: Leo Michelotti
** Date: July 7, 1995
** 
*/


std::complex<double> > w( std::complex<double> > );

//////////////////////////////////////////////////////////////

std::complex<double> > erfSeries( const std::complex<double> >& z ) 
{
  static std::complex<double> > series;
  static std::complex<double> > oldseries;
  static std::complex<double> > arg;
  static double  den;
  static std::complex<double> > term;

  series        = 1.0;
  oldseries     = 0.0;
  arg           = 2.0*z*z;
  den           = 1.0;
  term          = 1.0;

  while( series != oldseries ) {
    oldseries = series;
    den      += 2.0;
    term     *= ( arg/den );
    series   += term;
  }  

  return z*series;
}

//////////////////////////////////////////////////////////////

std::complex<double> > erf( const std::complex<double> >& z ) 
{
  if( ( fabs(imag(z)) > 3.9 ) || ( fabs(real(z)) > 3.0 ) ) {
    std::complex<double> > u( - imag(z), real(z) );
    return ( 1.0 - std::exp(u*u)*w(u) );
  }

  static std::complex<double> > series;
  static std::complex<double> > oldseries;
  static std::complex<double> > arg;
  static std::complex<double> > term;
  static double  den;
  static double  fctr_x;

  series        = 1.0;
  oldseries     = 0.0;
  arg           = - z*z;
  den           = 1.0;
  term          = 1.0;
  fctr_x        = 0.0;

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

std::complex<double> > erfc( const std::complex<double> >& z ) 
{
  static const std::complex<double> > one( 1.0, 0.0 );
  return ( one - erf( z ) );
}

//////////////////////////////////////////////////////////////

std::complex<double> > w( std::complex<double> > z ) 
{
  static const std::complex<double> > mi( 0., -1. );
  static double x;
  static double y;
  
  x = real(z);
  y = imag(z);

  if( y < 0.0 )
    return 2.0*std::exp( -z*z ) - w( -z );

  if( x < 0.0 ) 
    return conj( w( std::complex<double> >( - x, y ) ) );

  if( ( x > 6.0 ) || ( y > 6.0 ) ) 
    return ( - mi * z * (
                        ( 0.5124242  /( z*z - 0.2752551 )) + 
                        ( 0.05176536 /( z*z - 2.724745  ))
                        ) 
           );

  if( ( x > 3.9 ) || ( y > 3.0 ) ) 
    return ( - mi * z * (
                        ( 0.4613135   /( z*z - 0.1901635 )) + 
                        ( 0.09999216  /( z*z - 1.7844927 )) + 
                        ( 0.002883894 /( z*z - 5.5253437 ))
                        ) 
           );

  return std::exp( -z*z )*( 1.0 - erf( mi*z ) );

}

