/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
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
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - all implementation details now completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES 
**************************************************************************
*************************************************************************/
#ifndef TJET_H
#define TJET_H

#include <slist.h>
#include <IntArray.h>
#include <VectorD.h>

#include <TJetEnvironment.h>
#include <TJL.h>
#include <TJL1.h>
#include <complex>

// Forward declarations

template<typename T> 
class TJet;

template<typename T> 
class TJetVector;

template<typename T> 
class TLieOperator;

TJet<double > fabs( const TJet<double >& );
TJet<double > real( const TJet<std::complex<double> >& );
TJet<double > imag( const TJet<std::complex<double> >& );

template<typename T> 
std::ostream& operator<<(std::ostream&, const TJet<T>&);

template<typename T> 
std::istream& operator>>(std::istream&, TJet<T>&);

template<typename T> 
TJet<T> operator+( const TJet<T>&, const TJet<T>& );

template<typename T> 
TJet<T> operator+( const TJet<T>&, const T& );

template<typename T> 
TJet<T> operator+( const T&, const TJet<T>& );

TJet<std::complex<double> > operator+( const TJet<double >& x,               const TJet<std::complex<double> >& y );
TJet<std::complex<double> > operator+( const TJet<std::complex<double> >& x, const TJet<double >& y );
TJet<std::complex<double> > operator+( const std::complex<double> & x,       const TJet<double >& y );
TJet<std::complex<double> > operator+( const TJet<double >& y,               const std::complex<double> & x );
TJet<std::complex<double> > operator+( const double& x,                      const TJet<std::complex<double> >& y );
TJet<std::complex<double> > operator+( const TJet<std::complex<double> >& y, const double& x );

template<typename T> 
TJet<T> operator-( const TJet<T>&, const TJet<T>& );

template<typename T> 
TJet<T> operator-( const TJet<T>& );

template<typename T> 
TJet<T> operator-( const TJet<T>&, const T& );

template<typename T> 
TJet<T> operator-( const T&, const TJet<T>& );

TJet<std::complex<double> > operator-( const TJet<double >& x,               const TJet<std::complex<double> >& y );
TJet<std::complex<double> > operator-( const TJet<std::complex<double> >& x, const TJet<double >& y );
TJet<std::complex<double> > operator-( const std::complex<double> & x,       const TJet<double >& y );
TJet<std::complex<double> > operator-( const TJet<double >& y,               const std::complex<double> & x );
TJet<std::complex<double> > operator-( const double& x,                      const TJet<std::complex<double> >& y );
TJet<std::complex<double> > operator-( const TJet<std::complex<double> >& y, const double& x );

template<typename T> 
TJet<T> operator*( const TJet<T>&, const TJet<T>& );

template<typename T> 
TJet<T> operator*( const TJet<T>&, const T& );

template<typename T> 
TJet<T> operator*( const T&, const TJet<T>& );

template<typename T> 
TJet<T> operator*( const TJet<T>&, const int& );

template<typename T> 
TJet<T> operator*( const int&, const TJet<T>& );

TJet<std::complex<double> > operator*( const TJet<double >& x,               const TJet<std::complex<double> >& y );
TJet<std::complex<double> > operator*( const TJet<std::complex<double> >& x, const TJet<double >& y );
TJet<std::complex<double> > operator*( const std::complex<double> & y,       const TJet<double >& x );
TJet<std::complex<double> > operator*( const TJet<double >& x,               const std::complex<double> & y );
TJet<std::complex<double> > operator*( const TJet<std::complex<double> >& x, const double& y );
TJet<std::complex<double> > operator*( const double& x,                      const TJet<std::complex<double> >& y );

template<typename T> 
TJet<T> operator/( const TJet<T>&, const TJet<T>& );

template<typename T> 
TJet<T> operator/( const TJet<T>&, const T& );  

template<typename T> 
TJet<T> operator/( const T&,       const TJet<T>& ); 

template<typename T> 
TJet<T> operator/( const TJet<T>&, const int& ); 

TJet<std::complex<double> > operator/( const TJet<double >& x,               const TJet<std::complex<double> >& y );
TJet<std::complex<double> > operator/( const TJet<std::complex<double> >& x, const TJet<double >& y );
TJet<std::complex<double> > operator/( const std::complex<double> & x,       const TJet<double >& y );
TJet<std::complex<double> > operator/( const TJet<double >& y,               const std::complex<double> & x );
TJet<std::complex<double> > operator/( const TJet<std::complex<double> >& x, const double& y );
TJet<std::complex<double> > operator/( const double& x,                      const TJet<std::complex<double> >& y );

