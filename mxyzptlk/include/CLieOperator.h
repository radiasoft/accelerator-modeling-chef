/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      CLieOperator.h
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


#ifndef CLIEOPERATOR_HXX
#define CLIEOPERATOR_HXX

#include "JetCVector.h"

class CLieOperator : public JetCVector
{
public:

  // Friends   ??? Still necessary?
  friend class  JetC;
  friend struct JLC;

  // Constructors and destructors_____________________________________
  CLieOperator( JetC__environment* = JetC::lastEnv );
  CLieOperator( const CLieOperator& );
  CLieOperator( const JetC& );     // Creates a Hamiltonian vector field.
  CLieOperator( char*, 
                JetC__environment* = JetC::lastEnv ); // Creates the identity function.  
  ~CLieOperator();

  // Member functions (public)
  void SetComponent(       int  i  /* index     */,
                     const JetC& x /* component */ 
                   )
                   { setVariable( x, i ); }
  void setVariable( const JetC&, int );

  void setVariable( const Complex&, const int& ); // WARNING: alters the environment!!

  CLieOperator& operator=( const JetCVector& );

  void peekAt();
  void peekAt( char* );

  // Friendly operators ...........................
  friend ostream& operator<<(ostream&, CLieOperator&);
  friend istream& operator>>(istream&, CLieOperator&);

  // Lie operations ...................................
  JetC operator^( const JetC& ) const ;      
                                // Action as a Lie operator
  JetC expMap( const Complex&, const JetC& );    
                                // Performs exponential map on
                                // the second argument.
  JetC expMap( const JetC&, const JetC& );
                                // The orbit parameter is here
                                // a JetC variable.
  JetCVector expMap( const Complex&, const JetCVector& );
                                // Performs exponential map 
                                // componenet wise.
  JetCVector expMap( const JetC&, const JetCVector& );

  JetC       expMap( const JetC& );
  JetCVector expMap( const JetCVector& );

  // Lie bracket of two Lie operators
  friend CLieOperator operator^( /* const */ CLieOperator&,
                                 /* const */ CLieOperator& );
  // ??? const has to be completely fixed in MXYZPTLK

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
} ;

inline CLieOperator& CLieOperator::operator=( const JetCVector& x )
{
  return (CLieOperator&) JetCVector::operator=( x );
}


inline JetC CLieOperator::expMap( const JetC& x )
{
  return this->expMap( 1.0, x );
}


inline JetCVector CLieOperator::expMap( const JetCVector& x )
{
  return this->expMap( 1.0, x  );
}

#endif
