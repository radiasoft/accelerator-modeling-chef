#include "displayinfo.h"
#include <algorithm>
#include <iostream>


bool OrderedPairLowerEqual(const OrderedPair& p1, const OrderedPair& p2) {

  return (p1.x <= p2.x);
  
}

OrderedPair Curve::DefaultLinearInterpolator::operator()( const OrderedPair* beg_pair, const OrderedPair* end_pair, double x) {

  OrderedPair op;
  const OrderedPair* pptr;

  op.x = x;
  op.y = 0;

  const OrderedPair* hi_ptr = std::upper_bound(beg_pair, end_pair, op, OrderedPairLowerEqual);

  if ( hi_ptr == beg_pair) {  //  outside of interpolation interval (below)
  
    pptr =  beg_pair+1;
    op.y =  beg_pair->y + ( x - beg_pair->x ) * ( pptr->y -  beg_pair->y)/(pptr->x -  beg_pair->x);

    return op;
  };
  
  if ( hi_ptr == end_pair) {  //  outside of interpolation interval (above)  

    pptr =  end_pair-1;
    op.y =  end_pair->y  +  (x - end_pair->x)  *  ( end_pair->y  -  pptr->y)/ (end_pair->x  - pptr->x);

    return op;
  };
  
  double xhi = hi_ptr->x;
  double yhi = hi_ptr->y;
  
  double xlo = (--hi_ptr)->x;
  double ylo = (  hi_ptr)->y;

  op.y = ylo + ((yhi-ylo)/(xhi-xlo))*(x-xlo);

  return op;

}


