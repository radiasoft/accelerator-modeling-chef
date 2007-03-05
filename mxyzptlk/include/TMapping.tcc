/**************************************************************************
*************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.tcc
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
******  Dec 2006 ostiguy@fnal
******
******  - New TJetVector base class implementation. See  TJetVector.h for details. 
****** 
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TMAPPING_TCC
#define TMAPPING_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <mxyzptlk/TJet.h>


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
TMapping<T>::TMapping( int n, EnvPtr<T> const& pje )
: TJetVector<T>(n, pje)
{ }

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( EnvPtr<T> const& pje )
: TJetVector<T>(pje )
{ }

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( TMapping<T> const& x ) 
: TJetVector<T>( x ) {}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( TJetVector<T> const& x ) 
: TJetVector<T>( x ) {}  // there should be a check here to make sure dims make sense


//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( TVector<T> const& x,   EnvPtr<T>  const& env ) 
: TJetVector<T>( x, env ) {}


//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping( const char*, EnvPtr<T> const& pje  ) 
: TJetVector<T>( TVector<T>(pje->spaceDim()), pje )
{
 
 if( pje->spaceDim() == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T>::TMapping<T>( char*, TJetEnvironment<T>* ) ",
          "Phase space has dimension 0." ) );
 }
 

 int s  = pje->spaceDim();

 for( int i=0; i<s; ++i) (this->comp_)[i].setVariable( i, pje );

}



//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::~TMapping<T>() {}


//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



template<typename T>
TMapping<T> TMapping<T>::operator()(  TMapping<T> const& x ) const
{

 if( x.comp_.size() != (this->myEnv_)->numVar() ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T> TMapping<T>::operator()( const TMapping<T>& ) const",
          "Incompatible dimensions." ) );
 }

 TMapping<T> z( (this->comp_.size()), x.myEnv_ );

 for( int i=0; i< (this->myEnv_)->spaceDim(); ++i) {
  z.comp_[i] = (this->comp_)[i]( x );
 }

   return z;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::operator*( TMapping<T> const& x ) const
{
  return operator()( x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void operator*=( TMapping<T>& x, TMapping<T> const& y ) {
 x = x*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMapping<T>::jacobian() const 
{
 int            nv = (this->myEnv_)->numVar();   // ??? Is this right?
 //int*           d = new int[ nv ];
 int           d[ nv ];
 TMatrix<T>    M( (this->comp_.size()), nv,  T() );

 for( int i=0; i<nv; ++i) d[i] = 0;
 for( int j=0; j<nv; ++j) {
  d[j] = 1;
  for( int i=0; i< (this->comp_.size()); ++i)  {
      M( i, j ) = (this->comp_)[i].derivative( d );
  }
  d[j] = 0;
 }

 // delete [] d;
 return M;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMapping<T>::Jacobian() const 
{
return jacobian();
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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
   if( (this->myEnv_)->getRefPoint()[i] != 0.0 ) {
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
 
 if( zero_mapped_into_zero ) return _epsInverse( (this->myEnv_) );

 // --------------------------------------
 // Otherwise,onstruct an idempotent 
 // and compute its inverse and susequently
 // set the constant terms 
 // --------------------------------------


 EnvPtr<T> pje_inv( TJetEnvironment<T>:: makeInverseJetEnvironment(*this) );

 TMapping<T> z( *this );  // copies the current mapping instance ...
 
 // ... Temporarily zero out the reference point 

 EnvPtr<T> tmp_pje_inv_zeroed( 
                         TJetEnvironment<T>::makeJetEnvironment(pje_inv->maxWeight(), pje_inv->numVar(), pje_inv->spaceDim(), 0, 0  )); 

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

   z = z._epsInverse( z.myEnv_ );

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
     z.comp_[j].addTerm( TJLterm<T>( z.myEnv_->allZeroes(), (this->myEnv_)->getRefPoint()[j], z.myEnv_ ) );

  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::_epsInverse( EnvPtr<T> const& pje) const 
{

 TMapping<T>  z( (this->comp_.size()), pje ); // the second argument creates an "empty mapping"
 TMapping<T>  id( "ident", pje );
 TMapping<T>  v( (this->comp_.size()), pje );

 TMatrix<T> M( (this->comp_.size()), (this->comp_.size()), 0.0 );


 if( (this->comp_.size()) == this->myEnv_->numVar() ) 
    M = Jacobian().inverse();
 else{                                            
    M = Jacobian().Square().inverse();
    (*pcout) << " TMapping<T>::_epsInverse() Warning: Jacobian Matrix is not square. This should not be called ! " << std::endl;
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


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::Inverse() const 
{

 return inverse();

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#endif // TMAPPING_TCC
