#include <Symplectic.h>
#include <iosetup.h>

using  FNAL::pcout;
using  FNAL::pcerr;

using std::endl;

/// ******************************************
///   class Symplectic
/// ******************************************

Symplectic::Symplectic() : Integrator() {
  nStep = 10;
}

Symplectic::~Symplectic() {
}

Vector Symplectic::symplectic2( ODE& yMap1, ODE& yMap2, Vector yInit,
  double t0, double t1 ) {
  /// yInit: Starting values of the ODE
  /// t0: Starting value of the independent variable
  /// yMap1: Exact map of H_1
  /// yMap2: Exact map of H_2
  /// H = H_1 + H_2

  if ( yInit.Dim() != yMap1.getDim() ) {
    (*pcerr) << "Error in Symplectic::symplectic2: Dimensions don't match." << endl;
    return 0;
  }
  else {
    int i;
    double stepSize;
    int dim;
  
    dim = yInit.Dim();
    Vector yTemp(dim);
    double length;
  
    length = t1-t0;
    stepSize = length/nStep;
    yTemp = yInit;
    for (i=0; i<nStep; i++) {
      yTemp = yMap1(yTemp, stepSize/2);
      yTemp = yMap2(yTemp, stepSize);
      yTemp = yMap1(yTemp, stepSize/2);
    }
    return yTemp;
  }
}

Vector Symplectic::symplectic4( ODE& yMap1, ODE& yMap2, Vector yInit,
  double t0, double t1 ) {
  /// yInit: Starting values of the ODE
  /// t0: Starting value of the independent variable
  /// yMap1: Exact map of H_1
  /// yMap2: Exact map of H_2
  /// H = H_1 + H_2

  if ( yInit.Dim() != yMap1.getDim() ) {
    (*pcerr) << "Error in Symplectic::symplectic4: Dimensions don't match." << endl;
    return 0;
  }
  else {
    int i;
    double stepSize;
    int dim;
  
    dim = yInit.Dim();
    Vector yTemp(dim);
    double power = 1./3.;
    double a;
    double length;
  
    length = t1-t0;
    a = 1 - pow(2, 1./3.);
    stepSize = length/nStep;
    yTemp = yInit;
    for (i=0; i<nStep; i++) {
      yTemp = yMap1(yTemp, stepSize/2/(1+a));
      yTemp = yMap2(yTemp, stepSize/(1+a));
      yTemp = yMap1(yTemp, stepSize*a/2/(1+a));
      yTemp = yMap2(yTemp, stepSize*(a-1)/(1+a));
      yTemp = yMap1(yTemp, stepSize*a/2/(1+a));
      yTemp = yMap2(yTemp, stepSize/(1+a));
      yTemp = yMap1(yTemp, stepSize/2/(1+a));
    }
    return yTemp;
  }
}

Vector Symplectic::integrate( int choice, ODE& yMap1, ODE& yMap2, Vector yInit,
  double t0, double t1, double /* minStepSize */, double /* eps */ ){
  return symplectic4(yMap1, yMap2, yInit, t0, t1);
  if (choice == 1){
    return symplectic2(yMap1, yMap2, yInit, t0, t1);
  }
  else if (choice == 2){
    return symplectic4(yMap1, yMap2, yInit, t0, t1);
  }
  else{
    (*pcerr) << "Symplectic::integrate: Invalid choice of integrator." << endl;
    return 0;
  }
}
