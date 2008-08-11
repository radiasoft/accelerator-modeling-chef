/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetVector.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
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
******
******  Revision History
******   
******  Feb 2005       Jean-Francois Ostiguy
******                 ostiguy@fnal.gov 
****** 
****** - new memory management scheme 
******                                                            
******  Sep-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to use a single class template parameter
******   rather than two. Mixed mode operations now handled using 
******   implicit conversion.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - implementation details completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
******  Dec 2006 ostiguy@fnal.gov    
****** 
******  - New implementation. TJetVector is now based on a std::vector<Jet>
******    container (the previous version was based on a dynamically allocated 
******    array of raw Jet*). Since Jet is now basically an envelope for a 
******    a smart_ptr to implementation, its memory footprint is minimal. 
******    Use of value semantics for Jet components vastly simplifies memory 
******    management.       
******  
******  Apr 2007 ostiguy@fnal.gov
******
******   - added STL-style iterators
****** 
**************************************************************************
*************************************************************************/
#ifndef TJETVECTOR_H
#define TJETVECTOR_H

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <mxyzptlk/TJet.h>
#include <mxyzptlk/EnvPtr.h>

// Forward declarations
typedef class TVector<double> Vector;

template<typename T>
class TMatrix;

template<typename T> 
class TJetVector;

template<typename T> 
TJetVector<T> operator-( TJetVector<T> const& );

template<typename T> 
TJetVector<T> operator*( TJet<T> const&,       TJetVector<T> const& );

template<typename T, typename U>
TJetVector<T> operator*( U       const&,       TJetVector<T> const& );

template<typename T, typename U>
TJetVector<T> operator*( TJetVector<T> const&,  U const& );

template<typename T> 
TJetVector<T> operator*( TMatrix<T>    const&,  TJetVector<T> const& );

template<typename T>
TJetVector<T> operator*( TJet<T>       const&,  Vector const& );

template<typename T> 
TJetVector<T> operator^( Vector const&,  TJetVector<T> const& );

template<typename T> 
std::ostream& operator<<( std::ostream&, TJetVector<T> const& );

template<typename T> 
std::istream& operator>>( std::istream&, TJetVector<T>& );

//---------------------------------------------
// Class TJetVector template
//--------------------------------------------

template<typename T>
class TJetVector {

template<typename U>
friend class TJetVector;
 
protected:

  EnvPtr<T>                       myEnv_;
  std::vector< TJet<T> >          comp_;

public:

  typedef typename std::vector<TJet<T> >::value_type           value_type;
  typedef typename std::vector<TJet<T> >::iterator             iterator;
  typedef typename std::vector<TJet<T> >::const_iterator const_iterator;

  typedef typename std::vector<TJet<T> >::reverse_iterator             reverse_iterator;
  typedef typename std::vector<TJet<T> >::const_reverse_iterator const_reverse_iterator;

  iterator                begin();
  const_iterator          begin()  const;

  iterator                end();
  const_iterator          end()    const;

  reverse_iterator        rbegin();
  const_reverse_iterator  rbegin() const;

  reverse_iterator        rend();
  const_reverse_iterator  rend()   const;


  TJetVector( EnvPtr<T> const& env = TJetEnvironment<T>::topEnv() );

  TJetVector( int n,  EnvPtr<T> const& env = TJetEnvironment<T>::topEnv() );

  template<typename iterator_t>
  TJetVector( iterator_t itstart, iterator_t itend,  EnvPtr<T> const& env = TJetEnvironment<T>::topEnv() ); 

  TJetVector( TJetVector const& );

  template<typename U>
  TJetVector(TJetVector<U> const&);


 ~TJetVector();

  TJet<T> const&   operator[]      ( int idx) const { return comp_[idx]; } 
  TJet<T>&         operator[]      ( int idx)       { return comp_[idx]; }  

                   // WARNING: There is no way to use this and be
                   //          assured that environments are consistent.

  // Algebraic functions ...
  TJetVector&       operator=      ( TJetVector const& );

  TJetVector        operator+      ( TJetVector const& ) const;
  TJetVector        operator+      ( Vector     const& ) const;
  TJetVector        operator+=     ( TJetVector const& );
  TJetVector        operator+=     ( Vector     const& );
  friend TJetVector operator-<>    ( TJetVector const& );
  TJetVector        operator-      ( TJetVector const& ) const;
  TJetVector        operator-      ( Vector     const& ) const;
  TJetVector        operator-=     ( TJetVector const& );
  TJetVector        operator-=     ( Vector     const& );

  friend TJetVector operator*<>    ( TJet<T>    const&,  TJetVector const& );
  friend TJetVector operator*<>    ( T          const&,  TJetVector const& );
  friend TJetVector operator*<>    ( TJetVector const&,  T          const& );
  friend TJetVector operator*<>    ( TMatrix<T> const&,  TJetVector const& );

  TJetVector        operator*      ( TJet<T>    const&    ) const;
  TJetVector        operator*      ( T const& ) const;
  TJetVector        operator*=     ( TJet<T>    const&    );
  TJetVector        operator*=     ( T                    );
  TJetVector        operator/      ( TJet<T>    const&    ) const;
  TJetVector        operator/      ( T                    ) const;
  TJetVector        operator/=     ( TJet<T>    const&    );
  TJetVector        operator/=     ( T                    );


