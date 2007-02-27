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
****** - refactored code to use a single class template parameter
******   rather than two. Mixed mode operations now handled using 
******   implicit conversion.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - centralized TJetEnvironment management
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******
****** Sep 2006  ostiguy@fnal.gov
******
****** - eliminated doubly linked list representation for polynomials
******  
******  
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef TJL_H
#define TJL_H

#include <basic_toolkit/dlist.h>
#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/TMatrix.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/TJetEnvironment.h>
#include <gms/FastPODAllocator.h>
#include <basic_toolkit/ReferenceCounter.h>
#include <vector>
#include <mxyzptlk/JLPtr.h>
#include <mxyzptlk/EnvPtr.h>
#include <mxyzptlk/TJLterm.h>

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
class TJLIterator;

template<typename T>
bool operator==( TJL<T> const& x,     TJL<T> const& y ); 

template<typename T>
bool operator==( TJL<T> const& x,          const T& y ); 

template<typename T>
bool operator==( T const&      x,      TJL<T> const& y );

template<typename T>
bool operator!=( TJL<T> const& x,      TJL<T> const& y ); 

template<typename T>
bool operator!=( TJL<T> const& x,           T const& y ); 

template<typename T>
bool operator!=( T const&      x,       TJL<T> const& y ); 

template<typename T>
std::ostream& operator<<( std::ostream& os, TJL<T> const& x ); 

template<typename T>
std::istream& operator>>( std::istream& is, TJL<T>& x ); 


template <typename T>
JLPtr<T>   operator+(JLPtr<T> const & x, JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>&  operator+=(JLPtr<T>& x,       JLPtr<T> const& y  );  

//..............................................................................................................................

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const &x);  

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const & x,  JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>&  operator-=(JLPtr<T>& x,        JLPtr<T> const& y  );  

// ..........................................................................

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  T const& y  );  

