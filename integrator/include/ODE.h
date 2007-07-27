#ifndef ODE_H
#define ODE_H

#include <basic_toolkit/VectorD.h>

class ODE
{
 private:

 protected:
   int dimen;

 public:
   ODE() : dimen(1) {}
   virtual ~ODE() {}

   int getDim() { return dimen; }
   virtual Vector operator() (const Vector&, double) = 0;
};

#endif // ODE_H
