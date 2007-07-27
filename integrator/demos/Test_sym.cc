#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <integrator/ODE.h>
#include <basic_toolkit/VectorD.h>
#include <integrator/Symplectic.h>

using namespace std;


// ******************************************
// class DriftMap
// ******************************************
class DriftMap : public ODE 
{
 private:
 public:
   DriftMap();
   virtual ~DriftMap();
   Vector operator() (const Vector&, double);
};


DriftMap::DriftMap() 
:   ODE() 
{
  dimen = 2;
}

DriftMap::~DriftMap() 
{
}

Vector DriftMap::operator() (const Vector& state, double t )
{
  Vector ret(dimen);
  if (dimen == 2){
    ret(0) = state(0)+t*state(1);
    ret(1) = state(1);
    return ret;
  }
  else{
    (*pcerr) << "DriftMap::operator(): Wrong dimension." << std::endl;
    return 0;
  }
}


// ******************************************
// class KickMap
// ******************************************
class KickMap : public ODE {

 private:
 public:
   KickMap();
   virtual ~KickMap();
   Vector operator() (const Vector&, double);
};


KickMap::KickMap() 
:   ODE() 
{
  dimen = 2;
}


KickMap::~KickMap() {
}


Vector KickMap::operator() (const Vector& state, double t ){
  Vector ret(dimen);
  if (dimen == 2){
    ret(0) =  state(0);
    ret(1) = -t*state(0)+state(1);
    return ret;
  }
  else{
    (*pcerr) << "KickMap::operator(): Wrong dimension." << std::endl;
    return 0;
  }
}


// ******************************************
// Main program
// ******************************************

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
