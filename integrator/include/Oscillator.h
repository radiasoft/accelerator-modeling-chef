#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <integrator/ODE.h>

class Oscillator : public ODE {

 private:
 public:
   Oscillator();
   virtual ~Oscillator();
   Vector operator() (const Vector&, double);
};

#endif /// OSCILLATOR_H