  TJet<T>       operator*      (TJetVector const& )  const; // dot product
  TJet<T>       operator*      (Vector    const& )   const; // dot product

  friend TJetVector operator^<>    (Vector     const&,   TJetVector const& );

  TJetVector        operator^      (TJetVector const& ) const; // cross product:
                                                                // only works if
                                                                // the vector is
                                                                // three-dimensional
  TJetVector        operator^      (Vector const& ) const;


  // Boolean functions ...
  bool operator==     ( TJetVector const& ) const;
  bool operator==     ( T          const& ) const;
  bool operator!=     ( TJetVector const& ) const;
  bool operator!=     ( T          const& ) const;
  bool operator<      ( TJetVector const& ) const;
  bool operator<=     ( TJetVector const& ) const;
  bool operator>      ( TJetVector const& ) const;
  bool operator>=     ( TJetVector const& ) const;
  bool IsNull         ()                    const;
  bool IsUnit         ()                    const;
  bool IsNilpotent    ()                    const;


  // Functions related to differentiation
  void weightedDerivative( IntArray const&, T* );
  void derivative( IntArray const&,  T* );
  

  // Queries ...
  int         Dim()          const;
  EnvPtr<T>   Env()          const;
  TVector<T>  standardPart() const;
  TVector<T>  getReference() const;

  // Utilities ..

  void              resize(int n );
  void              printCoeffs      () const;
  TJet<T>           Norm             () const;
  TJetVector const  Unit             () const;  // returns a unit vector
   

  // rotates v through an angle theta using *this as the axis

  TJetVector   Rotate( TJetVector const& v,    double         theta ) const;
  TJetVector   Rotate( TJetVector const& v,    TJet<T> const& theta ) const;


  friend std::ostream& operator<<<>( std::ostream&, TJetVector  const& );
  friend std::istream& operator>><>( std::istream&, TJetVector&        );

  TJetVector filter( int, int ) const;
  TJetVector filter( bool (*[]) ( IntArray const&, T const& ) ) const;

};

//--------------------------------------
// Inline functions ...
//--------------------------------------

// .. Functions included for symmetry

template<typename T>
inline bool operator==( T const& x, TJetVector<T> const& y )
{ return y == x; }

template<typename T>
inline bool operator!=( T const& x, TJetVector<T> const& y )
{ return !(y == x); }


// ... Member functions (aka methods)

template<typename T>
inline int TJetVector<T>::Dim() const 
{ return comp_.size(); }

template<typename T>
inline EnvPtr<T> TJetVector<T>::Env() const 
{ return  myEnv_; }

template<typename T>
inline TJetVector<T> operator+( Vector const& x, TJetVector<T> const& y )
{ return y.operator+( x ); }

template<typename T>
inline TJetVector<T> operator-( Vector const& x, TJetVector<T> const& y )
{ return operator-( y.operator-( x ) ); }

template<typename T>
inline TJet<T> operator*( Vector const& x, TJetVector<T> const& y )
{ return y.operator*( x ); }

template<typename T>
TJetVector<T> operator*( Vector const& x,   TJet<T>      const& y )
{ return operator*(y,x); }


// Specializations _________________________________

// the complex form of the dot product requires x*conj(y) 

 template<>
 template<>
 TJetVector<std::complex<double> >::TJetVector(TJetVector<double> const&);

 template<>  TJet<double>  
             TJetVector<double>::operator*      ( TJetVector<double> const& ) const;

 template<>  TJet<std::complex<double> >  
             TJetVector<std::complex<double> >::operator*      ( TJetVector<std::complex<double> > const& ) const;

 template<>
 TJetVector<double> TJetVector<double>::Rotate(  TJetVector<double> const&, double              theta ) const;

 template<>
 TJetVector<double> TJetVector<double>::Rotate(  TJetVector<double> const&, TJet<double> const& theta ) const;


 template<> 
 TJetVector<std::complex<double> > operator*( TJetVector<std::complex<double> > const&, double                            const& );

 template<> 
 TJetVector<std::complex<double> > operator*( double                            const&, TJetVector<std::complex<double> > const& );



// these specializations are not implemented ... (they do not really make much sense unless norms are compared)  

 template<> bool TJetVector<std::complex<double> >::operator<  ( TJetVector<std::complex<double> > const& ) const;
 template<> bool TJetVector<std::complex<double> >::operator<= ( TJetVector<std::complex<double> > const& ) const;
 template<> bool TJetVector<std::complex<double> >::operator>  ( TJetVector<std::complex<double> > const& ) const;
 template<> bool TJetVector<std::complex<double> >::operator>= ( TJetVector<std::complex<double> > const& ) const;

 template<>
 TJetVector<std::complex<double> > 
 TJetVector<std::complex<double> >::Rotate(  TJetVector<std::complex<double> > const&, double              theta ) const;

 template<>
 TJetVector<std::complex<double> > 
 TJetVector<std::complex<double> >::Rotate(  TJetVector<std::complex<double> > const&, TJet<std::complex<double> > const& theta ) const;


#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJetVector.tcc>
#endif

#endif // TJETVECTOR_H
