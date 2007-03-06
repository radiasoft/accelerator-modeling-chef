/*
**
** Test of Mapping inversion algorithm.
** 
** --- Leo Michelotti
** --- September 15, 1993
**
*/

#include <basic_toolkit/Matrix.h>
#include <mxyzptlk/Mapping.h>

int main( int argc, char** argv ) 
{
  if( argc != 3 ) {
    cout << "Usage: " << argv[0] << " <deg> <limit>"
            "\nwhere deg   = highest order derivative stored"
            "\n      limit = maximum number of iterations."
         << endl;
    return -1;
  }

  int deg   = atoi( argv[1] );
  int limit = atoi( argv[2] );

  Jet__environment::BeginEnvironment(deg);
  coord x( 0.0 ), y( 0.0 ), z( 0.0 );
  Jet__environment::EndEnvironment();

  Mapping u, v, w;
  Mapping id( "ident" );


  w.SetComponent( 0, x + 3.0*y + x*y - y*z   );
  w.SetComponent( 1, y - x + z + x*z + y*y   );
  w.SetComponent( 2, z + 2.0*x + y*z - x*y*z );


  MatrixD M( 3, 3 );
  M = w.Jacobian();
  M = M.inverse();


  int i = 0;
  u = M*id;
  v = ( w(u) - id );
  while ( ( i++ < limit ) && ( v != 0.0 ) ) {
   u = u - M*v;
   v = ( w(u) - id );
  }
  if( i >= limit ) printf( "++++ Limit exceeded! ++++\n" );


  cout << "\n====== u.printCoeffs(); =================\n";
  u.printCoeffs();

  cout << "\n====== w.printCoeffs(); =================\n";
  w.printCoeffs();

  cout << "\n====== w(u).printCoeffs(); =================\n";
  w(u).printCoeffs();

  cout << "\n====== u(w).printCoeffs(); =================\n";
  u(w).printCoeffs();

  return 0;
}
