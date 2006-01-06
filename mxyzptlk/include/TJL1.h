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
#include <JL1Ptr.h>
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
std::ostream& operator<<( std::ostream& os, const TJL1<T>& x ); 

template<typename T>
std::istream& operator>>( std::istream& is,  TJL1<T>& x ); 

JL1Ptr<double>::Type real( const JL1Ptr<std::complex<double> >::Type & z ); 
JL1Ptr<double>::Type imag( const JL1Ptr<std::complex<double> >::Type & z );

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator*(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y);

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator*(boost::intrusive_ptr<TJL1<T> > const& x, T const& y );

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator/(T const& x, boost::intrusive_ptr<TJL1<T> > const& y);

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator/(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y);

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator+(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y);

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator-(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y);

template<typename T>
boost::intrusive_ptr<TJL1<T> > operator-(boost::intrusive_ptr<TJL1<T> > const& x);

// these functions are called by TJet
 
template<typename T>
boost::intrusive_ptr<TJL1<T> > makeJL( typename EnvPtr<T>::Type pje, T x );

template<typename T>
void discardJL( TJL1<T>* p);  

template<typename T>
boost::intrusive_ptr<TJL1<T> > cloneJL( TJL1<T>* p);  

//***********************************************************************************************************************************


template<typename T> 
class TJL1: public ReferenceCounter {

  friend class TJL1<double>;
  friend class TJL1<std::complex<double> >;
 
 private: 

  int                                 _count;      // The number of terms in the variable ( in this case, dimension of _jcb +1 )
  int                                 _weight;     // The maximum weight of (the actual) terms  (set to 1)   
  int                                 _accuWgt;    // Highest weight computed accurately        ( set to 1 )

  mutable typename EnvPtr<T>::Type    _myEnv; // Environment of the jet.
  
  T                                   _std;  // standard part
  T*                                  _jcb;  // jacobian

  int _constIterPtr;
  int _iterPtr;

  static std::vector<TJL1<T>* > TJL1<T>::_thePool;  // pool of discarded TJL1 objects

  void insert( TJLterm<T> const& );
  void append( TJLterm<T> const& );

  TJL1( typename EnvPtr<T>::Type, T value = T() );
  TJL1( const IntArray&, const T&,  typename EnvPtr<T>::Type );
  TJL1( const TJL1& );

  virtual ~TJL1();


  // Constructors and destructors (factory functions)_____________________________________

 public:

  // factory functions 

  static TJL1<T>* makeTJL( typename EnvPtr<T>::Type pje,  T value = T());
  static TJL1<T>* makeTJL( const IntArray&, const T&, typename EnvPtr<T>::Type pje );
  static TJL1<T>* makeTJL( const TJL1& );
  static TJL1<T>* makeTJL( const TJL1* );
  static void discardTJL( TJL1<T>* p);  

  
  // functions used to implement COW semantics ______________________________________________


  virtual void dispose() { TJL1<T>::discardTJL(  this ); }  // used by ReferenceCounter class 
  inline typename JL1Ptr<T>::Type clone() 
                           {return typename JL1Ptr<T>::Type( TJL1<T>::makeTJL(*this) ); } 


  // Public member functions__________________________________________


  operator JL1Ptr<std::complex<double> >::Type () const;
  operator JL1Ptr<double>::Type () const;

  void clear();     // clears all the terms. 

  int                      getCount()   { return _count;   }
  int                      getWeight()  { return _weight;  }
  int                      getAccuWgt() { return _accuWgt; }

 
  typename EnvPtr<T>::Type getEnv() const { return _myEnv; }    
  void                     setEnv( typename EnvPtr<T>::Type pje) { _myEnv = pje; }  // DANGER !  

  typename JL1Ptr<T>::Type  truncMult( typename JL1Ptr<T>::Type const& v, const int& wl )  const; 

  typename JL1Ptr<T>::Type  filter( const int& wgtLo, const int& wgtHi )      const; 

  typename JL1Ptr<T>::Type  filter( bool (*f) ( const IntArray&, const T& ) ) const; 


  void printCoeffs() const;               // prints term coefficients 
  void peekAt() const;                     

