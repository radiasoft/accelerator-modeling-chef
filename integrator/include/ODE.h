#ifndef ODE_H
#define ODE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Vector.h>

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
