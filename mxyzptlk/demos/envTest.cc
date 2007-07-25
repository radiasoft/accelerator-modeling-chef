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

#include <mxyzptlk/mxyzptlk.h>

using namespace std;

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

 Jet__environment_ptr a, b, c;

 Jet__environment::BeginEnvironment( 4 );
 coord x( atof( argv[1] ) );   
 coord y( atof( argv[2] ) );
 a = Jet__environment::EndEnvironment();

 Jet__environment::BeginEnvironment( 2 );
 coord r( 0.0 );
 b = Jet__environment::EndEnvironment();

 Jet__environment::BeginEnvironment( 10 );
 coord s( 0.0 );
 c = Jet__environment::EndEnvironment();

 Jet f(a), g(b), k(c);

 f = x*x + y*y;
 cout << " f.printCoeffs();" << endl;
 f.printCoeffs();

 g = exp(r);
 k = exp(s);
 cout << " g.printCoeffs();" << endl;
 g.printCoeffs();

 Jet h(a);

 std::vector<Jet> w(1);
 w[0] = f;
 h = g(w);
 cout << " h.printCoeffs();" << endl;
 h.printCoeffs();
 cout << " f.printCoeffs();" << endl;
 f.printCoeffs();

 h = k(w);
 cout << " h.printCoeffs();" << endl;
 h.printCoeffs();
 cout << " f.printCoeffs();" << endl;
 f.printCoeffs();

 cout << " exp( f ).printCoeffs();" << endl;
 exp( f ).printCoeffs();
}
