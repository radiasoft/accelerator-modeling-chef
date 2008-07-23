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

#include <basic_toolkit/RandomOrthogonal.h>
#include <basic_toolkit/MathConstants.h>

RandomOrthogonal::RandomOrthogonal( int n )
: dim_(n), passes_(1)
{

  omitted_ = new bool*[ dim_ ];
  for( int i=0; i< dim_; ++i) {
    omitted_[i] = new bool[ dim_ ];
    for(int j=0; j< dim_; ++j) { omitted_[i][j] = false; }
    omitted_[i][i] = true;
  }

  lowerTheta_ = new double*[ dim_ ];
  for( int i=0; i< dim_; ++i) {
    lowerTheta_[i] = new double[ dim_ ];
    for( int j=0; j< dim_; ++j) { lowerTheta_[i][j] = 0.0; }
  }

  upperTheta_ = new double*[ dim_ ];
  for( int i=0; i <dim_; ++i ) {
    upperTheta_[i] = new double[ dim_ ];
    for( int j=0; j< dim_; ++j) { upperTheta_[i][j] = M_TWOPI; }
    upperTheta_[i][i] = 0.0;
  }

  rangeTheta_ = new double*[ dim_ ];
  for( int i=0; i< dim_; ++i) {
    rangeTheta_[i] = new double[ dim_ ];
    for( int j=0; j<dim_; ++j) { rangeTheta_[i][j] = M_TWOPI; }
    rangeTheta_[i][i] = 0.0;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


RandomOrthogonal::~RandomOrthogonal()
{
  for( int i = 0; i < dim_; i++ ) {
    delete [] omitted_[i];
    delete [] lowerTheta_[i];
    delete [] upperTheta_[i];
    delete [] rangeTheta_[i];
  }

  delete [] omitted_;
  delete [] lowerTheta_;
  delete [] upperTheta_;
  delete [] rangeTheta_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::omitIndex( int i, int j )
{
  if( 0 <= i  &&  i < dim_  &&  0 <= j  &&  j < dim_ ) {
    omitted_[i][j] = true;
    omitted_[j][i] = true;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::omitIndex( int n )
{
  for( int i = 0; i < dim_; i++ ) {
    this->omitIndex( n, i );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::includeIndex( int i, int j )
{
  if( 0 <= i  &&  i < dim_  &&  0 <= j  &&  j < dim_ ) {
    omitted_[i][j] = false;
    omitted_[j][i] = false;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::includeIndex( int n )
{
  for( int i = 0; i < dim_; i++ ) {
    this->includeIndex( n, i );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::setNumberOfPasses( int n )
{
  if( 0 < n ) { passes_ = n; }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::setRange( int i, int j, double lo, double hi )
{
  // Check and condition the arguments
  if( i < 0  ||  dim_ <= i  ||
      j < 0  ||  dim_ <= j  ||
      hi <= lo )
  { return; }

  if( std::abs(lo) < 1.0e-12            ) { lo = 0.0;     }
  if( std::abs(hi - M_TWOPI ) < 1.0e-12 ) { hi = M_TWOPI; }

  while( lo < 0.0      ) { lo += M_TWOPI; }
  while( lo > M_TWOPI  ) { lo -= M_TWOPI; }
  while( hi < 0.0      ) { hi += M_TWOPI; }
  while( hi > M_TWOPI  ) { hi -= M_TWOPI; }

  // Set the range
  lowerTheta_[i][j] = lo;
  lowerTheta_[j][i] = lo;
  upperTheta_[i][j] = hi;
  upperTheta_[j][i] = hi;
  rangeTheta_[i][j] = hi - lo;
  rangeTheta_[j][i] = hi - lo;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void RandomOrthogonal::setRange( int n, double lo, double hi )
{
  for( int i = 0; i < dim_; i++ ) {
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

  TMatrix<double> R = TMatrix<double>::Imatrix( dim_ );
  TMatrix<double> U = TMatrix<double>::Imatrix( dim_ );
  TMatrix<double> W = TMatrix<double>::Imatrix( dim_ );

  for( int p = 0; p < passes_; p++ ) {
    for( int i = 0; i < dim_-1; i++ ) {
      for( int j = i+1; j < dim_; j++ ) {
        if( !(omitted_[i][j]) ) {
          double theta = lowerTheta_[i][j] + drand48()*rangeTheta_[i][j];
          double cs = cos( theta );
          double sn = sin( theta );
          W[i][i] =  cs;  W[i][j] = sn;
          W[j][i] = -sn;  W[j][j] = cs;
          R = R*W;
          W = U;
        }
      }
    }
  }

  return R;
}

