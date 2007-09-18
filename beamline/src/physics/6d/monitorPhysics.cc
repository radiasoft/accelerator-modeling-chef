/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      monitorPhysics.cc
******  Version:   3.0
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
****** April 2007  ostiguy@fnal.gov
****** - efficiency improvements: avoid unnecessary copying of Particle/JetParticle State.
******
****** August 2007 ostiguy@fnal.gov
******
****** - templatized propagators 
****** - bunch propagation
****** - added systematic and random errors
******
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/JetParticle.h>
#include <beamline/monitor.h>

using namespace std;


void monitor::localPropagate( Particle& p )
{

  Vector& state = p.State();

  if( length_ > 0.0 ) {

    double realLength = length_;
    double realCt     = ctRef_;
    length_          *= getDriftFraction();
    ctRef_           *= getDriftFraction();

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    xpos_  = state[0];
    ypos_  = state[1];
    npx_   = state[3];
    npy_   = state[4];

    length_ = realLength - length_;
    ctRef_ = realCt - ctRef_;
    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length_ = realLength;
    ctRef_ = realCt;
  } 

  else {
    xpos_  = state[0];
    ypos_  = state[1];
    npx_   = state[3];
    npy_   = state[4];
  }    
  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void monitor::localPropagate( JetParticle& p ) 
{

  Mapping& state = p.State();

  if( length_ > 0.0 ) {

   double realLength = length_;
   double realCt     = ctRef_;
   length_          *= getDriftFraction();
   ctRef_           *= getDriftFraction();

   bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
   xpos_  = state[0].standardPart();
   ypos_  = state[1].standardPart();
   npx_   = state[3].standardPart();
   npy_   = state[4].standardPart();
   
   length_ = realLength - length_;
   ctRef_  = realCt - ctRef_;
 
   bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length_ = realLength;
    ctRef_  = realCt;
  } 

  else {
    xpos_  = state[0].standardPart();
    ypos_  = state[1].standardPart();
    npx_   = state[3].standardPart();
    npy_   = state[4].standardPart();
  }
    
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::localPropagate( Particle& p ) 
{
  Vector& state = p.State();

  if( length_ > 0.0 ) {

    double realLength  = length_;
    double realCt      = ctRef_;

    length_    *= getDriftFraction(); 
    ctRef_     *= getDriftFraction();

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    xpos_  = state[0];
    npx_   = state[3];
    
    length_ = realLength - length_;
    ctRef_  = realCt - ctRef_;

    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length_ = realLength;
    ctRef_ = realCt;
  
 } 
 else {
    xpos_  = state[0];
    npx_   = state[3];
 }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::localPropagate( JetParticle& p ) 
{

  Mapping& state = p.State();

  if( length_ > 0.0 ) {

    double realLength = length_;
    double realCt     = ctRef_;
    length_          *= getDriftFraction();
    ctRef_           *= getDriftFraction();

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    xpos_  = state[0].standardPart();
    npx_   = state[3].standardPart();

    
    length_ = realLength - length_;
    ctRef_  = realCt - ctRef_;

    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length_ = realLength;
    ctRef_  = realCt;
  } 

  else {
    xpos_ = state[0].standardPart();
    npx_  = state[3].standardPart();
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::localPropagate( Particle& p ) 
{
  Vector& state = p.State();

  if( length_ > 0.0 ) {

    double realLength = length_;
    double realCt     = ctRef_;
    length_          *= getDriftFraction();
    ctRef_           *= getDriftFraction();

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    ypos_  = state[1];
    npy_   = state[4];
    
    length_ = realLength - length_;
    ctRef_ = realCt - ctRef_;

    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length_ = realLength;
    ctRef_  = realCt;
  } 

  else {

    ypos_  = state[1];
    npy_   = state[4];

  }    

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::localPropagate( JetParticle& p ) 
{
  Mapping& state = p.State();

  if( length_ > 0.0 ) {

    double realLength = length_;
    double realCt     = ctRef_;
    length_          *= getDriftFraction();
    ctRef_           *= getDriftFraction();

    bmlnElmnt::localPropagate( p );   // Drift through half the length.
    
    ypos_  = state[1].standardPart();
    npy_   = state[4].standardPart();
    
    length_  = realLength - length_;
    ctRef_   = realCt - ctRef_;

    bmlnElmnt::localPropagate( p ); // Drift through the other half.
    
    length_ = realLength;
    ctRef_  = realCt;
  } 

  else {
    ypos_ = state[1].standardPart();
    npy_  = state[4].standardPart();
  }
  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::localPropagate( ParticleBunch& b ) 
{ 

  double xpos = 0.0;
  double ypos = 0.0;

  for( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    localPropagate(*it); 
    xpos +=  xpos_; 
    ypos +=  ypos_; 
  }
  
  // record the bunch position
 
  xpos_ /= b.size();
  ypos_ /= b.size();

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void hmonitor::localPropagate( ParticleBunch& b ) 
{ 

  double xpos = 0.0;

  for( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    localPropagate(*it); 
    xpos +=  xpos_; 
  }
  
  // record the bunch position
 
  xpos_ /= b.size();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void vmonitor::localPropagate( ParticleBunch& b ) 
{ 

  double ypos = 0.0;

  for( ParticleBunch::iterator it = b.begin(); it != b.end(); ++it ) 
  {
    localPropagate(*it); 
    ypos +=  ypos_; 
  }
  
  // record the bunch position
 
  ypos_ /= b.size();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
