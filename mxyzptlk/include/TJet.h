/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJet.h
******  Version:   1.0
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef TJET_H
#define TJET_H

#include "slist.h"
#include "dlist.h"
#include "IntArray.h"
#include "VectorD.h"

#include "TJetEnvironment.h"
#include "TJL.h"

template<typename T1, typename T2> struct TJLterm;
template<typename T1, typename T2> struct TJL;
template<typename T1, typename T2> class  TJetVector;
template<typename T1, typename T2> class  TMapping;
template<typename T1, typename T2> class  TLieOperator;


template<typename T1,typename T2>
class TJet 
{
private:
  TJL<T1,T2>*     _jl;
  dlist_iterator* _constIterPtr;
  dlist_iterator* _iterPtr;

  TJet _epsInverse() const;  // Calculates the inverse of
                             // nilpotent Jets.
  TJet _concat() const;

  TJet _truncMult( const TJet&, const int& ) const;  // Used by division algorithm.

  friend TJet<double,FNAL::Complex> fabs( const TJet<double,FNAL::Complex>& );
  friend TJet<double,FNAL::Complex> real( const TJet<FNAL::Complex,double>& );
  friend TJet<double,FNAL::Complex> imag( const TJet<FNAL::Complex,double>& );
  friend TJet<FNAL::Complex,double> operator*( const FNAL::Complex&, const TJet<double,FNAL::Complex>& );

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

  TJL<T1,T2>* operator->() const { return _jl; }

public:
  static TJetEnvironment<T1,T2>* _workEnv;
  static TJetEnvironment<T1,T2>* _lastEnv; 

  // Constructors and destructors_____________________________________
  TJet( TJetEnvironment<T1,T2>* = _lastEnv );
  TJet( T1, TJetEnvironment<T1,T2>* = _lastEnv );
  TJet( const TJet& );
  TJet( const TJet<T2,T1>&  );
  TJet( const TJet<T2,T1>&, TJetEnvironment<T1,T2>* );
  ~TJet();

  void Reconstruct();
  void Reconstruct( TJetEnvironment<T1,T2>* pje );

  static void BeginEnvironment( int = 1 /* maximum weight */ );
  static void Parameters();
  static TJetEnvironment<T1,T2>* EndEnvironment( double* = 0 /* scale array */ );
  static void EnlargeEnvironment( TJetEnvironment<T1,T2>* );
  static TJetEnvironment<T1,T2>* CreateEnvFrom( TJetEnvironment<T2,T1>* );

  static const TJetEnvironment<T1,T2>* workEnvironment();
  static const TJetEnvironment<T1,T2>* lastEnvironment();

  void setEnvTo( const TJet& );                   // Changes environment to
  void setEnvTo( TJetEnvironment<T1,T2>* );       // that of the argument.

  int intEnv() const;                       // Returns integer representation
                                            // of the environment pointer.
  TJetEnvironment<T1,T2>* Env() const;      // ??? DANGER!!! These two break
  void Env( TJetEnvironment<T1,T2>* pje ) const; // ??? DANGER!!! protection!!


  TJL<T2,T1>* newJLOpp() const;                      // Used by conversion routines.
  TJL<T2,T1>* newJLOpp( TJetEnvironment<T2,T1>* ) const;

  // Public member functions__________________________________________
  TJLterm<T1,T2>* get();             // Pops the top term, which should be the 
                                     // one of lowest weight.
  TJLterm<T1,T2> firstTerm() const;  // Returns a JLterm equivalent to the top term,
                                     // which should be the one of lowest weight.
  TJLterm<T1,T2>  lowTerm()  const;  // Returns a JLterm equivalent to the 
                                     // non-zero term of lowest weight.
  void addTerm( TJLterm<T1,T2>* );   // Public only for diagnostic purposes.

  void            resetConstIterator();
  TJLterm<T1,T2>  stepConstIterator()  const;
  void            resetIterator();
  TJLterm<T1,T2>* stepIterator();

  bool isNilpotent() const;

  void peekAt() const;
  void printCoeffs() const;
  void peekAt( int ) const;       // Prints to Jet::scratchFile
  void printCoeffs( int ) const;
  int  termCount() const;         // Returns number of monomial terms

  void writeToFile( char*   /* Name of unopened file */ ) const;
  void writeToFile( FILE* ) const;

  void getReference( T1* ) const;
  int  getEnvNumVar() const;
  int  getEnvSpaceDim() const;
  int  getEnvMaxWeight() const;

  int  getWeight() const;
  int  getAccuWgt() const;

  void scaleBy( T1 );

  void setVariable( const T1&, const int&, TJetEnvironment<T1,T2>* );
               // Alters the environment
  void setVariable( const T1&, const int& );
               // Alters its environment
  void setVariable( const int&, TJetEnvironment<T1,T2>* );
               // Does not alter the environment.
  void setVariable( const int& );
               // Does not alter its environment.

