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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "ExpandingArray.h"

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


