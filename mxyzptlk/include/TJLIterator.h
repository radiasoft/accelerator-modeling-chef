/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJLIterator.h
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

#ifndef TJLITERATOR_H
#define TJLITERATOR_H

template <typename T>
class TJLterm;


template <typename T>
class TJLIterator {

public:

  TJLIterator( TJL<T> const& jl);     
  TJLterm<T> const * const operator++(); // pre-increment operator
  void reset(); 

 

private:

 TJLterm<T> const * const   _itbegin;
 TJLterm<T> const * const   _itend;
 TJLterm<T> const *         _it;


};

#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJLIterator.tcc>
#endif

#endif // TJLITERATOR_H



