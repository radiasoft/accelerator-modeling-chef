/*
**
** Test of Jet::Inverse() function.
** 
** --- Leo Michelotti
** --- September 16, 1993
**
*/


#include <stdio.h>
#include <iostream>
#include "mxyzptlk.h"

using namespace std;

main( int argc, char** argv ) {

 if( argc != 2 ) {
  printf( "Usage: %s <degree>\n", argv[0] );
  exit(0);
 }

 int deg = atoi( argv[1] );

 Jet__environment::BeginEnvironment( deg );
 coord x(1.0);
 Jet__environment::EndEnvironment();

 Jet f, g;

 f = x + x*x + x*x*x;
 g = f.Inverse();
 g.printCoeffs();
 f(&g).printCoeffs();
 g(&f).printCoeffs();

 f = 1.0 + x + x*x + x*x*x;
 g = f.Inverse();
 g.printCoeffs();
 f(&g).printCoeffs();
 g(&f).printCoeffs();

 Jet    h,u;
 double w;

 h = f(&g);
 u = g(&f);

 w = 0.0;
 while( w < 9.1 ) {
  cout << w << "  " << h(&w) << "  " << u(&w) << endl;
  w += 0.25;
 }
}
