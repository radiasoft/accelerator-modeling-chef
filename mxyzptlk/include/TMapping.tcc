/**************************************************************************
*************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.cc
******  Version:   1.0
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
******* URA/FNAL reserves all rights.
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
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
******  Revision History:
******
******  Revision History
******   
******  May 2005       Jean-Francois Ostiguy
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
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for implicit instantiations, define MXYZPTLK_IMPLICIT_TEMPLATES
******
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TMAPPING_TCC
#define TMAPPING_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <GenericException.h>
#include <iosetup.h>
#include <TMapping.h>
#include <TJL.h>

using namespace std;
using FNAL::pcout;
using FNAL::pcerr;

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER


//    Globals |||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

using namespace std;

//    Constructors and destructors    |||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( int n,
                                  const TJet<T>* pj,
                                  typename EnvPtr<T>::Type pje )
: TJetVector<T>( n, pj, pje )
{

}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( const TMapping<T>& x ) 
: TJetVector<T>( (const TJetVector<T>&) x ) 
{

}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( const TJetVector<T>& x ) 
: TJetVector<T>( x ) 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( const char*, typename EnvPtr<T>::Type pje  ) 
: TJetVector<T>( pje->spaceDim(), 0, pje )
{
 
 if( pje->spaceDim() == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T>::TMapping<T>( char*, TJetEnvironment<T>* ) ",
          "Phase space has dimension 0." ) );
 }
 
 int s = pje->spaceDim();
(this->_myEnv) = pje;

 for( int i = 0; i < s; i++ ) (this->_comp)[i].setVariable( i, pje );
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::~TMapping<T>() 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::operator()( const TMapping<T>& x ) const
{
 if( x._dim != (this->_myEnv)->numVar() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T> TMapping<T>::operator()( const TMapping<T>& ) const",
          "Incompatible dimensions." ) );
 }

 TMapping<T> z( (this->_dim), 0, x._myEnv );

 for( int i = 0; i < (this->_myEnv)->spaceDim(); i++) {
  z._comp[i] = (this->_comp)[i]( x );
 }

   return z;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::operator*( const TMapping<T>& x ) const
{
  return operator()( x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void operator*=( TMapping<T>& x, const TMapping<T>& y ) {
 x = x*y;
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMapping<T>::Jacobian() const 
{
 int            nv = (this->_myEnv)->numVar();   // ??? Is this right?
 int*           d = new int[ nv ];
 TMatrix<T>    M( (this->_dim), nv, ((T) 0.0) );

 for( int i = 0; i < nv; i++  ) d[i] = 0;
 for( int j = 0; j < nv; j++ ) {
  d[j] = 1;
  for( int i = 0; i < (this->_dim); i++ ) M( i, j ) = (this->_comp)[i].derivative( d );
  d[j] = 0;
 }

 delete [] d;
 return M;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::Inverse() const 
{ 
 if( (this->_myEnv)->spaceDim() != (this->_dim) ) {
  throw( typename TJL<T>::BadDimension( (this->_myEnv)->spaceDim(), (this->_dim), __FILE__, __LINE__, 
         "Mapping Mapping::Inverse() const ",
         "Phase space dimensions do not match." ) );
 }

 int    nv   = (this->_myEnv)->numVar();

 bool                      ref_pt_is_zero;
 boost::scoped_array<bool> ref_pt_image_is_zero(new bool  [(this->_dim)]);
 bool                      zero_mapped_into_zero;

 boost::scoped_array<T>    ref( new T[nv] );
 
 // ------------------------------------------
 // determine if zero is mapped in to zero ... 
 //-------------------------------------------
 ref_pt_is_zero = true;       // true if ref point is the origin.

 for( int i=0; i < (this->_dim); ++i ) 
   if( (this->_myEnv)->getRefPoint()[i] != 0.0 ) {
     ref_pt_is_zero = false;
     break;
 }

 for( int j=0; j < (this->_dim); ++j )
   ref_pt_image_is_zero[j] = ( (this->_comp)[j].standardPart() == 0.0 );

 zero_mapped_into_zero = false;
 if(  ref_pt_is_zero ) { 
     for( int j = 0; j < (this->_dim); j++ ) 
        zero_mapped_into_zero &= ref_pt_image_is_zero[j];
 }

 // If zero maps into zero, return inverse immediately .....................
 
 if( zero_mapped_into_zero ) return _epsInverse( (this->_myEnv) );

 // --------------------------------------
 // Otherwise,onstruct an idempotent 
 // and compute its inverse and susequently
 // set the constant terms 
 // --------------------------------------

 typename EnvPtr<T>::Type pje_inv( TJetEnvironment<T>:: makeInverseJetEnvironment(*this) );

 TMapping<T> z( *this );  // copies the current mapping instance ...
  
 // ... Temporarily zero out the reference point 

 typename EnvPtr<T>::Type tmp_pje_inv_zeroed( 
                         TJetEnvironment<T>::makeJetEnvironment(pje_inv->maxWeight(), pje_inv->numVar(), pje_inv->spaceDim(), 0, 0  )); 

 z._myEnv = tmp_pje_inv_zeroed;


 for( int j=0; j < (this->_dim); j++ ) {

   if( !ref_pt_image_is_zero[j] ) { 

     // NOTE: calling get() here implicitly clones z._comp[j] !!!!;
     (z._comp[j].get())->_deleted = true; // pops out and deletes the first term (i.e. the weight==0 term);        

     // replace the weight==0 term with a new one that has value 0 
     TJLterm<T> tmp_term( z._myEnv->allZeroes(), T(), z._myEnv);
     z._comp[j].addTerm( TJLterm<T>( z._myEnv->allZeroes(), T(), z._myEnv));
   }
 }

 // In case the original reference point was not zero, set the reference point of each component to zero 
 // before taking an inverse.
  
  for( int j = 0; j < (this->_dim); ++j )  {         
         z._comp[j].setEnvTo( z._myEnv );    // NOTE: setEnvTo() clones z._comp[j] !!!!         
   }

   z = z._epsInverse( z._myEnv );
 
 
//---------------------------------------------------------------------
// Reset the environment with one that has the correct reference point 
// and make final adjustments before returning. ..................
//---------------------------------------------------------------------
 
  z._myEnv = pje_inv;

  for( int i=0; i<(this->_dim); i++ ) 
     z._comp[i].setEnvTo( pje_inv );    // clones again !!!


 // ... A little test ...
 
  z.standardPart( ref.get() );
  for( int j = 0; j < (this->_dim); j++ ) 
   if( ref[j] != 0.0 ) {
    (*pcout) << "*** WARNING ***                                    \n"
            "*** WARNING *** TJet<T>::Inverse()                     \n"
            "*** WARNING *** ref[" << j << "] = " << ref[j] << "    \n"
            "*** WARNING ***                                        \n"
         << endl;
   }

 // ... Add the correct reference point and return ...

  for( int j = 0; j< z._dim; j++ ) 
     z._comp[j].addTerm( TJLterm<T>( z._myEnv->allZeroes(), (this->_myEnv)->getRefPoint()[j], z._myEnv ) );

  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::_epsInverse(  typename EnvPtr<T>::Type pje) const 
{

 TMapping<T>  z( (this->_dim), 0, pje ); // the second argument creates an "empty mapping"
 TMapping<T>  id( "ident", pje );
 TMapping<T>  v( (this->_dim), 0, pje );

 TMatrix<T> M( (this->_dim), (this->_dim), 0.0 );


 if( (this->_dim) == (this->_myEnv)->numVar() ) 
    M = Jacobian().inverse();
 else{                                            
    M = Jacobian().Square().inverse();
    std::cout << " TMapping<T>::_epsInverse : this should not be called ! " << std::endl;
 }

 z = M*id;


 v = ( operator()(z) - id );



 int  i = 0;
 while ( ( i++ < MX_MAXITER ) && ( ( v - v.filter(0,1)) != T() ) ) {
               // This assumes linear is handled well enough
               // by the TMatrix<T> routine.  
  z = z - M*v;
  v = ( operator()(z) - id );
 }

 if( i >= MX_MAXITER ) {
  (*pcerr) << "\n\n"
       << "*** WARNING ***                                            \n" 
          "*** WARNING *** TMapping<T>& TMapping<T>::_epsInverse()                     \n" 
          "*** WARNING *** Over " << MX_MAXITER << " iterations used. \n"
          "*** WARNING *** result may be incorrect.                   \n" 
          "*** WARNING ***                                            \n" 
       << endl;
 }

 return z;
}


#endif // TMAPPING_TCC