template<typename T> 
bool operator==( const TJet<T>&, const TJet<T>& );

template<typename T> 
bool operator==( const TJet<T>&, const T& );

template<typename T> 
bool operator==( const T&, const TJet<T>& );

template<typename T> 
bool operator!=( const TJet<T>& x, const TJet<T>& y ); 

template<typename T> 
bool operator!=( const TJet<T>& x, const T& y );

template<typename T> 
bool operator!=( const T& x, const TJet<T>& y ); 

template<typename T> 
TJet<T> operator^( const TJet<T>&, const TJet<T>& );

template<typename T> 
TJLterm<T> operator*( const TJLterm<T>&, const TJLterm<T>& );

template<typename T> 
TJet<T> acos ( const TJet<T>& );

template<typename T> 
TJet<T> asin ( const TJet<T>& );

template<typename T> 
TJet<T> atan ( const TJet<T>& );

template<typename T> 
TJet<T> cos  ( const TJet<T>& );

template<typename T> 
TJet<T> cosh ( const TJet<T>& );

template<typename T> 
TJet<T> exp  ( const TJet<T>& );

template<typename T> 
TJet<T> log  ( const TJet<T>& );

template<typename T> 
TJet<T> log10( const TJet<T>& );

template<typename T> 
TJet<T> pow  ( const TJet<T>&, const double& );

template<typename T> 
TJet<T> pow  ( const TJet<T>&, int );

template<typename T> 
TJet<T> sin  ( const TJet<T>& );

template<typename T> 
TJet<T> sinh ( const TJet<T>& );

template<typename T> 
TJet<T> sqrt ( const TJet<T>& );

template<typename T> 
TJet<T> tan  ( const TJet<T>& );

template<typename T> 
TJet<T> tanh ( const TJet<T>& );

template<typename T> 
TJet<T> erfc ( const TJet<T>& );

TJet<std::complex<double> > erf    ( const TJet<std::complex<double> >& );
TJet<double > erf    ( const TJet<double >& );

// JL factory functions

template<typename T>
typename JLPtr<T>::Type makeJL( typename EnvPtr<T>::Type pje,  T value = T());

template<typename T>
typename JLPtr<T>::Type makeJL( const IntArray&, const T&, typename EnvPtr<T>::Type pje );


// ==============================================================================

template<typename T>

class TJet: public gms::FastAllocator 
{

 // the following declarations are needed for the type conversion operator 
 // since the double and complex version are distinct classes 

  friend class TJet<double>;
  friend class TJet<std::complex<double> >;

  friend class TLieOperator<T>;

protected:

  mutable typename JLPtr<T>::Type   _jl; 

 private:

  TJet _epsInverse() const;  // Calculates the inverse of
                             // nilpotent Jets.

  friend TJet<double > fabs( const TJet<double >& ); // ?????
  friend TJet<double > real( const TJet<std::complex<double> >& );
  friend TJet<double > imag( const TJet<std::complex<double> >& );

  // private constructor.  

  TJet(const typename JLPtr<T>::Type& jl );

protected:

  const typename JLPtr<T>::Type& operator->() const { return _jl; }

public:

  // Constructors and destructors_____________________________________

  TJet( typename EnvPtr<T>::Type     = _lastEnv );
  TJet( T, typename EnvPtr<T>::Type  = _lastEnv );
  TJet( const TJet& );
  
  ~TJet();

  operator TJet<std::complex<double> >() const;
  operator TJet<double> () const;

  void Reconstruct();
  void Reconstruct( typename EnvPtr<T>::Type );
  void Reconstruct( const IntArray&, const T&, typename EnvPtr<T>::Type );

  // Environment management __________________________________
 
public:

  static typename EnvPtr<T>::Type      _lastEnv; 
  static typename EnvPtr<T>::Type lastEnvironment();
  static void setLastEnv(typename EnvPtr<T>::Type pje); 


  // static void EnlargeEnvironment( const TJetEnvironment<T>* );


  void setEnvTo( const TJet& );                   // Changes environment to
  void setEnvTo( typename EnvPtr<T>::Type );      // that of the argument.

  int intEnv() const;                             // Returns integer representation
                                                  // of the environment pointer.

  typename EnvPtr<T>::Type Env() const;           // ??? DANGER!!! These two break

