#include "DriftMap.h"

using namespace std;

/// ******************************************
///   class DriftMap
/// ******************************************

DriftMap::DriftMap() : ODE() {
  dimen = 2;
}

DriftMap::~DriftMap() {
}

Vector DriftMap::operator() (const Vector& state, double t ){
  Vector ret(dimen);
  if (dimen == 2){
    ret(0) = state(0)+t*state(1);
    ret(1) = state(1);
    return ret;
  }
  else{
    cerr << "DriftMap::operator(): Wrong dimension." << endl;
    return 0;
  }
}
