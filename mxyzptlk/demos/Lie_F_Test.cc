#include <stdlib.h>
#include "Jet.h"
#include "LieOperator.h"

using namespace std;

class field {
 private:
  Jet v[3];
 public:
  field( coord&, coord&, coord& );
  Jet operator[]( int );
};

field::field( coord& x, coord& y, coord& z ) {
 v[0] = - 2.0*x*y;
 v[1] =   x*x - y*y;
 v[2] =   0.0;
}

Jet field::operator[]( int i ) {
 return v[i];
}

main( int argc, char** argv ) {

 if( argc != 6 ) {
  printf( "\n" );
  printf( "Usage: %s  d m t p0 nb \n", argv[0] );
  printf( "where d  = degree of the jet\n" );
  printf( "      m  = mass \n" );
  printf( "      t  = time\n" );
  printf( "      p0 = longitudinal momentum\n" );
  printf( "      nb = number of cells in time partition\n" );
  exit(0);
 }

 int i, j;
 for( i = 0; i < argc; i++ ) printf( "%s   ", argv[i] );
 printf( "\n\n" );

 int    deg = atoi( argv[1] ); // Degree of the jet
 double msq = atof( argv[2] ); // Mass of the proton
 double t   = atof( argv[3] ); // Time in the sextupole
 double p0  = atof( argv[4] ); // Momentum offset
 int    nb  = atoi( argv[5] ); // Number of cells in partition

 Jet__environment::BeginEnvironment( deg );
 coord  x( 0.0 ),  y( 0.0 ),  z( 0.0 );
 coord px( 0.0 ), py( 0.0 ), pz( 0.0 );
 Jet__environment::EndEnvironment();

 msq = msq*msq;
 double vel = p0 / sqrt( p0*p0 + msq );
 field B( x, y, z );

 double dt = t / nb;
 t = 0;

 LieOperator v;
 Jet q[6], s[6];  s[0] = x;  s[1] = y;  s[2] = z;
                 s[3] = px; s[4] = py; s[5] = p0 + pz;
 Jet E;
 E = msq;
 for( i = 3; i < 6; i++ ) E += s[i]*s[i];
 E = sqrt(E);

 for( i = 0; i < nb; i++ ) {
  v.SetComponent(  0, s[3]		        / E );
  v.SetComponent(  1, s[4]		        / E );
  v.SetComponent(  2, s[5]   	                / E );
  v.SetComponent(  3, ( s[4]*B[2] - s[5]*B[1] ) / E );
  v.SetComponent(  4, ( s[5]*B[0] - s[3]*B[2] ) / E );
  v.SetComponent(  5, ( s[3]*B[1] - s[4]*B[0] ) / E );

  for( j = 0; j < 6; j++ ) 
   q[j] = v.expMap( dt, s[j] );

  for( j = 0; j < 6; j++ ) 
   s[j] = q[j];
 }

 for( j = 0; j < 6; j++ ) {
  printf( "\nResults for s[%d] ============================\n", j );
  s[j].printCoeffs();
 }

}
