/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      Mapping.h
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


#ifndef MAP_HXX
#define MAP_HXX

#include "JetVector.h"

class Mapping : public JetVector
{
 private:

  Mapping EpsInverse( Jet__environment* ) const;

 protected:
  // Friends
  // friend class  Jet;
  // friend struct JL;
  // friend class  CJetVector;

 public: 

  Mapping( const int&  /* dimension */   = Jet::lastEnv->SpaceDim,
           const Jet*  /* components */  = 0, 
                 Jet__environment*       = Jet::lastEnv 
     );
  Mapping( const Mapping& );
  Mapping( const JetVector& );
  Mapping( char*, 
           Jet__environment* = Jet::lastEnv );    // Produces the identity.
  ~Mapping();

  Mapping& operator=( const Mapping& );
  Vector  operator()( const Vector& ) const;
  Mapping operator()( const Mapping& ) const;  // Mapping composition.
  Mapping operator* ( const Mapping& ) const;  // Mapping composition also; an alias.

  MatrixD Jacobian() const;
  Jet& operator()( const int& ) const; 

  Mapping Inverse() const;

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

void 
operator*=( Mapping& x, const Mapping& y );


// POSTPONED class CMapping : public CJetVector
// POSTPONED {
// POSTPONED protected:
// POSTPONED 
// POSTPONED   // Friends
// POSTPONED   friend class  Jet;
// POSTPONED   friend struct JL;
// POSTPONED   friend class  CJetVector;
// POSTPONED 
// POSTPONED public: 
// POSTPONED 
// POSTPONED   CMapping()           : CJetVector() {}
// POSTPONED   CMapping( CMapping& x )  : CJetVector( (CJetVector&) x ) {}
// POSTPONED   CMapping( CJetVector& x ) : CJetVector( x ) {}
// POSTPONED   CMapping( JetC* x )  : CJetVector( x ) {}
// POSTPONED   CMapping( char* x )  : CJetVector( x ) {}
// POSTPONED   ~CMapping() {}
// POSTPONED 
// POSTPONED   void SetComponent( const int  i /* index     */,
// POSTPONED                      const JetC& x /* component */ 
// POSTPONED                    )
// POSTPONED                    { setVariable( x, i ); }
// POSTPONED };

inline 
Jet& 
Mapping::operator()( const int& i ) 
const
{
  return JetVector::operator()( i );   // Regrettably, this is necessary.
}


#endif // MAP_HXX
