#include "Frame.h"

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
    for( int i = 0; i < 2; i++ ) {
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
    e = e.inverse().transpose();
    for( int j = 0; j < 2; j++ ) {
      e( i, j ) = x(j);
    }
    e = e.transpose().inverse();
  }  

  return ret;
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
    for( int i = 0; i < 2; i++ ) ret(i) = e.getElement(i,j);
  }

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
    w = e.inverse().transpose();
    for( int j = 0; j < 2; j++ ) ret(j) = w(i,j);
  }

  return ret;
}

short int Frame::rotate( double theta, 
                         const Vector& u )
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
    for( i = 0; i < 2; i++ ) {
      for( k = 0; k < 2; k++ ) {
        w(k) = e(k,i);
      }
      u.Rotate( w, theta );
      for( k = 0; k < 2; k++ ) {
        e(k,i) = w(k);
      }
    }
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

