/*
 * Header file for class JetCVector
 * 
 * Version 1.0       April, 1995
 * 
 * Leo Michelotti
 * michelot@calvin.fnal.gov
 * 
 */


#ifndef JETCVECTOR_H
#define JETCVECTOR_H

#ifdef __VISUAL_CPP__
#include <iostream>
#include <iomanip>
using std::cout;
using std::setw;
using std::setprecision;
#else
#include <iostream.h>
#endif

#include <math.h>
#include "JetC.h"
#include "VectorD.h"
#include "Matrix.h"

class JetVector;

typedef char (*MX_R_FUNCCPTR)(const IntArray&, const Complex&);

class JetCVector 
{
protected:
  int                 dim;
  JetC*               comp;
  JetC__environment*  myEnv;

public:
  // Constructors and the destructor ...
  JetCVector( const int&  /* dim */        = JetC::lastEnv->NumVar,
              const JetC* /* components */ = 0, 
                    JetC__environment*     = JetC::lastEnv );
  JetCVector( const JetCVector& );
  ~JetCVector();

  void        Reconstruct();
  void        Reconstruct( const int&, JetC__environment* pje );

  // Assignment ...
  void             Set              ( const JetC* );
  void             SetComponent     ( const int&, const JetC& );
  JetC&             operator()       ( const int& ) const; 
                   // return or set component
                   // WARNING: There is no way to use this and be
                   //          assured that environments are consistent.

  // Algebraic functions ...
  JetCVector&       operator=      ( const JetCVector& );
  JetCVector&       operator=      ( const JetVector&  );

  JetCVector        operator+      ( const JetCVector& ) const;
  JetCVector        operator+=     ( const JetCVector& );
  friend JetCVector operator-      ( const JetCVector& );
  JetCVector        operator-      ( const JetCVector& ) const;
  JetCVector        operator-=     ( const JetCVector& );
  JetCVector        operator*      ( const JetC&    ) const;
  JetCVector        operator*      ( const Complex& ) const;
  JetCVector        operator*      ( const double&  ) const;
  friend JetCVector operator*      ( const JetC&, const JetCVector& );
  friend JetCVector operator*      ( const Complex&, const JetCVector& );
  friend JetCVector operator*      ( const double&,  const JetCVector& );
  JetCVector        operator*=     ( const JetC&    );
  JetCVector        operator*=     (       Complex  );
  JetCVector        operator/      ( const JetC&    ) const;
  JetCVector        operator/      (       Complex  ) const;
  JetCVector        operator/=     ( const JetC&    );
  JetCVector        operator/=     (       Complex  );


  JetC              operator*      ( const JetCVector& ) const; // dot product
  JetCVector        operator^      ( const JetCVector& ) const; // cross product:
                                                                // only works if
                                                                // the vector is
                                                                // three-dimensional
  JetCVector        operator^      ( Vector ) const;
  friend
  JetCVector        operator^      ( Vector, const JetCVector& );

  friend JetCVector operator*( /* const */ MatrixC&, const JetCVector& );
  // friend CMapping operator*( /* const */ MatrixC&, const Mapping& );

  // Boolean functions ...
  char          operator==     ( const JetCVector& ) const;
  char          operator==     ( const Complex&    ) const;
  char          operator!=     ( const JetCVector& ) const;
  char          operator!=     ( const Complex&    ) const;
  char          operator<      ( const JetCVector& ) const;
  char          operator<=     ( const JetCVector& ) const;
  char          operator>      ( const JetCVector& ) const;
  char          operator>=     ( const JetCVector& ) const;
  char          IsNull         () const;
  char          IsUnit         () const;
  char          IsNilpotent    () const;


  // Functions related to differentiation
  void weightedDerivative( int*, Complex* );
  void derivative( int*, Complex* );
  

  // Queries ...
  int  Dim() const;
  JetC__environment* Env() const;
  int AccuWgt() const;
  int Weight()  const;
  void standardPart( Complex* ) const;
  void getReference( Complex* r ) const;

  // Utilities ..
  void        peekAt           () const;
  void        printCoeffs      () const;
  JetC        Norm             () const;
  JetCVector  Unit             () const;           // returns unit vector
  void        Rotate           ( JetCVector& v, 
                                 double theta ) const;
  void        Rotate           ( JetCVector& v, 
                                 const JetC& theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend ostream& operator<<( ostream&, const JetCVector& );

  JetCVector filter( int, int );
  JetCVector filter( MX_R_FUNCCPTR [] );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;



// Inline functions ...

inline int  JetCVector::Dim() const { return dim; }

inline JetC__environment* JetCVector::Env() const { return myEnv; }



#endif
