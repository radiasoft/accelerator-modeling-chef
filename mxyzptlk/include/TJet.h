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
******  Feb 2005   Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******
******  Efficiency improvements.
******  - New memory management.
******
******  Sept 2005   ostiguy@fnal.gov
******              - new code based on a single template parameter
******                instead of two. Mixed mode handled
******                using conversion operators.
******  
**************************************************************************
*************************************************************************/


#ifndef TJET_H
#define TJET_H

#include <slist.h>
#include <dlist.h>
#include <IntArray.h>

#include <TJetEnvironment.h>
#include <TJL.h>
#include <complex>


// Forward declarations
class Vector;

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

// Class TJet template
template<typename T>

class TJet: public gms::FastAllocator 
{

 // the following declarations are needed for the type conversion operator 
 // since the double and complex version are distinct classes 

  friend class TJet<double>;
  friend class TJet<std::complex<double> >;

  friend class TLieOperator<T>;

private:
  TJL<T>*     _jl;
  dlist_iterator* _constIterPtr;
  dlist_iterator* _iterPtr;

  TJet _epsInverse() const;  // Calculates the inverse of
                             // nilpotent Jets.
  TJet _concat() const;

  TJet _truncMult( const TJet&, const int& ) const;  // Used by division algorithm.

  friend TJet<double > fabs( const TJet<double >& );
  friend TJet<double > real( const TJet<std::complex<double> >& );
  friend TJet<double > imag( const TJet<std::complex<double> >& );

  // Utility functions; used by public analytic functions ...
 
 static TJet _epsCos( const TJet& );
  static TJet _epsExp( const TJet& );
  static TJet _epsPow( const TJet&, const double& );
  static TJet _epsSin( const TJet& );
  static TJet _epsSqrt( const TJet& ); 

protected:

  static slist _newCoords;
  static slist _newValues;
  static int   _currentIndex;

  TJL<T>* operator->() const { return _jl; }

public:

  // Constructors and destructors_____________________________________

  TJet( TJetEnvironment<T>* = _lastEnv );
  TJet( T, TJetEnvironment<T>* = _lastEnv );
  TJet( const TJet& );
  
  ~TJet();

  operator TJet<std::complex<double> >() const;
  operator TJet<double> () const;

  void Reconstruct();
  void Reconstruct( TJetEnvironment<T>* pje );
  void Reconstruct( const IntArray&, const T&, TJetEnvironment<T>* pje );

  // Environment management __________________________________
 
public:

  static TJetEnvironment<T>* _workEnv; 
  static TJetEnvironment<T>* _lastEnv; 

  static const TJetEnvironment<T>* workEnvironment();
  static const TJetEnvironment<T>* lastEnvironment();

  static void BeginEnvironment( int = 1 /* maximum weight */ );

  static void Parameters();

  static TJetEnvironment<T>* EndEnvironment( double* = 0 /* scale array */ );

  static void EnlargeEnvironment( const TJetEnvironment<T>* );
 
  static TJetEnvironment<T>* CreateEnvFrom( const Vector&, int );
    // POSTCONDITION: The _lastEnv environment pointer is NOT changed.

  static TJetEnvironment<double>*                CreateEnvFrom( const TJetEnvironment<std::complex<double> >* );
  static TJetEnvironment<std::complex<double> >* CreateEnvFrom( const TJetEnvironment<double>* );


  void setEnvTo( const TJet& );                   // Changes environment to
  void setEnvTo( TJetEnvironment<T>* );           // that of the argument.

  int intEnv() const;                             // Returns integer representation
                                                  // of the environment pointer.

  TJetEnvironment<T>* Env() const;                // ??? DANGER!!! These two break

  void Env( TJetEnvironment<T>* pje ) const;      // ??? DANGER!!! protection!!


  // Public member functions__________________________________________

  TJLterm<T>* storePtr() {return _jl->storePtr();}  // reserve and get a ptr to the next available free block  
  
  TJLterm<T>* get();             // Pops the top term, which should be the 
                                     // one of lowest weight.
  TJLterm<T> firstTerm() const;  // Returns a JLterm equivalent to the top term,
                                     // which should be the one of lowest weight.
  TJLterm<T>  lowTerm()  const;  // Returns a JLterm equivalent to the 
                                     // non-zero term of lowest weight.
  void addTerm( TJLterm<T>* );   // Public only for diagnostic purposes.

  void              resetConstIterator();
  TJLterm<T>        stepConstIterator()  const;
  const TJLterm<T>& stepConstIteratorRef()  const;
  const TJLterm<T>* stepConstIteratorPtr()  const;
  void              resetIterator();
  TJLterm<T>*       stepIterator();

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

