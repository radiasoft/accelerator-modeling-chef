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

#include <mxyzptlk/Jet.h>

main( int argc, char** argv ) {

 if( argc != 5 ) {
  std::cout << "\nUsage: " << argv[0] 
            << "  x n_x y n_y\n"
            << std::endl;
  exit(0);
 }

 IntArray deg(2);
 deg[0] = atoi( argv[2] );
 deg[1] = atoi( argv[4] );

 Jet__environment::BeginEnvironment( deg[0] + deg[1] );
 coord x( atof( argv[1] ) ), y( atof( argv[3] ) );
 Jet__environment::EndEnvironment();

 std::cout << "Answer: " 
           << ( exp(-x*x) * sin(y) ).derivative( deg ) 
           << std::endl;

}
