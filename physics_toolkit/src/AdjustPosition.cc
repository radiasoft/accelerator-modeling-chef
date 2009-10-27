/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      AdjustPosition.cc
******  Version:   1.1
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
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
**************************************************************************
*************************************************************************/

#include <basic_toolkit/GenericException.h>
#include <beamline/BmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/Alignment.h>

using namespace std;

int AdjustPosition( BmlnElmnt* p_be, JetParticle const& arg_jp, char )
{
  int ret   = 0;

  // --- Must initialize these variables before the goto -----
  // --- (especially for g++ compiler ) ----------------------

  JetParticle jetparticle(arg_jp);
  Particle    particle(arg_jp);

  double x_i  = particle.x();
  double xp_i = particle.npx();

  Vector inState(particle.state().Dim());

  int i = 0;

  Alignment v( p_be->alignment() );
  // ---------------------------------------------------------

  // --- Set up indices -----------------------------

  static double x    = arg_jp.xIndex();
  static double y    = arg_jp.yIndex();
  static double cdt  = arg_jp.cdtIndex();
  static double xp   = arg_jp.npxIndex();
  static double yp   = arg_jp.npyIndex();
  static double dpop = arg_jp.ndpIndex();

  bool firstTime = true;

  if( firstTime ) {
    firstTime = false;
  }
  // -----------------------------------------------

  // --- Check for forbidden object types. -----
  if( 0 == strcmp( p_be->Type(), "sbend" ) || 
      0 == strcmp( p_be->Type(), "Slot"  ) || 
      0 == strcmp( p_be->Type(), "drift" ) ||   
      0 == strcmp( p_be->Type(), "thinrfcavity" )
    ) 
  {
    cerr << "*** ERROR ***                                        \n"
            "*** ERROR ***  AdjustPositionHorizontally            \n"
            "*** ERROR ***  The beamline element argument cannot  \n"
            "*** ERROR ***  be a " << p_be->Type() << ".          \n"
            "*** ERROR ***                                        \n"
         << endl;
    ret = 1;
    goto end;
  }
  // ------------------------------------------


  inState[x]  = x_i;
  inState[xp] = xp_i;

  double f, m, z;

  // Initialize the derivative...

  jetparticle.setState( inState );
  p_be->propagate( jetparticle );

  m =  jetparticle.state().jacobian()[xp][x];
  if( fabs(m) < 1.0e-12 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "int AdjustPosition( BmlnElmnt* p_be, const JetParticle& arg_jp, char )", 
           "Horrible, inexplicable error: multi-valued solution is suspected." ) );
  }
  m = 1.0 / m;

  // Initialize the difference ...
  z          = x_i;
  inState[x] = z;
  particle.state()= inState;
  p_be->propagate( particle );
  f =  (particle.state())[xp] + xp_i;

  while( ( i < 5 ) || ( ( i < 15 ) && (fabs(f) > 1.0e-9) ) ) 
  {
    i++;

    // One Newton's step ...
    z -= m*f;
    inState[x]  = z;

    // Recalculate inverse derivative ...
    jetparticle.setState(inState);

    p_be->propagate( jetparticle );
    m = ( jetparticle.state().jacobian() )[xp][x];
    if( fabs(m) < 1.0e-12 ) {
      ostringstream uic;
      uic  << "Horrible, inexplicable error at step "
           <<  i << ": multi-valued solution is suspected.";
      throw( GenericException( __FILE__, __LINE__, 
             "int AdjustPosition( BmlnElmnt* p_be, const JetParticle& arg_jp, char )", 
             uic.str().c_str() ) );
    }
    m = 1.0 / m;

    // Recalculate difference ...
    particle.state()= inState;
    p_be->propagate( particle );
    f = ( particle.state() )[xp] + xp_i;
  }


  // Step procedure when Newton's method fails ...
  if( i >= 15 ) {
    cerr << "*** WARNING ***                                      \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** No convergence within 15 Newton      \n"
            "*** WARNING *** iterations for magnet "
         <<                               p_be->Name() 
         <<                           ". Proceeding to find       \n"
            "*** WARNING *** best solution by stepping.           \n"
            "*** WARNING ***                                      \n"
         << endl;

    inState[x] = 0.0;
    double delta = 1.0e-4;           // step 0.1 mm

    particle.state() = inState;
    p_be->propagate( particle );
    double error = ( particle.state() )[xp] + xp_i;

    inState[x] = delta;
    particle.state() = inState;
    p_be->propagate( particle );
    f = ( particle.state() )[xp] + xp_i;

    if(      ( ( f >= 0.0 && error >= 0.0 ) || ( f <= 0.0 && error <= 0.0 ) ) 
          && ( fabs(error) < fabs(f) ) ) 
    {
      delta = - delta;
    }

    inState[x] = 0.0;
    while( fabs(delta) > 0.9e-6 ) {
      inState[x] += delta;
      particle.state() = inState;
      p_be->propagate( particle );
      f = ( particle.state() )[xp] + xp_i;

      while( ( ( f <= 0.0 && error <= 0.0 ) || ( f >= 0.0 && error >= 0.0 ) ) && 
             ( fabs(f) < fabs(error) ) )
      {
        error = f;
        inState[x] += delta;
      	particle.state()= inState;
      	p_be->propagate( particle );
      	f = ( particle.state() )[xp] + xp_i;
      }

      inState[x] -= delta;
      delta      *= (-0.1);
    }

    cerr << "*** WARNING ***                                      \n"
            "*** WARNING *** CF_rbend::AdjustPosition             \n"
            "*** WARNING *** The step procedure suggests a best   \n"
            "*** WARNING *** solution with magnet displacement "
         <<                                  (-1000.0)*inState[x]
         <<                                           " mm with   \n"
            "*** WARNING *** bend angle error "
         <<                                   (2.0e6)*error
         <<                                       " microradians. \n"
            "*** WARNING ***                                      \n"
         << endl;

    z = inState[x];
  }


  // Set alignment of the object.
  // p_be->align->getAlignment().xOffset -= z;

  v.setXOffset(v.xOffset() - z );
  p_be->setAlignment( v );

  // ??? Does not work when in and out faces are not parallel.

  // Clean up and return.

  end:

  return ret;
}

