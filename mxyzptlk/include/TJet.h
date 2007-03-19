/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJet.h
******  Version:   1.0
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
******                All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
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
******  Revision History:
******
******  Feb-May 2005  Jean-Francois Ostiguy
******                ostiguy@fnal.gov
******
******  - Efficiency improvements.
******  - New memory management scheme.
******
******  Sept-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to use single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******
****** Sep 2006 
******
****** - eliminated dlist representation for (non-zero) monomials.   
****** - eliminated archaic "Reconstruct" members. Use placement new syntax instead. 
****** Mar 2007 ostiguy@fnal.gov  
****** - Introduced new compact monomial indexing scheme based on monomial ordering
******   rather than previous scheme based explicitly on monomial exponents tuple.
****** - monomial multiplication handled via a lookup-table.
****** - added STL compatible monomial term iterators   
****** 
**************************************************************************
*************************************************************************/
#ifndef TJET_H
#define TJET_H

#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/VectorD.h>
#include <gms/FastAllocator.h>

#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/TJL.h>
#include <mxyzptlk/TJL1.h>
#include <complex>

// Forward declarations

template<typename T> 
class TJetVector;

template<typename T> 
class TLieOperator;

TJet<double> fabs( TJet<double>                const& );

TJet<double> real( TJet<std::complex<double> > const& );

TJet<double> imag( TJet<std::complex<double> > const& );

template<typename T> 
std::ostream& operator<<(std::ostream&, TJet<T>const&);

template<typename T> 
std::istream& operator>>(std::istream&, TJet<T>&);

template<typename T> 
TJet<T> operator+( TJet<T> const&, TJet<T> const& );

template<typename T> 
TJet<T> operator+( TJet<T> const&, T const& );

template<typename T> 
TJet<T> operator+( T const&, TJet<T> const& );

TJet<std::complex<double> > operator+( TJet<double >               const& x, TJet<std::complex<double> > const& y );
TJet<std::complex<double> > operator+( TJet<std::complex<double> > const& x, TJet<double >               const& y );
TJet<std::complex<double> > operator+( std::complex<double>        const& x, TJet<double >               const& y );
TJet<std::complex<double> > operator+( TJet<double >               const& y, std::complex<double>        const& x );
TJet<std::complex<double> > operator+( double                      const& x, TJet<std::complex<double> > const& y );
TJet<std::complex<double> > operator+( TJet<std::complex<double> > const& y, double                      const& x );

template<typename T> 
TJet<T> operator-( TJet<T> const&, TJet<T> const& );

template<typename T> 
TJet<T> operator-( TJet<T> const& );

template<typename T> 
TJet<T> operator-( TJet<T> const&, T const& );

template<typename T> 
TJet<T> operator-( T const&, TJet<T> const& );

TJet<std::complex<double> > operator-( TJet<double >               const& x, TJet<std::complex<double> > const& y );
TJet<std::complex<double> > operator-( TJet<std::complex<double> > const& x, TJet<double >               const& y );
TJet<std::complex<double> > operator-( std::complex<double>        const& x, TJet<double >               const& y );
TJet<std::complex<double> > operator-( TJet<double >               const& y, std::complex<double>        const& x );
TJet<std::complex<double> > operator-( double                      const& x, TJet<std::complex<double> > const& y );
TJet<std::complex<double> > operator-( TJet<std::complex<double> > const& y, double                      const& x );

template<typename T> 
TJet<T> operator*( TJet<T> const&, TJet<T> const& );

template<typename T> 
TJet<T> operator*( TJet<T> const&, T const& );

template<typename T> 
TJet<T> operator*( T const&, TJet<T> const& );

template<typename T> 
TJet<T> operator*( TJet<T> const&, const int& );

template<typename T> 
TJet<T> operator*( int const&, TJet<T> const& );

