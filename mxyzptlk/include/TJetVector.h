/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetVector.h
******  Version:   1.0
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
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


#ifndef TJETVECTOR_H
#define TJETVECTOR_H

#include <iostream>
#include <iomanip>
#include <math.h>

#include "TJet.h"

class Vector;

template<typename T1, typename T2>
class TJetVector 
{
protected:
  short int       _dim;
  TJet<T1,T2>*             _comp;
  TJetEnvironment<T1,T2>*  _myEnv;

public:
  // Constructors and the destructor ...
  TJetVector( short int /* dim */ = (TJet<T1,T2>::lastEnvironment())->_numVar,
              const TJet<T1,T2>* /* components */ = 0, 
              const TJetEnvironment<T1,T2>* = (TJet<T1,T2>::lastEnvironment()) );
  TJetVector( const TJetVector& );
  ~TJetVector();

  void             Reconstruct();
  void             Reconstruct( short int, TJetEnvironment<T1,T2>* );

  // Assignment ...
  void             Set             ( const TJet<T1,T2>* );
  void             SetComponent    ( short int, const TJet<T1,T2>& );
  TJet<T1,T2>      operator()      ( short int ) const; 
  TJet<T1,T2>&     operator()      ( short int ); 
                   // WARNING: There is no way to use this and be
                   //          assured that environments are consistent.

  // Algebraic functions ...
  TJetVector&       operator=      ( const TJetVector& );
  TJetVector&       operator=      ( const TJetVector<T2,T1>&  );

  TJetVector        operator+      ( const TJetVector& ) const;
  TJetVector        operator+=     ( const TJetVector& );
  friend TJetVector operator-<>    ( const TJetVector& );
  TJetVector        operator-      ( const TJetVector& ) const;
  TJetVector        operator-=     ( const TJetVector& );
  TJetVector        operator*      ( const TJet<T1,T2>&    ) const;
  TJetVector        operator*      ( const T1& ) const;
  TJetVector        operator*      ( const T2&  ) const;
  friend TJetVector operator*<>    ( const TJet<T1,T2>&, const TJetVector& );
  friend TJetVector operator*<>    ( const T1&, const TJetVector& );
  friend TJetVector operator*<>    ( const T2&, const TJetVector& );
  TJetVector        operator*=     ( const TJet<T1,T2>&    );
  TJetVector        operator*=     (       T1  );
  TJetVector        operator/      ( const TJet<T1,T2>&    ) const;
  TJetVector        operator/      (       T1  ) const;
  TJetVector        operator/=     ( const TJet<T1,T2>&    );
  TJetVector        operator/=     (       T1  );


  TJet<T1,T2>       operator*      ( const TJetVector& ) const; // dot product
  TJetVector        operator^      ( const TJetVector& ) const; // cross product:
                                                                // only works if
                                                                // the vector is
                                                                // three-dimensional
  TJetVector        operator^      ( const Vector& ) const;
  friend TJetVector operator^<>    ( const Vector&, const TJetVector& );

  friend TJetVector operator*<>    ( const MatrixC&, const TJetVector& );

  // Boolean functions ...
  bool operator==     ( const TJetVector& ) const;
  bool operator==     ( const T1&    ) const;
  bool operator==     ( const T2&    ) const;
  bool operator!=     ( const TJetVector& ) const;
  bool operator!=     ( const T1&    ) const;
  bool operator<      ( const TJetVector& ) const;
  bool operator<=     ( const TJetVector& ) const;
  bool operator>      ( const TJetVector& ) const;
  bool operator>=     ( const TJetVector& ) const;
  bool IsNull         () const;
  bool IsUnit         () const;
  bool IsNilpotent    () const;


  // Functions related to differentiation
  void weightedDerivative( int*, T1* );
  void derivative( int*, T1* );
  

  // Queries ...
  short int  Dim() const;
  const TJetEnvironment<T1,T2>* Env() const;
  int AccuWgt() const;
  int Weight()  const;
  void standardPart( T1* ) const;
  void getReference( T1* ) const;

  // Utilities ..
  void        peekAt           () const;
  void        printCoeffs      () const;
  TJet<T1,T2> Norm             () const;
  TJetVector  Unit             () const;           // returns unit vector
  void        Rotate           ( TJetVector& v, double theta ) const;
  void        Rotate           ( TJetVector& v, const TJet<T1,T2>& theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend std::ostream& operator<<<>( std::ostream&, const TJetVector& );
  friend std::istream& operator>><>( std::istream&, TJetVector& );

  TJetVector filter( int, int );
  TJetVector filter( bool (*[]) ( const IntArray&, const T1& ) );
} ;


// Templated friend function declarations
template<typename T1,typename T2> TJetVector<T1,T2> operator-( const TJetVector<T1,T2>& );
template<typename T1,typename T2> TJetVector<T1,T2> operator*( const TJet<T1,T2>&, const TJetVector<T1,T2>& );
template<typename T1,typename T2> TJetVector<T1,T2> operator*( const T1&, const TJetVector<T1,T2>& );
template<typename T1,typename T2> TJetVector<T1,T2> operator*( const T2&, const TJetVector<T1,T2>& );
template<typename T1,typename T2> TJetVector<T1,T2> operator^( const Vector&, const TJetVector<T1,T2>& );
template<typename T1,typename T2> TJetVector<T1,T2> operator*( const MatrixC&, const TJetVector<T1,T2>& );
template<typename T1,typename T2> std::ostream& operator<<( std::ostream&, const TJetVector<T1,T2>& );
template<typename T1,typename T2> std::istream& operator>>( std::istream&, TJetVector<T1,T2>& );


// Inline functions ...

// .. Functions included for symmetry
template<typename T1, typename T2>
inline bool operator==( const T1& x, const TJetVector<T1,T2>& y )
{ return y == x; }

template<typename T1, typename T2>
inline bool operator==( const T2& x, const TJetVector<T1,T2>& y )
{ return y == x; }

template<typename T1, typename T2>
inline bool operator!=( const T1& x, const TJetVector<T1,T2>& y )
{ return !(y == x); }

template<typename T1, typename T2>
inline bool operator!=( const T2& x, const TJetVector<T1,T2>& y )
{ return !(y == x); }


// ... Member functions (aka methods)
template<typename T1, typename T2>
inline short int TJetVector<T1,T2>::Dim() const 
{ return _dim; }

template<typename T1, typename T2>
inline const TJetEnvironment<T1,T2>* TJetVector<T1,T2>::Env() const 
{ return _myEnv; }


typedef TJetVector<double,FNAL::Complex> JetVector;
typedef TJetVector<FNAL::Complex,double> JetCVector;

#endif // TJETVECTOR_H
