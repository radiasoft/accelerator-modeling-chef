#include "PhysicsConstants.h"

#include "mxyzptlk.h"
#include "beamline.h"
#include "combinedFunction.h"

main( int argc, char** argv )
{
  if( argc != 3 ) {
    cout << "Usage: " << argv[0] << " <x>  <x'>" << endl;
    exit(0);
  }

  #include "/home/room1/michelotti/CVS/fnal/machines/vlhc/include/LF3TeV.cfg"

  double x_i  = atof( argv[1] );
  double xp_i = atof( argv[2] );

  Jet::BeginEnvironment( 1 );
  coord c_a(x_i),   c_b(0.0),  c_c(0.0),
        c_d(xp_i),  c_e(0.0),  c_f(0.0);
  Jet::EndEnvironment();

  double inState [] = { 0, 0, 0, 0, 0, 0 };
  enum { x, y, cdt, xp, yp, dpop };
  inState[x]  = x_i;
  inState[xp] = xp_i;

  JetProton jp( PH_NORM_mp + ke );
  Proton     p( PH_NORM_mp + ke );
  jp.setState( inState );

  double answer = arcqd.AdjustPosition( jp );
  cout << "Answer: " << 1000.0*answer << " mm" << endl;

  cout << "A little test:" << endl;
  inState[x]  = 0.0;
  inState[xp] = 0.0;
  p .setState( inState );
  cout << p.State() << endl;
  arcqd.propagate(p);
  cout << p.State() << endl;

}
