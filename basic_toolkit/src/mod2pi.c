#define twopi 6.2831853072

double mod2pi( x )
double x;
{
if ( x == 0.0 ) return x;
while ( x < 0.0 ) x+= twopi;
while ( x >= twopi ) x -= twopi;
return x;
}
