#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <VectorD.h>
#include <Symplectic.h>
#include <DriftMap.h>
#include <KickMap.h>

using namespace std;

main()
{
  Symplectic sym;
  sym.setNStep( 10 );

  DriftMap map1;
  KickMap map2;

  Vector s(2);
  s(0) = 1.;
  s(1) = 0.;
 
  Vector s1(2);
 
  cout.precision(16);
  cout << "Start integration:" << endl;
  s1 = sym.integrate( 2, map1, map2, s, 0, 4*atan(1.0) );
  cout << s1 << endl;
  cout << "Symplecticity error = " << 1-(s1(0)*s1(0)+s1(1)*s1(1)) << endl;
  cout << "End integration:" << endl;
}
