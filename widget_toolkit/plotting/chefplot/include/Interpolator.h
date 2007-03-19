////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       Interpolator.h                                                                 //
//                                                                                            //
// AUTHOR(S):  Jean-Francois Ostiguy                                                          // 
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
//             Accelerator Division / Accelerator Integration Dept                            //
//             Fermi National Laboratory, Batavia, IL                                         //
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
// DATE:       September 2004                                                                 //
//                                                                                            //
// COPYRIGHT: Universities Research Association                                               //
//                                                                                            //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <OrderedPair.h>
#include <qwt5/qwt_double_rect.h>


class Interpolator {

  public:
  
  virtual OrderedPair operator()( OrderedPair const* data_begin, OrderedPair const* data_end, double x) = 0;
  virtual ~Interpolator() = 0;

};

inline Interpolator::~Interpolator() {}

#endif // INTERPOLATOR_H