  void Env( typename EnvPtr<T>::Type env ) const; // ??? DANGER!!! protection!!


  // Public member functions__________________________________________

  TJLterm<T>* storePtr() {return _jl->storePtr();}  // reserve and get a ptr to the next available free block  
  
  TJLterm<T>* get();               // Pops the top term, which should be the 
                                   // one of lowest weight. **** eliminate ! ****
  TJLterm<T>  getLowestWeight();   // Removes the top term(lowest weight);

  TJLterm<T>  firstTerm() const;   // Returns a JLterm equivalent to the top term,
                                   // which should be the one of lowest weight.
  TJLterm<T>  lowTerm()  const;    // Returns a JLterm equivalent to the 
                                   // non-zero term of lowest weight.


  const TJLterm<T>* stepConstIteratorPtr() const;   
  void              resetConstIterator();
  //const TJLterm<T>& stepConstIteratorRef() const;   
  //void              resetIterator();
  //TJLterm<T>*       stepIterator();

  void addTerm( const TJLterm<T>& ); 

  bool isNilpotent() const;

  void peekAt()      const;
  void printCoeffs() const;
  int  termCount()   const;         // Returns number of monomial terms

  void writeToFile( char*   /* Name of unopened file */ ) const;
  void writeToFile( std::ofstream& ) const;

  void getReference( T* ) const;
  int  getEnvNumVar()     const;
  int  getEnvSpaceDim()   const;
  int  getEnvMaxWeight()  const;

  int  getWeight()  const;
  int  getAccuWgt() const;

  void scaleBy( T );

  void setVariable( const T&, const int&, typename EnvPtr<T>::Type pje);
  void setVariable( const int&,           typename EnvPtr<T>::Type pje );
  void setVariable( const T&, const int& );
  void setVariable( const int& );


  T        standardPart() const;
  void     clear();
  T        weightedDerivative( const int* ) const;
  T        derivative( const int* ) const;
  TJet<T>  filter(  const int&, const int& ) const;  
                                  // Returns only those JLterms whose weight 
                                  // are between two specified values, inclusive.
  TJet<T>  filter( bool (*)( const IntArray&, const T& ) ) const; 
                                   // Returns those JLterms for which the 
                                   // argument is satisfied.
  T        operator() ( const Vector& ) const;
  T        operator() ( const T* ) const;	   
                                   // Performs a multinomial evaluation of 
				   // the Jet variable.  Essentially acts as a 
				   // power series expansion.
  TJet<T>  operator() ( const TJet<T>* ) const;        // Self explanatory ...
  TJet<T>  operator() ( const TJetVector<T>& ) const;  // Self explanatory ...

  TJet<T>  D( const int* ) const ;	      // Performs differentiation of a Jet variable.
  TJet<T>  D( const IntArray& ) const ;   // Performs differentiation of a Jet variable.


  // Operators________________________________________________________

  friend std::ostream& operator<<<>(std::ostream&, const TJet&);
  friend std::istream& operator>><>(std::istream&, TJet&);

  friend bool operator==<>( const TJet&, const TJet& );
  friend bool operator==<>( const TJet&, const T& );
  friend bool operator==<>( const T&, const TJet& );

  // bool operator==( const TJet& ) const; // ??? what is this for
  // bool operator==( const T& ) const;    // ??? what is this for

  TJet& operator=( const TJet& );
  TJet& operator=( const T& );
  TJet& DeepCopy( const TJet& );

  TJet& operator+=( const TJet& );
  TJet& operator+=( const T& );
  TJet& operator-=( const TJet& );
  TJet& operator-=( const T& );

  TJet& operator*=( const TJet& );
  TJet& operator*=( const T& );
  TJet& operator/=( const TJet& );
  TJet& operator/=( const T& );

  friend TJet operator+<>( const TJet&, const TJet& );
  friend TJet operator+<>( const TJet&, const T& );
  friend TJet operator+<>( const T&, const TJet& );
  friend TJet operator-<>( const TJet&, const TJet& );
  friend TJet operator-<>( const TJet& );
  friend TJet operator-<>( const TJet&, const T& );
  friend TJet operator-<>( const T&, const TJet& );

  friend TJet operator*<>( const TJet&, const TJet& );
  friend TJet<std::complex<double> > operator*( const TJet<double >&, const TJet<std::complex<double> >& );
  friend TJet<std::complex<double> > operator*( const TJet<std::complex<double> >&, const TJet<double >& );
  friend TJet operator*<>( const TJet&, const T& );
  friend TJet operator*<>( const T&, const TJet& );     // ??? Change this!!

