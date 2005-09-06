/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.1
******                                    
******  File:      Distribution.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


/*
 * Distribtion class written by Jim Holt.
 * August 11, 1994
 *
 * Modified by Leo Michelotti
 * December 4, 2003
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <iostream>

#include "Distribution.h"
#include "VectorD.h"

#ifdef WIN32

/* srand48 and drand48 are not available under Win32 */
/* The stubs below allow the code to compile         */
long    srand48(long int) { return 1;  }
double  drand48()         { return 0.5;}

#endif

using namespace std;

Gaussian::Gaussian( double m, double s, long seed ) 
: Distribution( seed ), 
  _mean( m ),
  _sigma( s )
{
}


Gaussian::~Gaussian() 
{
}


void Gaussian::setSigma( double s )
{
  if( s >= 0.0 ) { _sigma = s; }
}


double Gaussian::getValue() const
{
  double w, r, random1, random2;
  do {
    random1 = 2.0*drand48() - 1.0;  // ??? Why was it done this way???
    random2 = 2.0*drand48() - 1.0;
    r = random1*random1 + random2*random2;
  } while (r >= 1.0);

  double factor = sqrt(-2.0*log(r)/r);
  w = random2 * factor;

  return _mean + _sigma*w;
}


MultiGaussian::MultiGaussian( const Vector& average, 
                              const Vector& deviation,
                              long seed )
: Distribution( seed ), 
  _mean( average ),
  _sigma( deviation ),
  _covariance( average.Dim(), average.Dim() ),
  _R( average.Dim(), average.Dim() )
{
  int i;
  int n = average.Dim();
  for( i = 0; i < n; i++ ) { _covariance(i,i) = deviation(i)*deviation(i); }
  RandomOrthogonal generator( n );
  _R = generator.build();
  _covariance = _R * _covariance * _R.transpose();
}


MultiGaussian::~MultiGaussian()
{
}


void MultiGaussian::setSigma( const Vector& s )
{
  if( s.Dim() != _sigma.Dim() ) { 
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void MultiGaussian::setSigma( const Vector& s )"
            "\n*** WARNING *** Incompatible dimensions: returning."
         << endl;
    return; 
  }
 
  for( int i = 0; i < s.Dim(); i++ ) {
    if( s(i) >= 0.0 ) { _sigma(i) = s(i); }
  }
}


void MultiGaussian::setRotation( const MatrixD& M )
{
  if( !(M.isOrthogonal()) ) { 
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void void MultiGaussian::setRotation( const MatrixD& M )"
            "\n*** WARNING *** Argument is not an orthogonal matrix. Returning."
         << endl;
    return; 
  }
  
  if( _R.rows() != M.rows() ) {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void void MultiGaussian::setRotation( const MatrixD& M )"
            "\n*** WARNING *** Argument is wrong size. Returning."
         << endl;
    return; 
  }

  _R = M;
}


Vector MultiGaussian::getValue() const
{
  int n = _mean.Dim();
  Vector ret( n );

  double w, r, random1, random2;
  double factor;
  for( int i = 0; i < n; i++ ) {
    do {
      random1 = 2.0*drand48() - 1.0;  // ??? Why was it done this way???
      random2 = 2.0*drand48() - 1.0;
      r = random1*random1 + random2*random2;
    } while (r >= 1.0);
    factor = sqrt(-2.0*log(r)/r);
    w = random2 * factor;
    ret(i) = w*_sigma(i);
  }

  ret = _mean + _R*ret;
  return ret;
}
