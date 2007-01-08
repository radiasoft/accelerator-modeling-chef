////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       CHEFInterpolator.h                                                             //
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

#ifndef CHEFINTERPOLATOR_H
#define CHEFINTERPOLATOR_H

#include <Interpolator.h>


class CHEFInterpolator {

  public:
  
  QwtDoublePoint operator()( QwtDoublePoint const* data_begin, QwtDoublePoint const* data_end, double x);
 ~CHEFInterpolator() {}
 
};


#endif //CHEFINTERPOLATOR_H