TJet<std::complex<double> > operator*( TJet<double >               const& x, TJet<std::complex<double> > const& y );
TJet<std::complex<double> > operator*( TJet<std::complex<double> > const& x, TJet<double >               const& y );
TJet<std::complex<double> > operator*( std::complex<double>        const& y, TJet<double >               const& x );
TJet<std::complex<double> > operator*( TJet<double >               const& x, std::complex<double>        const& y );
TJet<std::complex<double> > operator*( TJet<std::complex<double> > const& x, double                      const& y );
TJet<std::complex<double> > operator*( double                      const& x, TJet<std::complex<double> > const& y );

template<typename T> 
TJet<T> operator/( TJet<T> const&, TJet<T> const& );

template<typename T> 
TJet<T> operator/( TJet<T> const&, T const& );  

template<typename T> 
TJet<T> operator/( T const&,       TJet<T> const& ); 

template<typename T> 
TJet<T> operator/( TJet<T> const&, const int& ); 

TJet<std::complex<double> > operator/( TJet<double >               const & x,     TJet<std::complex<double> > const& y );
TJet<std::complex<double> > operator/( TJet<std::complex<double> > const & x,     TJet<double >               const& y );
TJet<std::complex<double> > operator/( std::complex<double>        const& x,      TJet<double >               const& y );
TJet<std::complex<double> > operator/( TJet<double >               const& y,      std::complex<double>        const& x );
TJet<std::complex<double> > operator/( TJet<std::complex<double> > const& x,      double                      const& y );
TJet<std::complex<double> > operator/( double                      const& x,      TJet<std::complex<double> > const& y );

template<typename T> 
bool operator==( TJet<T> const&, TJet<T> const& );

template<typename T> 
bool operator==( TJet<T> const&, T const& );

template<typename T> 
bool operator==( T  const&, TJet<T> const& );

template<typename T> 
bool operator!=( TJet<T> const& x, TJet<T> const& y ); 

template<typename T> 
bool operator!=( TJet<T> const& x, T const& y );

template<typename T> 
bool operator!=( T  const& x, TJet<T>  const& y ); 

template<typename T> 
TJet<T> operator^(    TJet<T>  const&,  TJet<T>   const& );

template<typename T> 
TJLterm<T> operator*( TJLterm<T> const&,  TJLterm<T> const& );

template<typename T> 
TJet<T> acos ( TJet<T> const& );

template<typename T> 
TJet<T> asin ( TJet<T> const& );

template<typename T> 
TJet<T> atan ( TJet<T> const& );

template<typename T> 
TJet<T> cos  ( TJet<T> const& );

template<typename T> 
TJet<T> cosh ( TJet<T> const& );

template<typename T> 
TJet<T> exp  ( TJet<T> const& );

template<typename T> 
TJet<T> log  ( TJet<T> const& );

template<typename T> 
TJet<T> log10( TJet<T> const& );

template<typename T> 
TJet<T> pow  ( TJet<T> const&, const double& );

template<typename T> 
TJet<T> pow  ( TJet<T> const&, int );

template<typename T> 
TJet<T> sin  ( TJet<T> const& );

template<typename T> 
TJet<T> sinh ( TJet<T> const& );

template<typename T> 
TJet<T> sqrt ( TJet<T> const& );

template<typename T> 
TJet<T> tan  ( TJet<T> const& );

template<typename T> 
TJet<T> tanh ( TJet<T> const& );

template<typename T> 
TJet<T> erfc ( TJet<T> const& );

TJet<std::complex<double> > erf    ( const TJet<std::complex<double> >& );
TJet<double > erf    ( const TJet<double >& );

//---------------------
// JL factory functions
//----------------------

JLPtr<std::complex<double> > makeJL(JLPtr<double> const& x);
JL1Ptr<std::complex<double> > makeJL(JL1Ptr<double> const& x);

template<typename T>
JLPtr<T> makeJL( EnvPtr<T> const& pje,  T value = T());

template<typename T>
JL1Ptr<T> makeJL( EnvPtr<T> const& pje,  T value = T());

template<typename T>
JLPtr<T> makeJL( const IntArray&, T const&, EnvPtr<T> const& pje );

