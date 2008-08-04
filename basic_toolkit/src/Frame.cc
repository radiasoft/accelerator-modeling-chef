/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Frame.cc

******  Date:      March 15, 2005
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******
****** Revision History:
******
****** October, 2007   Leo Michelotti
******                 michelotti@fnal.gov
****** 
****** - fixed application of orthonormal corrector from
******   left-multiplication to right-multiplication.
****** - added corrector to two more methods: .rotate
******   and .relativeTo.
******   ; it should not be needed there, but you never know.
******
**************************************************************************
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/Frame.h>

using namespace std;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame::Frame()
 : o_(3), e_(3,3)
{

  // default: zero offset and rotation matrix = unit diagonal

   for( int i=0; i<3; ++i) {
      for( int j=0; j< 3; ++j) {
        e_[i][j] = ( i == j ) ? 1.0 : 0.0; 
      }
   }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame::Frame( Frame const& x )
: o_(x.o_), e_(x.e_) 
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame::~Frame()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame const& Frame::identityFrame() 
{
  static Frame identity;
  return identity;
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame& Frame::operator=( Frame const& x )
{
  if( this == &x )  return *this;
  
  o_ = x.o_;
  e_ = x.e_;
  
  return *this;
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Frame::setOrigin ( Vector const& x )
{
   int ret = 0;

  if( x.Dim() != 3 ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::setAxis                  \n"
            "***WARNING*** Dimension of vector is not 3    \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 2;
  }

  else {
    o_ = x;
  }  

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 int Frame::setAxis   ( int j, Vector const& x )
{
   int ret = 0;

  if( j < 0 || 2 < j ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::setAxis                  \n"
            "***WARNING*** Index argument out of range     \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 1;
  }

  else if( x.Dim() != 3 ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::setAxis                  \n"
            "***WARNING*** Dimension of vector is not 3    \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 2;
  }

  else {
    for( int i=0; i < 3; ++i) {
      e_[i][j] = x[i];
    }
  }  

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 int Frame::setDualAxis   ( int i, Vector const& x )
{
   int ret = 0;

  if( i < 0 || 2 < i ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::setDualAxis              \n"
            "***WARNING*** Index argument out of range     \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 1;
  }

  else if( x.Dim() != 3 ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::setDualAxis              \n"
            "***WARNING*** Dimension of vector is not 3    \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 2;
  }

  else {
    e_ = e_.inverse();
    for( int j=0; j<3; ++j) {
      e_[i][j] = x[j];
    }
    e_ = e_.inverse();
  }  

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Frame::setOrthonormalAxes( MatrixD const& axisSystem )
{
  MatrixD oldSystem = e_;

  e_ = axisSystem;
  if( isOrthonormal() ) {
    return true;
  }
  else {
    e_ = oldSystem;
    return false;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Frame::isOrthonormal() const
{
  MatrixD u = (e_.transpose())*e_;

  for( int i=0; i<3; ++i) {
    for( int j=0; j<3; ++j) {
      if( i == j ) {
        if ( std::abs( u[i][j] - 1.0 ) > 1.0e-9 ) { return false; }
      }
      else {
        if( std::abs( u[i][j] > 1.0e-12  )) { return false; }
      }
    }
  }
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Frame::getOrigin() const
{
  return o_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Frame::getAxis( int j ) const
{
  Vector ret(3);

  if( j < 0 || 2 < j ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::getAxis                  \n"
            "***WARNING*** Index argument out of range     \n"
            "***WARNING***                                 \n"
         << endl;
  }

  else {
    for( int i=0; i<3; ++i) ret[i] = e_[i][j];
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Frame::getxAxis() const
{
  Vector ret(3);
  for( int i = 0; i < 3; ++i) ret[i] = e_[i][0];
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Frame::getyAxis() const
{
  Vector ret(3);
  for( int i = 0; i<3; ++i) ret[i] = e_[i][1];
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Frame::getzAxis() const
{
  Vector ret(3);
  for( int i=0; i< 3; ++i) ret[i] = e_[i][2];
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector Frame::getDualAxis( int i ) const
{
  Vector ret(3);
  MatrixD w(3,3);

  if( i < 0 || 2 < i ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::getDualAxis              \n"
            "***WARNING*** Index argument out of range     \n"
            "***WARNING***                                 \n"
         << endl;
  }

  else {
    w = e_.inverse();
    for( int j=0; j<3; ++j) ret[j] = w[i][j];
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int Frame::rotate( double theta, 
                   Vector const& u,
                   bool   rotateOrigin )
{
  int ret = 0;

  if( u.Dim() != 3 ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::rotate                   \n"
            "***WARNING*** Dimension of vector is not 3    \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 2;
  }

  else {
    Vector w(3);

    for( int i=0; i<3; ++i ) {

      for( int k=0; k<3; ++k) {
        w[k] = e_[k][i];
      }

      w = u.Rotate( w, theta );

      for( int k =0; k<3; ++k) {
        e_[k][i] = w[k];
      }

    }
  }

  // A correction to keep orthogonality
  // errors from building up.
  Matrix corrector = (-0.5)*((e_.transpose())*(e_));
  for( int i=0; i<3; ++i) {
    corrector[i][i] = corrector[i][i] + 1.5;
  }
  e_ = e_*corrector;

  // Origin is rotated by default ...
  if( rotateOrigin ) {
    o_ = u.Rotate( o_, theta );
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 int Frame::translate ( Vector const& x )
{
   int ret = 0;

  if( x.Dim() != 3 ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::rotate                   \n"
            "***WARNING*** Dimension of vector is not 3    \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 2;
  }

  else {
    o_ += x;
  }

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Frame::reset()
{
  o_[0] = 0.0;
  o_[1] = 0.0;
  o_[2] = 0.0;

  for( int i=0; i<3; ++i) {
    for( int j=0; j< 3; ++j ) {
       e_[i][j] = ( i == j) ? 1.0 : 0.0;
    }
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame Frame::relativeTo( Frame const& f ) const
{
  // Assumes orthonormal Frame
  Frame ret;

  MatrixD W = ( f.e_ ).transpose();

  ret.o_ = W * ( o_ - f.o_ );
  ret.e_ = W * ( e_ );

  // A correction to keep orthogonality
  // errors from building up.
  Matrix corrector = (-0.5)*((ret.e_.transpose())*(ret.e_));
  for( int i=0; i<3; ++i) {
    corrector[i][i] = corrector[i][i] + 1.5;
  }
  ret.e_ = (ret.e_)*corrector;

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame Frame::patchedOnto( Frame const& f ) const
{
  // Assumes orthonormal Frame
  Frame ret;

  MatrixD W = f.e_;
  ret.o_ = f.o_ + W * ( this->o_ );
  ret.e_ = W * ( this->e_ );

  // A correction to keep orthogonality
  // errors from building up.

  Matrix corrector = (-0.5)*((ret.e_.transpose())*(ret.e_));
  for( int i=0; i<3; ++i) {
    corrector[i][i] = corrector[i][i] + 1.5;
  }
  ret.e_ = (ret.e_)*corrector;

  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame Frame::tween(   const Frame& one, const Frame& two
                    , double pct, bool doChecks )
{
  static bool firstTime = true;
  static const std::complex<double>  c_zero(0.,0.);
  static const std::complex<double>  c_one(1.,0.);

  // Trim the pct argument
  if( doChecks ) {
    if( pct < 0.0 ) { pct = 0.0; }
    if( 1.0 < pct ) { pct = 1.0; }
  }

  // Simplest case: pct = 0 or 1
  if( std::abs(pct) < 1.0e-10) {  
    if( firstTime ) {
      cerr << "\n***WARNING***"
           << "\n***WARNING*** File: " << __FILE__ << ", Line " << __LINE__
           << "\n***WARNING*** Function: Frame::tween"
           << "\n***WARNING*** Argument pct is being reset to 0.0."
           << "\n***WARNING*** This message is printed only once."
           << "\n***WARNING***"
           << endl;
      firstTime = false;
    }
    return one;
  }
  if( std::abs(1.0 - pct) < 1.0e-10) {  
    if( firstTime ) {
      cerr << "\n***WARNING***"
           << "\n***WARNING*** File: " << __FILE__ << ", Line " << __LINE__
           << "\n***WARNING*** Function: Frame::tween"
           << "\n***WARNING*** Argument pct is being reset to 1.0."
           << "\n***WARNING*** This message is printed only once."
           << "\n***WARNING***"
           << endl;
      firstTime = false;
    }
    return two;
  }


  // Check orthonormal condition
  if( doChecks ) {
    if( (!(one.isOrthonormal())) || (!(two.isOrthonormal())) ) {
      cerr << "\n***WARNING***"
           << "\n***WARNING*** File: " << __FILE__ << ", Line " << __LINE__
           << "\n***WARNING*** Function: Frame::tween"
           << "\n***WARNING*** This method only works with orthonormal frames."
           << "\n***WARNING***"
           << endl;
    }
  }


  // Construct mid Frame

  Frame middle( two.relativeTo(one) );

  // ... the rotation matrix

  MatrixD W = middle.getAxes();
  
  // ... eigenvalues and eigenvectors

  MatrixC U      = W.eigenVectors();
  MatrixC UI     = U.inverse();
  MatrixC lambda = UI*W*U;
  
  // ...... more paranoia

  if( doChecks ) {
    for( int i=0; i<3; ++i) {
      for( int j=0; j< 3; ++j) {
        if( i == j ) {
          if( 1.0e-8 < std::abs( 1.0 - std::abs(lambda[i][i]) ) ) {
            throw( GenericException( __FILE__, __LINE__
                   , "Frame::tween"
                   , "Horrible! Inexplicable!! Rotation matrix failed test!!!") );
	  }
	}
        else {
          if( 1.0e-8 < std::abs(lambda[i][j]) ) {
            throw( GenericException( __FILE__, __LINE__
                   , "Frame::tween"
                   , "Horrible! Inexplicable!! Rotation matrix failed test!!!") );
	  }
	}
      }
    }
  }


  // Reset the array lambda to correspond to a partial rotation,
  //  and trim its off-diagonal elements.
  // We've already determined the magnitudes of the diagonal
  //  elements of lambda are all 1.0.
  // Note: according to the manual, the result of atan2 should lie
  //  within the range (-pi,pi). I'm going to assume this is correct.
  //  Even paranoia has its limits.
  double theta;

  for( int i = 0; i < 3; ++i) {
    for( int j = 0; j < 3; ++j) {
      if( i == j ) {
        if( std::abs(imag(lambda[i][i])) < 1.0e-9*std::abs(real(lambda[i][i])) ) {
          lambda[i][i] = c_one;
	}
        else {
          theta = pct*atan2( imag(lambda[i][i]), real(lambda[i][i]) );
          lambda[i][i] = std::complex<double>( cos(theta), sin(theta) );
	}
      }
      else {
        lambda[i][j] = c_zero;
      }
    }
  }
  

  middle.setOrigin( pct*middle.getOrigin() );

  middle.setOrthonormalAxes( real( U*lambda*UI ) );

  return middle.patchedOnto(one);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Frame::convertInPlace( Vector& p, Vector& v ) const
{
  MatrixD U( e_.transpose() );
  p = U*(p-o_);
  v = U*v;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Frame::isRightHanded() const
{
  return ( e_.determinant() > 0 );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<< ( ostream& os, const Frame& f )
{
  /* 
     os << "Frame origin: " << f.o_ << endl;
     os << "Frame x axis: ( " 
     << f.e_(0,0) << ", " << f.e_(1,0) << ", " << f.e_(2,0) << " )"
     << endl;
     os << "Frame y axis: ( " 
     << f.e_(0,1) << ", " << f.e_(1,1) << ", " << f.e_(2,1) << " )"
     << endl;
     os << "Frame z axis: ( " 
     << f.e_(0,2) << ", " << f.e_(1,2) << ", " << f.e_(2,2) << " )"
     << endl;
     return os;
     */
  os << f.o_[0]    << " " << f.o_[1]    << " " << f.o_[2]    << endl;
  os << f.e_[0][0] << " " << f.e_[1][0] << " " << f.e_[2][0] << endl;
  os << f.e_[0][1] << " " << f.e_[1][1] << " " << f.e_[2][1] << endl;
  os << f.e_[0][2] << " " << f.e_[1][2] << " " << f.e_[2][2] << endl;
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& operator>> ( istream& is, Frame& f )
{
  is >> f.o_[0]    >> f.o_[1]    >> f.o_[2];
  is >> f.e_[0][0] >> f.e_[1][0] >> f.e_[2][0];
  is >> f.e_[0][1] >> f.e_[1][1] >> f.e_[2][1];
  is >> f.e_[0][2] >> f.e_[1][2] >> f.e_[2][2];
  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD const& Frame::getAxes() const
{
  return e_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

