/*
**
** Demo program:
** 
** Simple calculation of a derivative.
** 
** --- Leo Michelotti
** --- August 18, 1995
**
*/

#include "mxyzptlk.h"

main( int argc, char** argv ) {

 if( argc != 5 ) {
  cout << "\nUsage: " << argv[0] 
       << "  x n_x y n_y\n"
       << endl;
  exit(0);
 }

 int deg [2];
 deg[0] = atoi( argv[2] );
 deg[1] = atoi( argv[4] );

 Jet::BeginEnvironment( deg[0] + deg[1] );
 coord x( atof( argv[1] ) ), y( atof( argv[3] ) );
 Jet::EndEnvironment();

 cout << "Answer: " 
      << ( exp(-x*x) * sin(y) ).derivative( deg ) 
      << endl;

}