template<typename T>
JL1Ptr<T> makeJL( const IntArray&, T const&, EnvPtr<T> const& pje );


// ==============================================================================

template <bool cond, typename U, typename V> 
class JLType;

template <typename U, typename V> 
class JLType<true, U, V> {
 public:
  typedef U  ResultT;
};

template <typename U, typename V> 
class JLType<false, U, V> {
 public:
  typedef V  ResultT;
};
 

//-------------------------------------------------------------------------
// class TJet
//-------------------------------------------------------------------------

template<typename T>
class TJet: public gms::FastAllocator  {

  public:

#ifdef FIRST_ORDER_JETS  
  typedef typename  JLType<true, JL1Ptr<T>,             JLPtr<T> >::ResultT         jl_t;    
  typedef typename  JLType<true, TJL1<T>,                 TJL<T> >::ResultT        tjl_t;    

#else 
  typedef typename  JLType<false, JL1Ptr<T>,             JLPtr<T> >::ResultT         jl_t;    
  typedef typename  JLType<false, TJL1<T>,                 TJL<T> >::ResultT        tjl_t;    
#endif
 
 // the following declarations are needed for the type conversion operator 
 // since the double and complex version are distinct classes 


  template<class U>
  friend class TJet;

  friend class TLieOperator<T>;

protected:

  mutable jl_t   _jl; 

 private:

  TJet _epsInverse() const;  // Calculates the inverse of
                             // nilpotent Jets.

  friend TJet<double > fabs( TJet<double> const& ); // ?????
  friend TJet<double > real( TJet<std::complex<double> > const& );
  friend TJet<double > imag( TJet<std::complex<double> > const& );

  TJet( typename TJet::jl_t const& jl );

protected:

  typename TJet::jl_t & operator->() const { return _jl; }

public:

  // Constructors and destructors_____________________________________

  TJet( EnvPtr<T> const&     env     = TJetEnvironment<T>::getLastEnv() );
  TJet( T, EnvPtr<T> const&  env     = TJetEnvironment<T>::getLastEnv() );

  TJet( TJet<T> const& );

  template<typename U>
  TJet( TJet<U> const& );

  virtual ~TJet();


  // Environment management __________________________________
 
public:


  void setEnvTo( TJet const& );                   // Changes environment to
  void setEnvTo( EnvPtr<T> const& );              // that of the argument.

  int intEnv() const;                             // Returns integer representation
                                                  // of the environment pointer.

  EnvPtr<T> Env() const;                          // ??? DANGER!!! These two break
  void Env( EnvPtr<T> const& env ) const;         // ??? DANGER!!! protection!!


  // Public member functions__________________________________________

  void addTerm( TJLterm<T> const& ); 
  // void appendTerm( TJLterm<T> const& ); 

  bool isNilpotent() const;

  void peekAt()      const;
  void printCoeffs() const;
  int  termCount()   const;  // Returns number of monomial terms

