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
******  Authors:    Leo Michelotti         michelotti@fnal.gov
******              Jean-Francois Ostiguy  ostiguy@fnal.gov
******
**************************************************************************
*************************************************************************/


#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/lambertson.h>
#include <beamline/LambertsonPropagators.h>


namespace {

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;


template<typename Particle_t>
void propagate( thinLamb& elm, Particle_t& p);
 

template<>
void propagate( thinLamb& elm,    Particle& p) 
{ 

  typedef PropagatorTraits<Particle>::State_t       State_t;
  typedef PropagatorTraits<Particle>::Component_t   Component_t;

  ++(elm.turnNumber_);
 
  State_t& state = p.State();

  bool is_finite = true;
  int i = 0;
  while( is_finite && (i < 6) ) {
    if( 0 == finite( state[i] ) ) {
      cout << "LAMBERTSON: Will remove particle: infinite state detected: "
           << state
           << " : with tag: " << p.getTag()
           << endl;
      p.setTag( std::string("KILL") );
      is_finite = false;
    }
    ++i;
  }

// FIXME 
// Belows are temporary disabled

#ifdef USE_xSeptum
/*
  if( is_finite ) {
    if( std::string::npos != (p.getTag()).find("KICK") ) {
      cout << "LAMBERTSON: Will remove particle: state = "
           << state
           << " : with tag: " << p.getTag()
           << endl;
      p.setTag( std::string("KILL") );
    }
  }
*/

  std::vector<double> TrState(6);
  std::vector<double> RefState(6);

  double xSeptum = elm.getSeptum();

  //for( int i= 0; i < 6; ++i) { 
  //  TrState[i] = state[i];
  //}

  // Now figure out if the particle is really over the magnet septum.

  if ( state[0] < - xSeptum ) {
    p.setTag( std::string(" / LAMBERTSON: EXTRACTED / ") );

    // Transfer the particle's state for the extraction beamline

    //for ( int i = 0; i < 6; ++i ) TrState[i] -= RefState[i];
    //ExtFlag = 1;
  }

  // Tranfer the particle in the extraction region into a different
  // ParticleBunch and remove it from the main bunch.

  //if (ExtFlag) {
  //  if (align) this->leaveLocalFrame( *pCur );
  //  ExtPB->append(*pCur);
  //  PB.remove(*pCur);
  //}

  //if (ExtPB->size() != 0) {
  //  ExtBeamline->propagate(*ExtPB);
  //}

#endif

} // empty specialization 


template<>
void propagate( thinLamb& elm, JetParticle& p) 
{ 
  typedef PropagatorTraits<JetParticle>::State_t       State_t;
  typedef PropagatorTraits<JetParticle>::Component_t   Component_t;
 
  State_t& state = p.State(); 
} // empty specialization 


#if 0
template<>
void propagate( thinLamb& elm, JetParticleBunch& b)
{  
  typedef PropagatorTraits<JetParticle>::State_t       State_t;
  typedef PropagatorTraits<JetParticle>::Component_t   Component_t;
 
  // NOT DEFINED -- FIX ME ! 
}
#endif


#if 0
template<>
void propagate( thinLamb& elm, ParticleBunch& PB )
{
  typedef PropagatorTraits<Particle>::State_t       State_t;
  typedef PropagatorTraits<Particle>::Component_t   Component_t;

  if ( PB.empty() ) { return; }  // no more particles

  cout << "lambertson propagator is called" << endl;
  
  //++turnNumber_;
  ++(elm.turnNumber_);

  for( ParticleBunch::iterator it = PB.begin();
       it != PB.end();
       ++it ) 
  {
    State_t& state = (*it).State();

    bool is_finite = true;
    int i = 0;
    while( is_finite && (i < 6) ) {
      if( 0 == finite( state[i] ) ) {
        cout << "LAMBERTSON: Will remove particle: infinite state detected: " 
             << state
             << " : with tag: " << (*it).getTag()
             << endl;
        (*it).setTag( std::string("KILL") );
        is_finite = false;
      }
      ++i;
    }

    if( is_finite ) {
      if( std::string::npos != ((*it).getTag()).find("KICK") ) {
        cout << "LAMBERTSON: Will remove particle: state = "
             << state
             << " : with tag: " << (*it).getTag()
             << endl;
        (*it).setTag( std::string("KILL") );
      }
      else if( std::string::npos != ((*it).getTag()).find("SEPTUM") ) {
        cout << "LAMBERTSON: Will remove particle: state = "
             << state
             << " : with tag: " << (*it).getTag()
             << endl;
        (*it).setTag( std::string("LAMBERTSON") );
      }
    }
  }

  PB.remove( thinLamb::toGo );

  #if 0
  // OLD CODE
  // FIXME !
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
  #endif
}
#endif

//----------------------------------------------------------------------------------
// Workaround for gcc < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

template void propagate( thinLamb& elm,        Particle&  p );
template void propagate( thinLamb& elm,     JetParticle&  p );
//template void propagate( thinLamb& elm,  ParticleBunch&  PB );
//template void propagate( thinLamb& elm, JetParticleBunch& b );

#endif
//-----------------------------------------------------------------------------------

} // anonymous namespace


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void thinLamb::Propagator::setup(thinLamb& elm)
{}


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

//void thinLamb::Propagator::operator()( thinLamb& elm,  ParticleBunch& b ) 
//{ 
//  ::propagate(elm,b);
//}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//void thinLamb::Propagator::operator()( thinLamb& elm,  JetParticleBunch& b ) 
//{ 
//  ::propagate(elm,b);
//}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

