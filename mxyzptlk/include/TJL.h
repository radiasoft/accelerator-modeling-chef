/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJL.h
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******* Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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
******  Major Revision History:
******
******  Feb 2005   Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
****** - Efficiency improvements
****** - Major memory management redesign. 
******  
******  Sep-Dec 2005  ostiguy@fnal.gov
******
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - centralized TJetEnvironment management
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES 
******  
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef TJL_H
#define TJL_H

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
class TJL;

template<typename T>
bool operator==( const TJL<T>& x, const TJL<T>& y ); 
template<typename T>
bool operator==( const TJL<T>& x, const T& y ); 
template<typename T>
bool operator==( const T& y, const TJL<T>& x );
template<typename T>
bool operator!=( const TJL<T>& x, const TJL<T>& y ); 
template<typename T>
bool operator!=( const TJL<T>& x, const T& y ); 
template<typename T>
bool operator!=( const T& x, const TJL<T>& y ); 
template<typename T>
std::ostream& operator<<( std::ostream& os, const TJL<T>& x ); 
template<typename T>
std::istream& operator>>( std::istream& is,  TJL<T>& x ); 

// ---------------------------------------------------------------------------------------------------------
// NOTE:
// The arguments in the operator declations below can unfortunately not be declared in terms of of the pseudo 
// class JLPtr<T>; doing so results in failure of the template argument matching algorithm. The new 
// C++-0x standard provides support for true "templated typdefs". This feature should be taken advantage of 
// when it becomes available so as to remove explicit details of the smart pointer implementatation from this
// header file.  -JFO  
//-----------------------------------------------------------------------------------------------------------   

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator+(boost::intrusive_ptr<TJL<T> > const & x, boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >&  operator+=(boost::intrusive_ptr<TJL<T> >& x,      boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator-(boost::intrusive_ptr<TJL<T> > const &x);  

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator-(boost::intrusive_ptr<TJL<T> > const & x,  boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >&  operator-=(boost::intrusive_ptr<TJL<T> >& x,      boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator*(boost::intrusive_ptr<TJL<T> > const & x,  boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator*(boost::intrusive_ptr<TJL<T> > const & x,  T const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator*(T const & x,                              boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >&  operator*=(boost::intrusive_ptr<TJL<T> > & x,     boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator/(boost::intrusive_ptr<TJL<T> > const & x,  boost::intrusive_ptr<TJL<T> > const& y  );  

template <typename T>
boost::intrusive_ptr<TJL<T> >   operator/(boost::intrusive_ptr<TJL<T> > const & x,  T const& y  );  


JLPtr<double>::Type real( const JLPtr<std::complex<double> >::Type & z ); 
JLPtr<double>::Type imag( const JLPtr<std::complex<double> >::Type & z );


// **********************************************************************************************************

template<typename T> 
class TJL: public ReferenceCounter<TJL<T> > {

  friend class TJL<double>;
  friend class TJL<std::complex<double> >;
 
 private: 

  dlist                               _theList;    // Data structure to hold all the information.
  int                                 _count;      // The number of JL terms in the variable
  int                                 _weight;     // The maximum weight of (the actual) terms    
  int                                 _accuWgt;    // Highest weight computed accurately

  mutable typename EnvPtr<T>::Type    _myEnv; // Environment of the jet.
  
  TJLterm<T>*                         _jltermStore;
  TJLterm<T>*                         _jltermStoreCurrentPtr;
  int                                 _jltermStoreCapacity;

  mutable dlist_iterator*             _constIterPtr;
  mutable dlist_iterator*             _iterPtr;

  static std::vector<TJL<T>* >        _thePool;  
                                           // declared public because the TJet
                                           // conversion function need access FIXME !

  void initStore();                  // setup and initialize the jlterm store using default capacity 
  void initStore(int capacity);      // setup and initialize the jlterm store 

  void insert( TJLterm<T>  const& );  // prefer this form. Allocation performed by insert(). 
  void append( TJLterm<T>  const&);   // prefer this form. Allocation performed by append().

  void insert( TJLterm<T>  const*);   // implies a pointer to an already allocated obj.         
  void append( TJLterm<T>  const*);   // implies a pointer to an already allocated obj.  

  TJLterm<T>* remove( dlink* );

  TJL( typename EnvPtr<T>::Type, T value = T() );
  TJL( const IntArray&, const T&,  typename EnvPtr<T>::Type );
  TJL( const TJL& );
  TJL( const TJL* );

  ~TJL();

  static typename JLPtr<T>::Type _epsSin(  typename JLPtr<T>::Type const& epsilon );
  static typename JLPtr<T>::Type _epsCos(  typename JLPtr<T>::Type const& epsilon );
  static typename JLPtr<T>::Type _epsSqrt( typename JLPtr<T>::Type const& epsilon ); 
  static typename JLPtr<T>::Type _epsExp(  typename JLPtr<T>::Type const& epsilon ); 
  static typename JLPtr<T>::Type _epsPow(  typename JLPtr<T>::Type const& epsilon, const double& s ); 


  // Constructors and destructors (factory functions)_____________________________________

 public:

  // factory functions 


  static typename JLPtr<T>::Type makeTJL( typename EnvPtr<T>::Type pje,  T value = T());
  static typename JLPtr<T>::Type makeTJL( const IntArray&, const T&, typename EnvPtr<T>::Type pje );
  static typename JLPtr<T>::Type makeTJL( const TJL& );
  static void discardTJL( TJL<T>* p);  

  
  // functions used to implement COW semantics ______________________________________________


  void dispose() { TJL<T>::discardTJL(  this ); }  // used by ReferenceCounter class 
  inline typename JLPtr<T>::Type clone() 
                           {return typename JLPtr<T>::Type( TJL<T>::makeTJL(  *this ) ); } 



  // Public member functions__________________________________________


  operator JLPtr<std::complex<double> >::Type () const;
  operator JLPtr<double>::Type () const;

  void clear();     // clears all the terms. 

  int                      getCount()   { return _count;   }
  int                      getWeight()  { return _weight;  }
  int                      getAccuWgt() { return _accuWgt; }

 
  typename EnvPtr<T>::Type getEnv() const { return _myEnv; }    
  void                     setEnv( typename EnvPtr<T>::Type pje) { _myEnv = pje; }  // DANGER !  

  typename JLPtr<T>::Type  truncMult( typename JLPtr<T>::Type const& v, const int& wl )  const; 

  typename JLPtr<T>::Type  filter( const int& wgtLo, const int& wgtHi )      const; 

  typename JLPtr<T>::Type  filter( bool (*f) ( const IntArray&, const T& ) ) const; 


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
 
  TJLterm<T>* removeTerm( TJLterm<T>* a); // Unconditionally remove term pointed to by a;  a is not deleted !
 
  TJLterm<T>* storePtr();                 // returns a ptr to the next available block in the JLterm store;

  bool isNilpotent() const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T* ) const;
  void   scaleBy( T );

  TJL& Negate();

  friend boost::intrusive_ptr<TJL<T> >  
         operator+<>(boost::intrusive_ptr<TJL<T> > const & x, boost::intrusive_ptr<TJL<T> > const& y  );  

  friend boost::intrusive_ptr<TJL<T> >& 
         operator+= <>(boost::intrusive_ptr<TJL<T> >& x,      boost::intrusive_ptr<TJL<T> > const& y  );  

  friend boost::intrusive_ptr<TJL<T> >  
         operator-<>(boost::intrusive_ptr<TJL<T> > const & x );  

  friend boost::intrusive_ptr<TJL<T> >  
         operator-<>(boost::intrusive_ptr<TJL<T> > const & x, boost::intrusive_ptr<TJL<T> > const& y  );  

  friend boost::intrusive_ptr<TJL<T> >&  
         operator-=<>(boost::intrusive_ptr<TJL<T> >& x,         boost::intrusive_ptr<TJL<T> > const& y  );  

  friend boost::intrusive_ptr<TJL<T> >  
         operator*<>(boost::intrusive_ptr<TJL<T> > const & x, boost::intrusive_ptr<TJL<T> > const& y  );  

  friend boost::intrusive_ptr<TJL<T> >   
         operator*<>(boost::intrusive_ptr<TJL<T> > const & x,  T const& y  );  

  friend boost::intrusive_ptr<TJL<T> >   
         operator*<>(T const & x,                              boost::intrusive_ptr<TJL<T> > const& y  );  

  friend boost::intrusive_ptr<TJL<T> >& 
        operator*= <>(boost::intrusive_ptr<TJL<T> >& x,        boost::intrusive_ptr<TJL<T> > const& y  );  

  friend boost::intrusive_ptr<TJL<T> >  
         operator/<>(boost::intrusive_ptr<TJL<T> > const & x,  boost::intrusive_ptr<TJL<T> > const& y  );  
  
  friend boost::intrusive_ptr<TJL<T> >   
         operator/<>(boost::intrusive_ptr<TJL<T> > const & x,  T const& y  );  

  friend typename JLPtr<double>::Type real( const JLPtr<std::complex<double> >::Type & z ); 
  friend typename JLPtr<double>::Type imag( const JLPtr<std::complex<double> >::Type & z );


  typename JLPtr<T>::Type sin()              const;
  typename JLPtr<T>::Type cos()              const;
  typename JLPtr<T>::Type asin()             const;
  typename JLPtr<T>::Type atan()             const;
  typename JLPtr<T>::Type sqrt()             const;
  typename JLPtr<T>::Type exp()              const;
  typename JLPtr<T>::Type pow(int)           const;
  typename JLPtr<T>::Type pow(const double&) const;
  typename JLPtr<T>::Type log()              const;
  typename JLPtr<T>::Type compose(typename JLPtr<T>::Type const y[ ]) const; 
  typename JLPtr<T>::Type D( const int* n ) const; 

  void              resetConstIterator();
  TJLterm<T>        stepConstIterator()     const;
  const TJLterm<T>& stepConstIteratorRef()  const;
  const TJLterm<T>* stepConstIteratorPtr()  const;
  void              resetIterator();
  TJLterm<T>*       stepIterator();

  void setVariable( const T&, const int& );
  void setVariable( const T& x, const int& j, typename EnvPtr<T>::Type pje );
  void setVariable( const int&, typename EnvPtr<T>::Type pje );               

  T standardPart() const;

  T weightedDerivative( const int* ) const;
  T derivative( const int* ) const;

  T operator()( const Vector& ) const;
  T operator()( const T* ) const;
                       // Performs a multinomial evaluation of
                       // the TJL variable.  Essentially acts as a
                       // power series expansion.
  TJL& operator()( const TJL* ) const;
                       // Self explanatory ...

  // Arithmetic operators // some of these may no longer be needed ... FIXME !

  TJL& operator=( const TJL& );
  TJL& operator=( const T& );

  TJL& operator+=( const TJL& );
  TJL& operator+=( const T& );

  //TJL& operator-=( const TJL& );
  //TJL& operator-=( const T& );

  //TJL& operator*=( const TJL& );
  //TJL& operator*=( const T& );

  //TJL& operator/=( const TJL& );
  //TJL& operator/=( const T& );

  friend   bool operator==<>( const TJL<T>& x, const TJL<T>& y ); 
  friend   bool operator==<>( const TJL<T>& x, const T& y ); 
  friend   bool operator==<>( const T& y, const TJL<T>& x );
  friend   bool operator!=<>( const TJL<T>& x, const TJL<T>& y ); 
  friend   bool operator!=<>( const TJL<T>& x, const T& y ); 
  friend   bool operator!=<>( const T& x, const TJL<T>& y ); 
  friend   std::ostream& operator<< <T>( std::ostream& os, const TJL<T>& x ); 
  friend   std::istream& operator>><T>(  std::istream& is,  TJL<T>& x ); 


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


// specializations 

template<> TJL<double>::operator JLPtr<std::complex<double> >::Type () const;              // implemented
template<> TJL<std::complex<double> >::operator JLPtr<double>::Type () const;              // implemented

template<> TJL<std::complex<double> >::operator JLPtr<std::complex<double> >::Type () const; // NOT implemented
template<> TJL<double>::operator JLPtr<double>::Type () const;                               // NOT implemented



#ifdef  MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJL.tcc>
#endif

#endif // TJL_H
