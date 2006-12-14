/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      beamline_twiss.cc
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
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <iomanip>
#include <basic_toolkit/iosetup.h>
#include <physics_toolkit/LattFuncSage.h>    // ??? Only temporary, until beamline::twiss functions vanish.
#include <beamline/Particle.h>  
#include <beamline/JetParticle.h>  
#include <beamline/BeamlineIterator.h>  
#include <physics_toolkit/ClosedOrbitSage.h> 

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


int beamline::twiss( JetParticle& p, double const& dpp, int flag ) 
{
  static char firstTime = 1;
  if( firstTime ) {
    firstTime = 0;
    (*pcerr) << "*** WARNING ***                                      \n"
            "*** WARNING *** beamline::twiss                      \n"
            "*** WARNING ***                                      \n"
            "*** WARNING *** This member function is obsolete.    \n"
            "*** WARNING *** Use a LattFuncSage instead.          \n"
            "*** WARNING ***                                      \n"
         << endl;
  }

  int ret = 0;

  if( !twissDone )
  {
    int i;
    Vector zero(BMLN_dynDim);
    for ( i = 0; i < BMLN_dynDim; i++ ) zero[i] = 0.0;
  
    p.SetReferenceEnergy( this->Energy() );
    p.setState(zero);
  
    ClosedOrbitSage clsg( this );
    LattFuncSage lfs( this );
    lfs.set_dpp( dpp );
  
  
    // Put p on closed orbit and construct one-turn map
    clsg.setForcedCalc();
    if( ( ret = clsg.findClosedOrbit( &p ) ) == 0 )
    {
      (*pcerr) << "beamline::twiss: Closed orbit successfully calculated." << endl;
    }
    else
    {
      (*pcerr) << "beamline::twiss: Closed orbit not successfully calculated." << endl;
      return ret;
    }
  
  
    // Initial check for Slots
    DeepBeamlineIterator dbi( *this );
    bmlnElmnt* q;
    char slotFound = 0;
    while((  q = dbi++  )) {
      if( strstr( "CF_rbend|rbend|Slot", q->Type() ) ) {
        slotFound = 1;
        break;
      }
    }
    dbi.reset();

    if( slotFound ) {
      if( flag ) {
        ret = lfs.Slow_CS_Calc( &p );
      }
      else {
        ret = lfs.Slow_CS_Calc( &p, Sage::no );
      }
    }
    else {
      if( flag ) {
        ret = lfs.Fast_CS_Calc( &p );
      }
      else {
        ret = lfs.Fast_CS_Calc( &p, Sage::no );
      }
    }  

    if( ret != 0 ) {
      (*pcerr) << "beamline::twiss: Problem calculating the Twiss \n"
  	   << "parameters." << endl;
      return ret;
    }

    // Calculate dispersion
    // This also puts the chromaticity ring data on the beamline.
    if( flag > 0 ) {
      if( 1 == flag ) {
        ret = lfs.Disp_Calc( &p );
      }
      else {
        ret = lfs.FAD_Disp_Calc( &p );
      }
    }
    else {
      if( 0 == flag ) {
        ret = lfs.Disp_Calc( &p, Sage::no );
      }
      else {
        ret = lfs.FAD_Disp_Calc( &p, Sage::no );
      }
    }
  
    if( ret != 0 ) {
      (*pcerr) << "beamline::twiss: Problem calculating dispersion."
  	   << endl;
      return ret;
    }
  
  
  
    // Combine dispersion and lattice function information ...
    lattFunc*  plf;
    lattFunc*  qlf;
    
    if( flag ) {
      while( q = dbi++ ) {
        if( 0 != ( plf = (lattFunc*) q->dataHook.find("Twiss") ) ) {
          if( 0 != ( qlf = (lattFunc*) q->dataHook.find("Dispersion") ) ) {
            plf->dispersion.hor = qlf->dispersion.hor;
            plf->dispersion.ver = qlf->dispersion.ver;
            plf->dPrime.hor     = qlf->dPrime.hor;
            plf->dPrime.ver     = qlf->dPrime.ver;
            q->dataHook.eraseAll("Dispersion");
          }
          else {
            (*pcerr) << "beamline::twiss: Dispersion data missing from "
                 << q->Type() << "  " << q->Name()
                 << endl;
            ret = -1;
            return ret;
          }
        }
        else {
          (*pcerr) << "beamline::twiss: Twiss data missing from "
               << q->Type() << "  " << q->Name()
               << endl;
          ret = -1;
          return ret;
        }
      }
    }  
  
  
    // .......... Cleaning up and leaving ...................
  
    if( flag ) {
      lattFunc* latticeFunctions = new lattFunc;
      (*latticeFunctions) = *((lattFunc*)( lastElement()->dataHook.find( "Twiss" ) ));
      this->dataHook.eraseFirst( "Twiss" );
      this->dataHook.insert( new Barnacle( "Twiss", latticeFunctions ) );
      // dataHook.eraseFirst( "Ring" );
      // dataHook.insert( new Barnacle( "Ring", latticeRing ) );
    }
  
  
    twissDone = 1;
  }

  return ret;
}


