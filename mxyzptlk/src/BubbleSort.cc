#include <iostream.h>

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
