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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
******
****** - support for reference counted elements
****** - Pass particles by reference
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
#include <mxyzptlk/Mapping.h>
#include <beamline/beamline.h>
#include <physics_toolkit/Sage.h>

class LattFuncSage : public Sage {

public:

 LattFuncSage( BmlPtr bml      );
 LattFuncSage( beamline const& );
  
 struct tunes {
   double hor;
   double ver;
 };

 struct lattRing  {
   struct tune_type{
     double hor;
     double ver;
   } tune;
   struct chromaticity_type {
     double hor;
     double ver;
   } chromaticity;

 };

 struct lattFunc {

   lattFunc();

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
 
  };


 int TuneCalc  ( JetParticle&, bool forceClosedOrbitCalc = true );
               // TuneCalc implicitly assumes an uncoupled ring; results
               // are suspect-to-meaningless if coupling exists.
               // Upon exit, (a) the JetParticle argument is on the closed
               //                orbit and its state is the one-turn mapping.
               //            (b) the beamline's dataHook holds one barnacle
               //                labelled "Tunes" containing a
               //                LattFuncSage::tunes data struct.


 int NewSlow_CS_Calc( JetParticle const&,       Sage::CRITFUNC = 0 );
               // If default value is used for Sage::CRITFUNC, then
               // information is attached to all elements.
 int NewDisp_Calc   ( JetParticle const&, bool onClosedOrbit = false );

 int pushCalc       ( Particle const &, LattFuncSage::lattFunc const& );

 void eraseAll();

 std::vector<lattFunc> const& getTwissArray();
 lattRing              const& getLattRing();

  
 void   set_dpp( double );
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

 LattFuncSage( LattFuncSage const&); 

 static double           csH_; 
 static double           csV_; 
 static double           snH_; 
 static double           snV_;
 static Mapping*         theMapPtr_;

        double           dpp_;    
 std::vector<lattFunc>   lfvec_;
 lattRing                lr_;


};


#endif // LATTFUNCSAGE_H