int beamline::twiss( char, JetParticle& p ) {
  Jet*             z;
  double          csH, csV, snH, snV, t;
  int             i;

 // .......... Propagate a JetParticle to get transfer matrix


  lattFunc*       latticeFunctions = new lattFunc;
  lattRing*       latticeRing = new lattRing;

  if( ! twissDone ) {  // .... Check to see if this was done already.
   
      Vector zero(BMLN_dynDim);
      z              = new Jet      [ BMLN_dynDim ];

   // .......... Propagate a JetParticle to get transfer matrix
    for ( i = 0; i < BMLN_dynDim; i++ ) zero[i] = 0.0;
    p.setState( zero );
    propagate( p );
    
    MatrixD mtrx(BMLN_dynDim,BMLN_dynDim,0.0);
    Mapping map;
    map = p.getState();
    mtrx = map.Jacobian();

   // .......... Calculating tunes .........................

    csH = ( mtrx(0,0) + mtrx(3,3) ) / 2.0;  // cosine horizontal tune
    csV = ( mtrx(1,1) + mtrx(4,4) ) / 2.0;  // cosine vertical   tune

    if( fabs(csH) > 1.0 || fabs(csV) > 1.0 ) {
      (*pcerr) << "\n*** WARNING *** " << endl ;
      (*pcerr) << "*** WARNING *** beamline::twiss(JetParticle)  Lattice is unstable." << endl;
      (*pcerr) << "*** WARNING *** beamline::twiss() did not exit properly." << endl;
      (*pcerr) << "*** WARNING *** " << endl;
      delete latticeFunctions;
      delete latticeRing;
      return -1;
   }
    
    snH = sqrt( -1.0* mtrx(0,3)*mtrx(3,0) - 
               (mtrx(0,0) - mtrx(3,3))*
               (mtrx(0,0) - mtrx(3,3))/4.0);
    snV = sqrt( -1.0*mtrx(1,4) * mtrx(4,1) - 
               (mtrx(1,1) - mtrx(4,4))*
               (mtrx(1,1) - mtrx(4,4))/4.0);


    if( mtrx(0,3) < 0.0 ) snH *= -1.0;       // ?? Is there a better way of
    if( mtrx(1,4) < 0.0 ) snV *= -1.0;       // ?? changing the sign?
    
   t = asin( snH );
   if( csH < 0.0 ) t  = M_PI - t;              // 0 < t < 2 pi
   if( t < 0.0 )   t += M_TWOPI;
   latticeRing->tune.hor = ( t / M_TWOPI );

   t = asin( snV );
   if( csV < 0.0 ) t  = M_PI - t;              // 0 < t < 2 pi
   if( t < 0.0 )   t += M_TWOPI;
   latticeRing->tune.ver = t / M_TWOPI;
//    t = atan2(snH,csH);
//    latticeRing->tune.hor = ( t / M_TWOPI );

//    t = atan2(snV,csV);
//    latticeRing->tune.ver = ( t / M_TWOPI );


   // .......... Calculating betas and alphas ..............
    latticeFunctions->beta .hor = mtrx(0,3) / snH;
    latticeFunctions->beta .ver = mtrx(1,4) / snV;
    latticeFunctions->alpha.hor = ( mtrx(0,0) - mtrx(3,3) ) / (2.0*snH);
    latticeFunctions->alpha.ver = ( mtrx(1,1) - mtrx(4,4) ) / (2.0 * snV);
    latticeFunctions->psi  .hor = 0.0;
    latticeFunctions->psi  .ver = 0.0;
 
// calculate dispersion
    
    MatrixD A(4,4,0.0);
    MatrixD Disp(4,1,0.0);
    MatrixD Long(4,1,0.0);
    
    A(0,0) = mtrx(0,0);
    A(0,1) = mtrx(0,3);
    A(0,2) = mtrx(0,1);
    A(0,3) = mtrx(0,4);
    A(1,0) = mtrx(3,0);
    A(1,1) = mtrx(3,3);
    A(1,2) = mtrx(3,1);
    A(1,3) = mtrx(3,4);
    A(2,0) = mtrx(1,0);
    A(2,1) = mtrx(1,3);
    A(2,2) = mtrx(1,1);
    A(2,3) = mtrx(1,4);
    A(3,0) = mtrx(4,0);
    A(3,1) = mtrx(4,3);
    A(3,2) = mtrx(4,1);
    A(3,3) = mtrx(4,4);
   
    Long(0,0) = -1.0 * mtrx(0,5);
    Long(1,0) = -1.0 * mtrx(3,5);
    Long(2,0) = -1.0 * mtrx(1,5);
    Long(3,0) = -1.0 * mtrx(4,5);
    MatrixD id("I",4);
    Disp = (A - id).inverse() * Long;

    latticeFunctions->dispersion.hor = Disp(0,0);
    latticeFunctions->dPrime.hor = Disp(1,0);
    latticeFunctions->dispersion.ver = Disp(2,0);
    latticeFunctions->dPrime.ver = Disp(3,0);

   // .......... Cleaning up and leaving ...................
    delete [] z;
    twissDone = 1;
    dataHook.eraseFirst( "Twiss" );
    dataHook.insert( new Barnacle( "Twiss", latticeFunctions ) );
    dataHook.eraseFirst( "Ring" );
    dataHook.insert( new Barnacle( "Ring", latticeRing ) );
  }
  return 0;
} 