  TJLterm<T>* get();                      // Pops the top term, which should be the
                                          // one of lowest weight.
  TJLterm<T>  firstTerm() const;    
                                          // Returns a TJLterm<T> equivalent 
                                          // to the top term,
                                          // which should be the one of lowest weight.
  TJLterm<T>  lowTerm()   const;    
                                          // Returns a TJLterm<T> equivalent to the
                                          // non-zero term of lowest weight.
  void addTerm( const TJLterm<T>& );      
 
 
  bool isNilpotent() const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T* ) const;
  void   scaleBy( T );

  TJL1& Negate();
  void add     ( typename JL1Ptr<T>::Type const&  x);    // in place addition 

  typename JL1Ptr<T>::Type sin()              const;
  typename JL1Ptr<T>::Type cos()              const;
  typename JL1Ptr<T>::Type asin()             const;
  typename JL1Ptr<T>::Type atan()             const;
  typename JL1Ptr<T>::Type sqrt()             const;
  typename JL1Ptr<T>::Type exp()              const;
  typename JL1Ptr<T>::Type pow(int)           const;
  typename JL1Ptr<T>::Type pow(const double&) const;
  typename JL1Ptr<T>::Type log()              const;
  typename JL1Ptr<T>::Type compose(typename JL1Ptr<T>::Type const y[ ]) const; 
  typename JL1Ptr<T>::Type TJL1<T>::D( const int* n ) const; 
 
  //void               resetConstIterator();
  //TJLterm<T>         stepConstIterator()     const;
  //const TJLterm<T>& stepConstIteratorRef()  const;
  //const TJLterm<T>* stepConstIteratorPtr()  const;
  //void               resetIterator();
  //TJLterm<T>*        stepIterator();

  void setVariable( const T&, const int& );
  void setVariable( const T& x, const int& j, typename EnvPtr<T>::Type pje );
  void setVariable( const int&, typename EnvPtr<T>::Type pje );               

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

  friend   bool operator==<>( const TJL1<T>& x, const TJL1<T>& y ); 
  friend   bool operator==<>( const TJL1<T>& x, const T& y ); 
  friend   bool operator==<>( const T& y, const TJL1<T>& x );
  friend   bool operator!=<>( const TJL1<T>& x, const TJL1<T>& y ); 
  friend   bool operator!=<>( const TJL1<T>& x, const T& y ); 
  friend   bool operator!=<>( const T& x, const TJL1<T>& y ); 

  friend   std::ostream& operator<< <T>( std::ostream& os, const TJL1<T>& x ); 
  friend   std::istream& operator>><T>(  std::istream& is,  TJL1<T>& x ); 

  friend typename JL1Ptr<double>::Type real( const JL1Ptr<std::complex<double> >::Type & z ); 
  friend typename JL1Ptr<double>::Type imag( const JL1Ptr<std::complex<double> >::Type & z );

  friend boost::intrusive_ptr<TJL1<T> >  operator*<>(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y);
  friend boost::intrusive_ptr<TJL1<T> >  operator*<>(boost::intrusive_ptr<TJL1<T> > const& x, T const& y );

  friend boost::intrusive_ptr<TJL1<T> >  operator/<>(boost::intrusive_ptr<TJL1<T> > const& x,  boost::intrusive_ptr<TJL1<T> > const& y);
  friend boost::intrusive_ptr<TJL1<T> >  operator/<>(T const& x,         boost::intrusive_ptr<TJL1<T> > const& y);

  friend boost::intrusive_ptr<TJL1<T> >  operator+<>(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y);

  friend boost::intrusive_ptr<TJL1<T> >  operator-<>(boost::intrusive_ptr<TJL1<T> > const& x);
  friend boost::intrusive_ptr<TJL1<T> >  operator-<>(boost::intrusive_ptr<TJL1<T> > const& x, boost::intrusive_ptr<TJL1<T> > const& y);

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


template<>                 TJL1<double>::operator JL1Ptr<std::complex<double> >::Type () const;

template<>  TJL1<std::complex<double> >::operator JL1Ptr<double>::Type () const;                // not implemented
template<>  TJL1<double>::operator JL1Ptr<double>::Type () const;                               // not implemented
template<>  TJL1<std::complex<double> >::operator JL1Ptr<std::complex<double> >::Type () const; // not implemented


#ifdef  MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJL1.tcc>
#endif

#endif // TJL1_H
