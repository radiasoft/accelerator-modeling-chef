/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      Frame.cc
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


// #ifdef __sparc
// 
//   #ifndef bool
//   #define bool char
//   #else
//   #define boolAlreadyDefined
//   #endif
//   
//   #ifndef true
//   #define true 1
//   #else
//   #define trueAlreadyDefined
//   #endif
//   
//   #ifndef false
//   #define false 0
//   #else
//   #define falseAlreadyDefined
//   #endif
// 
// #endif // __sparc

#include "Frame.h"

using namespace std;

Frame::Frame()
: o(3), e(3,3)
{
  //                     e_1  e_2  e_3
  static double u [] = { 1.,  0.,  0.,
                         0.,  1.,  0.,
                         0.,  0.,  1.
                       };

  double* k = u;
  for( int i = 0; i < 3; i++ ) {
    for( int j = 0; j < 3; j++ ) {
      e(i,j) = *(k++);
    }
  }
}

Frame::Frame( const Frame& x )
: o(3), e(3,3)
{
  o = x.o;
  e = x.e;
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
  static short int ret;
  ret = 0;

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
  static short int ret;
  ret = 0;

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
  static short int ret;
  ret = 0;

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


bool Frame::isOrthonormal() const
{
  MatrixD u(3,3);

  u = e.transpose() - e.inverse();

  for( int i = 0; i < 3; i++ ) {
    for( int j = 0; j < 3; j++ ) {
      if( 1.0e-12 < fabs( u(i,j) ) ) return false;
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
  static short int ret;
  ret = 0;

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
  static short int ret;
  ret = 0;

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

  return ret;
}


ostream& operator<< ( ostream& os, /* const */ Frame& f )
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

istream& operator>> ( istream& is, /* const */ Frame& f )
{
  is >> f.o(0) >> f.o(1) >> f.o(2);
  is >> f.e(0,0) >> f.e(1,0) >> f.e(2,0);
  is >> f.e(0,1) >> f.e(1,1) >> f.e(2,1);
  is >> f.e(0,2) >> f.e(1,2) >> f.e(2,2);
  return is;
}



// #ifdef __sparc
// 
//   #ifndef boolAlreadyDefined
//   #undef bool
//   #else 
//   #undef boolAlreadyDefined
//   #endif
//   
//   #ifndef trueAlreadyDefined
//   #undef true
//   #else 
//   #undef trueAlreadyDefined
//   #endif
//   
//   #ifndef falseAlreadyDefined
//   #undef false
//   #else 
//   #undef falseAlreadyDefined
//   #endif
//   
// #endif
