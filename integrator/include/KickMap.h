#ifndef KICKMAP_H
#define KICKMAP_H

#include "ODE.h"

class KickMap : public ODE {

 private:
 public:
   KickMap();
   virtual ~KickMap();
   Vector operator() (const Vector&, double);
};

#endif /// KICKMAP_H
