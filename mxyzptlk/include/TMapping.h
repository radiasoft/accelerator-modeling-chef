/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.h
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


#ifndef TMAP_H
#define TMAP_H

#include "TJetVector.h"

template<typename T1, typename T2>
class TMapping : public TJetVector<T1,T2>
{
 private:
  TMapping<T1,T2> _epsInverse( const TJetEnvironment<T1,T2>* ) const;

 public: 
  TMapping( short int /* dimension  */ = (TJet<T1,T2>::_lastEnv)->_spaceDim,
            const TJet<T1,T2>* /* components */ = 0, 
            TJetEnvironment<T1,T2>*  = (TJet<T1,T2>::_lastEnv) );
  TMapping( const TMapping& );
  TMapping( const TJetVector<T1,T2>& );
  TMapping( char*, TJetEnvironment<T1,T2>* = JetC::lastEnv ); // Produces the identity.
  ~TMapping();

  TMapping& operator= ( const TMapping& );
  TMapping& operator= ( const TMapping<T2,T1>&  );
  TMapping  operator()( const TMapping& ) const;  // TMapping composition.
  TMapping  operator* ( const TMapping& ) const;  // TMapping composition also; an alias.
  TMapping* operator*=( const TMapping<T1,T2>& );

  TMatrix<T1> Jacobian() const; // Retained for backwards compatability
  TMatrix<T1> jacobian() const;
  TJet<T1,T2>  operator()( short int ) const; 
  TJet<T1,T2>& operator()( short int ); 

  TMapping Inverse() const;  // retained for backwards compatability
  TMapping inverse() const;
};

template<typename T1, typename T2>
inline TJet<T1,T2> TMapping<T1,T2>::operator()( short int ) const
{ return TJetVector<T1,T2>::operator()( i ); }

template<typename T1, typename T2>
inline TJet<T1,T2>& TMapping<T1,T2>::operator()( short int )
{ return TJetVector<T1,T2>::operator()( i ); }

template<typename T1, typename T2>
inline TMapping<T1,T2>& TMapping<T1,T2>::operator=( const TMapping<T1,T2>& x )
{ TJetVector<T1,T2>::operator=( (JetVector&) x ); return *this; }


typedef TMapping<double,FNAL::Complex> Mapping;
typedef TMapping<FNAL::Complex,double> MappingC;

#endif // TMAP_H