template <typename T>
JLPtr<T>   operator*(T const &       x,  JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>&  operator*=(JLPtr<T> &      x,  JLPtr<T> const& y  );  

// ..........................................................................

template <typename T>
JLPtr<T> operator/(  JLPtr<T> const & x,   JLPtr<T> const& y  );  

template <typename T>
JLPtr<T> operator/(  JLPtr<T> const & x,    T const& y  );  


//..............................................................................................................................


JLPtr<double> real(  TJL<std::complex<double> >* const& z ); 
JLPtr<double> imag(  TJL<std::complex<double> >* const& z );



// **********************************************************************************************************

template<typename T> 
class DLLLOCAL TJL: public ReferenceCounter<TJL<T> > {

template<typename U>  
friend class TJL;

friend class TJLIterator<T>;

 private: 

  static const double                 _mx_small;                // = 1.0e-12    //  abs coeff threshold for removal of a JLterm
  static const int                    _mx_maxiter;              // = 100        // Max no of iterations allowed  (e.g. transcendental functions)

  mutable EnvPtr<T>                   _myEnv;                   // Environment of the jet.

  int                                 _count;      // The number of JL terms with weight > 1 in the variable
  int                                 _weight;     // The maximum weight of (the actual) terms    
  int                                 _accuWgt;    // Highest weight computed accurately
  int                                 _lowWgt;     // the lowest weight present


  TJLterm<T>*                         _jltermStore;              // the store holds terms
  TJLterm<T>*                         _jltermStoreCurrentPtr;
  int                                 _jltermStoreCapacity;

  static std::vector<TJL<T>*  >&      _thePool;                  // TJL<T> objects recycling pool.  


  void initStore(int capacity);      // setup and initialize the jlterm store 

  void transferFromScratchPad( );    // transfer result from scratchpad into current TJL   
  void append( TJLterm<T>  const&);  

  TJL( EnvPtr<T> const&,  T value = T() );
  TJL( IntArray  const&,  T const&,  EnvPtr<T> const&);

  TJL( TJL const&);                    // this form is necessary to avoid 
                                       // problems with access to ReferenceCounter
  template <typename U>
  TJL( TJL<U> const& x);               // used for double to complex conversion

  ~TJL();

  static JLPtr<T> _epsSin(  JLPtr<T> const& epsilon );
  static JLPtr<T> _epsCos(  JLPtr<T> const& epsilon );
  static JLPtr<T> _epsSqrt( JLPtr<T> const& epsilon ); 
  static JLPtr<T> _epsExp(  JLPtr<T> const& epsilon ); 
  static JLPtr<T> _epsPow(  JLPtr<T> const& epsilon, const double& s ); 

  TJLterm<T>* storePtr();                        // returns a ptr to the next available block in the JLterm store;
  void growStore( );                             // grows the size of the store to twice its current size 
  void appendLinearTerms( int numvar );          // appends all linear monomial terms (_value set to 0)   
  
  
  static void op_add( T& x, T const& y) { x += y; } 
  static void op_sub( T& x, T const& y) { x -= y; } 

  template< void T_function( T&, T const&) >
  static JLPtr<T>          add(  JLPtr<T> const&, JLPtr<T> const&); 

  template<void T_function( T&, T const&) >
  static JLPtr<T>& inplace_add(  JLPtr<T>&,       JLPtr<T> const&);


  // Constructors and destructors (factory functions)_____________________________________

 public:

  // factory functions 


  static JLPtr<T> makeTJL( EnvPtr<T> const&  pje,  T value = T());
  static JLPtr<T> makeTJL( IntArray  const&, T const& value, EnvPtr<T> const& pje );

  template<typename U>
  static JLPtr<T> makeTJL( const TJL<U>& );

  static void discardTJL( TJL<T>* p);  

  
  // functions used to implement COW semantics ______________________________________________


  void dispose()           { TJL<T>::discardTJL(  this ); }  // used by ReferenceCounter class 

  inline JLPtr<T> clone() 
                           { return JLPtr<T>( TJL<T>::makeTJL(  *this ) ); } 



  // Public member functions__________________________________________

  void clear();           // clears all the terms. 

  int                      getCount()   const;  
  int                      getWeight()  const { return _weight;  } 
  int                      getAccuWgt() const { return _accuWgt; }

 
  EnvPtr<T>                getEnv() const { return this->_myEnv; }    
  void                     setEnv( EnvPtr<T> const& pje ) { this->_myEnv = pje; }  // DANGER !  

  JLPtr<T>  truncMult( JLPtr<T> const& v, const int& wl )  const; 

  JLPtr<T>  filter( const int& wgtLo, const int& wgtHi )      const; 

  JLPtr<T>  filter( bool (*f) ( const IntArray&, const T& ) ) const; 


  void printCoeffs() const;               // prints term coefficients 
  void peekAt() const;                     


  TJLterm<T>  firstTerm() const;    
                                          // Returns a TJLterm<T> equivalent 
                                          // to the top term,
                                          // which should be the one of lowest weight.

  void addTerm( TJLterm<T> const& );      
 
  void removeTerm( TJLterm<T> const& a);  // remove term a; 
 

  bool isNilpotent() const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T* ) const;
  void   scaleBy( T );

  TJL& Negate(); // in-place negation 

  friend JLPtr<T>  
         operator+<>(JLPtr<T> const & x, JLPtr<T> const& y  );  

  friend JLPtr<T>& 
         operator+= <>(JLPtr<T>& x,      JLPtr<T> const& y  );  

  friend JLPtr<T>  
         operator-<>(JLPtr<T> const & x );  

  friend JLPtr<T>  
         operator-<>(JLPtr<T> const & x, JLPtr<T> const& y  );  

  friend JLPtr<T>&  
         operator-=<>(JLPtr<T>& x,       JLPtr<T> const& y  );  

  friend JLPtr<T>  
         operator*<>(JLPtr<T> const & x, JLPtr<T> const& y  );  

  friend JLPtr<T>   
         operator*<>(JLPtr<T> const & x,  T const& y  );  

  friend JLPtr<T>   
         operator*<>(T const & x,         JLPtr<T> const& y  );  

  friend JLPtr<T>& 
        operator*= <>(JLPtr<T>& x,        JLPtr<T> const& y  );  

  friend JLPtr<T>  
         operator/<>( JLPtr<T> const & x, JLPtr<T> const& y  );  
  
  friend JLPtr<T>   
         operator/<>( JLPtr<T> const & x,  T const& y  );  


  friend JLPtr<double> real( JLPtr<std::complex<double> > const& z ); 
  friend JLPtr<double> imag( JLPtr<std::complex<double> > const& z );


  friend   bool operator==<>( TJL<T>   const& x, TJL<T>  const& y ); 
  friend   bool operator==<>( TJL<T>   const& x, T       const& y ); 
  friend   bool operator==<>( T        const& y, TJL<T>  const& x );
  friend   bool operator!=<>( TJL<T>   const& x, TJL<T>  const& y ); 
  friend   bool operator!=<>( TJL<T>   const& x, T       const& y ); 
  friend   bool operator!=<>( T        const& x, TJL<T>  const& y ); 

  JLPtr<T> sin()              const;
  JLPtr<T> cos()              const;
  JLPtr<T> asin()             const;
  JLPtr<T> acos()             const;
  JLPtr<T> atan()             const;
  JLPtr<T> sqrt()             const;
  JLPtr<T> exp()              const;
  JLPtr<T> pow(int)           const;
  JLPtr<T> pow(double const&) const;
  JLPtr<T> log()              const;
  JLPtr<T> compose(JLPtr<T> const y[ ]) const; 
  JLPtr<T> D( int const* n )  const; 


  void setVariable( T   const&,   int const& );
  void setVariable( T   const& x, int const& j,  EnvPtr<T> const& pje );
  void setVariable( int const&,   EnvPtr<T> const& pje );               

  T    standardPart()                 const  { return _jltermStore[0]._value; }
  void setStandardPart( T const& std)        { _jltermStore[0]._value = std;  } 

  T weightedDerivative( int const* ) const;
  T derivative( int const* )         const;

  T operator()( Vector const& ) const;
  T operator()( T const* )      const;
                       // Performs a multinomial evaluation of
                       // the TJL variable.  Essentially acts as a
                       // power series expansion.
  TJL& operator()( const TJL* ) const;
                       // Self explanatory ...

  // Arithmetic operators // some of these may no longer be needed ... FIXME !

  TJL& operator=( TJL const& );
  TJL& operator=( T   const& );

  TJL& operator+=( TJL const& );
  TJL& operator+=( T   const& );



  friend   std::ostream& operator<< <T>( std::ostream& os,  TJL<T> const& x ); 
  friend   std::istream& operator>><T>(  std::istream& is,  TJL<T>      & x ); 


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

};

