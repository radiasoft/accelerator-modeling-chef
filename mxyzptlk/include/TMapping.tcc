/**************************************************************************
*************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.tcc
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
****** - refactored code to use a single class template parameter
******   rather than two. Mixed mode operations now handled using 
******   implicit conversion.
****** - reference counting now based on using boost::intrusive pointer
****** - reference counted TJetEnvironment class
****** - implementation details completely moved to TJL   
****** - redesigned coordinate class Tcoord. New class Tparams for parameters
****** - header files support for both explicit and implicit template instantiations
******   (default for mxyzptlk = explicit)
******   for explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES
******
******  Dec 2006 ostiguy@fnal.gov
******
******  - New TJetVector base class implementation. See  TJetVector.h for details. 
****** 
******  Mar 2008 ostiguy@fnal.gov
******  - Map composition and evaluation code refactored. 
******  - Support for evaluation of complex maps
******  - added (missing) implementation for in-place Map composition
******  Aug 2008 ostiguy@fnal.gov
******  - intialization optimizations in inverse()
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <mxyzptlk/TJet.h>


using namespace std;
using FNAL::pcout;
using FNAL::pcerr;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( int n, EnvPtr<T> const& pje )
: TJetVector<T>(n, pje)
{ 
  if( n%2 != 0  ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T>::TMapping<T>( int n, EnvPtr<T> const& pj ) ",
          "phase space dimension n must be even ") );
 } 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( EnvPtr<T> const& pje )
: TJetVector<T>(pje )
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( TMapping<T> const& x ) 
: TJetVector<T>( x ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( TJetVector<T> const& x ) 
: TJetVector<T>( x ) 
{
   if( this->Dim()% 2 != 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T>::TMapping<T>( TJetVector<T> const& x ) ",
          "phase space dimension n must be even ") );
 } 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
template<typename iterator_t>
TMapping<T>::TMapping( iterator_t itstart, iterator_t itend, EnvPtr<T>  const& env ) 
: TJetVector<T>( itstart, itend, env ) 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( char const*, EnvPtr<T> const& pje  ) 
: TJetVector<T>( pje ? pje->spaceDim():0, pje )
{
 
 if( !pje ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T>::TMapping<T>( char const*, TJetEnvironment<T>* ) ",
          "Environment is null ") );
 }

 if( pje->spaceDim() == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T>::TMapping<T>( char*, TJetEnvironment<T>* ) ",
          "Phase space has dimension 0." ) );
 }
 
 int i=0;
 for( typename TMapping<T>::iterator it  = this->begin(); 
                                     it != this->end(); ++it, ++i) {
       *it = TJet<T>::makeCoordinate( pje, i ); 
 } 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::~TMapping<T>() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>& TMapping<T>::operator=( TMapping<T> const& x )
{  
   if ( &x == this ) return *this;

   TJetVector<T>::operator=(x);  
   return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::operator()(  TMapping<T> const& x ) const
{

 if( this->Dim() != x.Dim() ){
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T> TMapping<T>::operator()( const TMapping<T>& ) const",
          "Incompatible dimensions." ) );
 }

 TMapping<T> z( (this->comp_.size()), x.myEnv_ );

 typename TMapping<T>::iterator itz = z.begin(); 
 for( typename TMapping<T>::const_iterator it = this->begin(); it != this->end(); ++it, ++itz ) {
  (*itz) = (*it)( x );
 }

 return z;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TMapping<T>::operator()(  TVector<T> const& x ) const
{

 if(  this->Dim() != x.Dim() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T> TMapping<T>::operator()( TVector<T>& ) const",
          "Incompatible dimensions." ) );
 }

 TVector<T> z( x.Dim() );
 typename TVector<T>::iterator itz = z.begin(); 
 for( typename TMapping<T>::const_iterator it = this->begin(); it != this->end(); ++it, ++itz ) {
  (*itz) = (*it)( x );
 }

 return z;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::operator*( TMapping<T> const& x ) const
{
  // alias for composition operator
  return (*this)( x );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TMapping<T>::operator*( TVector<T> const& x ) const
{
  // alias for composition operator
  return (*this)( x );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>& TMapping<T>::operator*=(  TMapping<T> const& x ) 
{

 if( x.comp_.size() != (this->myEnv_)->numVar() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T> TMapping<T>::operator*=( TMapping<T> const& ) ",
          "Incompatible dimensions." ) );
 }

 for( typename TMapping<T>::iterator it = this->begin(); it != this->end(); ++it) {
  (*it) = (*it)( x );
 }

 return *this;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMapping<T>::jacobian() const 
{
 int           nv = (this->myEnv_)->numVar();   
 IntArray      d(nv);

 int dim =     this->comp_.size();
 TMatrix<T>    M( dim, nv,  T() );

 for( int j=0; j<nv; ++j) {
  d[j] = 1;
  for( int i=0; i<dim; ++i)  {
      M[i][j] = (this->comp_)[i].derivative( d );
  }
  d[j] = 0;
 }

 return M;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::inverse() const 
{ 
 if( (this->myEnv_)->spaceDim() != (this->comp_.size()) ) {
  throw GenericException( __FILE__, __LINE__, 
         "Mapping Mapping::Inverse() const ",
         "Phase space dimensions do not match." );
 }

 int    nv   = (this->myEnv_)->numVar();

 bool                      ref_pt_is_zero;
 boost::scoped_array<bool> ref_pt_image_is_zero(new bool  [(this->comp_.size())]);
 bool                      zero_mapped_into_zero;

 // ------------------------------------------
 // determine if zero is mapped in to zero ... 
 //-------------------------------------------
 ref_pt_is_zero = true;       // true if ref point is the origin.

 for( int i=0; i < (this->comp_.size()); ++i ) 
   if( (this->myEnv_)->refPoint()[i] != 0.0 ) {
     ref_pt_is_zero = false;
     break;
 }

 for( int j=0; j < (this->comp_.size()); ++j )
   ref_pt_image_is_zero[j] = ( (this->comp_)[j].standardPart() == 0.0 );

 zero_mapped_into_zero = false;
 if(  ref_pt_is_zero ) { 
     for( int j = 0; j < (this->comp_.size()); j++ ) 
        zero_mapped_into_zero &= ref_pt_image_is_zero[j];
 }

 // If zero maps into zero, return inverse immediately .....................
 
 if( zero_mapped_into_zero ) return epsInverse( (this->myEnv_) );

 // --------------------------------------
 // Otherwise, construct an idempotent 
 // ompute its inverse and subsequently
 // set the constant terms 
 // --------------------------------------


 EnvPtr<T> pje_inv( TJetEnvironment<T>:: makeInverseJetEnvironment(*this) );

 TMapping<T> z( *this );  // copies the current mapping instance ...
 
 // ... Temporarily zero out the reference point 

 EnvPtr<T> tmp_pje_inv_zeroed( 
                         TJetEnvironment<T>::makeJetEnvironment(pje_inv->maxWeight(), pje_inv->numVar(), pje_inv->spaceDim() )); 

 z.myEnv_ = tmp_pje_inv_zeroed;


 for( int j=0; j < (this->comp_.size()); j++ ) {

   if( !ref_pt_image_is_zero[j] ) { 

     z.comp_[j].setStandardPart( T () ); 
  }
 }
 // In case the original reference point was not zero, set the reference point of each component to zero 
 // before taking an inverse.
  
  for( int j = 0; j < (this->comp_.size()); ++j )  {         
         z.comp_[j].setEnvTo( z.myEnv_ );    // NOTE: setEnvTo() clones z.comp_[j] !!!!         
   }

   z = z.epsInverse( z.myEnv_ );

//---------------------------------------------------------------------
// Reset the environment with one that has the correct reference point 
// and make final adjustments before returning. ..................
//---------------------------------------------------------------------
 
  z.myEnv_ = pje_inv;

  for( int i=0; i<(this->comp_.size()); ++i) 
     z.comp_[i].setEnvTo( pje_inv );    


 // ... A little test ...
 
  TVector<T> ref = z.standardPart();
  for( int j=0; j< (this->comp_.size()); ++j) 
   if( ref[j] != 0.0 ) {
    (*pcout) << "*** WARNING ***                                    \n"
            "*** WARNING *** TJet<T>::Inverse()                     \n"
            "*** WARNING *** ref[" << j << "] = " << ref[j] << "    \n"
            "*** WARNING ***                                        \n"
         << endl;
   }

 // ... Add the correct reference point and return ...

  for( int j=0; j< z.comp_.size(); j++ ) 
     z.comp_[j].addTerm( TJLterm<T>( IntArray( z.myEnv_->spaceDim() ), (this->myEnv_)->refPoint()[j], z.myEnv_ ) );

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::epsInverse( EnvPtr<T> const& pje) const 
{

 TMapping<T>  id( "ident", pje );
 TMapping<T>  v( (this->comp_.size()), pje );

 TMatrix<T> M = jacobian().inverse();

 if( (this->comp_.size()) != this->myEnv_->numVar() ) 
 {                                            
    M = jacobian().Square().inverse();
    (*pcout) << " TMapping<T>::epsInverse() Warning: Jacobian Matrix is not square. This should not be called ! " << std::endl;
 }


 TMapping<T>  z = M*id;
 v = ( (*this)(z) - id );


 int  i = 0;
 while ( ( i++ < TMapping<T>::maxiter_ ) && ( ( v - v.filter(0,1)) != T() ) ) {
               // This assumes linear is handled well enough
               // by the TMatrix<T> routine.  
  z = z - M*v;
  v = ( (*this)(z) - id );
 }

 if( i >= TMapping<T>::maxiter_ ) {
  (*pcerr) << "\n\n"
       << "*** WARNING ***                                            \n" 
          "*** WARNING *** TMapping<T>& TMapping<T>::epsInverse()                     \n" 
          "*** WARNING *** Over " << TMapping<T>::maxiter_ << " iterations used. \n"
          "*** WARNING *** result may be incorrect.                   \n" 
          "*** WARNING ***                                            \n" 
       << endl;
 }

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
