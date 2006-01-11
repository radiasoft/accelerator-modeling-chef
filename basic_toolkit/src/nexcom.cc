/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      nexcom.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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

bool nexcom( int n, int k, int* r ) {
  int t, h, i;
  static bool firstEntry  = true;
  static bool allDone     = false;

  if( allDone ) {
    firstEntry = true;
    allDone = false;
    return false;
  }

  if( firstEntry ) {
    r[0] = n;
    if( k > 1 ) {
      for( i = 1; i < k; i++ ) r[i] = 0;
      firstEntry = false;
      }
    else allDone = true;
    return true;
  }

  h = 0;
  while( r[h++] == 0 ) ;
  h--;

  t = r[h];
  r[h] = 0;
  r[0] = --t;
  r[++h]++;

  allDone = ( r[k-1] == n );

  return true;
}
