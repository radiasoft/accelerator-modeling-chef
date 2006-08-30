/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetConversions.cc
******                                                                
******  Copyright Universities Research Association, Inc. / Fermilab   
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
*****   and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Email: michelotti@fnal.gov                         
******
******  Revision History:
******
******  Feb-May 2005  Jean-Francois Ostiguy
******                ostiguy@fnal.gov
******
******  - Efficiency improvements.
******  - New memory management scheme.
******
******  Sept-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/complexAddon.h>

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/VectorD.h>

#include <mxyzptlk/TJet.h>

#ifdef CHECKOUT
#undef CHECKOUT 
#endif
#define CHECKOUT(test,fcn,message)    \
  if( test ) {                        \
    throw( GenericException(          \
             __FILE__, __LINE__,      \
             fcn, message        ) ); \
  }

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<double> real( const TJet<complex<double> >& z ) 
{

  return TJet<double>( real(z._jl) );    

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> imag( const TJet<complex<double> >& z ) 
{

  return TJet<double>( imag(z._jl) );    

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> fabs( const TJet<double>& x )
{
 double u;

 if( (x._jl)->getCount() == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "Jet fabs( const Jet& ) { ",
          "Argument is zero." ) );
 }
 
 if( (u = x.standardPart()) != 0.0 ) 
 {

   return std::abs(u);

 }
 else
 {
   throw( GenericException( __FILE__, __LINE__, 
          "Jet fabs( const Jet& ) ",
          "Argument is nilpotent." ) );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> erf( const TJet<double>& z ) 
{
  EnvPtr<double> pje = z.Env();

  TJet<double> series    ( pje );
  TJet<double> oldseries ( pje );
  TJet<double> arg       ( pje );
  TJet<double> term      ( pje );

  static double  den;
  static double  fctr_x;
  static int     counter;

  series        = 1.0;
  oldseries     = 0.0;
  arg           = - z*z;
  den           = 1.0;
  term          = 1.0;
  fctr_x        = 0.0;

  counter = 0;
  while( ( series != oldseries ) || counter++ < pje->maxWeight() ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return (2.0/MATH_SQRTPI)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > erf( const TJet<complex<double> >& z ) 
{
  EnvPtr<complex<double> > pje = z.Env();

  if( ( fabs(imag(z.standardPart())) > 3.9 ) || 
      ( fabs(real(z.standardPart())) > 3.0 ) ) {
    TJet<complex<double> > u( pje );
    u = complex<double> ( 0., 1. )*z;
    u = complex<double> ( 1., 0. ) - exp(u*u)*w(u);
    return u;
  }

  TJet<complex<double> >    series    ( pje );
  TJet<complex<double> >    oldseries ( pje );
  TJet<complex<double> >    arg       ( pje );
  TJet<complex<double> >    term      ( pje );

  static double  den;
  static double  fctr_x;
  static int     counter;

  series        = complex_1;
  oldseries     = complex_0;  // ??? Why necessary?
   arg           = - z*z;
  den           = 1.0;
  term          = complex_1;
  fctr_x        = 0.0;

  counter = 0;
  while( ( series != oldseries ) || counter++ < pje->maxWeight() ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return complex<double> (2.0/MATH_SQRTPI,0.0)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > w( const TJet<complex<double> >& z ) 
{
  const complex<double>  mi( 0., -1. );
  double x;
  double y;
  TJet<complex<double> >  answer( z.Env() );
  
  x = real( z.standardPart() );
  y = imag( z.standardPart() );

  if( ( x < 0.0 ) || ( y < 0.0  ) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<complex<double> > w( const TJet<complex<double> >& ) ",
           "Argument must have positive standard part." ) );
  }

  if( ( x > 6.0 ) || ( y > 6.0 ) ) 
    answer = ( - mi * z * (
                          ( 0.5124242  /( z*z - 0.2752551 )) + 
                          ( 0.05176536 /( z*z - 2.724745  ))
                          ) 
             );

  else if( ( x > 3.9 ) || ( y > 3.0 ) ) 
    answer = ( - mi * z * (
                          ( 0.4613135   /( z*z - 0.1901635 )) + 
                          ( 0.09999216  /( z*z - 1.7844927 )) + 
                          ( 0.002883894 /( z*z - 5.5253437 ))
                          ) 
             );

  else answer = exp( -z*z )*(  1.0 -erf( mi*z ) );

  return answer;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||| Mixed Mode arithmetic
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<complex<double> > operator+( const TJet<double>& x, const TJet<complex<double> >& y ) 
{
  return TJet<std::complex<double> >(x) + y;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return  x + TJet<std::complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( const double& x, const TJet<complex<double> >& y ) 
{
  return std::complex<double>(x,0.0) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( const TJet<complex<double> >& x, const double& y ) 
{
  return x + complex<double>(y,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+(const std::complex<double>& x,  const TJet<double>& y ) 
{
  return ( x + TJet<std::complex<double> >(y) );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+(  const TJet<double>& x, const std::complex<double>& y) 
{
  return ( TJet<std::complex<double> >(x) + y );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<double>& x, const TJet<complex<double> >& y ) 
{
  return TJet<complex<double> >(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return x - TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const double& x, const TJet<complex<double> >& y ) 
{
  return complex<double>(x,0.0) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<complex<double> >& x, const double& y ) 
{
  return x - complex<double> (y,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const std::complex<double>& x, const TJet<double>& y ) 
{
  return x - TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<double>& x,   const std::complex<double>& y)
{
  return TJet<complex<double> >(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<double>& x, const TJet<complex<double> >& y ) 
{
  return TJet<complex<double> >(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return x*TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const complex<double>& y, const TJet<double>& x ) 
{  

  
  TJet<complex<double> > z( x);

  return y*TJet<complex<double> >( x );


}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<double>& x, const complex<double> & y ) 
{  
  return TJet<complex<double> >(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const double& y, const TJet<complex<double> >& x ) 
{  
  return (complex<double>(y) * x);
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<complex<double> >& x, const double& y ) 
{  
  return ( x * complex<double>(y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const TJet<double>& x, const TJet<complex<double> >& y ) 
{

  return TJet<complex<double> >(x)/y;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return x/TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const complex<double> & x, const TJet<double>& y ) 
{
  return x/TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const TJet<double>& y, const complex<double> & x ) 
{
  return TJet<complex<double> >(y)/x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/(const TJet<complex<double> >& x, const double& y)
{
  return x/complex<double> (y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/(const double& x, const TJet<complex<double> >& y)
{
  return complex<double> (x)/y;
}



