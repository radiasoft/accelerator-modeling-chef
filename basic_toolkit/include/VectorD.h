/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      VectorD.h
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
 * Header file for class Vector
 * 
 * Version 1.0       Sep, 1991
 *         2.0       Mar, 1995
 *         2.1       Apr, 1995
 * 
 * Leo Michelotti
 * michelot@calvin.fnal.gov
 * 
 */


#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <exception>
#include <string>

#include <math.h>
#include "TMatrix.h"     // Needed only for final utility.
#include "OutputFormat.h"

class Vector 
{
private:
  int           dim;
  double*       comp;
  OutputFormat* ofPtr;

  static OutputFormat* defOFPtr;  // default OutputFormat

public:
  // Constructors and the destructor ...
  Vector(       int      /* dimension */  = 3,
          const double*  /* components */ = 0,
                OutputFormat*             = 0 );
  Vector( const Vector& );
  ~Vector();

  // Assignment ...
  void        Set              ( const double* );
  void        set              ( double, double, double );  // anachronistic
  double      operator()       ( int ) const; // return component
  double&     operator()       ( int );       // set    component

  // Algebraic functions ...
  Vector&       operator=      ( const Vector& );

  Vector        operator+      ( const Vector& ) const;
  Vector        operator+=     ( const Vector& );
  friend Vector operator-      ( const Vector& );
  Vector        operator-      ( const Vector& ) const;
  Vector        operator-=     ( const Vector& );
  Vector        operator*      (       double  ) const;
  friend Vector operator*      (       double, const Vector& );
  Vector        operator*=     (       double  );
  Vector        operator/      (       double  ) const;
  Vector        operator/=     (       double  );

  double        operator*      ( const Vector& ) const; // dot product
  Vector        operator^      ( const Vector& ) const; // cross product:
                                                        // only works if
                                                        // the vector is
                                                        // three-dimensional

  // Boolean functions ...
  char          operator==     ( const Vector& ) const;
  char          operator!=     ( const Vector& ) const;
  char          operator<      ( const Vector& ) const;
  char          operator<=     ( const Vector& ) const;
  char          operator>      ( const Vector& ) const;
  char          operator>=     ( const Vector& ) const;
  char          IsNull         () const;
  char          IsUnit         () const;

  // Queries ...
  int  Dim() const { return dim; }

  // Utilities ..
  static 
  void        setDefaultFormat ( const OutputFormat& );
  void        setOutputFormat  ( OutputFormat* x ) { ofPtr = x; }
  OutputFormat* getOutputFormat()                  { return ofPtr; }

  Vector      Abs              () const;
  double      Norm             () const;
  Vector      Unit             () const;           // returns unit vector
  void        Rotate           ( Vector& v, 
                                 double  theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend std::ostream& operator<<( std::ostream&, const Vector& );

  // Exceptions ...
  struct GenericException : public std::exception
  {
    GenericException( const char* fcn, const char* msg );
    // 1st argument: identifies function containing throw
    // 2nd         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string w;
  };

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;


// Utilities
Vector operator*( const TMatrix<double>&, const Vector& );

#endif  // VECTOR_H