  T1 standardPart() const;
  void clear();
  T1 weightedDerivative( const int* ) const;
  T1 derivative( const int* ) const;
  TJet<T1,T2> filter(  const int&, const int& ) const;  
                                  // Returns only those JLterms whose weight 
                                  // are between two specified values, inclusive.
  TJet<T1,T2> filter( bool (*)( const IntArray&, const T1& ) ) const; 
                                   // Returns those JLterms for which the 
                                   // argument is satisfied.
  T1 operator() ( const Vector& ) const;
  T1 operator() ( const T1* ) const;	   
                                   // Performs a multinomial evaluation of 
				   // the Jet variable.  Essentially acts as a 
				   // power series expansion.
  TJet<T1,T2>  operator() ( const TJet<T1,T2>* ) const;        // Self explanatory ...
  TJet<T1,T2>  operator() ( const TJetVector<T1,T2>& ) const;  // Self explanatory ...

  TJet<T1,T2>  D( const int* ) const ;	      // Performs differentiation of a Jet variable.
  TJet<T1,T2>  D( const IntArray& ) const ;   // Performs differentiation of a Jet variable.


  // Operators________________________________________________________
  friend std::ostream& operator<<<>(std::ostream&, const TJet&);
  friend std::istream& operator>><>(std::istream&, TJet&);
  friend bool operator==<>( const TJet&, const TJet& );
  friend bool operator==<>( const TJet&, const T1& );
  friend bool operator==<>( const T1&, const TJet& );

  TJet& operator=( const TJet& );
  TJet& operator=( const TJet<T2,T1>& );
  TJet& operator=( const T1& );
  TJet& DeepCopy( const TJet& );

  TJet& operator+=( const TJet& );
  TJet& operator+=( const T1& );
  TJet& operator-=( const TJet& );
  TJet& operator-=( const T1& );
  TJet& operator*=( const TJet& );
  TJet& operator*=( const T1& );
  TJet& operator/=( const TJet& );
  TJet& operator/=( const T1& );


  friend TJet operator+<>( const TJet&, const TJet& );
  friend TJet operator+<>( const TJet&, const T1& );
  friend TJet operator+<>( const T1&, const TJet& );
  friend TJet operator-<>( const TJet&, const TJet& );
  friend TJet operator-<>( const TJet& );
  friend TJet operator-<>( const TJet&, const T1& );
  friend TJet operator-<>( const T1&, const TJet& );
  friend TJet operator*<>( const TJet&, const TJet& );
  friend TJet operator*<>( const TJet&, const T1& );
  friend TJet operator*<>( const T1&, const TJet& );     // ??? Change this!!
  friend TJet operator/<>( const TJet&, const TJet& );   // ??? Friendliness
  friend TJet operator/<>( const TJet&, const T1& );     // ??? unnecessary
  friend TJet operator/<>( const T1&, const TJet& ); 
  friend TJet operator/<>( const TJet&, const T2& );     // ??? unnecessary
  friend TJet operator/<>( const T2&, const TJet& ); 
  friend TJet operator^<>( const TJet&, const TJet& );   // Poisson bracket

  friend TJet<T2,T1> operator*<>( T2&, const TJet<T1,T2>& );
  friend TJet<T2,T1> operator*<>( const TJet<T1,T2>&, const T2& );

  friend TJLterm<T1,T2> operator*<>( const TJLterm<T1,T2>&, const TJLterm<T1,T2>& );
  friend TJet operator*<>( const TJet&, const int& );
  friend TJet operator*<>( const int&, const TJet& );
  friend TJet operator/<>( const TJet&, const int& ); 


  // Utility arithmetic functions ...
  void Negate();
  void Mult( const T1& );


  // Inversion ...
  TJet Inverse() const;  // Returns the inverse jet
                         //  only when the number of
                         //  coordinates = 1.

  static slist _environments;  // environments is made public for
                               // convenience, but no user program
                               // should play around directly
                               // with it.
};

template<typename T1,typename T2> bool operator!=( const TJet<T1,T2>&, const TJet<T1,T2>& );
template<typename T1,typename T2> bool operator!=( const TJet<T1,T2>&, const T1& );
template<typename T1,typename T2> bool operator!=( const T1&, const TJet<T1,T2>& );

// *******************************************************************

template<typename T1, typename T2>
class Tcoord : public TJet<T1,T2> 
{
private:
  int _index;
public:
  Tcoord( T1 = 0.0 );
  Tcoord( const Tcoord& );
  ~Tcoord();

  void    operator=( const T1& );
  Tcoord& operator=( const Tcoord& );
  Tcoord& operator=( const TJet<T1,T2>& );

  inline int Index() { return _index; }
};


template<typename T1, typename T2>
inline TJetEnvironment<T1,T2>* TJet<T1,T2>::Env() const
{
  return _jl->_myEnv;
}

template<typename T1, typename T2>
inline void TJet<T1,T2>::Env( TJetEnvironment<T1,T2>* pje ) const
{
  pje = _jl->_myEnv;
}


