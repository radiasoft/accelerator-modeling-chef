/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TLieOperator.h
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


#ifndef TLIEOPERATOR_H
#define TLIEOPERATOR_H

#include "TJetVector.h"

// Forward declarations
template<typename T1, typename T2>
class TLieOperator;

template<typename T1, typename T2>
std::ostream& operator<<( std::ostream&, TLieOperator<T1,T2>& );

template<typename T1, typename T2>
std::istream& operator>>( std::istream&, TLieOperator<T1,T2>& );

template<typename T1, typename T2>
TLieOperator<T1,T2> operator^( const TLieOperator<T1,T2>&, const TLieOperator<T1,T2>& );


// Class TLieOperator template
template<typename T1, typename T2>
class TLieOperator : public TJetVector<T1,T2>
{
public:
  // Constructors and destructors_____________________________________
  TLieOperator( TJetEnvironment<T1,T2>* = (TJet<T1,T2>::_lastEnv) );
  TLieOperator( const TLieOperator& );
  TLieOperator( const TJet<T1,T2>& );  // Creates a Hamiltonian vector field.
  TLieOperator( char*, TJetEnvironment<T1,T2>* = (TJet<T1,T2>::_lastEnv) ); 
                                       // Creates the identity function.  
  ~TLieOperator();

  // Member functions (public)
  void SetComponent( int           /* index     */,
                     const TJet<T1,T2>&  /* component */  );
  void setComponent( int           /* index     */,
                     const TJet<T1,T2>&  /* component */  );
  void setVariable( const TJet<T1,T2>&, int );
  void setVariable( const T1&, int ); 
  // WARNING: this version alters the environment!!

  TLieOperator& operator=( const TJetVector<T1,T2>& );

  void peekAt();
  void peekAt( char* );

  // Friendly operators ...........................
  friend std::ostream& operator<<<>( std::ostream&, TLieOperator& );
  friend std::istream& operator>><>( std::istream&, TLieOperator& );

  // Lie operations ...................................
  TJet<T1,T2> operator^( const TJet<T1,T2>& ) const ;      
                                // Action as a Lie operator
  TJet<T1,T2> expMap( const T1&, const TJet<T1,T2>& );    
                                // Performs exponential map on
                                // the second argument.
  TJet<T1,T2> expMap( const TJet<T1,T2>&, const TJet<T1,T2>& );
                                // The orbit parameter is here
                                // a TJet<T1,T2> variable.
  TJetVector<T1,T2> expMap( const T1&, const TJetVector<T1,T2>& );
                                // Performs exponential map 
                                // componenet wise.
  TJetVector<T1,T2> expMap( const TJet<T1,T2>&, const TJetVector<T1,T2>& );

  TJet<T1,T2>       expMap( const TJet<T1,T2>& );
  TJetVector<T1,T2> expMap( const TJetVector<T1,T2>& );

  // Lie bracket of two Lie operators
  friend TLieOperator operator^<>( const TLieOperator&, const TLieOperator& );
};


// Inline functions

template<typename T1, typename T2>
inline TLieOperator<T1,T2>& TLieOperator<T1,T2>::operator=( const TJetVector<T1,T2>& x )
{ return (TLieOperator<T1,T2>&) TJetVector<T1,T2>::operator=( x ); }

template<typename T1, typename T2>
inline TJet<T1,T2> TLieOperator<T1,T2>::expMap( const TJet<T1,T2>& x )
{ return this->expMap( 1.0, x ); }

template<typename T1, typename T2>
inline TJetVector<T1,T2> TLieOperator<T1,T2>::expMap( const TJetVector<T1,T2>& x )
{ return this->expMap( 1.0, x  ); }

template<typename T1, typename T2>
inline void TLieOperator<T1,T2>::setComponent( int i, const TJet<T1,T2>& x )
{ this->setVariable( x, i ); }

template<typename T1, typename T2>
inline void TLieOperator<T1,T2>::SetComponent( int i, const TJet<T1,T2>& x )
{ this->setVariable( x, i ); }

#endif // TLIEOPERATOR_H
