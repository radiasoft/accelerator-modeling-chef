/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      MappingC.h
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


#ifndef MAPC_HXX
#define MAPC_HXX

#include "JetCVector.h"

class MappingC : public JetCVector
{
 private:

  MappingC EpsInverse( JetC__environment* ) const;

 protected:
  // Friends
  // friend class  JetC;
  // friend struct JL;
  // friend class  CJetCVector;

 public: 

  MappingC( const int&  /* dimension */    = JetC::lastEnv->SpaceDim,
        const JetC*  /* components */  = 0, 
              JetC__environment*       = JetC::lastEnv 
     );
  MappingC( const MappingC& );
  MappingC( const JetCVector& );
  MappingC( char*, 
        JetC__environment* = JetC::lastEnv );    // Produces the identity.
  ~MappingC();

  MappingC& operator=( const MappingC& );
  MappingC& operator=( const Mapping&  );
  MappingC operator()( const MappingC& ) const;  // MappingC composition.
  MappingC operator* ( const MappingC& ) const;  // MappingC composition also; an alias.

  MatrixC Jacobian() const;
  JetC& operator()( const int& ) const; 

  MappingC Inverse() const;

  // friend MappingC operator*( /* const */ MatrixC&, const MappingC& );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

void 
operator*=( MappingC& x, const MappingC& y );


// inline MappingC operator*( /* const */ MatrixC& x, const MappingC& y ) 
// {
//   return (MappingC) ( x*( (JetCVector&) y ) );
// }


inline 
JetC& 
MappingC::operator()( const int& i ) 
const
{
  return JetCVector::operator()( i );
}


inline
MappingC& 
MappingC::operator=( const Mapping& x )
{
  JetCVector::operator=( (JetVector&) x );
  return *this;
}


#endif // MAP_HXX
