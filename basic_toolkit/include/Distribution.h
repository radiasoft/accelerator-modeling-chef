/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.1
******                                    
******  File:      Distribution.h
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Authors:   Jim Holt
******             Leo Michelotti
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

#ifndef DISTRIBUTION_HXX
#define DISTRIBUTION_HXX

#include <cstdlib>

#include "VectorD.h"
#include "TMatrix.h"

class Distribution 
{
 public:
  Distribution( long seed = 0 ) 
  : _initialSeed( seed ) { srand48(seed); }
  virtual ~Distribution() { }

  virtual double getValue() { return drand48();}

 protected:
  long   _initialSeed;
};


class Gaussian : public Distribution 
{
 public:
  Gaussian( double mean = 0.0, double sigma = 1.0, long seed = 0 );
  ~Gaussian();

  void setMean( double m ) { _mean = m; }
  double mean() const { return _mean; }
  void setSigma( double );
  double sigma() const { return _sigma; }

  double getValue() const;

 private:
  double _mean;
  double _sigma;
};


class MultiGaussian : public Distribution 
{
 public:
  MultiGaussian( const Vector& average, const Vector& sigma, long seed = 0 );
  ~MultiGaussian();

  void setMean( const Vector& m ) { _mean = m; }
  Vector mean() const { return _mean; }
  void setSigma( const Vector& );
  Vector sigma() const { return _sigma; }
  MatrixD covariance() const { return _covariance; }
  void setRotation( const MatrixD& M );

  Vector getValue() const;

 private:
  Vector   _mean;
  Vector   _sigma;
  MatrixD  _covariance;
  MatrixD  _R;   // _R.transpose() * _covariance * _R
                 // is diagonal.
};

#endif  // DISTRIBUTION_HXX
