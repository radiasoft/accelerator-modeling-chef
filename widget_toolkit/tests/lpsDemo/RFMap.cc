////////////////////////////////////////////////////////////
// 
// File:          RFMap.cc
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

#include <math.h>

#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>

#include <RFMap.h>

#define I_PHI 0
#define I_E   1
#define DIM   2

using namespace AESOP_domain;

RFMap::RFMap()
{
  _multiplier = 0.3;
  _phi_s  = _multiplier*(M_PI/2.0);
  _sps    = sin(_phi_s);
  _E_s    = 100.0;

  _eV     = 0.001;
  _h      = 100;

  _factor = M_TWOPI*_h;
}

RFMap::~RFMap()
{
}


void RFMap::operator()( Vector& in_out ) const
{
  static double u;
  in_out(I_PHI) += _factor*( ( _E_s / (_E_s + in_out(I_E) ) ) - 1.0 );
  u = in_out(I_PHI) + _phi_s;

  while( u < - M_PI ) { u += M_TWOPI; }
  while( M_PI <= u  ) { u -= M_TWOPI; }

  in_out(I_E) += _eV*( sin(u) - _sps );
}


void RFMap::operator()( JetVector& in_out ) const
{
  Jet u;
  in_out(I_PHI) += _factor*( ( _E_s / (_E_s + in_out(I_E) ) ) - 1.0 );
  u = in_out(I_PHI) + _phi_s;

  // while( u < - M_PI ) { u += M_TWOPI; }
  // while( M_PI <= u  ) { u -= M_TWOPI; }

  in_out(I_E) += _eV*( sin(u) - _sps );
}


void RFMap::setMultiplier( double x )
{
  _multiplier = x;
  _phi_s  = _multiplier*(M_PI/2.0);
  _sps    = sin(_phi_s);
}
