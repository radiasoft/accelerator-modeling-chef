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

#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/TMatrix.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/TJetEnvironment.h>
#include <basic_toolkit/ReferenceCounter.h>
#include <gms/FastPODAllocator.h>
#include <vector>
#include <mxyzptlk/JLPtr.h>
#include <mxyzptlk/EnvPtr.h>
#include <mxyzptlk/TJLterm.h>

#include<boost/iterator/iterator_facade.hpp>

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
bool operator==( TJL1<T> const& x, TJL1<T> const& y ); 

template<typename T>
bool operator==( TJL1<T> const& x, const T& y ); 

template<typename T>
bool operator==( T       const& y, TJL1<T> const& x );

template<typename T>
bool operator!=( TJL1<T> const& x, TJL1<T> const& y ); 

template<typename T>
bool operator!=( TJL1<T> const& x, const T& y ); 

template<typename T>
bool operator!=( T       const& x, TJL1<T> const& y ); 

template<typename T>
std::ostream& operator<<( std::ostream& os, TJL1<T> const& x); 

template<typename T>
std::istream& operator>>( std::istream& is,  TJL1<T>&  x); 

JL1Ptr<double> real( JL1Ptr<std::complex<double> > const& z ); 
JL1Ptr<double> imag( JL1Ptr<std::complex<double> > const& z );

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
  
  template <typename U>
  friend class TJL1;
  
 
  // Constructors and destructors (factory functions)_____________________________________

 public:

  // factory functions 

  static JL1Ptr<T> makeTJL( EnvPtr<T>  const& pje,  T value = T());
  static JL1Ptr<T> makeTJL( IntArray   const&,      T const&, EnvPtr<T> const& pje );

  template<typename U>
  static JL1Ptr<T> makeTJL( TJL1<U> const& );

  static JL1Ptr<T> makeTJL( TJL1 const* ); /// eliminate ?

  static void discardTJL( TJL1<T>* p);  

  
  // functions used to implement COW semantics ______________________________________________


  void dispose()           { TJL1<T>::discardTJL(  this ); }  // used by ReferenceCounter class 
  inline JL1Ptr<T> clone() 
                           {return JL1Ptr<T>( TJL1<T>::makeTJL(*this) ); } 

  // Compatibility 

  TJLterm<T>* storePtr() {return 0;}                  // returns a ptr to the next available block in the JLterm store;


  // Public member functions__________________________________________

  void clear();     // clears all the terms. 

  int                      getCount()   { return count_;   }
  int                      getWeight()  { return weight_;  }
  int                      getAccuWgt() { return accuWgt_; }

 
  EnvPtr<T>                getEnv() const                { return myEnv_; }    
  void                     setEnv( EnvPtr<T> const& pje) { myEnv_ = pje; }  // DANGER !  

  JL1Ptr<T>  truncMult( JL1Ptr<T> const& v, const int& wl )    const; 

  JL1Ptr<T>  filter( const int& wgtLo, const int& wgtHi )      const; 

  JL1Ptr<T>  filter( bool (*f) ( const IntArray&, T const& ) ) const; 


  void printCoeffs() const;               // prints term coefficients 
  void peekAt() const;                     

  TJLterm<T>  firstTerm() const;          // Returns a TJLterm<T> equivalent 
                                          // to the top term but does not remove it
 
  TJLterm<T>  lowTerm()   const;          // Returns a TJLterm<T> equivalent to the
                                          // **non-zero** term of lowest weight.

  void addTerm( TJLterm<T> const& );      
 
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
  JL1Ptr<T> D( const int* n ) const; 
 

  void setVariable( T const&, const int& );
  void setVariable( T const& x, const int& j, EnvPtr<T> const& pje );
  void setVariable( const int&, EnvPtr<T> const& pje );               

  T standardPart() const;
  void setStandardPart( T const& std) { terms_[0] = std; } 

  T weightedDerivative( const int* ) const;
  T derivative( const int* ) const;

  T operator()( Vector const& ) const;
  T operator()( T      const* ) const;
                       // Performs a multinomial evaluation of
                       // the TJL1 variable.  Essentially acts as a
                       // power series expansion.
  TJL1& operator()( const TJL1* ) const;
                       // Self explanatory ...

  friend   bool operator==<>( TJL1<T> const& x, TJL1<T> const& y ); 
  friend   bool operator==<>( TJL1<T> const& x, T const& y ); 
  friend   bool operator==<>( T       const& y, TJL1<T> const& x );
  friend   bool operator!=<>( TJL1<T> const& x, TJL1<T> const& y ); 
  friend   bool operator!=<>( TJL1<T> const& x, T const& y ); 
  friend   bool operator!=<>( T const& x, TJL1<T> const& y ); 

  friend   std::ostream& operator<< <T>( std::ostream& os, TJL1<T> const& x ); 
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

  TJL1& operator=( TJL1 const& );
  TJL1& operator=( T const& );

  TJL1& operator+=( TJL1 const& );
  TJL1& operator+=( T const& );

  TJL1& operator-=( TJL1 const& );
  TJL1& operator-=( T const& );

  TJL1& operator*=( TJL1 const& );
  TJL1& operator*=( T const& );

  TJL1& operator/=( TJL1 const& );
  TJL1& operator/=( T const& );

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


   template <typename U>
   class iter_ :  public boost::iterator_facade< iter_<U> , U , boost::forward_traversal_tag > {

   private:
      struct enabler {};

    public:

      iter_()               : m_node(0) {}

      explicit iter_( U* p) : m_node(p) {}

      template <class OtherType> 
      iter_( iter_<OtherType> const& other , 
             typename boost::enable_if<boost::is_convertible<OtherType*, U*> , enabler>::type = enabler())
           : m_node(other.m_node) {}

  
    private:

      friend class boost::iterator_core_access;

      TJLterm<U> dereference() const;
      void increment();
      bool equal( iter_ const& other) const; 

      U* m_node;
   };

  typedef iter_<T>                iterator;
  typedef iter_<T const>    const_iterator;
  
  iterator       begin();
  const_iterator begin() const;

  iterator       end();
  const_iterator end()   const;

 private: 

  int                                 count_;      // The number of terms in the variable ( in this case, dimension of _jcb +1 )
  int                                 weight_;     // The maximum weight of (the actual) terms  (set to 1)   
  int                                 accuWgt_;    // Highest weight computed accurately        ( set to 1 )

  mutable EnvPtr<T>                   myEnv_;      // Environment of the jet.
  
  T*                                  jcb_;        // jacobian
  T*                                  terms_;      // all terms     // NOTE: standard part = terms_[0]; jcb_ = &terms_[1]; 



  static std::vector<TJL1<T>* > thePool_;  // pool of discarded TJL1 objects

  void insert( TJLterm<T> const& );
  void append( TJLterm<T> const& );

  TJL1( EnvPtr<T> const&, T value = T() );
  TJL1( IntArray  const&, T const&,  EnvPtr<T> const&);
  TJL1(  TJL1 const& );

  template<typename U> 
  TJL1( TJL1<U> const& );  
 
  ~TJL1();


};

//  ================== specializations ====================


template<> 
template<> 
TJL1<std::complex<double> >::TJL1( TJL1<double> const& );  

template<> 
template<> 
JL1Ptr<std::complex<double> >  TJL1<std::complex<double> >::makeTJL( TJL1<double> const& );


#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJL1.tcc>
#endif

#endif // TJL1_H
