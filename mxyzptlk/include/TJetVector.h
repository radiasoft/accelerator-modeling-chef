/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetVector.h
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
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******
******  Revision History
******                                                                
******  Sept 2005   ostiguy@fnal.gov
******
******              - new code based on a single template parameter
******                instead of two. Mixed mode now handled
******                using conversion operators.
******
******                                                                
******                                                                
**************************************************************************
*************************************************************************/

#ifndef TJETVECTOR_H
#define TJETVECTOR_H

#include <iostream>
#include <iomanip>
#include <math.h>

#include <TJet.h>


// Forward declarations
class Vector;

template<typename T>
class TMatrix;

template<typename T> 
class TJetVector;

template<typename T> 
TJetVector<T> operator-( const TJetVector<T>& );

template<typename T> 
TJetVector<T> operator*( const TJet<T>&, const TJetVector<T>& );

template<typename T> 
TJetVector<T> operator*( const T&, const TJetVector<T>& );

template<typename T> 
TJetVector<T> operator^( const Vector&, const TJetVector<T>& );

template<typename T> 
TJetVector<T> operator*( const TMatrix<T>&, const TJetVector<T>& );

template<typename T> 
std::ostream& operator<<( std::ostream&, const TJetVector<T>& );

template<typename T> 
std::istream& operator>>( std::istream&, TJetVector<T>& );


// Class TJetVector template
template<typename T>
class TJetVector 
{

protected:
  int                  _dim;
  TJet<T>*             _comp;
  TJetEnvironment<T>*  _myEnv;

public:
  // Constructors and the destructor ...
  TJetVector( int /* dim */ = (TJet<T>::lastEnvironment())->_numVar,
              const TJet<T>* /* components */ = 0, 
              const TJetEnvironment<T>* = (TJet<T>::lastEnvironment()) );
  TJetVector( const TJetVector& );
  ~TJetVector();

  // operator TJetVector<std::complex<double> > ();
  
  void             Reconstruct();
  void             Reconstruct( int, TJetEnvironment<T>* );
  void             CopyFrom( const TJetVector& );

  // Assignment ...
  void             Set             ( const TJet<T>* );
  void             SetComponent    ( int, const TJet<T>& );
  TJet<T>      operator()      ( int ) const; 
  TJet<T>&     operator()      ( int ); 
                   // WARNING: There is no way to use this and be
                   //          assured that environments are consistent.

  // Algebraic functions ...
  TJetVector&       operator=      ( const TJetVector& );

  TJetVector        operator+      ( const TJetVector& ) const;
  TJetVector        operator+      ( const Vector& ) const;
  TJetVector        operator+=     ( const TJetVector& );
  TJetVector        operator+=     ( const Vector& );
  friend TJetVector operator-<>    ( const TJetVector& );
  TJetVector        operator-      ( const TJetVector& ) const;
  TJetVector        operator-      ( const Vector& ) const;
  TJetVector        operator-=     ( const TJetVector& );
  TJetVector        operator-=     ( const Vector& );

  friend TJetVector operator*<>    ( const TJet<T>&, const TJetVector& );
  friend TJetVector operator*<>    ( const T&, const TJetVector& );
  friend TJetVector operator*<>    ( const TMatrix<T>&, const TJetVector& );

  TJetVector        operator*      ( const TJet<T>&    ) const;
  TJetVector        operator*      ( const T& ) const;
  TJetVector        operator*=     ( const TJet<T>&    );
  TJetVector        operator*=     (       T  );
  TJetVector        operator/      ( const TJet<T>&    ) const;
  TJetVector        operator/      (       T  ) const;
  TJetVector        operator/=     ( const TJet<T>&    );
  TJetVector        operator/=     (       T  );


  TJet<T>       operator*      ( const TJetVector& ) const; // dot product
  TJet<T>       operator*      ( const Vector& )     const; // dot product

  friend TJetVector operator^<>    ( const Vector&, const TJetVector& );

  TJetVector        operator^      ( const TJetVector& ) const; // cross product:
                                                                // only works if
                                                                // the vector is
                                                                // three-dimensional
  TJetVector        operator^      ( const Vector& ) const;


  // Boolean functions ...
  bool operator==     ( const TJetVector& ) const;
  bool operator==     ( const T&    ) const;
  bool operator!=     ( const TJetVector& ) const;
  bool operator!=     ( const T&    ) const;
  bool operator<      ( const TJetVector& ) const;
  bool operator<=     ( const TJetVector& ) const;
  bool operator>      ( const TJetVector& ) const;
  bool operator>=     ( const TJetVector& ) const;
  bool IsNull         () const;
  bool IsUnit         () const;
  bool IsNilpotent    () const;


  // Functions related to differentiation
  void weightedDerivative( int*, T* );
  void derivative( int*, T* );
  

  // Queries ...
  int  Dim() const;
  const TJetEnvironment<T>* Env() const;
  int AccuWgt() const;
  int Weight()  const;
  void standardPart( T* ) const;
  void getReference( T* ) const;

  // Utilities ..
  void        peekAt           () const;
  void        printCoeffs      () const;
  TJet<T> Norm             () const;
  TJetVector  Unit             () const;           // returns unit vector
  void        Rotate           ( TJetVector& v, double theta ) const;
  void        Rotate           ( TJetVector& v, const TJet<T>& theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend std::ostream& operator<<<>( std::ostream&, const TJetVector& );
  friend std::istream& operator>><>( std::istream&, TJetVector& );

  TJetVector filter( int, int );
  TJetVector filter( bool (*[]) ( const IntArray&, const T& ) );
} ;


template<typename T>
TJetVector<T> operator*( const TJet<T>&, const Vector& );


// Inline functions ...

// .. Functions included for symmetry
template<typename T>
inline bool operator==( const T& x, const TJetVector<T>& y )
{ return y == x; }

template<typename T>
inline bool operator!=( const T& x, const TJetVector<T>& y )
{ return !(y == x); }


// ... Member functions (aka methods)
template<typename T>
inline int TJetVector<T>::Dim() const 
{ return _dim; }

template<typename T>
inline const TJetEnvironment<T>* TJetVector<T>::Env() const 
{ return _myEnv; }

template<typename T>
inline TJetVector<T> operator+( const Vector& x, const TJetVector<T>& y )
{ return y.operator+( x ); }

template<typename T>
inline TJetVector<T> operator-( const Vector& x, const TJetVector<T>& y )
{ return operator-( y.operator-( x ) ); }

template<typename T>
inline TJet<T> operator*( const Vector& x, const TJetVector<T>& y )
{ return y.operator*( x ); }

template<typename T>
TJetVector<T> operator*( const Vector& x, const TJet<T>& y )
{ return operator*(y,x); }

#ifdef MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJetVector.tcc>
#endif

#endif // TJETVECTOR_H
