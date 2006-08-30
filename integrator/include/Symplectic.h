#ifndef SYMPLECTIC_H
#define SYMPLECTIC_H

#include <integrator/Integrator.h>

class Symplectic : public Integrator {

 private:
   Vector symplectic2( ODE&, ODE&, Vector, double t0 = 0, double t1 = 1 );
     /// Second-order symplectic integrator
   Vector symplectic4( ODE&, ODE&, Vector, double t0 = 0, double t1 = 1 );
     /// Fourth-order symplectic integrator
 public:
   Symplectic();
   virtual ~Symplectic();
   Vector integrate( int choice, ODE&, ODE&, Vector, double t0 = 0, 
                     double t1 = 1, double minStepSize = 1e-2, 
                     double eps = 1e-6 );
};

#endif /// SYMPLECTIC_H
