#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "RungeKutta.h"

#ifdef __VISUAL_CPP__
using std::cout;
using std::endl;
#endif

/// ******************************************
///   class RungeKutta
/// ******************************************

RungeKutta::RungeKutta() : Integrator() {
  nStep = 10;
}

RungeKutta::~RungeKutta() {
}

Vector RungeKutta::rk4OneStep( ODE& yPrime, Vector yInit, double t0, 
  double stepSize ) {
  /// yInit: Starting values of the ODE
  /// t0: Starting value of the independent variable
  /// stepSize: Step size
  /// yPrime: right hand side of the ODE

  double halfStepSize;
  double sixthStepSize;
  int dim;

  dim = yInit.Dim();
  Vector yTemp(dim);
  Vector k1(dim);
  Vector k2(dim);
  Vector k3(dim);
  Vector k4(dim);
    
  halfStepSize = stepSize/2;
  sixthStepSize = stepSize/6;
  k1 = yPrime(yInit, t0); /// First step
  k2 = yPrime(yInit+halfStepSize*k1, t0+halfStepSize); /// Second step
  k3 = yPrime(yInit+halfStepSize*k2, t0+halfStepSize); /// Third step
  k4 = yPrime(yInit+stepSize*k3, t0+stepSize); /// Fourth step
  yTemp = yInit+sixthStepSize*(k1+2.0*(k2+k3)+k4);
  return yTemp;
}

Vector RungeKutta::rk4( ODE& yPrime, Vector yInit, double t0, double t1 ) {
  /// yInit: Starting values of the ODE
  /// t0: Starting value of the independent variable
  /// yPrime: right hand side of the ODE

  if ( yInit.Dim() != yPrime.getDim() ) {
    cerr << "Error in RungeKutta::rk4OneStep: Dimensions don't match." << endl;
    return 0;
  }
  else {
    int i;
    double stepSize;
    double t;
    int dim;

    dim = yInit.Dim();
    Vector yTemp(dim);
    
    stepSize = (t1-t0)/nStep;
    yTemp = yInit;
    for (i=0; i<nStep; i++) {
      t = t0 + i*stepSize;
      yTemp = rk4OneStep(yPrime, yTemp, t, stepSize);
    }
    return yTemp;
  }
}

Vector RungeKutta::rk4ASC( ODE& yPrime, Vector yInit, double t0, double t1, 
  double minStepSize, double eps ) {
  /// yinit: Starting values of the ODE
  /// t0: Starting value of the independent variable
  /// minStepSize: minimum step size allowed
  /// eps: accuracy required
  /// yPrime: right hand side of the ODE

  if ( yInit.Dim() != yPrime.getDim() ) {
    cerr << "Error in RungeKutta::rk4OneStep: Dimensions don't match." << endl;
    return 0;
  }
  else {
    int dim;

    dim = yInit.Dim();
    Vector yTemp(dim);
    Vector yTemp1(dim);
    Vector yTemp2(dim);
    double length;
    double stepSize;
    double halfStepSize;
    double epsTemp;
    double t;
    int i;
  
    length = t1-t0; 
    yTemp = yInit;
  
    t = t0;
    while (t<length+t0) {
      if ((length+t0-t)<length/nStep) stepSize = length+t0-t;
      else stepSize = length/nStep;
      epsTemp = 2*eps;
      while ((epsTemp>=eps)&&(stepSize>=minStepSize)) {
        yTemp1 = rk4OneStep(yPrime, yTemp, t, stepSize);
        halfStepSize = stepSize/2;
        yTemp2 = rk4OneStep(yPrime, yTemp, t, halfStepSize);
        yTemp2 = rk4OneStep(yPrime, yTemp2, t+halfStepSize, halfStepSize);
        epsTemp = 0;
        for (i=0; i<dim; i++) {
          epsTemp += fabs(yTemp1(i)-yTemp2(i));
        }
        if (epsTemp>=eps) stepSize = halfStepSize;
      }
      yTemp = yTemp1;
      t += stepSize;
      cout << stepSize << ", " << t << endl;
      if (stepSize<minStepSize) {
        cerr << "Warning from RungeKutta::RK4ASC: Minimum step size reached.";
        cerr << endl;
        cerr << "Relative error is " << epsTemp << endl;
      }
    }
    return yTemp;
  }
}

Vector RungeKutta::integrate( int choice, ODE& yPrime, ODE& /* yMap2 */, 
  Vector yInit, double t0, double t1, double minStepSize, double eps ){
  if (choice == 1){
    return rk4(yPrime, yInit, t0, t1);
  }
  else if (choice == 2){
    return rk4ASC(yPrime, yInit, t0, t1, minStepSize, eps);
  }
  else{
    cerr << "RungeKutta::integrate: Invalid choice of integrator." << endl;
    return 0;
  }
}
