/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TML.tcc
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
******             Email: michelotti@fnal.gov                         
****** 
******  Revision (Sep 2005):
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov                                   
******             
******   - reorganized code to support explicit template instantiations
******   
******                                                                
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <TML.h>

using namespace std;

template<typename T>
TML<T>::TML() 
{
  _m = 0;
  _r = 0;
  _c = 0;
  _rc = 1;
}


template<typename T>
TML<T>::TML(int rows, int columns, T initval) {
  int i,sz;
  _r = rows;
  _c = columns;
  _m = new T* [_r];
  _rc = 1;

  sz = _r*_c;
  T* dataPtr = new T [ sz ];
  for( i = 0; i < _r; i++ ) { 
    _m[i] = dataPtr;
    dataPtr += _c;
  }

  for( dataPtr = &(_m[0][0]); dataPtr <= &(_m[_r-1][_c-1]); dataPtr++ )
  {
    *dataPtr = initval;
  }
}


template<typename T>
TML<T>::TML(int rows, int columns, const T* values) {
  int i, sz;
  _r = rows;
  _c = columns;
  _m = new T* [_r];
  _rc = 1;

  sz = _r*_c;
  T* dataPtr = new T [ sz ];
  for( i = 0; i < _r; i++ ) { 
    _m[i] = dataPtr;
    dataPtr += _c;
  }

  memcpy( (void*) (_m[0]), (void*) values, sz*sizeof(T) );
}


template<typename T>
TML<T>::TML( const TML<T>& X ) 
{
  static char firstCall= 1;
  if(firstCall) {
    T w;
    firstCall = 0;
    cerr <<   "*** WARNING *** The TML<"
         << typeid(w).name()
         << "> copy constructor was called for"
         << "\n*** WARNING *** some inexplicable reason."
         << "\n*** WARNING *** You have a serious problem!" 
         << endl;
  }

  int i, sz;
  _r = X._r;
  _c = X._c;
  _m = new T* [_r];
  _rc = 1;

  sz = _r*_c;
  T* dataPtr = new T [ sz ];
  for( i = 0; i < _r; i++ ) { 
    _m[i] = dataPtr;
    dataPtr += _c;
  }

  memcpy( (void*) (_m[0]), (void*) ((X._m)[0]), sz*sizeof(T) );
}


template<typename T>
TML<T>::~TML() {
  _clear();
}

template<typename T>
void TML<T>::_clear() {
  if((_r == 0) && (_c == 0)) return;
  delete [] _m[0];
  delete [] _m;
  _r = 0;
  _c = 0;
}


template<typename T>
TML<T>& TML<T>::operator=(const TML& X) 
{
  if( this != &X ) {
    _clear();

    int i, sz;
    _r = X._r;
    _c = X._c;
    _m = new T* [_r];

    sz = _r*_c;
    T* dataPtr = new T [ sz ];
    for( i = 0; i < _r; i++ ) { 
      _m[i] = dataPtr;
      dataPtr += _c;
    }

    memcpy( (void*) (_m[0]), (void*) ((X._m)[0]), sz*sizeof(T) );
  }

  return *this;
}


template<typename T>
bool TML<T>::operator==(const TML<T>& x)
{
  if((x._r != _r) || (x._c != _c)) { return false; }
  for(int i = 0; i < _r; i++) {
    for(int j = 0; j < _c; j++) {
      if(x._m[i][j] != _m[i][j]) { return false; }
    }
  }
  return true;
}


template<typename T>
bool TML<T>::operator==(const T& x) 
{
  if(_r != _c) return false;
  for(int i=0; i< _r; i++) {
    if(_m[i][i] != x) return false;
    for(int j =0; j < _c; j++) {
      if((i != j) && (_m[i][j] != 0.0)) { return false; }
    }
  }
  return true;
}


template<typename T>
TML<T>& TML<T>::operator+=( const T& x) 
{
  for(int i = 0; i < _r; i++) {
    _m[i][i] += x;
    if(std::abs(_m[i][i]) < M_SMALL*std::abs(x)) {
      _m[i][i] = 0;
    }
  }
  return *this;
}


template<typename T>
TML<T>& TML<T>::operator-=( const T& x) 
{
  for(int i = 0; i < _r; i++) {
    _m[i][i] -= x;
    if(std::abs(_m[i][i]) < M_SMALL*std::abs(x)) {
      _m[i][i] = 0;
    }
  }
  return *this;
}


template<typename T>
bool operator==(const TML<T>& x, const TML<T>& y) 
{
  if((x._r != y._r) || (x._c != y._c)) { return false; }
  for(int i = 0; i < x._r; i++) {
    for(int j = 0; j < x._c; j++) {
      if(x.m[i][j] != y.m[i][j]) return false;
    }
  }
  return true;
}