template<typename T1, typename T2>
inline int TJet<T1,T2>::intEnv() const
{  
  return int( _jl->_myEnv );
}


template<typename T1, typename T2>
inline int TJet<T1,T2>::termCount() const
{
  return _jl->_count;
}

template<typename T1, typename T2>
inline int TJet<T1,T2>::getEnvNumVar() const 
{
  return _jl->_myEnv->_numVar;
}

template<typename T1, typename T2>
inline int TJet<T1,T2>::getEnvSpaceDim() const 
{
  return _jl->_myEnv->_spaceDim;
}

template<typename T1, typename T2>
inline int TJet<T1,T2>::getEnvMaxWeight() const 
{
  return _jl->_myEnv->_maxWeight;
}

template<typename T1, typename T2>
inline int TJet<T1,T2>::getWeight() const 
{
  return _jl->_weight;
}

template<typename T1, typename T2>
inline int TJet<T1,T2>::getAccuWgt() const 
{
  return _jl->_accuWgt;
}

template<typename T1, typename T2>
inline bool TJet<T1,T2>::isNilpotent() const 
{
  return _jl->isNilpotent();
}

template<typename T1, typename T2>
inline const TJetEnvironment<T1,T2>* TJet<T1,T2>::workEnvironment()
{
  return _workEnv;
}

template<typename T1, typename T2>
inline const TJetEnvironment<T1,T2>* TJet<T1,T2>::lastEnvironment()
{
  return _lastEnv;
}


// Templated operators

template<typename T1,typename T2> std::ostream& operator<<(std::ostream&, const TJet<T1,T2>&);
template<typename T1,typename T2> std::istream& operator>>(std::istream&, TJet<T1,T2>&);
template<typename T1,typename T2> bool operator==( const TJet<T1,T2>&, const TJet<T1,T2>& );
template<typename T1,typename T2> bool operator==( const TJet<T1,T2>&, const T1& );
template<typename T1,typename T2> bool operator==( const T1&, const TJet<T1,T2>& );

template<typename T1,typename T2> TJet<T1,T2> operator+( const TJet<T1,T2>&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator+( const TJet<T1,T2>&, const T1& );
template<typename T1,typename T2> TJet<T1,T2> operator+( const T1&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator-( const TJet<T1,T2>&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator-( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator-( const TJet<T1,T2>&, const T1& );
template<typename T1,typename T2> TJet<T1,T2> operator-( const T1&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator*( const TJet<T1,T2>&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator*( const TJet<T1,T2>&, const T1& );
template<typename T1,typename T2> TJet<T1,T2> operator*( const T1&, const TJet<T1,T2>& );  
template<typename T1,typename T2> TJet<T1,T2> operator/( const TJet<T1,T2>&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator/( const TJet<T1,T2>&, const T1& );  
template<typename T1,typename T2> TJet<T1,T2> operator/( const T1&, const TJet<T1,T2>& ); 
template<typename T1,typename T2> TJet<T1,T2> operator/( const TJet<T1,T2>&, const T2& );  
template<typename T1,typename T2> TJet<T1,T2> operator/( const T2&, const TJet<T1,T2>& ); 
template<typename T1,typename T2> TJet<T1,T2> operator^( const TJet<T1,T2>&, const TJet<T1,T2>& );

template<typename T1,typename T2> TJet<T2,T1> operator*( T2&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T2,T1> operator*( const TJet<T1,T2>&, const T2& );

template<typename T1,typename T2> TJLterm<T1,T2> operator*( const TJLterm<T1,T2>&, const TJLterm<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator*( const TJet<T1,T2>&, const int& );
template<typename T1,typename T2> TJet<T1,T2> operator*( const int&, const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> operator/( const TJet<T1,T2>&, const int& ); 


// Templated overloaded analytic functions
template<typename T1,typename T2> TJet<T1,T2> acos ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> asin ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> atan ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> cos  ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> cosh ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> exp  ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> log  ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> log10( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> pow  ( const TJet<T1,T2>&, const double& );
template<typename T1,typename T2> TJet<T1,T2> pow  ( const TJet<T1,T2>&, int );
template<typename T1,typename T2> TJet<T1,T2> sin  ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> sinh ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> sqrt ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> tan  ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> tanh ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> erf  ( const TJet<T1,T2>& );
template<typename T1,typename T2> TJet<T1,T2> erfc ( const TJet<T1,T2>& );

TJet<double,FNAL::Complex> fabs( const TJet<double,FNAL::Complex>& );
TJet<double,FNAL::Complex> real( const TJet<FNAL::Complex,double>& );
TJet<double,FNAL::Complex> imag( const TJet<FNAL::Complex,double>& );

typedef TJet<double,FNAL::Complex> Jet;
typedef TJet<FNAL::Complex,double> JetC;
typedef Tcoord<double,FNAL::Complex> coord;
typedef Tcoord<FNAL::Complex,double> coordC;

#endif // TJET_H
