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
