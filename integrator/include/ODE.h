#ifndef ODE_H
#define ODE_H

#ifndef __VISUAL_CPP__
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include <math.h>

#include <basic_toolkit/VectorD.h>

class ODE{

 private:

 protected:
   int dimen;

 public:
   virtual ~ODE();

   int getDim(){
     return dimen;
   }
   virtual Vector operator() (const Vector&, double) = 0;
};

#endif /// ODE_H
