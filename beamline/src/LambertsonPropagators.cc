/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LambertsonPropagators.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/lambertson.h>
#include <beamline/LambertsonPropagators.h>


namespace {

template<typename Particle_t>
void propagate( thinLamb& elm, Particle_t& p);
 
template<>
void propagate( thinLamb& elm,    Particle& p) { } // empty specialization 

template<>
void propagate( thinLamb& elm, JetParticle& p) { } // empty specialization 

template<>
void propagate( thinLamb& elm, ParticleBunch& p)
{

  /********************************************* 

  if ( !ExtBeamline) return; 

  std::vector<double> TrState(6);
  std::vector<double> dummy(6);

  ParticleBunch ExtPB; 

   
  if ( PB.empty()  ) return; // no more particles

  bool ExtFlag = false;


  do {
    
   ExtFlag = 0;

   ParticleBunch::iterator it_cur  = PB.begin();
   ParticleBunch::iterator it_next = it_cur + 1;

    for( int i= 0; i < 6; ++i) { 
      TrState[i] = pCur->state[i];
    }

    // Now figure out if the particle is really over the magnet septum.
 
   if ( it_cur->State()[0] > xSeptum ) {
      for ( i = 0; i < 6; i++ ) TrState[i] -= RefState[i];
      ExtFlag = 1;
    }

    for ( i = 0; i < 6; i++ )  pCur->state[i] = TrState[i];

    pNxt =  (Particle*)getNext();

    // Tranfer the particle in the extraction region into a different
    // ParticleBunch and remove it from the main bunch.

    if (ExtFlag) {
      if (align) this->leaveLocalFrame( *pCur );
      ExtPB->append(*pCur);
      PB.remove(pCur);
    }
  } while ( (pCur = pNxt) );

  if (ExtPB->size() != 0) {
    ExtBeamline->propagate(*ExtPB);
  }

  *******************/
}

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( thinLamb& elm,    Particle& p );
template void propagate( thinLamb& elm, JetParticle& p );

#endif
//-----------------------------------------------------------------------------------

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::Propagator::operator()( thinLamb& elm,    Particle& p ) 
{ 
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::Propagator::operator()( thinLamb& elm, JetParticle& p ) 
{ 
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinLamb::Propagator::operator()( thinLamb& elm,  ParticleBunch& p ) 
{ 
  ::propagate(elm,p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

