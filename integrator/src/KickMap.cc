#include "KickMap.h"

/// ******************************************
///   class KickMap
/// ******************************************

KickMap::KickMap() : ODE() {
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
    cerr << "KickMap::operator(): Wrong dimension." << endl;
    return 0;
  }
}
