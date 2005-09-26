/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.h
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
******  Revision History
******                                                                
******  Sept 2005   ostiguy@fnal.gov
******
******              - new code based on a single template parameter
******                instead of two. Mixed mode now handled
******                using conversion operators.
******
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TMAPPING_H
#define TMAPPING_H

#include <TJetVector.h>

template<typename T>
class TMapping : public TJetVector<T>
{

  friend class TMapping<double>;
  friend class TMapping<std::complex<double> >;

 private:
  TMapping<T> _epsInverse( TJetEnvironment<T>* ) const;

 public: 
  TMapping( int /* dimension  */ = (TJet<T>::_lastEnv)->_spaceDim,
            const TJet<T>* /* components */ = 0, 
            TJetEnvironment<T>*  = (TJet<T>::_lastEnv) );

  TMapping( const TMapping& );

  TMapping( const TJetVector<T>& );

  TMapping( const char*, TJetEnvironment<T>* = (TJet<T>::_lastEnv) ); // Produces the identity.
  ~TMapping();

  TMapping& operator= ( const TMapping& );

  operator TMapping<std::complex<double> > () const;

  Vector    operator()( const Vector& ) const;
  TMapping  operator()( const TMapping& ) const;  // TMapping composition.
  TMapping  operator* ( const TMapping& ) const;  // TMapping composition also; an alias.
  TMapping* operator*=( const TMapping<T>& );

  TMatrix<T> Jacobian() const; // Retained for backwards compatability
  TMatrix<T> jacobian() const;
  TJet<T>    operator()( int ) const; 
  TJet<T>&   operator()( int ); 

  TMapping Inverse() const;  // retained for backwards compatability
  TMapping inverse() const;
};


// specializations

template<>
TMapping<double>::operator TMapping<std::complex<double> > () const;

// Inline methods

template<typename T>
inline TJet<T> TMapping<T>::operator()( int i ) const
{ return TJetVector<T>::operator()( i ); }

template<typename T>
inline TJet<T>& TMapping<T>::operator()( int i )
{ return TJetVector<T>::operator()( i ); }

template<typename T>
inline TMapping<T>& TMapping<T>::operator=( const TMapping<T>& x )
{ TJetVector<T>::operator=( (TJetVector<T>&) x ); return *this; }

#ifdef MXYZPTLK_IMPLICIT_TEMPLATES
#include <TMapping.tcc>
#endif

#endif // TMAP_H
