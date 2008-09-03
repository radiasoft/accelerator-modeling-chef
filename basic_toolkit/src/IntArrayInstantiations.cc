/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      IntArrayInstantiations.cc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov        
******
******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

// =================================================================================
// If implicit template instantiations are desired, *do not* compile this file !
// =================================================================================

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef BASICTOOLKIT_EXPLICIT_TEMPLATES 

#include <complex>
#include <numeric>
#include <functional>
#include <algorithm>

#include <basic_toolkit/IntArray.h>
#include <basic_toolkit/IntArray.tcc>

using namespace std;
using std::ostringstream;

// ----------------------------------------------------------------------------
// Instantiations related to IntArray
// ----------------------------------------------------------------------------

template
IntArray::IntArray( int* it1, int* it2 );

template
IntArray::IntArray( int const* it1, int const* it2 );

template
void  IntArray::Set( int* it1, int* it2);

template
void  IntArray::Set( int const* it1, int const* it2);

template 
class std::vector<IntArray::exponent_t>;

namespace {
 IntArray::iterator               itdummy1;
 IntArray::const_iterator         itdummy2;  
 IntArray::reverse_iterator       itdummy3;
 IntArray::const_reverse_iterator itdummy4;
}
    
template
void std::fill( IntArray::iterator, IntArray::iterator, int const&); 


template
int std::accumulate( IntArray::iterator, IntArray::iterator,  int);

template
int std::accumulate( IntArray::const_iterator, IntArray::const_iterator,  int);

template 
class std::plus<IntArray::exponent_t>;

template 
class std::less_equal<IntArray::exponent_t>;

template
class std::greater<IntArray::exponent_t>;

template
class std::greater_equal<IntArray::exponent_t>;

template
IntArray::iterator std::transform( IntArray::iterator, IntArray::iterator, IntArray::iterator, IntArray::iterator,  std::plus<IntArray::exponent_t> );

template
IntArray::iterator std::transform( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator, IntArray::iterator,  std::plus<IntArray::exponent_t> );

template
IntArray::iterator std::transform( IntArray::iterator, IntArray::iterator, IntArray::iterator, IntArray::iterator,  std::multiplies<IntArray::exponent_t> );

template
IntArray::iterator std::transform( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator, IntArray::iterator,  std::multiplies<IntArray::exponent_t> );


template
bool std::equal( IntArray::iterator, IntArray::iterator, IntArray::iterator);

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::less<IntArray::exponent_t> );

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::less_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::iterator, IntArray::iterator, IntArray::iterator,  IntArray::iterator, std::greater<IntArray::exponent_t>);

template
bool std::equal( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::less<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::less_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::greater<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_iterator, IntArray::const_iterator, IntArray::const_iterator,  IntArray::const_iterator, std::greater_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::less<IntArray::exponent_t> );

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::less_equal<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::greater<IntArray::exponent_t>);

template
bool std::lexicographical_compare( IntArray::const_reverse_iterator, IntArray::const_reverse_iterator, IntArray::const_reverse_iterator,  IntArray::const_reverse_iterator, std::greater_equal<IntArray::exponent_t>);

template
void std::fill<int*, int>(int*, int*, int const&);

template
double* std::fill_n<double*, int, int>(double*, int, int const&);

template
IntArray::iterator 
std::find_if( IntArray::iterator, IntArray::iterator, std::binder2nd<std::not_equal_to<int> > ); 
   
template
int std::distance( IntArray::iterator, IntArray::iterator );

template
IntArray::iterator
std::copy( IntArray::const_iterator, 
           IntArray::const_iterator, IntArray::iterator ); 

template
IntArray::iterator
std::copy( IntArray::iterator, 
           IntArray::iterator, IntArray::iterator ); 


#endif
