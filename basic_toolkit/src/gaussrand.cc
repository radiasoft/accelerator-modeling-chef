/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      gaussrand.cc
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


#include <iostream>
#include <stdlib.h>
#include <math.h>

double gaussrand()
{
  static double V1, V2, S, X;
  static double max = RAND_MAX;
  static int phase = 0;

  if(phase == 0) {
    do {
      double U1 = ((double) rand()) / max;
      double U2 = ((double) rand()) / max;
      V1 = 2 * U1 - 1;
      V2 = 2 * U2 - 1;
      S = V1 * V1 + V2 * V2;
    }
    while(S >= 1 || S == 0);

    X = V1 * sqrt(-2 * log(S) / S);
  } 
 
  else {
    X = V2 * sqrt(-2 * log(S) / S);
  }

  phase = 1 - phase;
  return X;
}
  
// Implementation of the gaussian random number generator, this will keep
// producing numbers until 5 is found (note: 5 will never be found as this
// produces numbers between -3 and 3)
// 
// int main(){
//   double x=0;
//   
//   while ( x <= 5. )
//   {
//     x = gaussrand();
// 
//     if( fabs(x) < 1.0 )      cout << "*** ";
//     else if( fabs(x) < 2.0 ) cout << "*** *** ";
//     else if( fabs(x) < 3.0 ) cout << "*** *** *** ";
//     else if( fabs(x) < 4.0 ) cout << "*** *** *** *** ";
//     else if( fabs(x) < 5.0 ) cout << "*** *** *** *** *** ";
// 
//     cout << x << endl;
//   }
// 
//   return 0;
// }
// 

