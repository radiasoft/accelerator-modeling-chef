#include <stdlib.h>
#include <math.h>

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
