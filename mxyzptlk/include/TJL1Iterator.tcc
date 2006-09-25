/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJL1Iterator.tcc
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
******  URA/FNAL reserves all rights.
******                                                                
******  Author:  Jean-Francois Ostiguy  
******            ostiguy@fnal.gov
******  
******   For explicit instantiations, define MXYZPTLK_EXPLICIT_TEMPLATES 
******  
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/
#include <mxyzptlk/TJLterm.h>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
TJL1Iterator<T>::TJL1Iterator( TJL1<T> const& jl) {

int numVar = jl._myEnv->numVar();

IntArray exponents(numVar);

_terms.push_back( boost::shared_ptr<TJLterm<T> >() ); // padding to emulate semantics of existing
                                                 // iterators 

_terms.push_back( boost::shared_ptr<TJLterm<T> >( new TJLterm<T>(exponents, jl._std ) ) );    

 for (int i=0; i< jl._myEnv->numVar(); ++i ) {
   
   exponents(i) = 1;
   _terms.push_back( boost::shared_ptr<TJLterm<T> >( new TJLterm<T>(exponents, jl._jcb[i]) ) );    
   exponents(i) = 0;

 }
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
TJLterm<T> const * const TJL1Iterator<T>::operator++() { 
 
   ++_it;
   if ( _it == _terms.end() ) return 0; 
   return ( (*_it).get() );


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
void TJL1Iterator<T>::reset() {

 _it = _terms.begin();

}
