#ifndef SIMPSONSRULE_H
#define SIMPSONSRULE_H

#include "AbsIntegrator.h"

class Simpson : public AbsIntegrator<double>::Method
{
public:
  Simpson();

  double integrate( double /* lower bound: t_i */,
                    double /* upper bound: t_f */ ) const;
};

#endif // SIMPSONSRULE_H
