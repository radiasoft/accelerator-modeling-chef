/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
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
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******  URA/FNAL reserves all rights.
******
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******  REVISION HISTORY
******
******  Mar 2007           ostiguy@fnal.gov
******  - support for reference counted elements
******  - Pass particles by reference
******
******  Jan 2007           ostiguy@fnal.gov
******  - eliminated obsolete, unused methodsto compute LF
******  - eliminated unused data members
******
******  Feb 2014           michelotti@fnal.gov
******  - added member function LattFuncSage::FourPointDisp_Calc(...)
******    which uses a four-point algorithm written by James Amundson
******    to evaluate dispersions and chromaticities. Compared to the
******    two-point algorithm, error is reduced from O((dp/p)^2) to
******    O((dp/p)^5).
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


 int CourantSnyderLatticeFunctions( JetParticle const&,       Sage::CRITFUNC = 0 );
               // If default value is used for Sage::CRITFUNC, then
               // information is attached to all elements.

 int FourPointDisp_Calc( JetParticle const&, bool onClosedOrbit = false );
 int       NewDisp_Calc( JetParticle const&, bool onClosedOrbit = false );

 int pushCalc       ( Particle const &, LattFuncSage::lattFunc const& );

 void eraseAll();

 std::vector<lattFunc> const& getTwissArray();    // !!! ??? VERY BADLY NAMED ??? !!!
 lattRing              const& getLattRing();


 void   set_dpp( double );
 double get_dpp();

 // status codes

 enum status { OK=0, SLOTS_DETECTED, UNSTABLE, INTEGER_TUNE, PHASE_ERROR, WRONG_COUNT, NOT_WRITTEN, TOO_MANY_VECTORS };

private:

 LattFuncSage( LattFuncSage const&);

 double                  dpp_;
 std::vector<lattFunc>   lfvec_;
 lattRing                lr_;


};


#endif // LATTFUNCSAGE_H