  void setVariable( const T&, const int&, TJetEnvironment<T>* );
               // Alters the environment
  void setVariable( const T&, const int& );
               // Alters its environment
  void setVariable( const int&, TJetEnvironment<T>* );
               // Does not alter the environment.
  void setVariable( const int& );
               // Does not alter its environment.

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
  // friend bool operator==<>( const TJet&, const TJet& );
  // friend bool operator==<>( const TJet&, const T& );
  // friend bool operator==<>( const T&, const TJet& );
  bool operator==( const TJet& ) const;
  bool operator==( const T& ) const;

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

  friend TJet<T> acos<> ( const TJet<T>& );
  friend TJet<T> asin<> ( const TJet<T>& );
  friend TJet<T> atan<> ( const TJet<T>& );
  friend TJet<T> cos<>  ( const TJet<T>& );
  friend TJet<T> cosh<> ( const TJet<T>& );
  friend TJet<T> exp<>  ( const TJet<T>& );
  friend TJet<T> log<>  ( const TJet<T>& );
  friend TJet<T> log10<>( const TJet<T>& );
  friend TJet<T> pow<>  ( const TJet<T>&, const double& );
  friend TJet<T> pow<>  ( const TJet<T>&, int );
  friend TJet<T> sin<>  ( const TJet<T>& );
  friend TJet<T> sinh<> ( const TJet<T>& );
  friend TJet<T> sqrt<> ( const TJet<T>& );
  friend TJet<T> tan<>  ( const TJet<T>& );
  friend TJet<T> tanh<> ( const TJet<T>& );
  friend TJet<T> erfc<> ( const TJet<T>& );
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

  static slist _environments;  // environments is made public for
                               // convenience, but no user program
                               // should play around directly
                               // with it.
};

// *******************************************************************

// specializations for class TJet

 template<>
 TJet<double>::operator TJet<std::complex<double> >() const;
 
 template<>
 TJet<std::complex<double> >::operator TJet<double> () const;

 template <>
 TJetEnvironment<double>* TJet<double>::CreateEnvFrom( const TJetEnvironment<std::complex<double> >* );

 template<>
 TJetEnvironment<std::complex<double> >* TJet<std::complex<double> >::CreateEnvFrom( const TJetEnvironment<double>* );

 template<>
 TJetEnvironment<double>* TJet<double>::CreateEnvFrom( const Vector&, int );

//***********************************************************************

template<typename T>
class Tcoord : public TJet<T> 
{
private:
  int _index;
public:
  Tcoord( T = T() );
  Tcoord( const Tcoord& );
  ~Tcoord();

  void    operator=( const T& );
  Tcoord& operator=( const Tcoord& );
  Tcoord& operator=( const TJet<T>& );

  inline int Index() { return _index; }
};


// Inline functions

template<typename T>
inline TJetEnvironment<T>* TJet<T>::Env() const
{
  return _jl->_myEnv;
}

template<typename T>
inline void TJet<T>::Env( TJetEnvironment<T>* pje ) const
{
  pje = _jl->_myEnv;
}


template<typename T>
inline int TJet<T>::intEnv() const
{  
  return int( _jl->_myEnv );
}


template<typename T>
inline int TJet<T>::termCount() const
{
  return _jl->_count;
}

template<typename T>
inline int TJet<T>::getEnvNumVar() const 
{
  return _jl->_myEnv->_numVar;
}

template<typename T>
inline int TJet<T>::getEnvSpaceDim() const 
{
  return _jl->_myEnv->_spaceDim;
}

template<typename T>
inline int TJet<T>::getEnvMaxWeight() const 
{
  return _jl->_myEnv->_maxWeight;
}

template<typename T>
inline int TJet<T>::getWeight() const 
{
  return _jl->_weight;
}

template<typename T>
inline int TJet<T>::getAccuWgt() const 
{
  return _jl->_accuWgt;
}

template<typename T>
inline bool TJet<T>::isNilpotent() const 
{
  return _jl->isNilpotent();
}

template<typename T>
inline const TJetEnvironment<T>* TJet<T>::workEnvironment()
{
  return _workEnv;
}

template<typename T>
inline const TJetEnvironment<T>* TJet<T>::lastEnvironment()
{
  return _lastEnv;
}

template<typename T>
inline bool operator==( const T& x, const TJet<T>& y )
{
  return y == x;
}

#ifdef  MXYZPTLK_IMPLICIT_TEMPLATES
#include <TJet.tcc>
#endif

#endif // TJET_H