int beamline::twiss( lattFunc& W_arg, JetParticle& p, int flag ) {
  static char firstTime = 1;

  if( firstTime ) {
    firstTime = 0;
    (*pcerr) << "***WARNING***                                           \n"
            "***WARNING***  beamline::twiss( lattFunc& W, JetParticle& p)      \n"
            "***WARNING***  Using LattFuncSage is preferred.         \n"
            "***WARNING***  This member function will disappear      \n"
            "***WARNING***  one day.                                 \n"
            "***WARNING***                                           \n"
         << endl;
  }

  int ret;
  LattFuncSage et( this );

  // This is a stupid conversion necessary because
  // LattFunc exists outside of LattFuncSage.
  LattFuncSage::lattFunc W;
  W.arcLength       = W_arg.arcLength;
  W.dispersion.hor  = W_arg.dispersion.hor;
  W.dispersion.ver  = W_arg.dispersion.ver;
  W.dPrime.hor      = W_arg.dPrime.hor;
  W.dPrime.ver      = W_arg.dPrime.ver;
  W.beta.hor        = W_arg.beta.hor;
  W.beta.ver        = W_arg.beta.ver;
  W.alpha.hor       = W_arg.alpha.hor;
  W.alpha.ver       = W_arg.alpha.ver;
  W.psi.hor         = W_arg.psi.hor;
  W.psi.ver         = W_arg.psi.ver;

  if( ! twissDone ) {  // .... Check to see if this was done already.
    if( flag ) {
      ret = et.Twiss_Calc( W, p );
    }
    else {
      ret = et.Twiss_Calc( W, p, Sage::no );
    }

    if( ret == 0 ) twissDone = 1;
    else {
      (*pcerr) << "***WARNING***                                     \n"
              "***WARNING*** beamline::twiss                     \n"
              "***WARNING*** Failed.                             \n"
              "***WARNING***                                     \n"
           << endl;
    }
  }
  return ret;
} 

