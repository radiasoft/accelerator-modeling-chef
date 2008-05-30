////////////////////////////////////////////////////////////
// 
// File:          RFMap.h
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Wednesday. October 24, 2001.  (original version)
// * Written for a demonstration of perturbation theory in
//   longitudinal phase space.
// 
// August, 2002 - March, 2003.
// * Modified and used while studying tomography
//   in longitudinal phase space.
// 
// Thursday. May 29, 2008.       (this file)
// 
////////////////////////////////////////////////////////////

#ifndef RFMAP_H
#define RFMAP_H

#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/JetVector.h>

namespace AESOP_domain
{

class RFMap
{
  public:
    RFMap();
    ~RFMap();
  
    void   setMultiplier( double );
    double getPhi_s();

    void operator()( Vector&    ) const;
    void operator()( JetVector& ) const;

  private:
    double _multiplier;
    double _phi_s;
    double _sps;
    double _E_s;
    double _factor;
    int    _h;
    double _eV;
};


inline double RFMap::getPhi_s()
{
  return _phi_s;
}


};  // end namespace AESOP

#endif // RFMAP_H
