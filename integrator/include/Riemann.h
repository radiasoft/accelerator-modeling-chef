#ifndef RIEMANNRULE_H
#define RIEMANNRULE_H

#include "AbsIntegrator.h"


class Riemann : public AbsIntegrator<double>::Method
{
public:
  Riemann();
  double integrate( double /* lower bound: t_i */,
                    double /* upper bound: t_f */ ) const;
  void setLower();
  void setUpper();

private:
  bool _lower;
};

#endif // RIEMANNRULE_H
