#ifndef DRIFTMAP_H
#define DRIFTMAP_H

#include "ODE.h"

class DriftMap : public ODE {

 private:
 public:
   DriftMap();
   virtual ~DriftMap();
   Vector operator() (const Vector&, double);
};

#endif /// DRIFTMAP_H
