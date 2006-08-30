/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      LattFuncSage.h
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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
******                                                                
**************************************************************************
*************************************************************************/


/*
 *  File: LattFuncSage.h
 *  
 *  Implementation for a sage for computing
 *  traditional, and untraditional,  lattice 
 *  functions.
 *  
 *  Leo Michelotti
 *  Nov. 19, 1998
 */

#ifndef LATTFUNCSAGE_H
#define LATTFUNCSAGE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/beamline.h>
#include <physics_toolkit/Sage.h>

class LattFuncSage : public Sage
{
public:
 LattFuncSage( const beamline*, bool = false );
 LattFuncSage( const beamline&, bool = false );
 // Second argument is used by class Sage
 // to control cloning. (See Sage.h)
 virtual ~LattFuncSage();
  
 struct tunes : BarnacleData {
   double hor;
   double ver;
   tunes() : hor(0.0), ver(0.0) {}
   tunes( const tunes& x ): BarnacleData( ), hor(x.hor), ver(x.ver) { }
   ~tunes(){}
 };

 struct lattRing : public BarnacleData {
   struct tune_type{
     double hor;
     double ver;
   } tune;
   struct chromaticity_type {
     double hor;
     double ver;
   } chromaticity;
   lattRing(): BarnacleData()
     { tune.hor = 0.;
       tune.ver = 0.;
       chromaticity.hor = 0.;
       chromaticity.ver = 0.;
     }
   ~lattRing() {}
   lattRing& operator=( const LattFuncSage::lattRing& x )
     { if( this != &x ) {
         tune.hor = x.tune.hor;
         tune.ver = x.tune.ver;
         chromaticity.hor = x.chromaticity.hor;
         chromaticity.ver = x.chromaticity.ver;
       }
       return *this;
     }
 };

 struct lattFunc : public BarnacleData {
   double arcLength;
   struct dispersion_type {
     double hor;
     double ver;
   } dispersion;
   struct dPrime_type {
     double hor;
     double ver;
   } dPrime;
   struct beta_type {
     double hor;
     double ver;
   } beta;
   struct alpha_type {
     double hor;
     double ver;
   } alpha;
   struct psi_type {
     double hor;
     double ver;
   } psi;
 
   lattFunc();
   ~lattFunc() {}
   lattFunc& operator=( const lattFunc& );
 };


 int TuneCalc       ( JetParticle*, bool forceClosedOrbitCalc = true );
               // TuneCalc implicitly assumes an uncoupled ring; results
               // are suspect-to-meaningless if coupling exists.
               // Upon exit, (a) the JetParticle argument is on the closed
               //                orbit and its state is the one-turn mapping.
               //            (b) the beamline's dataHook holds one barnacle
               //                labelled "Tunes" containing a
               //                LattFuncSage::tunes data struct.

 int Disp_Calc      ( JetParticle*, Sage::CRITFUNC = 0 );
 int NewDisp_Calc   ( JetParticle*, bool onClosedOrbit = false );
 int Fast_CS_Calc   ( /* const */ JetParticle*, Sage::CRITFUNC = 0 );
 int Slow_CS_Calc   ( /* const */ JetParticle*, Sage::CRITFUNC = 0 );
 int NewSlow_CS_Calc( /* const */ JetParticle*, Sage::CRITFUNC = 0 );
               // If default value is used for Sage::CRITFUNC, then
               // information is attached to all elements.
 int FAD_Disp_Calc  ( /* const */ JetParticle*, Sage::CRITFUNC = 0 );
               // Assumes no vertical dispersion.
               // Uses the 5x5 matrix formalism that 
               // everyone knows and loves, ignoring the
               // closed orbit. 
               // Attaches the lattRing "Ring" to the beamline
               // but no information to the beamline elements.
               // The purpose is to provide a quick, first order
               // calculation of dispersion at the beginning of the
               // beamline.
               // By the way, "FAD" stands for "Fast and Dirty."

 int Twiss_Calc     ( const LattFuncSage::lattFunc&, JetParticle&, Sage::CRITFUNC = 0 ); 
 int Twiss_Calc     ( JetParticle& );
               // These reproduce the old beamline::twiss
               // and therefore are both obsolete and wrong.

 int pushCalc       ( const Particle&, const LattFuncSage::lattFunc& );

 void eraseAll();

 lattFunc* get_lattFuncPtr( int );
 lattRing* get_lattRingPtr();
               // Returns 0 if the argument is
               // out of bounds.
               // Unsafe!! returns pointer.
  
 void set_dpp( double );
 double get_dpp();

 // Error identifiers
 static const int DONE;
 static const int SLOTS_DETECTED;
 static const int UNSTABLE;
 static const int INTEGER_TUNE;
 static const int PHASE_ERROR;
 static const int WRONG_COUNT;
 static const int NOT_WRITTEN;
 static const int TOO_MANY_VECTORS;

private:
 static double     _csH, _csV, _snH, _snV;
 static Mapping*   _theMapPtr;
        double     _dpp;    // used for dispersion calculations

 lattFunc* _lf;
 lattRing* _lr;

 void _deleteCalcs();
 void _finishConstructor();
};


inline double LattFuncSage::get_dpp()
{
  return _dpp;
}


#endif // LATTFUNCSAGE_H
