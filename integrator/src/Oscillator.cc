#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "Oscillator.h"

/// ******************************************
///   class Oscillator
/// ******************************************

Oscillator::Oscillator() : ODE() {
  dimen = 2;
}

Oscillator::~Oscillator() {
}

Vector Oscillator::operator() (const Vector& state, double /* t */ ){
  Vector ret(dimen);
  if (dimen == 2){
    ret(0) =   state(1);
    ret(1) = - state(0);
    return ret;
  }
  else{
    cerr << "Oscillator::operator(): Wrong dimension." << endl;
    return 0;
  }
}
