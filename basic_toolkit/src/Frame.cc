/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      Frame.cc
******  Version:   4.1
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <GenericException.h>
#include <Frame.h>

using namespace std;


Frame::Frame()
: o(3), e(3,3)
{
  //                     e_1  e_2  e_3
  const static double u [] = { 1.,  0.,  0.,
                               0.,  1.,  0.,
                               0.,  0.,  1.
                             };

  const double* k = u;
  for( int i=0; i < 3; ++i) {
    for( int j=0; j < 3; ++j) {
      e(i,j) = *(k++);
    }
  }
}


Frame::Frame( const Frame& x )
: o(x.o), e(x.e) 
{ 
}


Frame::~Frame()
{
}


Frame& Frame::operator=( const Frame& x )
{
  if( this != &x ) {
    o = x.o;
    e = x.e;
  }
  return *this;
}
  
short int Frame::setOrigin ( const Vector& x )
{
  short int ret = 0;

  if( x.Dim() != 3 ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::setAxis                  \n"
            "***WARNING*** Dimension of vector is not 3    \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 2;
  }

  else {
    o = x;
  }  

  return ret;
}

short int Frame::setAxis   ( int j, const Vector& x )
{
  short int ret = 0;

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
    for( int i = 0; i < 3; i++ ) {
      e( i, j ) = x(i);
    }
  }  

  return ret;
}

short int Frame::setDualAxis   ( int i, const Vector& x )
{
  short int ret = 0;

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
    e = e.inverse();
    for( int j = 0; j < 3; j++ ) {
      e( i, j ) = x(j);
    }
    e = e.inverse();
  }  

  return ret;
}


bool Frame::setOrthonormalAxes( const MatrixD& axisSystem )
{
  MatrixD oldSystem;
  oldSystem = e;
  e = axisSystem;
  if( this->isOrthonormal() ) {
    return true;
  }
  else {
    e = oldSystem;
    return false;
  }
}


// REMOVE: bool Frame::isOrthonormal() const
// REMOVE: {
// REMOVE:   MatrixD u(3,3);
// REMOVE: 
// REMOVE:   u = e.transpose() - e.inverse();
// REMOVE: 
// REMOVE:   for( int i = 0; i < 3; i++ ) {
// REMOVE:     for( int j = 0; j < 3; j++ ) {
// REMOVE:       if( 1.0e-12 < fabs( u(i,j) ) ) return false;
// REMOVE:     }
// REMOVE:   }
// REMOVE: 
// REMOVE:   return true;
// REMOVE: }


bool Frame::isOrthonormal() const
{
  MatrixD u(3,3);
  u = (e.transpose())*e;

  for( int i = 0; i < 3; i++ ) {
    for( int j = 0; j < 3; j++ ) {
      if( i == j ) {
        if( 1.0e-9 < fabs( u(i,j) - 1.0 ) ) { return false; }
      }
      else {
        if( 1.0e-12 < fabs( u(i,j) ) ) { return false; }
      }
    }
  }
  return true;
}


Vector Frame::getOrigin() const
{
  return o;
}

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
    for( int i = 0; i < 3; i++ ) ret(i) = e.getElement(i,j);
  }

  return ret;
}

Vector Frame::getxAxis() const
{
  Vector ret(3);
  for( int i = 0; i < 3; i++ ) ret(i) = e.getElement(i,0);
  return ret;
}

Vector Frame::getyAxis() const
{
  Vector ret(3);
  for( int i = 0; i < 3; i++ ) ret(i) = e.getElement(i,1);
  return ret;
}

Vector Frame::getzAxis() const
{
  Vector ret(3);
  for( int i = 0; i < 3; i++ ) ret(i) = e.getElement(i,2);
  return ret;
}

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
    w = e.inverse();
    for( int j = 0; j < 3; j++ ) ret(j) = w(i,j);
  }

  return ret;
}

short int Frame::rotate( double theta, 
                         const  Vector& u,
                         bool   rotateOrigin )
{
  short int ret = 0;

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
    int i, k;
    for( i = 0; i < 3; i++ ) {
      for( k = 0; k < 3; k++ ) {
        w(k) = e(k,i);
      }
      u.Rotate( w, theta );
      for( k = 0; k < 3; k++ ) {
        e(k,i) = w(k);
      }
    }
  }

  // Origin is rotated by default ...
  if( rotateOrigin ) {
    u.Rotate( o, theta );
  }

  return ret;
}

short int Frame::translate ( const Vector& x )
{
  short int ret = 0;

  if( x.Dim() != 3 ) {
    cerr << "***WARNING***                                 \n"
            "***WARNING*** Frame::rotate                   \n"
            "***WARNING*** Dimension of vector is not 3    \n"
            "***WARNING***                                 \n"
         << endl;
    ret = 2;
  }

  else {
    o += x;
  }

  return ret;
}



void Frame::reset()
{
  o(0) = 0.0;
  o(1) = 0.0;
  o(2) = 0.0;

  for( int i = 0; i < 3; i++ ) {
    e(i,i) = 1.0;
    for( int j = i+1; j < 3; j++ ) {
      e(i,j) = 0.0;
      e(j,i) = 0.0;
    }
  }
}


