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
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/VectorD.h>

#include <mxyzptlk/TJet.h>

using namespace MathConstants;
using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


static   const std::complex<double> complex_i (0.0, 1.0);
static   const std::complex<double> complex_1 (1.0, 0.0);
static   const std::complex<double> complex_0 (0.0, 0.0);  

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<double> real(  TJet<complex<double> > const& z ) 
{
  return TJet<double>( real(z.jl_) );    
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> imag( TJet<complex<double> > const& z ) 
{
  return TJet<double>( imag(z.jl_) );    
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> fabs( TJet<double> const& x )
{
 double u;

 if( (x.jl_)->getCount() == 0 ) {
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

TJet<double> erf( TJet<double> const& z ) 
{
  EnvPtr<double> pje = z.Env();

  TJet<double>     series =   TJet<double> (1.0, pje);
  TJet<double>  oldseries =   TJet<double>(0.0, pje);
  TJet<double>        arg = - z*z;
  double              den =   1.0;
  TJet<double>       term =   TJet<double>(1.0, pje);
  double           fctr_x =   0.0;

 int  counter = 0;
  while( ( series != oldseries ) || counter++ < pje->maxWeight() ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return (2.0/Math_SQRTPI)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > erf( TJet<complex<double> > const& z ) 
{
  EnvPtr<complex<double> > pje = z.Env();

  if( ( fabs(imag(z.standardPart())) > 3.9 ) || 
      ( fabs(real(z.standardPart())) > 3.0 ) ) {
    TJet<complex<double> > u( pje );
    u = complex<double> ( 0., 1. )*z;
    u = complex<double> ( 1., 0. ) - exp(u*u)*w(u);
    return u;
  }

  TJet<complex<double> >  series  ( complex_1, pje);
  TJet<complex<double> >  oldseries(pje); 
  TJet<complex<double> >  arg           = - z*z;
  double                  den           = 1.0;
   TJet<complex<double> > term          = complex_1;
  double                  fctr_x        = 0.0;

  int counter = 0;
  while( ( series != oldseries ) || counter++ < pje->maxWeight() ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return ( complex<double>(2.0/Math_SQRTPI,0.0) * z *series );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > w( TJet<complex<double> >const& z ) 
{
  complex<double> const    mi( 0., -1. );
  TJet<complex<double> >  answer( z.Env() );
  
  double x = real( z.standardPart() );
  double y = imag( z.standardPart() );

  if( ( x < 0.0 ) || ( y < 0.0  ) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<complex<double> > w( TJet<complex<double> > const& ) ",
           "Argument must have positive standard part." ) );
  }

  TJet<complex<double> > zsq = z*z;

  if( ( x > 6.0 ) || ( y > 6.0 ) ) {

   return  ( - mi * z * (
                          ( 0.5124242  /( zsq - 0.2752551 )) + 
                          ( 0.05176536 /( zsq - 2.724745  ))
                          ) );
  }

  else if( ( x > 3.9 ) || ( y > 3.0 ) ) {

    return ( - mi * z * (
                          ( 0.4613135   /( zsq - 0.1901635 )) + 
                          ( 0.09999216  /( zsq - 1.7844927 )) + 
                          ( 0.002883894 /( zsq - 5.5253437 ))
                          ) );
  }

  else { 
     return exp( -zsq ) * (  1.0 -erf( mi*z ) );
  } 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||| Mixed Mode arithmetic
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<complex<double> > operator+( TJet<double> const& x, TJet<complex<double> >const& y ) 
{
  return TJet<std::complex<double> >(x) + y;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( TJet<complex<double> >const& x, TJet<double> const& y ) 
{
  return  x + TJet<std::complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( double const&  x, TJet<complex<double> >const& y ) 
{
  return std::complex<double>(x,0.0) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( TJet<complex<double> >const& x, double const& y ) 
{
  return x + complex<double>(y,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( std::complex<double>const& x,  TJet<double>const& y ) 
{
  return ( x + TJet<std::complex<double> >(y) );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( TJet<double> const& x, std::complex<double> const& y) 
{
  return ( TJet<std::complex<double> >(x) + y );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( TJet<double>const& x, TJet<complex<double> >const& y ) 
{
  return TJet<complex<double> >(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( TJet<complex<double> >const& x, TJet<double> const& y ) 
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

TJet<complex<double> > operator-( TJet<complex<double> >const& x, double const& y ) 
{
  return x - complex<double> (y,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( std::complex<double> const& x, TJet<double> const& y ) 
{
  return x - TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( TJet<double> const& x,   std::complex<double> const& y)
{
  return TJet<complex<double> >(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( TJet<double> const& x, TJet<complex<double> >const& y ) 
{
  return TJet<complex<double> >(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( TJet<complex<double> >const& x, TJet<double> const& y ) 
{
  return x*TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( complex<double> const& y, TJet<double> const& x ) 
{  

  
  TJet<complex<double> > z( x);

  return y*TJet<complex<double> >( x );


}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( TJet<double> const& x, complex<double> const& y ) 
{  
  return TJet<complex<double> >(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( double const& y, TJet<complex<double> > const& x ) 
{  
  return (complex<double>(y) * x);
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( TJet<complex<double> >const& x, double const& y ) 
{  
  return ( x * complex<double>(y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( TJet<double> const& x, TJet<complex<double> >const& y ) 
{

  return TJet<complex<double> >(x)/y;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( TJet<complex<double> >const& x, TJet<double> const& y ) 
{
  return x/TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( complex<double> const& x, TJet<double> const& y ) 
{
  return x/TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( TJet<double> const& y, complex<double> const& x ) 
{
  return TJet<complex<double> >(y)/x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( TJet<complex<double> >const& x, double const& y)
{
  return x/complex<double> (y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( double const& x, TJet<complex<double> >const& y)
{
  return complex<double> (x)/y;
}



