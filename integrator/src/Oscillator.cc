#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <integrator/Oscillator.h>
#include <basic_toolkit/iosetup.h>

using FNAL::pcerr;
using FNAL::pcout;


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
    (*pcerr) << "Oscillator::operator(): Wrong dimension." << std::endl;
    return 0;
  }
}
