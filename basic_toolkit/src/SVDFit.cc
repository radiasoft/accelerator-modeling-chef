/*************************************************************************
**************************************************************************
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
******
******  File:      SVDFit.cc
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
******             Mail Stop 234
******             Batavia, IL   60510
******                                     
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******                                                                
****** Revision History:
******   October 31, 2006
******   - Initial version.  Still a work in progress.
******   
**************************************************************************
*************************************************************************/

#include <string>

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/SVDFit.h>

using FNAL::pcerr;
using FNAL::pcout;

using namespace std;

const double SVDFit::threshold_ = 1.0e-8;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

Matrix SVDFit::sqrt( Matrix& x )
{
  int r = x.rows();
  int c = x.cols();

  if( r == c ) {
    Matrix U(r,c);
    Vector W(c);
    Matrix V(c,c);
    x.SVD(U,W,V);

    // Next to using a diagonal matrix, this is
    // probably the slowest possible way to do this.
    double aaa;
    for( int j = 0; j < c; j++ ) {
      aaa = std::sqrt(W(j));
      for( int i = 0; i < r; i++ ) {
        U(i,j) *= aaa;
      }
    }
  
    return U*V.transpose();
  }
  else {
   ostringstream uic;
   uic <<   "The argument of sqrt must be a square matrix."
       << "\nYours is " << r << " x " << c << ".";
   (*pcerr) << "\n*** ERROR *** " 
            << "\n*** ERROR *** " << __FILE__ << "," << __LINE__
            << "\n*** ERROR *** Matrix SVDFit::sqrt( const Matrix& x )"
            << "\n*** ERROR *** ------------------------------"
            << "\n*** ERROR *** The argument of sqrt must be a square matrix."
            << "\n*** ERROR *** Yours is " << r << " x " << c << "."
            << "\n*** ERROR *** "
            << endl;
   throw GenericException( __FILE__, __LINE__, 
                           "Matrix SVDFit::sqrt( const Matrix& x )",
                           uic.str().c_str() );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


SVDFit::SVDFit()
:  rows_(3)
,  cols_(2)
,  response_(3,2)
,  solver_(2,3)
,  cov_(3,3)
,  covInv_(3,3)
,  sig_(3,3)
,  sigInv_(3,3)
,  xU_(3,2)
,  xW_(2)
,  xV_(2,2)
,  xU0_(3,2)
,  xW0_(2)
,  xV0_(2,2)
,  applyWeights_(false)
,  ready_(false)
,  limW_(SVDFit::threshold_)
{
  finishConstruction_();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


void SVDFit::reconstruct_( int r, int c )
{
  rows_         = r;
  cols_         = c;
  response_     = Matrix( r, c );
  solver_       = Matrix( c, r );
  cov_          = Matrix( r, r );
  covInv_       = Matrix( r, r );
  sig_          = Matrix( r, r );
  sigInv_       = Matrix( r, r );
  xU_           = Matrix( r, c );
  xW_           = Vector( c    );
  xV_           = Matrix( c, c );
  xU0_          = Matrix( r, c );
  xW0_          = Vector( c    );
  xV0_          = Matrix( c, c );
  applyWeights_ = false;
  ready_        = false;

  finishConstruction_();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


void SVDFit::finishConstruction_()
{
  for( int i=0; i < rows_; ++i ) {
    for( int j=0; j < rows_; ++j ) {

      if( i == j ) { cov_(i,j) = 1.0; }
      else         { cov_(i,j) = 0.0; }
    }
  }
  covInv_ = cov_;
  sig_    = cov_;
  sigInv_ = cov_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


SVDFit::~SVDFit()
{
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


void SVDFit::setLinearResponse( Matrix const& x )
{
  if( rows_ != x.rows() || cols_ != x.cols() ) { 
     reconstruct_( x.rows(), x.cols() ); 
  }
  response_ = x;
  buildSolver_();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


void SVDFit::setErrorCovariance( Matrix const& E )
{

  // NOTE: The efficiency of this should be improved
  // if it is to be used frequently.

  if( rows_ == E.rows() && 1 == E.cols() ) {  // Uncorrelated errors
     cov_    = Matrix( rows_, rows_ );
     sig_    = Matrix( rows_, rows_ );
     covInv_ = Matrix( rows_, rows_ );
     sigInv_ = Matrix( rows_, rows_ );

    for( int i=0;  i < rows_; ++i) {
      cov_(i,i)    = std::abs( E(i,0) );
      sig_(i,i)    = std::sqrt( fabs( E(i,0) ) );
      covInv_(i,i) = 1.0/std::abs( E(i,0) );
      sigInv_(i,i) = 1.0/std::sqrt( fabs( E(i,0) ) );
    }

    applyWeights_ = true;
    buildSolver_();
  }

  else if( rows_ == E.rows() && rows_ == E.cols() ) {
     cov_    = 0.5*( E + E.transpose() );  // Forces symmetry
     sig_    = SVDFit::sqrt(cov_);
     covInv_ = cov_.inverse();
     sigInv_ = sig_.inverse();

     applyWeights_ = true;
     buildSolver_();
  }

  else if( E.rows() != E.cols() ) {
    (*pcerr) << "\n*** WARNING *** " 
             << "\n*** WARNING *** " << __FILE__ << "," << __LINE__
             << "\n*** WARNING *** void SVDFit::setErrorCovariance( const Matrix& )"
             << "\n*** WARNING *** ------------------------------------------------"
             << "\n*** WARNING *** Your argument isn't even square: it is "
             <<                    E.rows() << " x " << E.cols() << "."
             << "\n*** WARNING *** " 
             << "\n*** WARNING *** No change will be made to SVDFit object." 
             << "\n*** WARNING *** " 
             << endl;
  }

  else {
    (*pcerr) << "\n*** WARNING *** " 
             << "\n*** WARNING *** " << __FILE__ << "," << __LINE__
             << "\n*** WARNING *** void SVDFit::setErrorCovariance( const Matrix& )"
             << "\n*** WARNING *** ------------------------------------------------"
             << "\n*** WARNING *** The argument is " << E.rows() << " x " << E.cols()
             << "\n*** WARNING *** but the current response matrix has " << rows_ << " rows."
             << "\n*** WARNING *** Either set a new response matrix or use another error matrix."
             << "\n*** WARNING *** " 
             << "\n*** WARNING *** No change will be made to SVDFit object." 
             << "\n*** WARNING *** " 
             << endl;
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


void SVDFit::setWeighted( bool x )
{
  applyWeights_ = x; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


void SVDFit::setNullSpaceThreshold( double x )
{
  limW_ = fabs(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


void SVDFit::buildSolver_()
{
  if( applyWeights_ ) {
    (sigInv_*response_).SVD( xU_, xW_, xV_ );
  }
  else {
    response_.SVD( xU_, xW_, xV_ );
  }
  xU0_ = xU_;    // Stored for later
  xV0_ = xV_;    // retrieval by user.
  xW0_ = xW_;


  // Suppressing (thresholded) null space.
  // -------------------------------------
  double wMax = -1.0; 
  for( int j = 0; j < cols_; j++ ) {
    if( wMax < xW_(j) ) { wMax = xW_(j); }
    // NOTE: singular values are non-negative 
    // NOTE: by definition and construction.
  }
  wMax *= limW_;

  for( int j = 0; j < cols_; j++ ) {
    if( xW_(j) < wMax ) {
      for (int i=0; i < rows_; i++) { xU_(i,j) = 0.; }
      xW_(j) = 0;
    }
  } 

  // ... Next to using a diagonal matrix, this is
  // ... probably the slowest possible way to do this.
  double aaa;
  for( int j=0; j < cols_; ++j) {
    if( 0 < xW_(j) ) {
      aaa = 1.0/xW_(j);
      for( int i = 0; i < cols_; i++ ) {
        xV_(i,j) *= aaa;
      }
    }
    else {
      for( int i = 0; i < cols_; i++ ) {
        xV_(i,j) = 0;  // Not strictly necessary,
      }                // but paranoia seldom hurts.
    }
  }


  if( applyWeights_ ) {
    solver_ = ( xV_ * xU_.transpose() ) * sigInv_;
  }
  else {
    solver_ = ( xV_ * xU_.transpose() );
  }

  ready_ = true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


Matrix SVDFit::solve( const Matrix& theData ) const
{
  if( ready_ ) 
  {
    if( rows_ == theData.rows() ) {
      return ( solver_*theData );
    }
    else {
      ostringstream uic;
      uic << "The argument must have "
          << cols_ 
          << " components but has "
          << theData.rows()
          << " instead.";
      (*pcerr) << "\n*** ERROR *** " 
               << "\n*** ERROR *** " << __FILE__ << "," << __LINE__
               << "\n*** ERROR *** Matrix SVDFit::solve( const Matrix& ) const"
               << "\n*** ERROR *** -------------------------------------------"
               << "\n*** ERROR *** The argument must have "
                                << cols_ 
                                << " components but has "
                                << theData.rows()
                                << " instead."
               << "\n*** ERROR *** " 
               << endl;
      throw GenericException( __FILE__, __LINE__, 
                              "Matrix SVDFit::solve( const Matrix& theData )",
                              uic.str().c_str() );
    }
  }

  else 
  {
    throw GenericException( __FILE__, __LINE__, 
                            "Matrix SVDFit::solve( const Matrix& theData )"
                            "\nYou first must establish a response matrix." );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


Matrix SVDFit::stateCovariance() const
{
  return ( solver_ * cov_ *  solver_.transpose() );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 


Matrix SVDFit::D() const
{
  int n = xW0_.Dim();
  Matrix ret(n,n);
  for( int i = 0; i < n; i++ ) {
    ret(i,i) = xW0_(i);
  }
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
