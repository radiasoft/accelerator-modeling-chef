/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJL1.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******  Authors: Jean-Francois Ostiguy   ostiguy@fnal.gov                                    
******           Leo Michelotti          michelotti@fnal.gov
******
******           Fermilab, Batavia IL 60510                                           
******  
******  NOTE:
******
******  The code in this file is a specialized first order implementation 
******  of the general class TJL, meant to integrate and interoperate as 
******  transparently as possible with it. It borrows liberally from 
******  an earlier specialized first order implementation authored by 
******  Leo Michelotti.  
******
******
**************************************************************************
*************************************************************************/
#ifndef TJL1_H
#define TJL1_H

#include <dlist.h>
#include <IntArray.h>
#include <TMatrix.h>
#include <VectorD.h>
#include <TJetEnvironment.h>
#include <ReferenceCounter.h>
#include <FastPODAllocator.h>
#include <vector>
#include <JLPtr.h>
#include <EnvPtr.h>
#include <TJLterm.h>

#define MX_SMALL       1.0e-12 // Used by TJL<T>::addTerm to decide 
                               // removal of a TJL<T>Cterm.

#define MX_ABS_SMALL   1.0e-15 // Used by TJL<T>::addTerm to decide 

#define MX_MAXITER     20      // Maximum number of iterations allowed
                               //   in iterative routines

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// *******************************************************************

template<typename T>
class TJL1;

template<typename T>
bool operator==( const TJL1<T>& x, const TJL1<T>& y ); 

template<typename T>
bool operator==( const TJL1<T>& x, const T& y ); 

template<typename T>
bool operator==( const T& y, const TJL1<T>& x );

template<typename T>
bool operator!=( const TJL1<T>& x, const TJL1<T>& y ); 

template<typename T>
bool operator!=( const TJL1<T>& x, const T& y ); 

template<typename T>
bool operator!=( const T& x, const TJL1<T>& y ); 

template<typename T>
std::ostream& operator<<( std::ostream& os, const TJL1<T>& x); 

template<typename T>
std::istream& operator>>( std::istream& is,  TJL1<T>&  x); 

JL1Ptr<double> real( const JL1Ptr<std::complex<double> > & z ); 
JL1Ptr<double> imag( const JL1Ptr<std::complex<double> > & z );

//...................................................................................................................................

template<typename T>
JL1Ptr<T>  operator*(JL1Ptr<T> const& x, JL1Ptr<T> const& y);

template<typename T>
JL1Ptr<T>  operator*(JL1Ptr<T> const& x, T const& y );

//...................................................................................................................................

template<typename T>
JL1Ptr<T>  operator+(JL1Ptr<T> const& x, JL1Ptr<T> const& y);

template<typename T>
JL1Ptr<T>& operator+=(JL1Ptr<T> &x,      JL1Ptr<T> const& y  );

//...................................................................................................................................

template<typename T>
JL1Ptr<T>  operator-(JL1Ptr<T> const& x, JL1Ptr<T> const& y);

template<typename T>
JL1Ptr<T>  operator-(JL1Ptr<T> const& x);

//...................................................................................................................................

template<typename T>
JL1Ptr<T>  operator/( JL1Ptr<T> const& x, JL1Ptr<T> const& y);

template<typename T>
JL1Ptr<T>  operator/( JL1Ptr<T> const& x,  T        const& y);

template<typename T>
JL1Ptr<T>  operator/( T const& x,         JL1Ptr<T> const& y);




//***********************************************************************************************************************************


template<typename T> 
class TJL1: public ReferenceCounter<TJL1<T> > {
  
 struct term {
    T      value;
    bool   deleted;
  };

  template <typename U>
  friend class TJL1;
 
 private: 

  int                                 _count;      // The number of terms in the variable ( in this case, dimension of _jcb +1 )
  int                                 _weight;     // The maximum weight of (the actual) terms  (set to 1)   
  int                                 _accuWgt;    // Highest weight computed accurately        ( set to 1 )

  mutable EnvPtr<T>                    _myEnv; // Environment of the jet.
  
  term                                 _std;  // standard part
  term*                                _jcb;  // jacobian

  mutable int                         _constIterPtr;
  int                                 _iterPtr;

  static std::vector<TJL1<T>* > TJL1<T>::_thePool;  // pool of discarded TJL1 objects

  void insert( TJLterm<T> const& );
  void append( TJLterm<T> const& );

  TJL1( EnvPtr<T> const&, T value = T() );
  TJL1( const IntArray&, const T&,  EnvPtr<T> const&);
  TJL1(  TJL1 const& );

  template<typename U> 
  TJL1( TJL1<U> const& );  

  virtual ~TJL1();


  // Constructors and destructors (factory functions)_____________________________________

 public:

  // factory functions 

