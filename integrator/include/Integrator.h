#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#ifndef __VISUAL_CPP__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include <math.h>

#include "ODE.h"
#include "VectorD.h"

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
