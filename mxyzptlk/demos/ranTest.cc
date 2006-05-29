#include <math.h>
#include <stdio.h>
#include <iostream.h>
#include <rand48.h>

using namespace std;

extern double drand48( int* );

main( int argc, char** argv ) {
int seed( -9 );
int limit( 100 );

if( argc > 1 ) { 
  limit = atoi( argv[1] );
  if( argc > 2 ) {
    seed  = atoi( argv[2] );
  }
}

for( int i = 0; i < limit; i++ ) {
  cout << drand48() << endl;
}

}