  static JL1Ptr<T> makeTJL( EnvPtr<T>  const& pje,  T value = T());
  static JL1Ptr<T> makeTJL( const IntArray&, const T&, EnvPtr<T> const& pje );

  template<typename U>
  static JL1Ptr<T> makeTJL( const TJL1<U>& );

  static JL1Ptr<T> makeTJL( const TJL1* ); /// eliminate ?

  static void discardTJL( TJL1<T>* p);  

  
  // functions used to implement COW semantics ______________________________________________


  void dispose()           { TJL1<T>::discardTJL(  this ); }  // used by ReferenceCounter class 
  inline JL1Ptr<T> clone() 
                           {return JL1Ptr<T>( TJL1<T>::makeTJL(*this) ); } 

  // Comaptibility 

  TJLterm<T>* storePtr() {return 0;}                  // returns a ptr to the next available block in the JLterm store;


  // Public member functions__________________________________________

  void clear();     // clears all the terms. 

  int                      getCount()   { return _count;   }
  int                      getWeight()  { return _weight;  }
  int                      getAccuWgt() { return _accuWgt; }

 
  EnvPtr<T>                getEnv() const { return _myEnv; }    
  void                     setEnv( EnvPtr<T> const& pje) { _myEnv = pje; }  // DANGER !  

  JL1Ptr<T>  truncMult( JL1Ptr<T> const& v, const int& wl )  const; 

  JL1Ptr<T>  filter( const int& wgtLo, const int& wgtHi )      const; 

  JL1Ptr<T>  filter( bool (*f) ( const IntArray&, const T& ) ) const; 


  void printCoeffs() const;               // prints term coefficients 
  void peekAt() const;                     

  TJLterm<T>* get();                      // Pops the top term (lowest weight).
  //TJLterm<T>  get();                      // Pops the top term (lowest weight).

  TJLterm<T>  firstTerm() const;          // Returns a TJLterm<T> equivalent 
                                          // to the top term but does not remove it
 
  TJLterm<T>  lowTerm()   const;          // Returns a TJLterm<T> equivalent to the
                                          // **non-zero** term of lowest weight.

  void addTerm( const TJLterm<T>& );      
 