Frame Frame::relativeTo( const Frame& f ) const
{
  // Assumes orthonormal Frame
  Frame ret;
  MatrixD W( 3, 3 );

  W = ( f.e ).transpose();
  ret.o = W * ( this->o - f.o );
  ret.e = W * ( this->e );

  return ret;
}


Frame Frame::patchedOnto( const Frame& f ) const
{
  // Assumes orthonormal Frame
  Frame ret;
  MatrixD W( 3, 3 );

  W = f.e;
  ret.o = f.o + W * ( this->o );
  ret.e = W * ( this->e );

  // A correction to keep orthogonality
  // errors from building up.
  Matrix corrector(3,3);
  corrector = (-0.5)*((ret.e.transpose())*(ret.e));
  for( int i = 0; i < 3; i++ ) {
    corrector(i,i) = corrector(i,i) + 1.5;
  }
  ret.e = corrector*(ret.e);

  return ret;
}


Frame Frame::tween(   const Frame& one, const Frame& two
                    , double pct, bool doChecks )
{
  static bool firstTime = true;
  const std::complex<double>  c_zero(0.,0.);
  const std::complex<double>  c_one(1.,0.);

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
  // REMOVE: Frame middle;
  // REMOVE: middle.setOrigin( (1.0 - pct)*one.getOrigin() +  pct*two.getOrigin() );
  Frame middle( two.relativeTo(one) );

  // REMOVE: MatrixD E1(3,3), E2(3,3), W(3,3);
  MatrixD W(3,3);
  MatrixC U(3,3), UI(3,3), lambda(3,3);

  // ... the rotation matrix
  // REMOVE: E1 = one.getAxes();
  // REMOVE: E2 = two.getAxes();
  // REMOVE: W = E1.transpose() * E2;
  W = middle.getAxes();
  
  // ... eigenvalues and eigenvectors
  U = W.eigenVectors();
  UI = U.inverse();
  lambda = UI*W*U;
  
  // ...... more paranoia
  int i, j;
  if( doChecks ) {
    for( i = 0; i < 3; i++ ) {
      for( j = 0; j < 3; j++ ) {
        if( i == j ) {
          if( 1.0e-8 < std::abs( 1.0 - std::abs(lambda(i,i)) ) ) {
            throw( GenericException( __FILE__, __LINE__
                   , "Frame::tween"
                   , "Horrible! Inexplicable!! Rotation matrix failed test!!!") );
	  }
	}
        else {
          if( 1.0e-8 < std::abs(lambda(i,j)) ) {
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
  for( i = 0; i < 3; i++ ) {
    for( j = 0; j < 3; j++ ) {
      if( i == j ) {
        if( std::abs(imag(lambda(i,i))) < 1.0e-9*std::abs(real(lambda(i,i))) ) {
          lambda(i,i) = c_one;
	}
        else {
          theta = pct*atan2( imag(lambda(i,i)), real(lambda(i,i)) );
          lambda(i,i) = std::complex<double>( cos(theta), sin(theta) );
	}
      }
      else {
        lambda(i,j) = c_zero;
      }
    }
  }
  

  // REMOVE: // Construct the "tweened" axes.
  // REMOVE: // Exuding confidence; no paranoia.
  // REMOVE: middle.setOrthonormalAxes( E1*real( U*lambda*UI ) );
  // REMOVE: 
  // REMOVE: 
  // REMOVE: // Finished
  // REMOVE: return middle;

  // Finish
  middle.setOrigin( pct*middle.getOrigin() );
  middle.setOrthonormalAxes( real( U*lambda*UI ) );
  return middle.patchedOnto(one);
}


void Frame::convertInPlace( Vector& p, Vector& v ) const
{
  MatrixD U( e.transpose() );
  p = U*(p-o);
  v = U*v;
}


ostream& operator<< ( ostream& os, const Frame& f )
{
  /* 
     os << "Frame origin: " << f.o << endl;
     os << "Frame x axis: ( " 
     << f.e(0,0) << ", " << f.e(1,0) << ", " << f.e(2,0) << " )"
     << endl;
     os << "Frame y axis: ( " 
     << f.e(0,1) << ", " << f.e(1,1) << ", " << f.e(2,1) << " )"
     << endl;
     os << "Frame z axis: ( " 
     << f.e(0,2) << ", " << f.e(1,2) << ", " << f.e(2,2) << " )"
     << endl;
     return os;
     */
  os << f.o(0) << " " << f.o(1) << " " << f.o(2) << endl;
  os << f.e(0,0) << " " << f.e(1,0) << " " << f.e(2,0) << endl;
  os << f.e(0,1) << " " << f.e(1,1) << " " << f.e(2,1) << endl;
  os << f.e(0,2) << " " << f.e(1,2) << " " << f.e(2,2) << endl;
  return os;
}

istream& operator>> ( istream& is, Frame& f )
{
  is >> f.o(0) >> f.o(1) >> f.o(2);
  is >> f.e(0,0) >> f.e(1,0) >> f.e(2,0);
  is >> f.e(0,1) >> f.e(1,1) >> f.e(2,1);
  is >> f.e(0,2) >> f.e(1,2) >> f.e(2,2);
  return is;
}