  void writeToFile( char*   /* Name of unopened file */ ) const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T* ) const;
  int  getEnvNumVar()     const;
  int  getEnvSpaceDim()   const;
  int  getEnvMaxWeight()  const;

  int  getWeight()  const;
  int  getAccuWgt() const;

  void setVariable( T   const&,  int const&, EnvPtr<T> const& pje);
  void setVariable( int const&,              EnvPtr<T> const& pje );
  void setVariable( T   const&,  int const& );
  void setVariable( int const& );

  void     setStandardPart( T const& std ); 
  T        standardPart() const            { return _jl->standardPart();    }

  void     clear();
  T        weightedDerivative( int  const* ) const;  // FIXME !
  T                derivative( int  const*  ) const; // FIXME !

  TJet     filter( int const&, int  const& ) const;  
                                  // Returns only those JLterms whose weight 
                                  // are between two specified values, inclusive.
  TJet     filter( bool (*)( IntArray const&, T const& ) ) const; 
                                   // Returns those JLterms for which the 
                                   // argument is satisfied.

  T        operator() ( Vector const& ) const;
  T        operator() ( T      const*)  const;	   
                                   // Performs a multinomial evaluation of 
				   // the Jet variable.  Essentially acts as a 
				   // power series expansion.

  TJet     operator() ( TJetVector<T> const& ) const;       // Self explanatory ...
  TJet     operator() ( TJet<T> const* y )     const;       // Self explanatory .    FIXME !     


  TJet     D( int      const* ) const ;	   // Performs differentiation of a Jet variable.
  TJet     D( IntArray const& ) const ;   


  // Operators________________________________________________________

  friend std::ostream& operator<<<>(std::ostream&, TJet const&);
  friend std::istream& operator>><>(std::istream&, TJet&);

  friend bool operator==<>( TJet const&, TJet const& );
  friend bool operator==<>( TJet const&, T const& );
  friend bool operator==<>( T    const&, TJet const& );

  // bool operator==( TJet const& ) const; // ??? what is this for
  // bool operator==( T    const& ) const;    // ??? what is this for

  TJet& operator=( TJet const& );
  TJet& operator=( T    const& );

  TJet& operator+=( TJet const& );
  TJet& operator+=( T    const& );
  TJet& operator-=( TJet const& );
  TJet& operator-=( T    const& );

  TJet& operator*=( TJet const& );
  TJet& operator*=( T    const& );
  TJet& operator/=( TJet const& );
  TJet& operator/=( T    const& );

  friend TJet operator+<>( TJet const&, TJet const& );
  friend TJet operator+<>( TJet const&, T    const& );
  friend TJet operator+<>( T    const&, TJet const& );
  friend TJet operator-<>( TJet const&, TJet const& );
  friend TJet operator-<>( TJet const& );
  friend TJet operator-<>( TJet const&, T    const& );
  friend TJet operator-<>( T    const&, TJet const& );

  friend TJet operator*<>( TJet const&, TJet const& );
  friend TJet<std::complex<double> > operator*( TJet<double >               const&,  TJet<std::complex<double> > const& );
  friend TJet<std::complex<double> > operator*( TJet<std::complex<double> > const&,  TJet<double >               const& );

  friend TJet operator*<>( TJet const&, T const& );
  friend TJet operator*<>( T    const&, TJet const& );   

  friend TJet operator/<>( TJet const&, TJet const& );   
  friend TJet operator/<>( TJet const&, T const& );      
  friend TJet operator/<>( T    const&, TJet const& ); 

  friend TJet operator^<>( TJet const&, TJet const& );   // Poisson bracket

  friend TJLterm<T> operator*<>(  TJLterm<T> const&,  TJLterm<T> const& );
  friend TJet operator*<>( TJet const&, int  const& );
  friend TJet operator*<>( int  const&, TJet const& );
  friend TJet operator/<>( TJet const&, int  const& ); 

  friend TJet acos<> ( TJet<T> const& );
  friend TJet asin<> ( TJet<T> const& );  
  friend TJet atan<> ( TJet<T> const& ); 
  friend TJet cos<>  ( TJet<T> const& );
  friend TJet cosh<> ( TJet<T> const& );
  friend TJet exp<>  ( TJet<T> const& );
  friend TJet log<>  ( TJet<T> const& );
  friend TJet log10<>( TJet<T> const& );
  friend TJet pow<>  ( TJet<T> const&, double const& );
  friend TJet pow<>  ( TJet<T> const&, int );
  friend TJet sin<>  ( TJet<T> const& );
  friend TJet sinh<> ( TJet<T> const& );
  friend TJet sqrt<> ( TJet<T> const& );
  friend TJet tan<>  ( TJet<T> const& );
  friend TJet tanh<> ( TJet<T> const& );
  friend TJet erfc<> ( TJet<T> const& );

  friend TJet<std::complex<double> > erf    ( const TJet<std::complex<double> >& );
  friend TJet<double > erf    ( const TJet<double >& );
  friend TJet<std::complex<double> > w ( const TJet<std::complex<double> >& );


  // Utility arithmetic functions ...
  void Negate();
  void Mult( T const& );

  // Inversion ...
  TJet Inverse() const;        // Returns the inverse jet
                               //  only when the number of
                               //  coordinates = 1.

  // ( Monomial term) Iterators  ...

  template <class U>
  class iter_
  : public boost::iterator_adaptor<
        iter_<U>                     // Derived
      , U                            // Base
      , boost::use_default           // Value
      , boost::forward_traversal_tag // CategoryOrTraversal
    > {
  
    template <typename V>
    friend class iter_;

    private:
        struct enabler {};  // a private type avoids misuse  
    public:
      iter_()
        : iter_::iterator_adaptor_(U(0)) {}

       explicit iter_(U p)
        : iter_::iterator_adaptor_(p) {}

       template <class OtherType>
       iter_(
             iter_<OtherType> const& other
             , typename boost::enable_if<
                boost::is_convertible<OtherType,U>
                , enabler
                >::type = enabler()
             )
         : iter_::iterator_adaptor_(other.base()) {}

  };

  typedef iter_< typename TJL<T>::iterator>             iterator;
  typedef iter_< typename TJL<T>::const_iterator> const_iterator;
 
  iterator       begin();
  const_iterator begin() const;

  iterator       end();
  const_iterator end()   const;

};

