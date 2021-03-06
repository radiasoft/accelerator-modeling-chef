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
****** Sep-Dec 2005  ostiguy@fnal.gov
******
****** - refactored code to use a single class template parameter
******   rather than two. Mixed mode operations now handled using 
******   implicit conversion.
****** - reference counting now based on using boost::intrusive smart pointer
****** - reference counted TJetEnvironment 
****** - centralized TJetEnvironment management
****** - all implementation details now completely moved to private class TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******
****** Sep 2006  ostiguy@fnal.gov
******
****** - eliminated doubly linked list representation for polynomials
******  
****** Mar 2007  ostiguy@fnal.gov
******
****** - Introduced new compact monomial indexing scheme based on monomial ordering
******   to replace previous scheme based explicitly on monomial exponents tuple.
****** - monomial multiplication handled via a lookup-table.
****** - added STL compatible monomial term iterators    
****** - added get/setTermCoefficient to efficiently get/set a specific monomial
******   coefficient.
******
****** Mar 2008 ostiguy@fnal
******  - Jet composition and evaluation code refactored and optimized. 
******
****** August 2008 
******  - improved iterators 
******
****** Jan 2011  michelotti@fnal.gov
****** - Jim Amundson discovered error in signatures of real and imag functions.
******
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef TJL_H
#define TJL_H

#include <gms/FastAllocator.h>
#include <gms/FastPODAllocator.h>

#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/TMatrix.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/ReferenceCounter.h>
#include <mxyzptlk/JLPtr.h>
#include <mxyzptlk/EnvPtr.h>
#include <mxyzptlk/TJLterm.h>

#include<boost/iterator/iterator_facade.hpp>
#include<boost/iterator/reverse_iterator.hpp>
#include <vector>

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
class TJL;

