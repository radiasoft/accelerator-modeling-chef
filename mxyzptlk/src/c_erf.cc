#include "c_erf.h"
#include "MathConstants.h"

Complex erf( const Complex& z )
{
 static Complex sum, oldsum, c, arg;
 static double  n, b;
 
 arg    = - z*z;
 n   	= 0.0;
 b   	= 1.0;
 c   	= 1.0; 
 sum 	= 1.0;
 oldsum = 0.0;
 
 while( oldsum != sum ) {
  oldsum = sum;
  n += 1.0;
  b += 2.0;
  c *= arg/n;
  sum += c/b;
 }

 return ( 2.0*z / MATH_SQRTPI ) * sum;
}

Complex erfc( const Complex& z )
{
 return 1.0 - erf(z);
}


void w_sumit( Complex& sum, 
              Complex& b, 
              Complex& c, 
              const Complex& arg )
{
 static Complex oldsum;
 oldsum = sum + 1.0;
 while( oldsum != sum ) {
  oldsum = sum;
  c *= arg/b;
  b += 1.0;
  sum += c;
 }
}


Complex w( const Complex& z )
{
 static const Complex i( 0., 1. );
 static Complex evensum, oddsum;
 static Complex b, c, arg;

 arg = - z*z;

 b       = 1.0;
 c       = 1.0;
 evensum = 1.0;
 w_sumit( evensum, b, c, arg );

 b       = 1.5;
 c       = i*z/(MATH_SQRTPI / 2.0);
 oddsum  = c;
 w_sumit( oddsum, b, c, arg );

 return evensum + oddsum;
}


Complex erfc_asymp( const Complex& z )
{
 static Complex sum, arg, oldsum, term;
 static double num;

 num = 1.0;
 arg = 2.0*z*z;
 term = - num / arg;
 sum  = term;
 oldsum = 0.0;
 
 while( oldsum != sum ) {   // ??? This condition will never
  oldsum = sum;             // ??? be met.  Choose a finite
  num += 2.0;               // ??? number of terms.
  term = -term*( num/arg );
  sum += term;
 }

 return ( exp(-z*z) / ( MATH_SQRTPI*z ) )*( 1.0 + sum );
}
