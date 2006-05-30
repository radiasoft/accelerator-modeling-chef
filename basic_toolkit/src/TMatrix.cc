/*************************************************************************
**************************************************************************
**************************************************************************
******
******  Basic TOOLKIT:  Low level utility C++ classes.
******
******  File:      TMatrix.cc
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
******                                                                
******  Author:    Leo Michelotti                                     
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******             Email: michelotti@fnal.gov                         
****** 
******  Revision (Sep 2005):
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov                                   
******             
******   - reorganized code to support explicit template instantiations
******   - eliminated separate MatrixC class implementation
****** 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <TML.h>
#include <MLPtr.h>
#include <TMatrix.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <MathConstants.h>
#include <PhysicsConstants.h>
#include <iosetup.h>
#include <utils.h>


#ifdef WIN32
#include <Distribution.h> // for drand48
#endif


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
TMatrix<std::complex<double> >::TMatrix( TMatrix<double> const& m){ 

  _ml = m._ml; // implicit conversion

  return; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<double> TMatrix<double>::dagger() const {

//  for a real matrix, the Hermitian conjugate and the transpose
//  are the same

  
  MLPtr<double> p ( _ml->transpose() );
  return TMatrix<double>( p );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<std::complex<double> >::dagger() const
{

  MLPtr<std::complex<double> > p ( _ml->dagger() );
  return TMatrix<std::complex<double> >( p );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<double> real( const TMatrix<std::complex<double> >& x )
{


  MLPtr<double> p( real_part(x._ml ) ); 
  return TMatrix<double>( p ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<double> imag( const TMatrix<std::complex<double> >& x ) 
{

  MLPtr<double> p( imag_part(x._ml ) ); 
  return TMatrix<double>( p ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<double>::eigenValues() const
{
 
  MLPtr<std::complex<double> > p( _ml->eigenValues() ); 
  return  TMatrix<std::complex<double> >( p ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<std::complex<double> >::eigenValues() const 
{

  MLPtr<std::complex<double> > p( _ml->eigenValues() ); 
  return  TMatrix<std::complex<double> >( p ); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<double>::eigenVectors() const
{

  MLPtr<std::complex<double> > p( _ml->eigenVectors() ); 
  return  TMatrix<std::complex<double> >(p); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<std::complex<double> >::eigenVectors() const
{

  MLPtr<std::complex<double> > p( _ml->eigenVectors() ); 
  return  TMatrix<std::complex<double> >(p); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void TMatrix<double>::SVD( TMatrix<double>& U, TMatrix<double>& V, TMatrix<double>& W) 
{
  _ml->SVD( U._ml, V._ml, W._ml);

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TMatrix<std::complex<double>  > operator*(const TMatrix<std::complex<double>  >& x, const TMatrix<double>& y)
{
 
  TMatrix<std::complex<double> > z = y; //implicit conversion 

  MLPtr<std::complex<double>  > p(  multiply<std::complex<double> >(x._ml, z._ml) ); 
  return TMatrix<std::complex<double>  >(p); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<std::complex<double> > operator*(const TMatrix<double>& x, const TMatrix<std::complex<double> >& y)
{

  TMatrix<std::complex<double> > z = x; //implicit conversion 

  MLPtr<std::complex<double>  > p(   multiply<std::complex<double> >(z._ml, y._ml) ); 
  return TMatrix<std::complex<double>  >( p ); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


RandomOrthogonal::~RandomOrthogonal()
{
  for( int i = 0; i < _dim; i++ ) {
    delete [] _omitted[i];
    delete [] _lowerTheta[i];
    delete [] _upperTheta[i];
    delete [] _rangeTheta[i];
  }

  delete [] _omitted;
  delete [] _lowerTheta;
  delete [] _upperTheta;
  delete [] _rangeTheta;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::omitIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) {
    _omitted[i][j] = true;
    _omitted[j][i] = true;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::omitIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) {
    this->omitIndex( n, i );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::includeIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) {
    _omitted[i][j] = false;
    _omitted[j][i] = false;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::includeIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) {
    this->includeIndex( n, i );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::setNumberOfPasses( int n )
{
  if( 0 < n ) { _passes = n; }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::setRange( int i, int j, double lo, double hi )
{
  // Check and condition the arguments
  if( i < 0  ||  _dim <= i  ||
      j < 0  ||  _dim <= j  ||
      hi <= lo )
  { return; }

  if( std::abs(lo) < 1.0e-12            ) { lo = 0.0;     }
  if( std::abs(hi - M_TWOPI ) < 1.0e-12 ) { hi = M_TWOPI; }

  while( lo < 0.0      ) { lo += M_TWOPI; }
  while( lo > M_TWOPI  ) { lo -= M_TWOPI; }
  while( hi < 0.0      ) { hi += M_TWOPI; }
  while( hi > M_TWOPI  ) { hi -= M_TWOPI; }

  // Set the range
  _lowerTheta[i][j] = lo;
  _lowerTheta[j][i] = lo;
  _upperTheta[i][j] = hi;
  _upperTheta[j][i] = hi;
  _rangeTheta[i][j] = hi - lo;
  _rangeTheta[j][i] = hi - lo;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::setRange( int n, double lo, double hi )
{
  for( int i = 0; i < _dim; i++ ) {
    this->setRange( n, i, lo, hi );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<double> RandomOrthogonal::build()
{
  // This was programmed very inefficiently
  // but quickly. It will have to be redone
  // if used many times in an application.

  int i, j, p;
  double theta, cs, sn, tmp;
  TMatrix<double> R( "I", _dim );
  TMatrix<double> U( "I", _dim );
  TMatrix<double> W( "I", _dim );

  for( p = 0; p < _passes; p++ ) {
    for( i = 0; i < _dim-1; i++ ) {
      for( j = i+1; j < _dim; j++ ) {
        if( !(_omitted[i][j]) ) {
          theta = _lowerTheta[i][j] + drand48()*_rangeTheta[i][j];
          cs = cos( theta );
          sn = sin( theta );
          W(i,i) =  cs;  W(i,j) = sn;
          W(j,i) = -sn;  W(j,j) = cs;
          R = R*W;
          W = U;
        }
      }
    }
  }

  return R;
}
