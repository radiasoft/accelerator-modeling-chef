/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetVector.h
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


/*
 * Header file for class JetVector
 * 
 * Version 1.0       April, 1995
 * 
 * Leo Michelotti
 * michelot@calvin.fnal.gov
 * 
 */


#ifndef JETVECTOR_H
#define JETVECTOR_H

#include <iostream>

#include <math.h>
#include "Jet.h"
#include "VectorD.h"
#include "TMatrix.h"

class JetCVector;

typedef char (*MX_R_FUNCPTR)(const IntArray&, const double&);

class JetVector 
{
protected:
  int                dim;
  Jet*               comp;
  Jet__environment*  myEnv;

public:
  // Constructors and the destructor ...
  JetVector( const int&  /* dim */        = Jet::lastEnv->NumVar,
             const Jet*  /* components */ = 0, 
             const Jet__environment*      = Jet::lastEnv );
  JetVector( const JetVector& );
  ~JetVector();

  void        Reconstruct();
  void        Reconstruct( const int&, Jet__environment* pje );
  void        Reconstruct( const int& );
  void        CopyFrom( const JetVector& );

  // Assignment ...
  void             Set              ( const Jet* );
  void             SetComponent     ( const int&, const Jet& );
  Jet&             operator()       ( const int& ) const; 
                   // return or set component
                   // WARNING: There is no way to use this and be
                   //          assured that environments are consistent.

  // Algebraic functions ...
  JetVector&       operator=      ( const JetVector&   );
  JetVector&       operator=      ( const JetCVector&  );

  JetVector        operator+      ( const JetVector& ) const;
  friend JetVector operator+      ( const JetVector&, 
                                    const    Vector& );
  friend JetVector operator+      ( const    Vector&, 
                                    const JetVector& );
  JetVector        operator+=     ( const JetVector& );
  JetVector        operator+=     ( const    Vector& );
  friend JetVector operator-      ( const JetVector& );
  JetVector        operator-      ( const JetVector& ) const;
  friend JetVector operator-      ( const JetVector&, 
                                    const    Vector& );
  friend JetVector operator-      ( const    Vector&, 
                                    const JetVector& );
  JetVector        operator-=     ( const JetVector& );
  JetVector        operator-=     ( const    Vector& );
  JetVector        operator*      ( const Jet&    ) const;
  JetVector        operator*      (     double    ) const;
  friend JetVector operator*      ( const Jet&, const JetVector& );
  friend JetVector operator*      (     double, const JetVector& );
  JetVector        operator*=     ( const Jet&    );
  JetVector        operator*=     (     double    );
  JetVector        operator/      ( const Jet&    ) const;
  JetVector        operator/      (     double    ) const;
  JetVector        operator/=     ( const Jet&    );
  JetVector        operator/=     (     double    );


  Jet              operator*      ( const JetVector& ) const; // dot product
  friend Jet       operator*      ( const JetVector&, 
                                    const Vector& );
  friend Jet       operator*      ( const Vector&, 
                                    const JetVector& );
  JetVector        operator^      ( const JetVector& ) const; // cross product:
                                                        // only works if
                                                        // the vector is
                                                        // three-dimensional
  JetVector        operator^      ( Vector ) const;
  friend
  JetVector        operator^      ( Vector, const JetVector& );

  friend JetVector operator*( /* const */ MatrixD&, const JetVector& );
  // friend CMap operator*( /* const */ MatrixC&, const Map& );

  // Boolean functions ...
  char          operator==     ( const JetVector& ) const;
  char          operator==     ( const double&    ) const;
  char          operator!=     ( const JetVector& ) const;
  char          operator!=     ( const double&    ) const;
  char          operator<      ( const JetVector& ) const;
  char          operator<=     ( const JetVector& ) const;
  char          operator>      ( const JetVector& ) const;
  char          operator>=     ( const JetVector& ) const;
  char          IsNull         () const;
  char          IsUnit         () const;
  char          IsNilpotent    () const;


  // Functions related to differentiation
  void weightedDerivative( int*, double* );
  void derivative( int*, double* );
  

  // Queries ...
  int  Dim() const;
  Jet__environment* Env() const;
  int AccuWgt() const;
  int Weight()  const;
  void standardPart( double* ) const;
  void getReference( double* r ) const;

  // Utilities ..
  void        peekAt           () const;
  void        printCoeffs      () const;
  Jet         Norm             () const;
  JetVector   Unit             () const;           // returns unit vector
  void        Rotate           ( JetVector& v, 
                                 double theta ) const;
  void        Rotate           ( JetVector& v, 
                                 const Jet& theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend ostream& operator<<( ostream&, const JetVector& );
  friend istream& operator>>( istream&, JetVector& );

  JetVector filter( int, int );
  JetVector filter( MX_R_FUNCPTR [] );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;



// Multiplication of Jets and Vectors

JetVector operator*( const Jet&, const Vector& );


// Inline functions ...

inline int JetVector::Dim() const 
{ return dim; }

inline Jet__environment* JetVector::Env() const 
{ return myEnv; }

inline JetVector operator*( const Vector& x, const Jet& y )
{ return y*x; }

#endif  // JETVECTOR_H