  bool isNilpotent() const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T* ) const;
  void   scaleBy( T );

  TJL1& Negate();
  void add     ( JL1Ptr<T> const&  x);    // in place addition 

  JL1Ptr<T> sin()              const;
  JL1Ptr<T> cos()              const;
  JL1Ptr<T> asin()             const;
  JL1Ptr<T> atan()             const;
  JL1Ptr<T> sqrt()             const;
  JL1Ptr<T> exp()              const;
  JL1Ptr<T> pow(int)           const;
  JL1Ptr<T> pow(const double&) const;
  JL1Ptr<T> log()              const;
  JL1Ptr<T> compose(JL1Ptr<T> const y[ ]) const; 
  JL1Ptr<T> TJL1<T>::D( const int* n ) const; 
 
  void               resetConstIterator();
  const TJLterm<T>*  stepConstIteratorPtr()  const;


  void setVariable( const T&, const int& );
  void setVariable( const T& x, const int& j, EnvPtr<T> const& pje );
  void setVariable( const int&, EnvPtr<T> const& pje );               

  T standardPart() const;

  T weightedDerivative( const int* ) const;
  T derivative( const int* ) const;

  T operator()( const Vector& ) const;
  T operator()( const T* ) const;
                       // Performs a multinomial evaluation of
                       // the TJL1 variable.  Essentially acts as a
                       // power series expansion.
  TJL1& operator()( const TJL1* ) const;
                       // Self explanatory ...

  friend   bool operator==<>( const TJL1<T>& x, const TJL1<T>& y ); 
  friend   bool operator==<>( const TJL1<T>& x, const T& y ); 
  friend   bool operator==<>( const T& y, const TJL1<T>& x );
  friend   bool operator!=<>( const TJL1<T>& x, const TJL1<T>& y ); 
  friend   bool operator!=<>( const TJL1<T>& x, const T& y ); 
  friend   bool operator!=<>( const T& x, const TJL1<T>& y ); 

  friend   std::ostream& operator<< <T>( std::ostream& os, const TJL1<T>& x ); 
  friend   std::istream& operator>><T>(  std::istream& is,  TJL1<T>& x ); 

  friend JL1Ptr<double> real( const JL1Ptr<std::complex<double> >& z ); 
  friend JL1Ptr<double> imag( const JL1Ptr<std::complex<double> >& z );

  friend JL1Ptr<T>  operator*<>(JL1Ptr<T> const& x, JL1Ptr<T> const& y);
  friend JL1Ptr<T>  operator*<>(JL1Ptr<T> const& x, T const& y );

  friend JL1Ptr<T>  operator/<>(JL1Ptr<T> const& x, JL1Ptr<T> const& y);
  friend JL1Ptr<T>  operator/<>(JL1Ptr<T> const& x, T         const& y);
  friend JL1Ptr<T>  operator/<>(T const& x,         JL1Ptr<T> const& y);


  friend JL1Ptr<T>  operator+<>  ( JL1Ptr<T>  const& x,    JL1Ptr<T> const& y );
  friend JL1Ptr<T>& operator+= <>( JL1Ptr<T>       & x,    JL1Ptr<T> const& y );

  friend JL1Ptr<T>  operator-<>(JL1Ptr<T> const& x);
  friend JL1Ptr<T>  operator-<>(JL1Ptr<T> const& x, JL1Ptr<T> const& y);

  // Arithmetic operators // some of these may no longer be needed ... FIXME !

  TJL1& operator=( const TJL1& );
  TJL1& operator=( const T& );

  TJL1& operator+=( const TJL1& );
  TJL1& operator+=( const T& );

  TJL1& operator-=( const TJL1& );
  TJL1& operator-=( const T& );

  TJL1& operator*=( const TJL1& );
  TJL1& operator*=( const T& );

  TJL1& operator/=( const TJL1& );
  TJL1& operator/=( const T& );

    // Exception subclasses____________________________________________

  struct BadDimension: public std::exception
  {
    BadDimension( int, int, std::string, int, const char* = "", const char* = "" );
    // A binary operator has detected mismatch between its two arguments 
    // in number of Jet coordinates.
    // 1st argument: IntArray dimension of operator's first argument
    // 2nd         : IntArray dimension of operator's second argument
    // 3rd         : name of file in which exception is thrown
    // 4th         : line from which exception is thrown
    // 5th         : identifies function containing throw
    // 6th         : identifies type of error
    ~BadDimension() throw() {}
    const char* what() const throw();
    std::string errorString;
    int xdim, ydim;
  };

  struct BadEnvironment: public std::exception
  {
    BadEnvironment( int, int, int, std::string, int, const char* = "", const char* = "" );
    // Thrown by JetVector copy constructor: environment mismatch
    // in a component. THIS SHOULD NEVER HAPPEN.
    // 1st argument: component index
    // 2nd         : address of component's environment
    // 3rd         : address of JetVector environment
    // 4th         : name of file in which exception is thrown
    // 5th         : line from which exception is thrown
    // 6th         : identifies function containing throw
    // 7th         : identifies type of error
    ~BadEnvironment() throw() {}
    const char* what() const throw();
    std::string errorString;
    int i, env_1, env_2;
  };

  struct BadReference: public std::exception
  {
    BadReference( int, double, std::string, int, const char* = "", const char* = "" );
    // Attempt to convert a JetC__environment with non-real
    // reference point into a TJetEnvironment<T>.
    // 1st argument: reference point component index
    // 2nd         : imaginary part of component
    // 3rd         : name of file in which exception is thrown
    // 4th         : line from which exception is thrown
    // 5th         : identifies function containing throw
    // 6th         : identifies type of error
    ~BadReference() throw() {}
    const char* what() const throw();
    std::string errorString;
    int i;
    double im;
  };

  struct HorribleException : public std::exception
  {
    HorribleException( int, int, int, 
                       std::string, int, const char* = "", const char* = "" );
    // Thrown from Jet::EndEnvironment if index numbers don't align correctly.
    // This should be impossible.
    // 1st argument: currentIndex
    // 2nd         : newCoords.size()
    // 3rd         : newValues.size()
    // 4th         : name of file in which exception is thrown
    // 5th	   : line from which exception is thrown
    // 6th         : identifies function containing throw
    // 7th         : identifies type of error
    ~HorribleException() throw() {}
    const char* what() const throw();
    std::string errorString;
    int curIndex, coordSize, valueSize;
  };

  struct HideousException : public std::exception
  {
    HideousException( int, int, 
                       std::string, int, const char* = "", const char* = "" );
    // Thrown from Jet::EndEnvironment if index numbers don't align correctly.
    // This should be impossible.
    // 1st argument: currentIndex
    // 2nd         : newCoords.size()
    // 3rd         : newValues.size()
    // 4th         : name of file in which exception is thrown
    // 5th	   : line from which exception is thrown
    // 6th         : identifies function containing throw
    // 7th         : identifies type of error
    ~HideousException() throw() {}
    const char* what() const throw();
    std::string errorString;
    int i, n;
  };
};

//  ================== specializations ====================


template<> 
TJL1<std::complex<double> >::TJL1( TJL1<double> const& );  

template<> 
JL1Ptr<std::complex<double> >  TJL1<std::complex<double> >::makeTJL( TJL1<double> const& );


#ifdef  MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJL1.tcc>
#endif

#endif // TJL1_H