  friend TJet operator/<>( const TJet&, const TJet& );   // ??? Friendliness
  friend TJet operator/<>( const TJet&, const T& );     // ??? unnecessary
  friend TJet operator/<>( const T&, const TJet& ); 

  friend TJet operator^<>( const TJet&, const TJet& );   // Poisson bracket

  friend TJLterm<T> operator*<>( const TJLterm<T>&, const TJLterm<T>& );
  friend TJet operator*<>( const TJet&, const int& );
  friend TJet operator*<>( const int&, const TJet& );
  friend TJet operator/<>( const TJet&, const int& ); 

  friend TJet acos<> ( const TJet<T>& );
  friend TJet asin<> ( const TJet<T>& );  
  friend TJet atan<> ( const TJet<T>& ); 
  friend TJet cos<>  ( const TJet<T>& );
  friend TJet cosh<> ( const TJet<T>& );
  friend TJet exp<>  ( const TJet<T>& );
  friend TJet log<>  ( const TJet<T>& );
  friend TJet log10<>( const TJet<T>& );
  friend TJet pow<>  ( const TJet<T>&, const double& );
  friend TJet pow<>  ( const TJet<T>&, int );
  friend TJet sin<>  ( const TJet<T>& );
  friend TJet sinh<> ( const TJet<T>& );
  friend TJet sqrt<> ( const TJet<T>& );
  friend TJet tan<>  ( const TJet<T>& );
  friend TJet tanh<> ( const TJet<T>& );
  friend TJet erfc<> ( const TJet<T>& );

  friend TJet<std::complex<double> > erf    ( const TJet<std::complex<double> >& );
  friend TJet<double > erf    ( const TJet<double >& );
  friend TJet<std::complex<double> > w ( const TJet<std::complex<double> >& );


  // Utility arithmetic functions ...
  void Negate();
  void Mult( const T& );


  // Inversion ...
  TJet Inverse() const;        // Returns the inverse jet
                               //  only when the number of
                               //  coordinates = 1.

};

// specializations for class TJet

 template<>
 TJet<double>::operator TJet<std::complex<double> >() const;
 
 template<>
 TJet<std::complex<double> >::operator TJet<double> () const;


//------------------------------------------------------------------------------------
// Tcoord and Tparam classes
//------------------------------------------------------------------------------------

template<typename T>
class Tcoord: public TJet<T> 
{

public:

  Tcoord( T=T() ); 
 ~Tcoord();

  inline int Index() const { return _index; } 
  inline T   value() const { return _refpt; };  
   
  void instantiate(int index, typename EnvPtr<T>::Type pje);
 
 private:

  int       _index;  
  T         _refpt; 

  Tcoord( const Tcoord& );             // forbidden - no implementation
  void    operator=( const T& );       // forbidden - no implementation
  Tcoord& operator=( const Tcoord& );  // forbidden - no implementation
  Tcoord& operator=( const TJet<T>& ); // forbidden - no implementation

};


template<typename T>
class Tparam: public TJet<T> 
{

public:

  Tparam( T = T() );
 ~Tparam();

  inline int Index() { return _index; } 
  inline T   value() const { return _refpt; };  

  void instantiate(int index, typename EnvPtr<T>::Type pje);

 private:

  int       _index; // the parameter index  ( > numVar )
  T         _refpt; 

  Tparam( const Tparam& );                 // forbidden - no implementation
  void    operator=( const T& );           // forbidden - no implementation
  Tparam& operator=( const Tparam& );      // forbidden - no implementation
  Tparam& operator=( const TJet<T>& );     // forbidden - no implementation

};

// ----------------------------------------------------------------------------------------------
// Inline functions for class Jet
// ----------------------------------------------------------------------------------------------

template<typename T>
inline typename EnvPtr<T>::Type TJet<T>::Env() const
{
  return _jl->getEnv();
}

template<typename T>
inline void TJet<T>::Env(  typename EnvPtr<T>::Type pje ) const
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


template<typename T>
inline typename EnvPtr<T>::Type TJet<T>::lastEnvironment()
{
  return _lastEnv;
}

template<typename T>
inline void TJet<T>::setLastEnv(typename EnvPtr<T>::Type pje) 
{
  TJet<T>::_lastEnv =  pje;

}


#ifdef  MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJet.tcc>
#endif

#endif // TJET_H
