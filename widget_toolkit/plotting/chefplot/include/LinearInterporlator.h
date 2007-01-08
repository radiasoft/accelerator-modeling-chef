#ifndef LINEAR_INTERPOLATOR
#define LINEAR_INTERPOLATOR

#include "displayinfo.h"

class LinearInterpolator:public Interpolator {

  public:

  LinearInterpolator ( );

  OrderedPair operator()(const OrderedPair* beg_pair, const OrderedPair* end_pair, double s);
  
};


#endif
