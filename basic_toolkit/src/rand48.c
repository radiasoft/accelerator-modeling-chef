/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      rand48.c
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


#include <stdlib.h>
#include <math.h>

#if 0
double drand48(int* idum)
{
  static double y,maxran,v[98];
  double dum;
  static int iff=0;
  int j;
  unsigned i,k;
  
  if (*idum < 0 || iff == 0) {
    iff=1;
    i=2;
    do {
      k=i;
      i<<=1;
    } while (i);
    maxran=k;
    srand(*idum);
    *idum=1;
    for (j=1;j<=97;j++) dum=rand();
    for (j=1;j<=97;j++) v[j]=rand();
    y=rand();
  }
  j=1+97.0*y/maxran;
  if (j > 97 || j < 1) 
    printf("drand48: This cannot happen.\n");
  y=v[j];
  v[j]=rand();
  return y/maxran;
}
#endif
