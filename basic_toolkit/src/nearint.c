#include <math.h>
#include <stdio.h>

int nearestInteger( x )
double x;
{
double lo, hi;

lo = floor(x);
hi = ceil(x);
if( x - lo >= hi - x )
  return (int) hi;
else
  return (int) lo;
}
