#ifndef RUNGEKUTTA_H
#define RUNGEKUTTA_H

#include <integrator/Integrator.h>

class RungeKutta : public Integrator {

 private:
   Vector rk4OneStep( ODE&, Vector, double, double );
   Vector rk4( ODE&, Vector, double t0 = 0, double t1 = 1 );
   Vector rk4ASC( ODE&, Vector, double t0 = 0, double t1 = 1, 
                  double minStepSize = 1e-2, double eps = 1e-6 );
 public:
   RungeKutta();
   virtual ~RungeKutta();
   Vector integrate( int choice, ODE&, ODE&, Vector, double t0 = 0, 
                     double t1 = 1, double minStepSize = 1e-2, 
                     double eps = 1e-6 ); 
};

#endif /// RUNGEKUTTA_H
