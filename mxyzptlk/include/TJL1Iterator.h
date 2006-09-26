/*************************************************************************
**************************************************************************
**************************************************************************
******
******  MXYZPTLK:  A C++ implementation of differential algebra.
******
******  File:      TJL1Iterator.h
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

#ifndef TJL1ITERATOR_H
#define TJL1ITERATOR_H

#include <boost/shared_ptr.hpp>
#include <vector>

template <typename T>
class TJLterm;

template <typename T>
class TJL1;


template <typename T>
class TJL1Iterator {

public:

  TJL1Iterator( TJL1<T> const& jl);     
  TJLterm<T> const * const operator++(); // pre-increment operator
  TJLterm<T> const * const operator->() { return  (*_it).get();}
  TJLterm<T> const&        operator*()  { return *(*_it);      }
  void reset(); 

 
private:

  typename std::vector<boost::shared_ptr<TJLterm<T> > >            _terms; 
  typename std::vector<boost::shared_ptr<TJLterm<T> > >::iterator  _it;

};

#ifndef MXYZPTLK_EXPLICIT_TEMPLATES
#include <mxyzptlk/TJL1Iterator.tcc>
#endif

#endif // TJL1ITERATOR_H



