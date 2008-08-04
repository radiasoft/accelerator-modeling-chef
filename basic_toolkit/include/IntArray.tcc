/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      IntArray.tcc
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
**************************************************************************/

template< typename const_iterator_t>
IntArray::IntArray( const_iterator_t it1, const_iterator_t it2 )
  : weight_(0), weight_is_valid_(false)
{
  this->comp_.resize( it2 - it1 );
  std::copy( it1, it2, this->begin() ); 
  Sum(); // this sets weight_ and   weight_is_valid to true;     

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template< typename const_iterator_t>
void IntArray::Set( const_iterator_t it1, const_iterator_t it2 )
{

  comp_.resize( distance( it1, it2) );
  std::copy( it1, it2, begin() ); 

  Sum(); // this sets weight_ and   weight_is_valid to true;     

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

