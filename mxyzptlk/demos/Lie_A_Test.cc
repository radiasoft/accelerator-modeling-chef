/*
**
** Demo program:
** 
** A one-liner: calculates the Lie
** derivative of r**2 under the vector
** field of a damped harmonic oscillator.
** 
** --- Leo Michelotti
** --- August 10, 1993
**
*/

#include <stdlib.h>
#include <iostream.h>
#include "mxyzptlk.h"

main( int argc, char** argv ) {

 if( argc != 3 ) {
  cout << "Usage: " << argv[0] << "x  y \n" << endl;
  exit(0);
 }

 Jet::BeginEnvironment( 1 );
 coord x( atof( argv[1] ) );   
 coord p( atof( argv[2] ) );
 Jet::EndEnvironment();

 LieOperator v( x.Env() );
 
 v.setVariable(      p, 0 );
 v.setVariable( -x - p, 1 );

 cout << "Answer is: \n"
      << ( v.operator^( x*x + p*p ) ).standardPart();
}
