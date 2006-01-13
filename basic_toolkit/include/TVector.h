/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TVector.h
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
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******
******  Revision History:
******
******  Dec 2005   Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******
******  - templated version
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TVECTOR_H
#define TVECTOR_H

#include <complex>
#include <iostream>
#include <exception>
#include <string>

#include <math.h>
#include <OutputFormat.h>

template<typename T> 
class TMatrix;

template<typename T> 
class TVector;

template<typename T> 
std::ostream& operator<<( std::ostream& , const TVector<T>& );

template<typename T>
TVector<T> operator-( const TVector<T>& );

template<typename T>
TVector<T> operator*( T, const TVector<T>& );

template<typename T>
TVector<T> operator*( const TVector<T>&, T );

template<typename T>
TVector<T> operator*( const TMatrix<T>&, const TVector<T>& );


template<typename T>
class TVector {

private:
  int           _dim;
  T*            _comp;
  OutputFormat* _ofPtr;

  static OutputFormat* _defOFPtr;  // default OutputFormat

public:
  // Constructors and the destructor __________________________
  TVector( int  dimension= 3, const T* components=0, OutputFormat* ofmt=0 );
  TVector( const TVector& );
  ~TVector();

  // Assignment _____________________________

  void        Set              ( const T* );
  void        set              ( T, T, T );   // anachronistic
  T      operator()            ( int ) const; // return component
  T&     operator()            ( int );       // set    component

  // Algebraic functions ___________________________
  TVector&  operator= ( const TVector& );
  // ... Utility extension
  TVector&  operator= ( const TMatrix<T>& );

  TVector   operator+ ( const TVector& ) const;
  TVector   operator+=( const TVector& );

  friend  TVector<T> operator-<T>(    const TVector& );
  friend  TVector<T> operator*<T>( T, const TVector& );
  friend  TVector<T> operator*<T>( const TVector&, T ); 
  friend  TVector<T> operator*<T>( const TMatrix<T>&, const TVector<T>& );
  friend  std::ostream& operator<< <>( std::ostream& , const TVector<T>& );

  TVector          operator-      ( const TVector& ) const; 
  TVector          operator-=     ( const TVector& ); 
  TVector          operator*=     (       T  );
  TVector          operator/      (       T  ) const;
  TVector          operator/=     (       T  );

  T         operator*      (  const TVector<T>& ) const; // dot product
 
  TVector    operator^      ( const TVector& ) const; // cross product:
                                                        // only works if
                                                        // the vector is
                                                        // three-dimensional

  // Boolean functions ________________________

  bool          operator==     ( const TVector& ) const;
  bool          operator!=     ( const TVector& ) const;
  bool          operator<      ( const TVector& ) const;
  bool          operator<=     ( const TVector& ) const;
  bool          operator>      ( const TVector& ) const;
  bool          operator>=     ( const TVector& ) const;
  bool          IsNull         () const;
  bool          IsUnit         () const;

  // Queries ___________________________

  int  Dim() const { return _dim; }

  // Utilities __________________________


  static void   setDefaultFormat ( const OutputFormat& );
  void          setOutputFormat  ( OutputFormat* x ) { _ofPtr = x; }
  OutputFormat* getOutputFormat()                  { return _ofPtr; }

  TVector      Abs              () const;
  double       Norm             () const;
  TVector      Unit             () const;           // returns unit vector
  void         Rotate           ( TVector& v, double  theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend std::ostream& operator<< <T>(std::ostream& , const TVector<T>& );

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

};

// specializations _________________________________________________________


template <>  
double   TVector<double>::operator* ( const TVector<double>& x ) const; 

template <>  
std::complex<double> TVector<std::complex<double> >::operator* ( const TVector<std::complex<double> >& x ) const; 

template<>
bool TVector<double>::operator> ( const TVector<double>& x ) const;

// This is *not* implemented (it does not make much sense)
template<>
bool TVector<std::complex<double> >::operator> ( const TVector<std::complex<double> >& x ) const;

template<> 
double TVector<double>::Norm () const;

template<> 
double TVector<std::complex<double> >::Norm () const;

template<> 
bool TVector<double>::IsUnit() const;

template<> 
bool TVector<std::complex<double> >::IsUnit() const;


#ifdef BASICTOOLKIT_IMPLICIT_TEMPLATES
#include <TVector.tcc>
#endif

#endif  //TVECTOR_H