template<typename T>
class TJetVector;

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
JLPtr<T>   operator+( JLPtr<T> const& x, JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>& operator+=( JLPtr<T>&, T const& ); 

template <typename T>
JLPtr<T>& operator+=( JLPtr<T>&, JLPtr<T> const&  );

//..............................................................................................................................

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const & x,  JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>   operator-(JLPtr<T> const &x);  // unary minus 

template <typename T>
JLPtr<T>& operator-=( JLPtr<T>&, T const& ); 

template <typename T>
JLPtr<T>& operator-=( JLPtr<T>&, JLPtr<T> const&  );

// ..........................................................................

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>   operator*(JLPtr<T> const & x,  T const& y         );  

template <typename T>
JLPtr<T>   operator*(T const &       x,  JLPtr<T> const& y   );  

template <typename T>
JLPtr<T>&  operator*=(JLPtr<T> &      x,  JLPtr<T> const& y  );  

template <typename T>
JLPtr<T>&  operator*=(JLPtr<T> &      x,  T const& y         );  

// ..........................................................................

template <typename T>
JLPtr<T> operator/(  JLPtr<T> const & x,   JLPtr<T> const& y  );  

template <typename T>
JLPtr<T> operator/(  JLPtr<T> const & x,    T const& y  );  


//..............................................................................................................................

JLPtr<double> real( JLPtr<std::complex<double> > const& z );
JLPtr<double> imag( JLPtr<std::complex<double> > const& z );

// **********************************************************************************************************

template<typename T> 
class DLLLOCAL TJL: public ReferenceCounter<TJL<T> > {

template<typename U>  
friend class TJL;

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

  inline JLPtr<T> clone() const 
                           { return JLPtr<T>( TJL<T>::makeTJL(  *this ) ); } 



  // Public member functions__________________________________________

  void clear();           // clears all the terms. 

  int                      getCount()   const;
  int                      getWeight()  const { return weight_;  } 
  int                      getAccuWgt() const { return accuWgt_; }

 
  EnvPtr<T>                getEnv() const { return this->myEnv_; }    
  void                     setEnv( EnvPtr<T> const& pje ) { this->myEnv_ = pje; }  // DANGER !  

  JLPtr<T>  truncMult( JLPtr<T> const& v, const int& wl )  const; 

  JLPtr<T>  filter( int const& wgtLo, int const& wgtHi )      const; 

  JLPtr<T>  filter( bool (*f) ( IntArray const&, T const& ) ) const; 

  double    maxAbs() const;                

  void printCoeffs() const;               // prints term coefficients 

  TJLterm<T>  firstTerm() const;    
                                          // Returns a TJLterm<T> equivalent 
                                          // to the top term,
                                          // which should be the one of lowest weight.

  void addTerm( TJLterm<T> const& ); 
     
  void removeTerm( TJLterm<T> const& a);  // remove term a; 
 
  bool isNilpotent() const;
  void writeToFile( std::ofstream& ) const;

  TJL& Negate(); // in-place negation 

  friend JLPtr<T>  
         operator+<>(  JLPtr<T> const& lhs,  JLPtr<T>  const& rhs  );  

  friend JLPtr<T>& 
         operator+=<>( JLPtr<T>&       lhs,           T const& rhs  ); 

  friend JLPtr<T>& 
         operator+=<>( JLPtr<T>&       lhs,     JLPtr<T> const& rhs );

  friend JLPtr<T>  
         operator-<>(JLPtr<T> const & x );  

  friend JLPtr<T>  
         operator-<>(JLPtr<T> const &  lhs,     JLPtr<T> const& rhs  );  

  friend JLPtr<T>& 
         operator-=<>( JLPtr<T>&       lhs,            T const& rhs  ); 

  friend JLPtr<T>& 
         operator-=<>( JLPtr<T>&       lhs,     JLPtr<T> const& rhs  );

  friend JLPtr<T>  
         operator*<>(JLPtr<T> const &  lhs,     JLPtr<T> const& rhs  );  

  friend JLPtr<T>   
         operator*<>(JLPtr<T> const &  lhs,            T const& rhs  );  

  friend JLPtr<T>   
         operator*<>(T const &         lhs,     JLPtr<T> const& rhs  );  

  friend JLPtr<T>& 
         operator*= <>(JLPtr<T>&       lhs,     JLPtr<T> const& rhs  );  

  friend JLPtr<T>& 
         operator*= <>(JLPtr<T>&       lhs,            T const& rhs  );  

  friend JLPtr<T>  
         operator/<>( JLPtr<T> const&  lhs,     JLPtr<T> const& rhs  );  
  
  friend JLPtr<T>   
         operator/<>( JLPtr<T> const & lhs,            T const& rhs  );  


  friend JLPtr<double> real( JLPtr<std::complex<double> > const& z ); 
  friend JLPtr<double> imag( JLPtr<std::complex<double> > const& z );

  friend   bool operator==<>( TJL<T>   const& lhs, TJL<T>  const& rhs ); 
  friend   bool operator==<>( TJL<T>   const& lhs, T       const& rhs ); 
  friend   bool operator==<>( T        const& lhs, TJL<T>  const& rhs );
  friend   bool operator!=<>( TJL<T>   const& lhs, TJL<T>  const& rhs ); 
  friend   bool operator!=<>( TJL<T>   const& lhs, T       const& rhs ); 
  friend   bool operator!=<>( T        const& lhs, TJL<T>  const& rhs ); 

  JLPtr<T> sqrt() const;
  JLPtr<T> exp()  const;
  JLPtr<T> sin()  const;
  JLPtr<T> cos()  const;

  JLPtr<T> asin() const;
  JLPtr<T> acos() const;
  JLPtr<T> atan() const;

  static JLPtr<T> pow( JLPtr<T>&, int );
  static JLPtr<T> pow( JLPtr<T>&, double const&);

  JLPtr<T> log()              const;
  JLPtr<T> D( IntArray const& n ) const; 

  void setVariable(  int const&,  T const& x,  EnvPtr<T> pje );

  T const& standardPart()                 const  { return jltermStore_[0].value_; }
  void     setStandardPart( T const& std)        { jltermStore_[0].value_ = std;  } 

  T weightedDerivative( IntArray const& ) const;
  T derivative( IntArray const& )         const;

  void         setDerivative( IntArray const& exp, T const& value); 
  void setWeightedDerivative( IntArray const& exp, T const& value); 

  template<typename const_iterator_t>
  T evaluate( const_iterator_t itb, const_iterator_t ite) const; 

  template< typename const_iterator_t>
  JLPtr<T> compose(  const_iterator_t itb, const_iterator_t ite ) const; //  Jet composition operator  

  TJL& operator=( TJL const& );
  TJL& operator=( T   const& );

  friend   std::ostream& operator<< <T>( std::ostream& os,  TJL<T> const& x ); 
  friend   std::istream& operator>><T>(  std::istream& is,  TJL<T>      & x ); 


  template <typename U>
  class iter_ :  public boost::iterator_facade< iter_<U> , U , boost::random_access_traversal_tag > {
  
    template<typename V>
    friend class iter_;

    private:
      struct enabler {};

    public:

      iter_()               : node_(0) {}

      explicit iter_( U* p) : node_(p) {}

      template <class OtherType> 
      iter_( iter_<OtherType> const& other , 
             typename boost::enable_if<boost::is_convertible<OtherType*, U*> , enabler>::type = enabler())
           : node_(other.node_) {}

      bool operator< ( iter_ const& it ) const { return node_ < it.node_; }
      bool operator> ( iter_ const& it ) const { return node_ > it.node_; }

    private:

      friend class boost::iterator_core_access;

      U&   dereference() const  { return *node_; }  
        
      void            increment()                                   { ++node_;          } 
      void            decrement()                                   { --node_;          } 
      void            advance( typename iter_::difference_type n )  { node_ += n;       }   
      typename iter_::difference_type 
                      distance_to( iter_ const&  j)  const          { return (j.node_ - node_); }   
 
      bool equal( iter_ const& other) const 
            {
              return this->node_ == other.node_;
            }

      U* node_;
   };

  typedef iter_<TJLterm<T> >             iterator;
  typedef iter_<TJLterm<T> const>  const_iterator;

  typedef boost::reverse_iterator< iter_< TJLterm<T> > >             reverse_iterator;
  typedef boost::reverse_iterator< iter_< TJLterm<T> const > > const_reverse_iterator;
  
  iterator               begin();
  const_iterator         begin()  const;

  iterator               end();
  const_iterator         end()   const;

  reverse_iterator       rbegin();
  const_reverse_iterator rbegin() const;

  reverse_iterator       rend();
  const_reverse_iterator rend()   const;


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

 private: 

  static const double                 mx_small_;                // = 1.0e-12    //  abs coeff threshold for removal of a JLterm
  static const int                    mx_maxiter_;              // = 100        // Max no of iterations allowed  (e.g. transcendental functions)

  mutable EnvPtr<T>                   myEnv_;                   // Environment of the jet.

  int                                 count_;      // The number of JL terms with weight > 1 in the variable
  int                                 weight_;     // The maximum weight of (the actual) terms    
  int                                 accuWgt_;    // Highest weight computed accurately
  int                                 lowWgt_;     // the lowest weight present


  TJLterm<T>*                         jltermStore_;              // the store holds terms
  TJLterm<T>*                         jltermStoreCurrentPtr_;
  int                                 jltermStoreCapacity_;

  static std::vector<TJL<T>*  >&      thePool_;                  // TJL<T> objects recycling pool.  


  void initStore(int capacity);      // setup and initialize the jlterm store 

  void transferFromScratchPad();     // transfer result from scratchpad into current TJL   
  void append( TJLterm<T>  const&);  

  TJL( EnvPtr<T> const&,  T value = T() );
  TJL( IntArray  const&,  T const&,  EnvPtr<T> const&);

  TJL( TJL const&);                    // this form is necessary to avoid 
                                       // problems with access to ReferenceCounter
  template <typename U>
  TJL( TJL<U> const& x);               // used for double to complex conversion

  ~TJL();

  static JLPtr<T> epsSin(  JLPtr<T> const& epsilon );
  static JLPtr<T> epsCos(  JLPtr<T> const& epsilon );
  static JLPtr<T> epsExp(  JLPtr<T> const& epsilon ); 
  static JLPtr<T> epsPow(  JLPtr<T> const& epsilon, double const& s ); 

  TJLterm<T>* storePtr();                        // returns a ptr to the next available block in the JLterm store;
  void growStore( );                             // grows the size of the store to twice its current size 
  void appendLinearTerms( int numvar );          // appends all linear monomial terms ( value_ set to 0)   
  
  
  static void op_add( T& x, T const& y) { x += y; } 
  static void op_sub( T& x, T const& y) { x -= y; } 

  template< void T_function( T&, T const&) >
  static JLPtr<T>           add(  JLPtr<T> const&, JLPtr<T> const&); 

  template< void T_function( T&, T const&) >
  static JLPtr<T>&  inplace_add(JLPtr<T>& x, JLPtr<T> const& y  );


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
inline void TJL<T>::discardTJL( TJL<T>* p) 
{
  
   EnvPtr<T> nullEnv;

   p->clear();     

   p->myEnv_ = nullEnv; // nullify the environment. 

   thePool_.push_back(p);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
inline TJLterm<T>* TJL<T>::storePtr( ) {

  if ( (jltermStoreCurrentPtr_-jltermStore_) >= jltermStoreCapacity_ ) {
   growStore();
  }

  return jltermStoreCurrentPtr_++;
}         

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

template<typename T>
inline void TJL<T>::append( TJLterm<T> const& a) 
{

  // **** append a non-linear term (weight >1 )

  TJLterm<T>* p = new( this->storePtr() ) TJLterm<T>(a); 
  ++count_;

  weight_      = std::max(weight_,     p->weight_ );

}


#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJL.tcc>
#endif

#endif // TJL_H