//-------------------------------------------------------------------------------------
// specializations 
//-------------------------------------------------------------------------------------



template<> 
template<> 
TJL<std::complex<double> >::TJL( TJL<double> const& );  

template<> 
template<> 
JLPtr<std::complex<double> >  TJL<std::complex<double> >::makeTJL( TJL<double> const& );

//-------------------------------------------------------------------------------------
// Inline functions 
//-------------------------------------------------------------------------------------


template<typename T>
template<typename U>
inline JLPtr<T>  TJL<T>::makeTJL( TJL<U> const& x )
{

  if (_thePool.empty() ) 
     return  JLPtr<T>(new TJL<T>(x));
 
  TJL<T>* p = _thePool.back(); _thePool.pop_back(); 
  
  if (p->_jltermStoreCapacity < x._jltermStoreCapacity)  
  { 
       TJLterm<T>::array_deallocate( p->_jltermStore );
       p->initStore(x._jltermStoreCapacity);   
  
  }

  p->_count    = x._count;   
  p->_weight   = x._weight;  
  p->_accuWgt  = x._accuWgt;
  p->_lowWgt   = x._lowWgt;
  p->_myEnv    = x._myEnv;
 
   memcpy( p->_jltermStore, x._jltermStore, (x._jltermStoreCurrentPtr-x._jltermStore)*sizeof(TJLterm<T>) );
   p->_jltermStoreCurrentPtr = p->_jltermStore + (x._jltermStoreCurrentPtr - x._jltermStore);

  return JLPtr<T>(p);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
inline void TJL<T>::discardTJL( TJL<T>* p) 
{
  
   EnvPtr<T> nullEnv;

   p->clear();     

   p->_myEnv = nullEnv; // nullify the environment. 

   _thePool.push_back(p);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
inline TJLterm<T>* TJL<T>::storePtr( ) {

  if ( (_jltermStoreCurrentPtr-_jltermStore) < _jltermStoreCapacity ) 

     return _jltermStoreCurrentPtr++;

  growStore();

  return _jltermStoreCurrentPtr++;
}         

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template<typename T>
inline void TJL<T>::append( TJLterm<T> const& a) 
{

  // **** append a non-linear term (weight >1 )

  TJLterm<T>* p = new( this->storePtr() ) TJLterm<T>(a); 
  ++_count;

  _weight      = std::max(_weight,     p->_weight);

}


#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJL.tcc>
#endif

#endif // TJL_H
