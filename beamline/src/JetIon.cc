/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      JetIon.cc
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

#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/Ion.h>
#include <beamline/JetIon.h>


JetIon::JetIon( int Z, int A, int E, double dM )
: JetParticle( PH_NORM_mp*double(Z) + PH_NORM_mn*double(A-Z) + PH_NORM_me*double(E) + dM, 
               PH_NORM_mp*double(Z) + PH_NORM_mn*double(A-Z) + PH_NORM_me*double(E) + dM )
{
  q_ = PH_MKS_e*( double(Z) - double(E) );
  SetReferenceMomentum( 0.0 );
}


JetIon::JetIon( double M, double Q )
: JetParticle( M, M )
{
  q_ = Q;
  SetReferenceMomentum( 0.0 );
}


JetIon::JetIon( JetIon const& u )
: JetParticle(u) 
{}


JetIon::JetIon( Ion const& u, EnvPtr<double> const& pje )
: JetParticle(u, pje) 
{} 
 

JetIon::~JetIon() 
{}


JetIon& JetIon::operator=( JetIon const& u ) 
{
 if( &u == this ) { return *this ;}
 JetParticle::operator=(u);
 return *this;
}


JetIon* JetIon::Clone( void* p ) const 
{ 
  return p ? new(p) JetIon( *this ) : new JetIon( *this );
}