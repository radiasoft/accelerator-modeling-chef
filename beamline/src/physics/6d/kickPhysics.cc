/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      kickPhysics.cc
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

#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/kick.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

// =================================================
// vkick propagators
// =================================================

void vkick::localPropagate( Particle& p )
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File: "
             << __FILE__ << ", Line:" << __LINE__
             << "\n*** WARNING *** void vkick::localPropagate( Particle& p )"
                "\n*** WARNING *** -----------------------------------------"
                "\n*** WARNING *** An approximation has been introduced"
                "\n*** WARNING *** as a TEMPORARY bug workaround. It makes"
                "\n*** WARNING *** an extremely small error in the computed"
                "\n*** WARNING *** time coordinate; it \"fixes\" an extremely"
                "\n*** WARNING *** large error in the computed time coordinate."
                "\n*** WARNING ***                                   - lpjm"
                "\n*** WARNING *** "
             << endl;
    firstTime = false;
  }

  Vector& state = p.getState();

  double realLength, realCt;

  if( length_ > 0.0 ) {
    realLength  = length_;
    realCt      = ctRef_;
    length_    /= 2.0;
    ctRef_     /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    state[4] += strength_;
    bmlnElmnt::localPropagate( p );

    length_ = realLength;
    ctRef_  = realCt;
  }
  else {
    state[4] += strength_;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vkick::localPropagate( JetParticle& p )
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File: "
             << __FILE__ << ", Line:" << __LINE__
             << "\n*** WARNING *** void vkick::localPropagate( JetParticle& p )"
                "\n*** WARNING *** -----------------------------------------"
                "\n*** WARNING *** An approximation has been introduced"
                "\n*** WARNING *** as a TEMPORARY bug workaround. It makes"
                "\n*** WARNING *** an extremely small error in the computed"
                "\n*** WARNING *** time coordinate; it \"fixes\" an extremely"
                "\n*** WARNING *** large error in the computed time coordinate."
                "\n*** WARNING ***                                   - lpjm"
                "\n*** WARNING *** "
             << endl;
    firstTime = false;
  }

  Mapping& state = p.getState();

  double realLength, realCt;
  if( length_ > 0.0 ) {
    realLength = length_;
    realCt     = ctRef_;
    length_ /= 2.0;
    ctRef_ /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    ( state ).SetComponent( 4, state(4) + strength_ );
    bmlnElmnt::localPropagate( p );

    length_ = realLength;
    ctRef_ = realCt;
  }
  else {
    ( state ).SetComponent( 4, state(4) + strength_ );
  }
}


// =================================================
// hkick propagators
// =================================================

void hkick::localPropagate( Particle& p )
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File: "
             << __FILE__ << ", Line:" << __LINE__
             << "\n*** WARNING *** void hkick::localPropagate( Particle& p )"
                "\n*** WARNING *** -----------------------------------------"
                "\n*** WARNING *** An approximation has been introduced"
                "\n*** WARNING *** as a TEMPORARY bug workaround. It makes"
                "\n*** WARNING *** an extremely small error in the computed"
                "\n*** WARNING *** time coordinate; it \"fixes\" an extremely"
                "\n*** WARNING *** large error in the computed time coordinate."
                "\n*** WARNING ***                                   - lpjm"
                "\n*** WARNING *** "
             << endl;
    firstTime = false;
  }

  Vector& state = p.getState();

  double realLength, realCt;
  if( length_ > 0.0 ) {
    realLength = length_;
    realCt     = ctRef_;
    length_ /= 2.0;
    ctRef_ /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    state[3] += strength_;
    bmlnElmnt::localPropagate( p );

    length_ = realLength;
    ctRef_ = realCt;
  }
  else {
    state[3] += strength_;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hkick::localPropagate( JetParticle& p )
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File: "
             << __FILE__ << ", Line:" << __LINE__
             << "\n*** WARNING *** void hkick::localPropagate( JetParticle& p )"
                "\n*** WARNING *** -----------------------------------------"
                "\n*** WARNING *** An approximation has been introduced"
                "\n*** WARNING *** as a TEMPORARY bug workaround. It makes"
                "\n*** WARNING *** an extremely small error in the computed"
                "\n*** WARNING *** time coordinate; it \"fixes\" an extremely"
                "\n*** WARNING *** large error in the computed time coordinate."
                "\n*** WARNING ***                                   - lpjm"
                "\n*** WARNING *** "
             << endl;
    firstTime = false;
  }

  Mapping& state = p.getState();

  double realLength, realCt;

  if( length_ > 0.0 ) {
    realLength = length_;
    realCt     = ctRef_;
    length_   /= 2.0;
    ctRef_    /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    state.SetComponent( 3, state(3) + strength_ );
    bmlnElmnt::localPropagate( p );

    length_ = realLength;
    ctRef_ = realCt;
  }
  else {
    state.SetComponent( 3, state(3) + strength_ );
  }
}

// =================================================
// kick propagators
// =================================================

void kick::localPropagate( Particle& p ) 
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File: "
             << __FILE__ << ", Line:" << __LINE__
             << "\n*** WARNING *** void kick::localPropagate( Particle& p )"
                "\n*** WARNING *** -----------------------------------------"
                "\n*** WARNING *** An approximation has been introduced"
                "\n*** WARNING *** as a TEMPORARY bug workaround. It makes"
                "\n*** WARNING *** an extremely small error in the computed"
                "\n*** WARNING *** time coordinate; it \"fixes\" an extremely"
                "\n*** WARNING *** large error in the computed time coordinate."
                "\n*** WARNING ***                                   - lpjm"
                "\n*** WARNING *** "
             << endl;
    firstTime = false;
  }

  Vector& state = p.getState();

  double realLength, realCt;
  if( length_ > 0.0 ) {
    realLength = length_;
    realCt     = ctRef_;
    length_ /= 2.0;
    ctRef_ /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    state[3] += horizontalKick_;
    state[4] += verticalKick_;
    bmlnElmnt::localPropagate( p );

    length_ = realLength;
    ctRef_ = realCt;
  }
  else {
    state[3] += horizontalKick_;
    state[4] += verticalKick_;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void kick::localPropagate( JetParticle& p ) 
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File: "
             << __FILE__ << ", Line:" << __LINE__
             << "\n*** WARNING *** void kick::localPropagate( JetParticle& p ) "
                "\n*** WARNING *** -----------------------------------------"
                "\n*** WARNING *** An approximation has been introduced"
                "\n*** WARNING *** as a TEMPORARY bug workaround. It makes"
                "\n*** WARNING *** an extremely small error in the computed"
                "\n*** WARNING *** time coordinate; it \"fixes\" an extremely"
                "\n*** WARNING *** large error in the computed time coordinate."
                "\n*** WARNING ***                                   - lpjm"
                "\n*** WARNING *** "
             << endl;
    firstTime = false;
  }

  Mapping& state = p.getState();

  double realLength, realCt;
  if( length_ > 0.0 ) {
    realLength = length_;
    realCt     = ctRef_;
    length_ /= 2.0;
    ctRef_ /= 2.0;

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    state.SetComponent( 3, state(3) + horizontalKick_ );
    state.SetComponent( 4, state(4) + verticalKick_   );
    bmlnElmnt::localPropagate( p );

    length_ = realLength;
    ctRef_ = realCt;
  }
  else {
    state.SetComponent( 3, state(3) + horizontalKick_ );
    state.SetComponent( 4, state(4) + verticalKick_   );
  }
}
