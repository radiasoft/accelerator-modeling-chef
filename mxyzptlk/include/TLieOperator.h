/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TLieOperator.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
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
******  Revision History
******
******  Feb-may 2005   Jean-Francois Ostiguy
******                 ostiguy@fnal.gov 
****** 
****** - new memory management scheme 
******                                                            
******  Sep-Dec 2005  ostiguy@fnal.gov
******  
****** - refactored code to usea single class template parameter
******   instead of two. Mixed mode operations now handled using 
******   implicit conversion operators.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment
****** - implementation details completely moved to TJL   
****** - header files support for both explicit and implicit template 
******   instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
******  Dec 2006 ostiguy@fnal
******
******  - New TJetVector class implementation. See  TJetVector.h for details. 
******                                                                 
**************************************************************************
*************************************************************************/

#ifndef TLIEOPERATOR_H
#define TLIEOPERATOR_H

#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/TJetVector.h>

// Forward declarations

template<typename T>
class TLieOperator;

template<typename T>
std::ostream& operator<<( std::ostream&, TLieOperator<T> const& );

template<typename T>
std::istream& operator>>( std::istream&, TLieOperator<T>& );

template<typename T>
TLieOperator<T> operator^( TLieOperator<T> const&,  TLieOperator<T> const& );


template<typename T>
class DLLEXPORT TLieOperator : public TJetVector<T>
{

public:

  // Constructors and destructors_____________________________________

  TLieOperator( EnvPtr<T> const env = (TJetEnvironment<T>::getLastEnv()) );
  TLieOperator( TLieOperator const& );
  TLieOperator( TJet<T>      const& );      // Creates a Hamiltonian vector field.
  TLieOperator( char*, EnvPtr<T> const env = (TJetEnvironment<T>::getLastEnv) ); 
                                            // Creates the identity function.  
  ~TLieOperator();

  // Member functions (public)

  void SetComponent( int  index,  TJet<T> const&  component );
  void setComponent( int  index,  TJet<T> const&  component );
  void setVariable( TJet<T> const&, int );
  void setVariable( T       const&, int );   // WARNING: this version alters the environment!!

  TLieOperator& operator=( TJetVector<T> const& );

  void peekAt();
  void peekAt( char* );

  // Friendly operators ...........................
  friend std::ostream& operator<<<>( std::ostream&, TLieOperator const& );
  friend std::istream& operator>><>( std::istream&, TLieOperator & );

  // Lie operations ...................................

  // Lie bracket of two Lie operators
  friend TLieOperator operator^<>( TLieOperator const&, TLieOperator const& );

  TJet<T> operator^( TJet<T> const& ) const ;                     // Action as a Lie operator

  TJet<T> expMap( T       const&, TJet<T> const & );              // Performs exponential map on the second argument.
  TJet<T> expMap( TJet<T> const&, TJet<T> const & );              // The orbit parameter is here a TJet<T> variable.

  TJetVector<T> expMap( T       const&,  TJetVector<T> const& );  // Performs exponential map component-wise.
  TJetVector<T> expMap( TJet<T> const &, TJetVector<T> const& );

  TJet<T>       expMap( TJet<T>       const & );  
  TJetVector<T> expMap( TJetVector<T> const & );

};


// Inline functions

template<typename T>
inline TLieOperator<T>& TLieOperator<T>::operator=( TJetVector<T> const& x )
{ TJetVector<T>::operator=(x);  return *this;}

template<typename T>
inline TJet<T> TLieOperator<T>::expMap( TJet<T> const& x )
{ return this->expMap( 1.0, x ); }

template<typename T>
inline TJetVector<T> TLieOperator<T>::expMap( TJetVector<T> const& x )
{ return this->expMap( 1.0, x  ); }

template<typename T>
inline void TLieOperator<T>::setComponent( int i, TJet<T> const& x )
{ this->setVariable( x, i ); }

template<typename T>
inline void TLieOperator<T>::SetComponent( int i, TJet<T> const& x )
{ this->setVariable( x, i ); }


#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TLieOperator.tcc>
#endif

#endif // TLIEOPERATOR_H
