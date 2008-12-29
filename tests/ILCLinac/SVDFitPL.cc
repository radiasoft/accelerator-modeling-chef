/*************************************************************************
**************************************************************************
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
******
******  File:      SVDFitPL.cc
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
#include <SVDFitPL.h>

using FNAL::pcerr;
using FNAL::pcout;

using namespace std;

Matrix SVDFitPL::sqrt( Matrix& x )
{
  int r = x.rows();
  int c = x.cols();
  if( r == c ) {
    Matrix U(r,c);
    Vector W(c);
    Matrix V(c,c);
    x.SVD(U,W,V);

    #if 0
    BEGIN DIAGNOSTIC SECTION
    (*pcout) << "DGN: = " << __FILE__ << "," << __LINE__
             << ": U - v = \n"
             << ( U - V )
             << endl;
    END DIAGNOSTIC SECTION
    #endif

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
   uic <<   "Do try to stay focused!"
       << "\nThe argument of sqrt must be a square matrix."
       << "\nYours is " << r << " x " << c << ".";
   (*pcerr) << "\n*** ERROR *** " 
            << "\n*** ERROR *** " << __FILE__ << "," << __LINE__
            << "\n*** ERROR *** Matrix sqrt( const Matrix& x )"
            << "\n*** ERROR *** ------------------------------"
            << "\n*** ERROR *** Do try to stay focused!"
            << "\n*** ERROR *** The argument of sqrt must be a square matrix."
            << "\n*** ERROR *** Yours is " << r << " x " << c << "."
            << "\n*** ERROR *** "
            << endl;
   throw GenericException( __FILE__, __LINE__, 
                           "Matrix sqrt( const Matrix& x )",
                           uic.str().c_str() );
  }
}


SVDFitPL::SVDFitPL()
: _rows(3)
, _cols(2)
, _response(3,2)
, _solver(2,3)
, _cov(3,3)
, _covInv(3,3)
, _sig(3,3)
, _sigInv(3,3)
, _xU(3,2)
, _xW(2)
, _xV(2,2)
, _limW(0)
, _solveMode(-1)
, _applyWeights(false)
, _ready(false)
{
  _finishConstruction();
}


void SVDFitPL::_reconstruct( int r, int c )
{
  _rows         = r;
  _cols         = c;
  _response     = Matrix( r, c );
  _solver       = Matrix( c, r );
  _cov          = Matrix( r, r );
  _covInv       = Matrix( r, r );
  _sig          = Matrix( r, r );
  _sigInv       = Matrix( r, r );
  _xU           = Matrix( r, c );
  _xW           = Vector( c    );
  _xV           = Matrix( c, c );
  _applyWeights = false;
  _ready        = false;

  _finishConstruction();
}


void SVDFitPL::_finishConstruction()
{
  for( int i = 0; i < _rows; i++ ) {
    for( int j = 0; j < _rows; j++ ) {
      if( i == j ) { _cov(i,j) = 1.0; }
      else         { _cov(i,j) = 0.0; }
    }
  }
  _covInv = _cov;
  _sig    = _cov;
  _sigInv = _cov;
}


SVDFitPL::~SVDFitPL()
{
}


void SVDFitPL::setLinearResponse( const Matrix& x )
{
  if( _rows != x.rows() || _cols != x.cols() ) { 
    _reconstruct( x.rows(), x.cols() ); 
  }
  _response = x;
  _buildSolver();
}


void SVDFitPL::setErrorCovariance( const Matrix& E )
{
  // NOTE: The efficiency of this should be improved
  // if it is to be used frequently.

  if( _rows == E.rows() && 1 == E.cols() ) {  // Uncorrelated errors
    _cov = Matrix( _rows, _rows );
    _sig = Matrix( _rows, _rows );
    _covInv = Matrix( _rows, _rows );
    _sigInv = Matrix( _rows, _rows );

    for( int i = 0; i < _rows; i++ ) {
      _cov(i,i) = fabs( E(i,0) );
      _sig(i,i) = std::sqrt( fabs( E(i,0) ) );
      _covInv(i,i) = 1.0/fabs( E(i,0) );
      _sigInv(i,i) = 1.0/std::sqrt( fabs( E(i,0) ) );
    }

    _applyWeights = true;
    _buildSolver();
  }

  else if( _rows == E.rows() && _rows == E.cols() ) {
    _cov = 0.5*( E + E.transpose() );  // Forces symmetry
    _sig = this->sqrt(_cov);
    _covInv = _cov.inverse();
    _sigInv = _sig.inverse();

    _applyWeights = true;
    _buildSolver();
  }

  else if( E.rows() != E.cols() ) {
    (*pcerr) << "\n*** WARNING *** " 
             << "\n*** WARNING *** " << __FILE__ << "," << __LINE__
             << "\n*** WARNING *** void SVDFitPL::setErrorCovariance( const Matrix& )"
             << "\n*** WARNING *** ------------------------------------------------"
             << "\n*** WARNING *** Oh, please ..."
             << "\n*** WARNING *** Your argument isn't even square: it is "
             <<                    E.rows() << " x " << E.cols() << "."
             << "\n*** WARNING *** " 
             << "\n*** WARNING *** No change will be made to SVDFitPL object." 
             << "\n*** WARNING *** " 
             << endl;
  }

  else {
    (*pcerr) << "\n*** WARNING *** " 
             << "\n*** WARNING *** " << __FILE__ << "," << __LINE__
             << "\n*** WARNING *** void SVDFitPL::setErrorCovariance( const Matrix& )"
             << "\n*** WARNING *** ------------------------------------------------"
             << "\n*** WARNING *** How long must I endure this?"
             << "\n*** WARNING *** The argument is " << E.rows() << " x " << E.cols()
             << "\n*** WARNING *** but the current response matrix has " << _rows << " rows."
             << "\n*** WARNING *** Either set a new response matrix or use another error matrix."
             << "\n*** WARNING *** " 
             << "\n*** WARNING *** No change will be made to SVDFitPL object." 
             << "\n*** WARNING *** " 
             << endl;
  }
}

Vector SVDFitPL::getSingularValue() const { return _xW;}
Matrix SVDFitPL::getVMatrix() const { return _xVOrig;}
Matrix SVDFitPL::getUMatrix() const { return _xU;}

void SVDFitPL::setWeighted( bool x )
{
  _applyWeights = x; 
}


void SVDFitPL::_buildSolver()
{
  if( _applyWeights ) {
    (_sigInv*_response).SVD( _xU, _xW, _xV );
  }
  else {
    _response.SVD( _xU, _xW, _xV );
    _xVOrig = _xV;
  }
  if (_solveMode != -1) { 
 // ONly a specific eigen mode is applied.   
     for( int j = 0; j < _cols; j++ ) {
       if (j != _solveMode)  
         for (int i=0; i < _rows; i++) _xU(i,j) = 0.; 
    }
 
  } else {  
  // Removal of nullSpace 
  // Get the maximum w
    double wMax = -1.0;
    for( int j = 0; j < _cols; j++ ) 
      if (std::abs(_xW(j)) > wMax) 
        wMax = std::abs(_xW(j)); 
  
    for( int j = 0; j < _cols; j++ ) {
       if (std::abs(_xW(j))/wMax < _limW) 
         for (int i=0; i < _rows; i++) _xU(i,j) = 0.; 
    }
  }
  // Next to using a diagonal matrix, this is
  // probably the slowest possible way to do this.
  double aaa;
  for( int j = 0; j < _cols; j++ ) {
    aaa = 1.0/_xW(j);
    for( int i = 0; i < _cols; i++ ) {
      _xV(i,j) *= aaa;
    }
  }

  if( _applyWeights ) {
    _solver = ( _xV * _xU.transpose() ) * _sigInv;
  }
  else {
    _solver = ( _xV * _xU.transpose() );
  }

  _ready = true;
}



Matrix SVDFitPL::solve( const Matrix& theData ) const
{
  if( _ready ) 
  {
    if( _rows == theData.rows() ) {
      return ( _solver*theData );
    }
    else {
      ostringstream uic;
      uic <<   "You are obviously a moron."
          << "\nThe argument must have "
          << _cols 
          << " components but has "
          << theData.rows()
          << " instead.";
      (*pcerr) << "\n*** ERROR *** " 
               << "\n*** ERROR *** " << __FILE__ << "," << __LINE__
               << "\n*** ERROR *** Matrix SVDFitPL::solve( const Matrix& ) const"
               << "\n*** ERROR *** -------------------------------------------"
               << "\n*** ERROR *** You are obviously a moron."
               << "\n*** ERROR *** The argument must have "
                                << _cols 
                                << " components but has "
                                << theData.rows()
                                << " instead."
               << "\n*** ERROR *** " 
               << endl;
      throw GenericException( __FILE__, __LINE__, 
                              "Matrix SVDFitPL::solve( const Matrix& theData )",
                              uic.str().c_str() );
    }
  }

  else 
  {
    throw GenericException( __FILE__, __LINE__, 
                            "Matrix SVDFitPL::solve( const Matrix& theData )"
                            "Idiot!\nYou first must establish a response matrix." );
  }
}


Matrix SVDFitPL::getStateCovariance() const
{
  return ( _solver * _cov *  _solver.transpose() );
}


bool SVDFitPL::getWeighted() const
{
  return _applyWeights;
}
