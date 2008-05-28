/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BBLens.tcc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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
******  Authors:    Leo Michelotti         michelotti@fnal.gov                                    
******              Jean-Francois Ostiguy     ostiguy@fnal.gov
******
******  This is a streamlined, templated version of the function 
******  NormalizedEField authored by LM.
****** 
**************************************************************************
*************************************************************************/

#include <mxyzptlk/Jet.h>
#include <mxyzptlk/JetVector.h>
#include <basic_toolkit/utils.h>
#include <basic_toolkit/GenericException.h>

extern JetC    w( const JetC& );

template <>  
double BBLens::toDouble( double const& value) { return value;                }

template <>
double BBLens::toDouble ( Jet const& value)   { return value.standardPart(); } 

template <typename T >
typename EVector_t<T>::Type BBLens::NormalizedEField( T const& arg_x, T const& arg_y ) const
{

  double const SIGMA_LIMIT = 64.0;
  double const SIGMA_ROUND = 0.1;

  std::complex<double> const complex_i(0.0, 1.0);

  typedef  typename EVector_t<T>::Type::value_type      Component_t; 

  typedef  typename EVector_t<T>::ComplexComponentType  ComplexComponent_t; 
           
  typename EVector_t<T>::Type        retvec(3);

  T      x = arg_x;  
  T      y = arg_y;
  double sigmaX = sigmas_[0];
  double sigmaY = sigmas_[1];

  // Asymptotic limit ...

  if( ( sigmaX == 0.0 ) && ( sigmaY == 0.0 ) ) {

    double r = toDouble(x)*toDouble(x) + toDouble(y)*toDouble(y);
    if( r < 1.0e-20 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "Vector BBLens::NormalizedEField( double arg_x, double arg_y )", 
           "Asymptotic limit r seems too small." ) );
    }

    retvec[0] = x/r;
    retvec[1] = y/r;
    retvec[2] = 0.0;
    return retvec;
  }

  // Round beam limit ...

  if( useRound ) {
    if( ( std::abs( ( sigmaX - sigmaY ) / ( sigmaX + sigmaY ) ) < SIGMA_ROUND ) ||
        ( ( pow( toDouble(x)/sigmaX, 2.0 ) + pow( toDouble(y)/sigmaY, 2.0 ) ) > SIGMA_LIMIT )
      ) {

      double         r = toDouble(x)*toDouble(x) + toDouble(y)*toDouble(y);
      double meanSigma = 2.0*sigmaX*sigmaY;

      // Test for small r .....

      if( r > 1.0e-6*meanSigma ) {
  	r = ( 1.0 - exp(-r/ meanSigma ) ) / r;

  	retvec[0] = x*r;
  	retvec[1] = y*r;
  	retvec[2] = 0.0;
  	return retvec;
      }
 
      else {
  	retvec[0] = x/meanSigma;
  	retvec[1] = y/meanSigma;
  	retvec[2] = 0.0;
  	return retvec;
      }
    } 
  }


  // Elliptical beam ... 

  bool normal = ( sigmaX > sigmaY );

  if (!normal) { std::swap (x,y);  std::swap ( sigmaX, sigmaY); }

  double sgnx = 1.0; 
  double sgny = 1.0; 

  if( toDouble(arg_x) < 0.0 ) x = -x; sgnx = -sgnx; 
  if( toDouble(arg_y) < 0.0 ) y = -y; sgny = -sgny; 


  // The calculation ...

  double ds   = sqrt(2.0*(sigmaX*sigmaX - sigmaY*sigmaY));

  ComplexComponent_t arg1 =  ( x + complex_i* y) / ds;  

  double             r    = sigmaY/sigmaX;
  ComplexComponent_t arg2 = ( x + complex_i* y )* r/ds;

  ComplexComponent_t  retarg1 = w( arg1 ); //  ERFCMPLX( 2                   ( x + i y ) / (sigma_x**2 - sigma_y**2) )
  ComplexComponent_t  retarg2 = w( arg2 ); //  ERFCMPLX( 2 (sigma_y/sigma_x) ( x + i y ) / (sigma_x**2 - sigma_y**2) )

  // Normalization ...

  ComplexComponent_t  r0  = x/sigmaX;
  r0    = r0*r0;

  Component_t tmp1 = y/sigmaY;

  r0   += tmp1*tmp1;   // r = (x/sigmaX)**2 + (y/sigmaY)**2;

  ComplexComponent_t z    = retarg1;

  z   -= retarg2 * exp( - r0/2.0 );
  z   = z * ( - complex_i * MATH_SQRTPI / ds );

  // And return ...
  
  retvec[0] =   sgnx * real(z); 
  retvec[1] =  -sgny * imag(z); 
  retvec[2] =  0.0;

  if (!normal) { std::swap (retvec[0],retvec[1]); }  

  return retvec;

}

