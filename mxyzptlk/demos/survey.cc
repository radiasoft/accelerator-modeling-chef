/*
**
** Demo program:
** 
** Calculates the measurement sensitivity
** matrix d( x, y, z ) / d( r, theta, phi ) 
** corresponding to a monostatic radar observation
** (i.e., polar to Cartesian conversion).
** 
** --- Leo Michelotti
** --- August 19, 1993
** 
** Templated version.
** --- March 18, 2004
**
*/

#include <stdlib.h>
#include "Mapping"

int main( int argc, char** argv ) 
{
 if( argc != 4 ) {
  std::cout << "\nUsage: " << argv[0] 
            << "  <r>  <theta (deg)>  <phi (deg)>\n"
            << std::endl;
  exit(0);
 }

 const double d2r = M_PI / 180.0;
 MatrixD M( 3, 3 );

 Jet::BeginEnvironment( 1 );
 coord r     (     atof( argv[1] ) ),
       theta ( d2r*atof( argv[2] ) ),
       phi   ( d2r*atof( argv[3] ) );
 Jet::EndEnvironment();

 Mapping position;

 position.SetComponent( 0, r * sin( theta ) * cos( phi ) );
 position.SetComponent( 1, r * sin( theta ) * sin( phi ) );
 position.SetComponent( 2, r * cos( theta )              );

 M = position.Jacobian();
 std::cout << M << "\n\n" << M.inverse() << std::endl;

 return 0;
}
