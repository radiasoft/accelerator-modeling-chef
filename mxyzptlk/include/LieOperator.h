/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      LieOperator.h
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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


#ifndef LIEOPERATOR_HXX
#define LIEOPERATOR_HXX

#include "JetVector.h"

class LieOperator : public JetVector
{
public:

  // Friends   ??? Still necessary?
  friend class  Jet;
  friend struct JL;
  friend class  CLieOperator;

  // Constructors and destructors_____________________________________
  LieOperator( Jet__environment* = Jet::lastEnv );
  LieOperator( const LieOperator& );
  LieOperator( const Jet& );     // Creates a Hamiltonian vector field.
  LieOperator( char*, 
               Jet__environment* = Jet::lastEnv ); // Creates the identity function.  
  ~LieOperator();

  // Member functions (public)
  void SetComponent(       int  i /* index     */,
                     const Jet& x /* component */ 
                   )
                   { setVariable( x, i ); }
  void setVariable( const Jet&, int );

  void setVariable( const double&, const int& ); // WARNING: alters the environment!!

  LieOperator& operator=( const JetVector& );

  void peekAt();
  void peekAt( char* );

  // Friendly operators ...........................
  friend ostream& operator<<(ostream&, LieOperator&);
  friend istream& operator>>(istream&, LieOperator&);

  // Lie operations ...................................
  Jet operator^( const Jet& ) const; 
                                // Action as a Lie operator
  Jet expMap( const double&, const Jet& );    
                                // Performs exponential map on
                                // the second argument.
  Jet expMap( const Jet&, const Jet& );
                                // The orbit parameter is here
                                // a Jet variable.
  JetVector expMap( const double&, const JetVector& );
                                // Performs exponential map 
                                // componenet wise.
  JetVector expMap( const Jet&, const JetVector& );

  Jet       expMap( const Jet& );
  JetVector expMap( const JetVector& );

  // Lie bracket of two Lie operators
  friend LieOperator operator^( /* const */ LieOperator&,
                                /* const */ LieOperator& );
  // ??? const has to be completely fixed in MXYZPTLK

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;


inline LieOperator& LieOperator::operator=( const JetVector& x )
{
  return (LieOperator&) JetVector::operator=( x );
}


inline Jet LieOperator::expMap( const Jet& x )
{
  return this->expMap( 1.0, x );
}


inline JetVector LieOperator::expMap( const JetVector& x )
{
  return this->expMap( 1.0, x  );
}

#endif // LIEOPERATOR_HXX
