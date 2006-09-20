/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJLIterator.tcc
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
TJLIterator<T>::TJLIterator( TJL<T> const& jl): 
_itbegin(jl._jltermStore-1),
_itend(jl._jltermStoreCurrentPtr),
_it(_itbegin) { }
 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template <typename T>
TJLterm<T> const * const TJLIterator<T>::operator++() { 
 
   ++_it;
   if ( _it == _itend ) return 0; 
   return (_it);


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
void TJLIterator<T>::reset() {

 _it = _itbegin;

}
