/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetCVector.h
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

#include <iostream>
#include <iomanip>

#include <math.h>
#include "JetC.h"
#include "VectorD.h"
#include "TMatrix.h"

class JetVector;

typedef char (*MX_R_FUNCCPTR)(const IntArray&, const FNAL::Complex&);

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
              const JetC__environment*     = JetC::lastEnv );
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
  JetCVector        operator*      ( const FNAL::Complex& ) const;
  JetCVector        operator*      ( const double&  ) const;
  friend JetCVector operator*      ( const JetC&, const JetCVector& );
  friend JetCVector operator*      ( const FNAL::Complex&, const JetCVector& );
  friend JetCVector operator*      ( const double&,  const JetCVector& );
  JetCVector        operator*=     ( const JetC&    );
  JetCVector        operator*=     (       FNAL::Complex  );
  JetCVector        operator/      ( const JetC&    ) const;
  JetCVector        operator/      (       FNAL::Complex  ) const;
  JetCVector        operator/=     ( const JetC&    );
  JetCVector        operator/=     (       FNAL::Complex  );


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
  char          operator==     ( const FNAL::Complex&    ) const;
  char          operator!=     ( const JetCVector& ) const;
  char          operator!=     ( const FNAL::Complex&    ) const;
  char          operator<      ( const JetCVector& ) const;
  char          operator<=     ( const JetCVector& ) const;
  char          operator>      ( const JetCVector& ) const;
  char          operator>=     ( const JetCVector& ) const;
  char          IsNull         () const;
  char          IsUnit         () const;
  char          IsNilpotent    () const;


  // Functions related to differentiation
  void weightedDerivative( int*, FNAL::Complex* );
  void derivative( int*, FNAL::Complex* );
  

  // Queries ...
  int  Dim() const;
  JetC__environment* Env() const;
  int AccuWgt() const;
  int Weight()  const;
  void standardPart( FNAL::Complex* ) const;
  void getReference( FNAL::Complex* r ) const;

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
  friend istream& operator>>( istream&, JetCVector& );

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
