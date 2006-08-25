/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
******
******  File:      RandomOrthogonal.cc
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
******  Aug 2006 ostiguy@fnal.gov                                   
******             
****** - RandomOrthogonal class header and implementation now in
******   files distinct from TMatrix header and implementation.
****** 
**************************************************************************
*************************************************************************/

#include <RandomOrthogonal.h>
#include <MathConstants.h>

RandomOrthogonal::RandomOrthogonal( int n )
: _dim(n), _passes(1)
{

  _omitted = new bool*[ _dim ];
  for( int i=0; i< _dim; ++i) {
    _omitted[i] = new bool[ _dim ];
    for(int j=0; j< _dim; ++j) { _omitted[i][j] = false; }
    _omitted[i][i] = true;
  }

  _lowerTheta = new double*[ _dim ];
  for( int i=0; i< _dim; ++i) {
    _lowerTheta[i] = new double[ _dim ];
    for( int j=0; j< _dim; ++j) { _lowerTheta[i][j] = 0.0; }
  }

  _upperTheta = new double*[ _dim ];
  for( int i=0; i <_dim; ++i ) {
    _upperTheta[i] = new double[ _dim ];
    for( int j=0; j< _dim; ++j) { _upperTheta[i][j] = M_TWOPI; }
    _upperTheta[i][i] = 0.0;
  }

  _rangeTheta = new double*[ _dim ];
  for( int i=0; i< _dim; ++i) {
    _rangeTheta[i] = new double[ _dim ];
    for( int j=0; j<_dim; ++j) { _rangeTheta[i][j] = M_TWOPI; }
    _rangeTheta[i][i] = 0.0;
  }
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

