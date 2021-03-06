/*
**
** Test of Mapping::Inverse() 
** This does not work well because of retaining
** negligible terms (of order 10**-16).  I do
** not yet understand why these terms are being
** kept.
** 
** First test with non-nilpotent Jets
**
** --- Leo Michelotti
** --- February 9, 1995
**
*/

#include <mxyzptlk/Mapping.h>

using namespace std;

int main( int argc, char** argv )
{
  if( 2 != argc ) {
    cout << "Usage: " << argv[0] << " <deg>" << endl;
    return -1;
  }
  int deg = atoi( argv[1] );

  Jet__environment::BeginEnvironment( deg );
  coord x( -1.2 ), y( 3.5 ), z( 2.1 );
  Jet__environment::EndEnvironment();

  Mapping w, u;

  w.SetComponent( 0,  3.0 + x + 3.0*y + x*y - y*z   );
  w.SetComponent( 1, -1.0 + y - x + z + x*z + y*y   );
  w.SetComponent( 2,  2.0 + z + 2.0*x + y*z - x*y*z );

  cout << "\n====== w.printCoeffs(); =================\n" << endl; 
  w.printCoeffs();
  cout << "\n\nDGN: w, streamed:\n\n" << w << endl;

  u = w.Inverse();

  cout << "\n====== w.printCoeffs(); =================\n" << endl; 
  w.printCoeffs();
  cout << "\n\nDGN: w, streamed:\n\n" << w << endl;

  cout << "\n====== u.printCoeffs(); =================\n" << endl; 
  u.printCoeffs();
  cout << "\n\nDGN: u, streamed:\n\n" << u << endl;

  cout << "\n====== w(u).printCoeffs(); =================\n" << endl; 
  w(u).printCoeffs();

  cout << "\n====== u(w).printCoeffs(); =================\n" << endl; 
  u(w).printCoeffs();

  return 0;
}
