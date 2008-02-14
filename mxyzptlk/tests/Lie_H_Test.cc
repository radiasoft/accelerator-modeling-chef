/*
**
** Demo program:
** 
** Uses the exponential map method to 
** integrate equations of motion of a 
** sextupolar type vector field.
** 
** As part of demo, we introduce the
** class field to make writing the
** vector field easier, as well as to
** illustrate style and elegance.
** 
** --- Leo Michelotti
** --- August 10, 1993
**
*/

#include <mxyzptlk/LieOperator.h>
#include <mxyzptlk/Mapping.h>

using namespace std;

class field {
 private:
  Jet v[3];
 public:
  field( coord&, coord&, coord& );
  Jet operator[]( int );
};

field::field( coord& x, coord& y, coord& z ) {
 v[0] = - 2.0*x*y;
 v[1] =   x*x - y*y;
 v[2] =   0.0;
}

Jet field::operator[]( int i ) {
 return v[i];
}


int main( int argc, char** argv ) 
{
 if( argc != 4 ) {
  cout <<   "Usage: " << argv[0] << " d  m  t "
          "\nwhere  d  = highest order derivative"
          "\n       m  = mass"
          "\n       t  = upper limit of integration."
       << endl;
  return -1;
 }

 int    deg = atoi( argv[1] );
 double msq = atof( argv[2] );
 double t   = atof( argv[3] );

 msq = msq*msq;
 Jet__environment::BeginEnvironment( deg );
 coord  x( 0.0 ),  y( 0.0 ),  z( 0.0 );
 coord px( 0.0 ), py( 0.0 ), pz( 0.0 );
 Jet__environment::EndEnvironment();

 Jet E = sqrt( px*px + py*py + pz*pz + msq );
 field B( x, y, z );

 LieOperator v;
 v.SetComponent( 0,  px                    / E );
 v.SetComponent( 1,  py                    / E );
 v.SetComponent( 2,  pz                    / E );
 v.SetComponent( 3,  ( py*B[2] - pz*B[1] ) / E );
 v.SetComponent( 4,  ( pz*B[0] - px*B[2] ) / E );
 v.SetComponent( 5,  ( px*B[1] - py*B[0] ) / E );

 cout << "\n-*-*-*-  x  coefficients  -*-*-*-*-*" << endl;
 v.expMap( t, x ).printCoeffs();
 cout << "\n-*-*-*-  y  coefficients  -*-*-*-*-*" << endl;
 v.expMap( t, y ).printCoeffs();
 cout << "\n-*-*-*-  z  coefficients  -*-*-*-*-*" << endl;
 v.expMap( t, z ).printCoeffs();

 Mapping u, q;
 LieOperator w;

 w.SetComponent( 0, x  );
 w.SetComponent( 1, y  );   // ::expMap should take JetVector as argument
 w.SetComponent( 2, z  );   // and return a JetVector ... and similarly
 w.SetComponent( 3, px );   // for JetCVector.
 w.SetComponent( 4, py );
 w.SetComponent( 5, pz );
 // w.reset();

 u = v.expMap(  t, w );
 q = v.expMap( -t, w );

 cout << "\n-*-*-*-  u  coefficients  -*-*-*-*-*" << endl;
 u.printCoeffs();

 cout << "\n-*-*-*-  u  coefficients  -*-*-*-*-*" << endl;
 q.printCoeffs();
 
 cout << "\n-*-*-*-  u(q)  coefficients  -*-*-*-*-*" << endl;
 u(q).printCoeffs();

 cout << "\n-*-*-*-  q(u)  coefficients  -*-*-*-*-*" << endl;
 q(u).printCoeffs();

 cout << endl;
 return 0;
}