//-------------------------------------------------------------------------------------
// specializations for class TJet
//-------------------------------------------------------------------------------------

template<>
template<>
TJet<std::complex<double> >::TJet( TJet<double> const& );


//------------------------------------------------------------------------------------
// Tcoord and Tparam classes
//------------------------------------------------------------------------------------

template<typename T>
class Tcoord: public TJet<T> 
{

public:

  explicit Tcoord( T=T() ); 

 ~Tcoord();


  int Index() const { return _index; } 
  T   value() const { return _refpt; };  
   
  void instantiate(int index, EnvPtr<T> const& pje);
 
 private:

  Tcoord( Tcoord const& );                  // forbidden 
  Tcoord& operator=( Tcoord const& );       // forbidden

  int       _index;  
  T         _refpt; 

};


template<typename T>
class Tparam: public TJet<T> 
{

public:

  explicit Tparam( T = T() );
 ~Tparam();

  int Index()       { return _index; } 
  T   value() const { return _refpt; };  

  void instantiate(int index, EnvPtr<T> const& pje);

 private:

  int       _index; // the parameter index  ( > numVar )
  T         _refpt; 

  Tparam( Tparam const& );                 // forbidden - no implementation
  Tparam& operator=( Tparam const& );      // forbidden - no implementation

};

// ----------------------------------------------------------------------------------------------
// Inline functions for class Jet
// ----------------------------------------------------------------------------------------------

template<typename T>
inline EnvPtr<T> TJet<T>::Env() const
{
  return _jl->getEnv();
}

template<typename T>
inline void TJet<T>::Env(  EnvPtr<T> const& pje ) const
{
  _jl->setEnv(pje);;
}


template<typename T>
inline int TJet<T>::intEnv() const   // what is this for ????
{  
  return int( _jl->getEnv().get() );
}


template<typename T>
inline int TJet<T>::termCount() const
{
  return _jl->getCount();
}

template<typename T>
inline int TJet<T>::getEnvNumVar() const 
{
  return _jl->getEnv()->numVar();
}

template<typename T>
inline int TJet<T>::getEnvSpaceDim() const 
{
  return _jl->getEnv()->spaceDim();
}

template<typename T>
inline int TJet<T>::getEnvMaxWeight() const 
{
  return _jl->getEnv()->maxWeight();
}

template<typename T>
inline int TJet<T>::getWeight() const 
{
  return _jl->getWeight();
}

template<typename T>
inline int TJet<T>::getAccuWgt() const 
{
  return _jl->getAccuWgt();
}

template<typename T>
inline bool TJet<T>::isNilpotent() const 
{
  return _jl->isNilpotent();
}

// ================================================================



#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include<mxyzptlk/TJet.tcc>
#endif

#endif // TJET_H
