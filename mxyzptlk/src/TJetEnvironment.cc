/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                   
******  File:      TJetEnvironment.cc
******                                                                
******  Copyright Universities Research Association, Inc. / Fermilab   
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
*****   and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******  Author: Jean-Francois Ostiguy
******          ostiguy@fnal.gov
******
****** - Introduced new compact monomial indexing scheme based on monomial ordering
******   rather than previous scheme based explicitly on monomial exponents tuple.
****** - monomial multiplication handled via a lookup-table.
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <complex>
#include <mxyzptlk/EnvPtr.h>
#include <mxyzptlk/TJetEnvironment.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/iosetup.h>
#include <boost/scoped_array.hpp>

#include <mxyzptlk/TJLterm.h>
#include <mxyzptlk/TJL.h>

using FNAL::pcout;
using FNAL::pcerr;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
template<>
TJetEnvironment<std::complex<double> >::TJetEnvironment(TJetEnvironment<double> const& x):
   numVar_(x.numVar_),                              // number of variables
   spaceDim_(x.spaceDim_),                          // phase space dimensions
   dof_(x.dof_),                                    // degrees of freedom                             
   refPoint_(new std::complex<double>[x.numVar_]),   // reference point (set to zero by default)
   scale_(new double[x.numVar_]),                   // scale (set to 1.0e-3 by default) should be a Vector
   maxWeight_(x.maxWeight_),                        // maximum weight (polynomial order)
   pbok_(x.pbok_),                                  // THIS IS HERE FOR COMPATIBILITY WITH EARLIER VERSIONS
                                                    // _pbok was used as a flag to detect the presence of parameters 
                                                    // poisson bracket OK is true only when phase space dimension is even; 
                                                    // Consider simply checking the space dimensions before taking a PB ? 

  // scratch_( buildScratchPads( maxWeight_, numVar_) ) 
 {

    
  for (int i=0; i< numVar_; ++i) {   
        refPoint_[i]   = std::complex<double>(x.refPoint_[i], 0.0);
           scale_[i]   = x.scale_[i];
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
template<>
EnvPtr<std::complex<double> >  TJetEnvironment<std::complex<double> >::makeJetEnvironment( EnvPtr<double> const& env) {

 boost::scoped_array<std::complex<double> >    tmp_refpoints( new std::complex<double>[env->numVar()]); 
 boost::scoped_array<double>                   tmp_scale(new double[env->numVar()]);    
 
 for (int i=0; i< env->numVar(); ++i) {
     tmp_refpoints[i] = std::complex<double>(env->refPoint()[i], 0.0);
     tmp_scale[i]     = env->scale()[i];

 }

 std::list<EnvPtr<std::complex<double> > >::iterator env_iter;

 //-----------------------------------------------------
 // if a match already  exists, return it 
 //-----------------------------------------------------

 EnvPtr<std::complex<double> > pje;
 EnvPtr<std::complex<double> > tmppje;

 bool refpoints_are_equivalent = false;
 bool scales_are_equivalent    = false;

 for( env_iter  =  TJetEnvironment<std::complex<double> >::environments_.begin(); 
      env_iter !=  TJetEnvironment<std::complex<double> >::environments_.end(); 
     ++env_iter )                        { 
 
    tmppje = *env_iter; 
 
    if ( tmppje->maxWeight()    != env->maxWeight() )   continue;  
    if ( tmppje->numVar()       != env->numVar()    )   continue;  
    if ( tmppje->spaceDim()     != env->spaceDim()  )   continue;  
    
    refpoints_are_equivalent = true;
    for (int i=0; i<env->numVar(); ++i ) {
       refpoints_are_equivalent = refpoints_are_equivalent && (tmppje->refPoint()[i] == tmp_refpoints[i] );
    }
    if ( !refpoints_are_equivalent )   continue; 

#if 0
===============================================================
    scales_are_equivalent = true;
    for (int i=0; i<nvar; ++i ) {
      scales_are_equivalent = scales_are_equivalent && ( tmppje->scale()[i] == tmp_scale[i] );
    }
    if ( ! scales_are_equivalent )              continue; 
==================================================================
#endif
  
    // -----------------------------------------------------------
    // if we got here, a suitable environment already exists
    // -----------------------------------------------------------
     pje = tmppje; 
     break;
 }

 if  (pje) { // pje is not null 
     return pje;
 } else  {

     // NOTE: the 2nd argument (default=true) in the smart pointer constructor invocation is set to false. This
     //       prevents the reference count to be incremented. The effect is that the reference count will go to 0 when 
     //       the only instance of the smart ptr is the one left in the _environment list. When the ref count reaches 
     //       0, the custom deleter (dispose()) removes the env from the list.   
  
     // NOTE: The swap function here is used here to prevent the ref count of newly created env ptr to go from 1 to zero 
     //       and be prematurely deleted. Normally this is not a problem, but here the pointer is created (execeptionally)
     //         with an initial ref count of 0.


     EnvPtr<std::complex<double> > newpje( new TJetEnvironment<std::complex<double> >( env->maxWeight(), env->numVar(), env->spaceDim(), 
                                                                tmp_refpoints.get(), tmp_scale.get()), false);
  
     pje.swap( newpje );

     TJetEnvironment<std::complex<double> >::environments_.push_back( pje );

     return pje;
 }
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
EnvPtr<double> TJetEnvironment<double>::makeJetEnvironment(int maxweight, const Vector& v , double* scale) {
 
 boost::scoped_array<double> refpoints( new double[ v.Dim() ]);

 for (int i=0; i<v.Dim(); ++i) {
     refpoints[i] = v(i); 
 }

 return EnvPtr<double>( TJetEnvironment<double>::makeJetEnvironment(maxweight, v.Dim(), v.Dim(), refpoints.get(), scale) );
 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
EnvPtr<double>  TJetEnvironment<double>::getApproxJetEnvironment(int maxweight, const Vector& refpoints)
{

  //   Determine if a Jet environment already exists
  //   whose reference point is sufficiently close
  //   to the closed orbit. If so, returns it, otherwise
  //   return a NULL environment

  int nvar = refpoints.Dim();
  
  EnvPtr<double> pje; // null 
  Vector tolerance( nvar );

  for( int i=0;  i<nvar; ++i ) {
    // *** CHANGE ***
    // *** CHANGE *** The tolerance criterion should be user-determined.
    // *** CHANGE ***
    // tolerance(i) = std::max( 1.0e-6, std::abs(0.001*refpoints(i)));
    tolerance(i) =  std::abs(0.001*refpoints(i));
  }

  std::list<EnvPtr<double> >::iterator env_iter;

  for ( env_iter  = TJetEnvironment<double>::environments_.begin();  
        env_iter != TJetEnvironment<double>::environments_.end(); 
        ++env_iter)
  { 
    if( (*env_iter)->numVar() != nvar ) continue; 

    if( (*env_iter)->hasApproxReferencePoint( refpoints, tolerance ) ) {
      pje   = *env_iter; 
    }
   
  }

  return pje;  
    

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
template<>
template<>
EnvPtr<double> 
TJetEnvironment<std::complex<double> >::makeRealJetEnvironment(EnvPtr<std::complex<double> > const& env) {

 boost::scoped_array<double>                tmp_refpoints(new double[env->numVar()]); 
 boost::scoped_array<double>                tmp_scale(new double[env->numVar()]);    
 
 for (int i=0; i< env->numVar(); ++i) {
     tmp_refpoints[i] = std::real( env->refPoint()[i] );
     tmp_scale[i]     = env->scale()[i];

 }

 std::list<EnvPtr<double> >::iterator env_iter;

 //-----------------------------------------------------
 // if a match already  exists, return it 
 //-----------------------------------------------------

 EnvPtr<double> pje;
 EnvPtr<double> tmppje;

 bool refpoints_are_equivalent = false;
 bool scales_are_equivalent    = false;

 for( env_iter  = TJetEnvironment<double>::environments_.begin(); 
      env_iter != TJetEnvironment<double>::environments_.end(); 
     ++env_iter )                        { 
 
    tmppje = *env_iter; 

    if ( tmppje->maxWeight()    != env->maxWeight() )   continue;  
    if ( tmppje->numVar()       != env->numVar()    )   continue;  
    if ( tmppje->spaceDim()     != env->spaceDim()  )   continue;  
    
    refpoints_are_equivalent = true;
    for (int i=0; i<env->numVar(); ++i ) {
       refpoints_are_equivalent = refpoints_are_equivalent && (tmppje->refPoint()[i] == tmp_refpoints[i] );
    }
    if ( !refpoints_are_equivalent )   continue; 

#if 0
===============================================================
    scales_are_equivalent = true;
    for (int i=0; i<nvar; ++i ) {
      scales_are_equivalent = scales_are_equivalent && ( tmppje->scale()[i] == tmp_scale[i] );
    }
    if ( ! scales_are_equivalent )              continue; 
==================================================================
#endif
  
    // -----------------------------------------------------------
    // if we got here, a suitable environment already exists
    // -----------------------------------------------------------
     pje = tmppje; 
     break;
 }

 if  (pje) { // pje is not null 
     return pje;
 } else  {

     // NOTE: the 2nd argument (default=true) in the smart pointer constructor invocation is set to false. This
     //       prevents the reference count to be incremented. The effect is that the reference count will go to 0 when 
     //       the only instance of the smart ptr is the one left in the _environment list. When the ref count reaches 
     //       0, the custom deleter (dispose()) removes the env from the list.   
  
     // NOTE: The swap function here is used here to prevent the ref count of newly created env ptr to go from 1 to zero 
     //       and be prematurely deleted. Normally this is not a problem, but here the pointer is created (execeptionally)
     //         with an initial ref count of 0.

     EnvPtr<double> newpje( new TJetEnvironment<double>( env->maxWeight(), env->numVar(), env->spaceDim(), 
                                                                tmp_refpoints.get(), tmp_scale.get()), false);
  
     pje.swap( newpje );

     TJetEnvironment<double>::environments_.push_back( pje );

     return pje;
 }
}
