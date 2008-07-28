/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TVector.h
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
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******
******  Revision History:
******
******  Dec 2005   Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******
******  - templated version
******
******  Oct 2006   ostiguy@fnal.gov
******  
******  - new implementation based on std::vector<>
******
******   Mar 2007 ostiguy@fnal.gov 
******  - exposed iterator interface
******                                                                        
**************************************************************************
*************************************************************************/
#ifndef TVECTOR_H
#define TVECTOR_H

#include <complex>
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <memory>

#include <basic_toolkit/OutputFormat.h>
#include <boost/pool/pool_alloc.hpp>

template<typename T> 
class TVector;

template<typename T>
class TMatrix;

template<typename T> 
std::ostream& operator<<( std::ostream& , TVector<T> const& );

template <typename T>
TVector<T> operator-( TVector<T> const& );

template<typename T>
TVector<T> operator*( T          const&, TVector<T> const& );

template<typename T>
TVector<T> operator*( TVector<T> const&,       T const&);

template<typename T>
TMatrix<T> operator%( TVector<T> const&,       TVector<T> const&); // outer product 


template<typename T>
class DLLEXPORT TVector {

private:

  // **** disabled **** typedef typename std::vector<T, boost::pool_allocator<T> > vector_t;  

  typedef typename std::vector<T> vector_t;  

  vector_t                                        theVector_;
  OutputFormat*                                   ofPtr_;

  static OutputFormat*                            defOFPtr_;  // default OutputFormat

public:

  
  typedef typename vector_t::value_type                         value_type;

  typedef typename vector_t::iterator                             iterator;
  typedef typename vector_t::const_iterator                 const_iterator;

  typedef typename vector_t::reverse_iterator             reverse_iterator;
  typedef typename vector_t::const_reverse_iterator const_reverse_iterator;
 
  iterator                begin();
  const_iterator          begin()  const;

  iterator                end();
  const_iterator          end()    const;

  reverse_iterator        rbegin();
  const_reverse_iterator  rbegin() const;

  reverse_iterator        rend();
  const_reverse_iterator  rend()   const;

  // Constructors and the destructor __________________________

  TVector( int  dimension= 3,  T const* components=0, OutputFormat* ofmt=0 );

  template <typename Iterator_t>
  TVector( Iterator_t first,  Iterator_t last);

  TVector( TVector    const& );
  TVector( TMatrix<T> const& m); // implicit conversion

  void reset();

  ~TVector();

  friend  std::ostream& operator<< <>( std::ostream& , TVector<T> const& );

  // Assignment _____________________________

  T const&    operator[]            ( int idx)    const { return theVector_[idx]; }
  T&          operator[]            ( int idx)          { return theVector_[idx]; }

  // Algebraic functions ___________________________

  TVector&  operator= ( TVector const& );

  // ... Utility extension


  friend  TVector<T> operator-<T>( TVector const& );  // unary minus 
  friend  TVector<T> operator*<T>( T       const&,    TVector const&    );
  friend  TVector<T> operator*<T>( TVector const&,    T       const&    ); 


  TVector          operator+      ( TVector const& ) const;
  TVector&         operator+=     ( TVector const& );

  TVector          operator-      ( TVector const& ) const; 
  TVector&         operator-=     ( TVector const& ); 

  TVector&         operator/=     (       T const&       );
  TVector          operator/      (       T const&       ) const;

  TVector&         operator*=     (       T const&       );

  T          operator*       ( TVector<T> const&  ) const; // dot product
 
  TVector    operator^      ( TVector<T>  const& ) const; // cross product:
                                                          // only works if
                                                          // the vector is
                                                          // three-dimensional

  // ... Boolean functions

  bool          operator==     ( TVector const& ) const;
  bool          operator!=     ( TVector const& ) const;
  bool          IsNull         () const;
  bool          IsUnit         () const;

  //  ... Queries

  int  Dim() const { return theVector_.size(); }

  // ... Utilities


  static void   setDefaultFormat ( OutputFormat const&  ); 
  void          setOutputFormat  ( OutputFormat* x      ) { ofPtr_ = x; }
  OutputFormat* getOutputFormat()                         { return ofPtr_; }

  TVector      Abs              () const;
  double       Norm             () const;
  TVector      Unit             () const;           // returns unit vector
  void         Rotate           ( TVector& v, double const&  theta ) const;
                                                   // rotates v through 
                                                   // an angle theta using
                                                   // *this as the axis

  friend std::ostream& operator<< <T>(std::ostream& , TVector<T> const& );


  class op_mult: public std::binary_function<std::complex<double>&, std::complex<double>&, std::complex<double> > {
   public:   
       std::complex<double> operator()( std::complex<double> const& x, std::complex<double> const& y) const {
         return x * std::conj(y);   
       }
  };


};

// specializations _________________________________________________________


template <>  
double  TVector<double>::operator* ( TVector<double> const& x ) const; 

template <>  
std::complex<double> TVector<std::complex<double> >::operator* ( TVector<std::complex<double> > const& x ) const; 

// template<>
// bool TVector<double>::operator> ( TVector<double> const& x ) const;


//--------------------------------------------------------------------------
// These specializations are *not* implemented (it does not make much sense)
//--------------------------------------------------------------------------

//template<>
//bool TVector<std::complex<double> >::operator> ( TVector<std::complex<double> > const& x ) const;

template<> 
double TVector<double>::Norm () const;

template<> 
double TVector<std::complex<double> >::Norm () const;



#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/TVector.tcc>
#endif

#endif  //TVECTOR_H
