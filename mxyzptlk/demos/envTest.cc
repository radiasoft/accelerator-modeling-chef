/*
**
** Demo program:
** 
** Test of two different environments in one program.
** 
** --- Leo Michelotti
** --- April 17, 1996
**
*/

#include "mxyzptlk.h"

main( int argc, char** argv ) {

 if( argc != 3 ) {
  cout << "Usage: " << argv[0] << "  x  y"
       << endl;
  exit(0);
 }

 cout << "Command line: " << argv[0] << "  "
      << argv[1] << "  "
      << argv[2] << "  \n"
      << endl;

 Jet__environment *a, *b, *c;

 Jet::BeginEnvironment( 4 );
 coord x( atof( argv[1] ) );   
 coord y( atof( argv[2] ) );
 a = Jet::EndEnvironment();

 Jet::BeginEnvironment( 2 );
 coord r( 0.0 );
 b = Jet::EndEnvironment();

 Jet::BeginEnvironment( 10 );
 coord s( 0.0 );
 c = Jet::EndEnvironment();

 Jet f(a), g(b), k(c);

 f = x*x + y*y;
 cout << " f.printCoeffs();" << endl;
 f.printCoeffs();

 g = exp(r);
 k = exp(s);
 cout << " g.printCoeffs();" << endl;
 g.printCoeffs();

 Jet h(a);

 h = g(&f);
 cout << " h.printCoeffs();" << endl;
 h.printCoeffs();
 cout << " f.printCoeffs();" << endl;
 f.printCoeffs();

 h = k(&f);
 cout << " h.printCoeffs();" << endl;
 h.printCoeffs();
 cout << " f.printCoeffs();" << endl;
 f.printCoeffs();

 cout << " exp( f ).printCoeffs();" << endl;
 exp( f ).printCoeffs();
}
