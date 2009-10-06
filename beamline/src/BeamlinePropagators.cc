/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BeamlinePropagators.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******                                                                
******  Author: Jean-Francois Ostiguy  ostiguy@fnal.gov
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/BeamlinePropagators.h>

namespace {

template <typename particle_t >
void propagate( beamline const& bml, particle_t& p )
{
 for ( beamline::const_iterator it = bml.begin(); it != bml.end();  ++it ) { 

   (*it)->propagate( p );
 }
} 

} // anonymous namespace

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::Propagator::Propagator()
 : BasePropagator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::Propagator::Propagator(beamline const& elm)
 : BasePropagator(elm)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::Propagator::Propagator( beamline::Propagator const& p)
 : BasePropagator(p)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

beamline::Propagator* beamline::Propagator::clone() const
{
  return  new beamline::Propagator(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void beamline::Propagator::operator()( BmlnElmnt const& elm, ParticleBunch& p ) 
{
  ::propagate( static_cast<beamline const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::Propagator::operator()( BmlnElmnt const& elm, JetParticleBunch& p ) 
{
  ::propagate( static_cast<beamline const&>(elm), p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::Propagator::operator()( BmlnElmnt const& elm, Particle& p ) 
{
  ::propagate( static_cast<beamline const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void beamline::Propagator::operator()( BmlnElmnt const& elm, JetParticle& p ) 
{
  ::propagate( static_cast<beamline const&>(elm),p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void beamline::Propagator::setReferenceTime( double ct)
{ 
  BasePropagator::setReferenceTime(ct);
 
  //(*pcerr) << "*** WARNING ****: Attempt to explicitly set the reference time attribute of a beamline. " << std::endl;
  //(*pcerr) << "*** WARNING ****: This is most likely an error." << std::endl; 
  //(*pcerr) << "*** WARNING ****: Continuing, nonetheless... " << std::endl;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double beamline::Propagator::getReferenceTime()  const
{

  double ct = 0.0; 

#if  0 // FIXME

  for ( beamline::const_iterator it = begin(); it != end(); ++it ) {

    ct += (*it)->getReferenceTime();

  } 
#endif

  return ct; 

}     

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

