/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      BubbleSort.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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


#ifdef  _MSC_VER
#include <iostream>
using std::cerr;
using std::endl;
#else
#include <iostream.h>
#endif

void BubbleSort( double x[], int n )
{
  if( n < 2 ) {
    cerr << "*** WARNING ***                             \n" 
            "*** WARNING ***  BubbleSort                 \n" 
            "*** WARNING ***  n = " << n << " < 2        \n" 
            "*** WARNING ***                             \n" 
         << endl;
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
