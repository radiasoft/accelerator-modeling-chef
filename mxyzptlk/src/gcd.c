/*
    Function returns the greatest common divisor of x and y.
*/
int gcd( x, y )
int x;
int y;
{
int larger, smaller;
int remainder;

if( x < 0 ) x = -x;
if( y < 0 ) y = -y;

if( x == 0 ) {
  if( y == 0 ) return 1;
  else return y;
  }

if( y == 0 ) return x;

if ( x < y ) {
  larger = y;
  smaller = x;
  }
else {
  larger = x;
  smaller = y;
  }

remainder = larger%smaller;

while( remainder != 0 ) {
  larger = smaller;
  smaller = remainder;
  remainder = larger%smaller;
  }

return smaller;

}

