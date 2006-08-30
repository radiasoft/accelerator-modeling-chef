/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      BubbleSort.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
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
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <basic_toolkit/iosetup.h>

using FNAL::pcerr;
using FNAL::pcout;

void BubbleSort( double x[], int n )
{
  if( n < 2 ) {
    (*pcerr) << "*** WARNING ***                             \n" 
                "*** WARNING ***  BubbleSort                 \n" 
                "*** WARNING ***  n = " << n << " < 2        \n" 
                "*** WARNING ***                             \n" 
             << std::endl;
    return;
  }

  int i, j;
  j = n-1;
  double temp = x[j];
  for( i = 0; i < n-1; i++ ) {
    if( temp < x[i] ) {
      j = i;
      temp = x[j];
    }
  }

  temp = x[n-1];
  x[n-1] = x[j];
  x[j] = temp;

  if( n == 2 ) return;
  else         BubbleSort( x, n-1 );

}
