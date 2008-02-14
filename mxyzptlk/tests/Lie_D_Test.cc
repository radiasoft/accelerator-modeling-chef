/*
**
** Demo program:
** 
** Uses the exponential map method to 
** integrate a non-Hamiltonian, 
** nonlinear vector field that spirals inward.
** 
** --- Leo Michelotti
** --- August 10, 1993
**
** Revised:
** May   27, 2006
** March  9, 2007
** July  31, 2007
**
*/

#include <mxyzptlk/LieOperator.h>

using namespace std;

main( int argc, char** argv ) 
{
  if( argc != 3 ) {
   cout << "\n"
        << "Usage: " << argv[0] << "  d  t            \n"
        << "where  d  = highest degree derivative     \n"
        << "       t  = upper limit of integration.   \n"
        << endl;
   exit(0);
  }

  int    deg   = atoi( argv[1] );
  double limit = atof( argv[2] );

  int        n = 3;
  // IMPOSE: n is odd and larger than one.
  double gamma = 1;
  double omega = M_TWOPI;

  Jet__environment::BeginEnvironment( deg );
    coord x( 0 );
    coord y( 0 );
    param t( limit );
  Jet__environment::EndEnvironment();

  Jet rsq, r, f, g;
  LieOperator v;
 
  rsq = x*x + y*y;
  int nm1 = n-1;

  // This test is not strictly necessary, since pow
  // with a double second argument exists.
  if( 0 == nm1%2 ) {
    v.SetComponent( 0, - gamma*pow(rsq,nm1/2)*x - omega*y ); 
    v.SetComponent( 1, - gamma*pow(rsq,nm1/2)*y + omega*x );
  }
  else {
    cerr << "Sorry." << endl;
    return -1;
    // v.SetComponent( 0, - gamma*pow(r,nm1)*x - omega*y ); 
    // v.SetComponent( 1, - gamma*pow(r,nm1)*y + omega*x );
  }

  #if 0
  cout << "\n-*-*-*-  rsq coefficients  -*-*-*-*-*" << endl;
  rsq.printCoeffs();
  cout << "\n-*-*-*-  v coefficients  -*-*-*-*-*" << endl;
  v.printCoeffs();
  #endif

  cout << "\n-*-*-*-  x  coefficients  -*-*-*-*-*" << endl;
  ( f = v.expMap( t, x ) ).printCoeffs();
  cout << "\n-*-*-*-  y  coefficients  -*-*-*-*-*" << endl;
  ( g = v.expMap( t, y ) ).printCoeffs();

  // Enter a point and evaluate the map.
  double s_data[] = { 1, 1, limit };  // x_0 = 1, y_0 = 1 ???
  Vector s(3,s_data);
  double   range = 0.2
         , tlo   = limit - (range/2.0)
         , thi   = limit + (range/2.0)
         , delta = 0.01;

  cout.setf( ios::fixed,ios::floatfield);

  cout << "\n-*-*-*-  Exact solution -*-*-*-*-*" << endl;
  double th0 = atan2( s[1], s[0] );
  double r0  = sqrt( s[0]*s[0] + s[1]*s[1] );
  double aaa = 1.0/pow(r0,nm1);
  double R;
  for( double t = tlo; t < thi + delta / 2.0 ; t += delta ) {
    R = 1.0 / ( gamma*nm1*t + aaa );
    R = pow(R,1.0/double(nm1));
    #if 0
    cout         << t 
         << "  " << (R*cos(th0 + omega*t)) 
         << "  " << (R*sin(th0 + omega*t)) 
         << "  " << R
         << endl;
    #endif
    cout << "  " << (R*cos(th0 + omega*t)) 
         << "  " << (R*sin(th0 + omega*t)) 
         << endl;
  }

  cout << "  " << endl;
  for( double t = tlo; t < thi + delta / 2.0 ; t += delta ) {
    R = 1.0 / ( gamma*nm1*t + aaa );
    R = pow(R,1.0/double(nm1));
    cout << "  " << (r0*cos(th0 + omega*t)) 
         << "  " << (r0*sin(th0 + omega*t)) 
         << endl;
  }
  
  cout << "  " << endl;
  for( double t = tlo; t < thi + delta / 2.0 ; t += delta ) {
    s[2] = t;
    cout << "  " << f(s)
         << "  " << g(s)
         << endl;
  }

  #if 0
  cout << "( " << s[0] << ", " << s[1] << " ) maps to ( "
       << f(s) << ", " << g(s) << " )\n"
       << endl;

  for( double t = tlo; t < thi + delta / 2.0 ; t += delta ) {
   s[2] = t;
   cout << t << ": ( " << f(s) << ", " << g(s) << " )"
        << endl;
  }
  #endif
}
