/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Ion.h
******                                                                
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Author:    Leo Michelotti  (michelotti@fnal.gov)
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Jun 2015       michelotti@fnal.gov
******  - Original version.
******  
**************************************************************************
*************************************************************************/

#ifndef ION_H
#define ION_H


#include <beamline/Particle.h>

class JetParticle;
class JetIon;

class DLLEXPORT Ion : public Particle 
{
 public:

  // ctors
  Ion( int Z, int A, int E = 0, double dM = 0.0 );
    // Z = atomic number = number of protons
    // A = mass number   = number of nuclons
    // E = number of (tightly) bound electrons
    // ... (total charge) = ( Z - E)*(proton charge)
    // dM = mass defect  [GeV/c^2]
    // ... ion's mass = Z*(proton mass) + (A - Z)*(neutron mass) + E*(electron mass) + dM
  Ion( double M, double Q );
    // M = ion mass      [GeV/c^2]
    // Q = ion charge    [Coulombs]
  Ion( Ion const& );

  ~Ion();

  // Other copying and conversion methods
  explicit Ion( JetIon const& );
  Ion& operator=( Ion const& );
  Ion* Clone( void* p=0 ) const;
};

#endif // ION_H
