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
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES
******
******                                                                 
**************************************************************************
*************************************************************************/

#ifndef TLIEOPERATOR_H
#define TLIEOPERATOR_H

#include <TJetVector.h>

// Forward declarations
template<typename T>
class TLieOperator;

template<typename T>
std::ostream& operator<<( std::ostream&, TLieOperator<T>& );

template<typename T>
std::istream& operator>>( std::istream&, TLieOperator<T>& );

template<typename T>
TLieOperator<T> operator^( const TLieOperator<T>&, const TLieOperator<T>& );


template<typename T>
class TLieOperator : public TJetVector<T>
{

public:

  // Constructors and destructors_____________________________________

  TLieOperator( EnvPtr<T> const env = (TJet<T>::_lastEnv) );
  TLieOperator( const TLieOperator& );
  TLieOperator( const TJet<T>& );  // Creates a Hamiltonian vector field.
  TLieOperator( char*, EnvPtr<T> const env = (TJet<T>::_lastEnv) ); 
                                       // Creates the identity function.  
  ~TLieOperator();

  // Member functions (public)
  void SetComponent( int           /* index     */,
                     const TJet<T>&  /* component */  );
  void setComponent( int           /* index     */,
                     const TJet<T>&  /* component */  );
  void setVariable( const TJet<T>&, int );
  void setVariable( const T&, int ); 
  // WARNING: this version alters the environment!!

  TLieOperator& operator=( const TJetVector<T>& );

  void peekAt();
  void peekAt( char* );

  // Friendly operators ...........................
  friend std::ostream& operator<<<>( std::ostream&, TLieOperator& );
  friend std::istream& operator>><>( std::istream&, TLieOperator& );

  // Lie operations ...................................

  // Lie bracket of two Lie operators
  friend TLieOperator operator^<>( const TLieOperator&, const TLieOperator& );

  TJet<T> operator^( const TJet<T>& ) const ;      
                                // Action as a Lie operator
  TJet<T> expMap( const T&, const TJet<T>& );    
                                // Performs exponential map on
                                // the second argument.
  TJet<T> expMap( const TJet<T>&, const TJet<T>& );
                                // The orbit parameter is here
                                // a TJet<T> variable.
  TJetVector<T> expMap( const T&, const TJetVector<T>& );
                                // Performs exponential map 
                                // componenet wise.
  TJetVector<T> expMap( const TJet<T>&, const TJetVector<T>& );

  TJet<T>       expMap( const TJet<T>& );
  TJetVector<T> expMap( const TJetVector<T>& );

};


// Inline functions

template<typename T>
inline TLieOperator<T>& TLieOperator<T>::operator=( const TJetVector<T>& x )
{ return (TLieOperator<T>&) TJetVector<T>::operator=( x ); }

template<typename T>
inline TJet<T> TLieOperator<T>::expMap( const TJet<T>& x )
{ return this->expMap( 1.0, x ); }

template<typename T>
inline TJetVector<T> TLieOperator<T>::expMap( const TJetVector<T>& x )
{ return this->expMap( 1.0, x  ); }

template<typename T>
inline void TLieOperator<T>::setComponent( int i, const TJet<T>& x )
{ this->setVariable( x, i ); }

template<typename T>
inline void TLieOperator<T>::SetComponent( int i, const TJet<T>& x )
{ this->setVariable( x, i ); }


#ifdef  MXYZPTLK_IMPLICIT_TEMPLATES
#include <TLieOperator.tcc>
#endif

#endif // TLIEOPERATOR_H
