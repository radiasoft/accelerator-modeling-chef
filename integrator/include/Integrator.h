#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <cmath>
#include <integrator/ODE.h>
#include <basic_toolkit/VectorD.h>

class Integrator{

 private:

 protected:
   int nStep;

 public:
   Integrator();
   virtual ~Integrator();

   void setNStep(int n){
     nStep = n;
   }
   double getNStep(){
     return nStep;
   }

  virtual Vector integrate( int, ODE&, ODE&, Vector, double, double, double, 
                          double ) = 0; 
};

#endif /// INTEGRATOR_H
