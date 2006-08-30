/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.2
******                                    
******  File:      ExpandingArray.cc
******                                                                
******  Copyright (c) 2002 Universities Research Association, Inc.    
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
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/ExpandingArray.h>

// 
// Utility class ExpandingArray
// 
const int ExpandingArray::_blockSize = 512;

#define TWOPI (2.0*M_PI)

ExpandingArray::ExpandingArray()
{
  _maxIndex = _blockSize - 1;
  _x = new double[ _blockSize ];
  for( int i = 0; i < _blockSize; i++ ) {
    _x[i] = 0.0;
  }
}


ExpandingArray::~ExpandingArray()
{
  delete [] _x;
}


double& ExpandingArray::operator[]( int i )
{
  if( _maxIndex < i ) {
    int j;
    int n = _maxIndex + 1 + _blockSize;
    while( n < i+1 ) { n += _blockSize; }
    double* y = new double [ n ];
    for( j = 0; j <= _maxIndex; j++ ) {
      y[j] = _x[j];
    }
    for( j = _maxIndex+1; j < n; j++ ) {
      y[j] = 0.0;
    }
    delete [] _x;
    _x = y;
    _maxIndex = n-1;
  }

  return _x[i];
}


int ExpandingArray::size()
{
  return (_maxIndex + 1);
}


