/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      EdwardsTengSage.h
******  Version:   1.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - Pass particles by reference 
******                                                                
**************************************************************************
*************************************************************************/


/*
 *  This "lattice function" method is based on 
 *  D.A.Edwards and L.C.Teng
 *  "Parametrization of Linear Coupled Motion in Periodic Systems"
 *  IEEE Tr.NucSci.
 *  NS-20(3), 885(1973).
 *       -- Leo Michelotti
 *          June 6, 1994   ( D-Day + 50 years )
 *
 *  The Sage version is modified from class EdwardsTeng
 *       -- Leo Michelotti
 *          April 14, 2004
 */

#ifndef EDWARDSTENGSAGE_H
#define EDWARDSTENGSAGE_H

#include <vector>

#include <beamline/bmlnElmnt.h>
#include <mxyzptlk/Mapping.h>

#include <basic_toolkit/MathConstants.h>
#include <physics_toolkit/Sage.h>

class EdwardsTengSage : public Sage
{

public:

  struct Info {

    double arcLength;

    MatrixD map;    // cumulative map  // (in the previous version, declared as Mapping) 

    struct {
     double hor;
     double ver;
    } beta;
    struct {
     double hor;
     double ver;
    } alpha;
    double phi;

    MatrixD D;       // eigenvectors
    MatrixC EV;      // eigenvalues 

    Info();
  };

  struct Tunes {
    double hor;
    double ver;
  };

public:

  EdwardsTengSage( BmlPtr );
  EdwardsTengSage( beamline const& );

  int doCalc( JetParticle& , beamline::Criterion& = beamline::yes ); 
      // PRECONDITION: The JetParticle must be on the closed
      //               orbit with the identity mapping for its state.
      //               Its Jet environment's reference point 
      //               should be the closed orbit. It is not reset.
      // POSTCONDITION: The JetParticle has the one-turn
      //                mapping for its state.
      //                If default value is used for second argument, 
      //                information is attached to all elements.

  static int eigenTuneCalc( JetParticle const&, Tunes& );
  // PRECONDITION: The JetParticle is on the closed
  //               orbit. Its state is the one-turn mapping
  //               for an environment centered on the closed orbit.

  std::vector<EdwardsTengSage::Info> const& getETArray();

  void eraseAll();

private:

  int       attachETFuncs( ElmPtr, MatrixD const& one_turn_map );

  double    csH_; 
  double    csV_; 
  double    snH_; 
  double    snV_;

  Mapping   theMapPtr_;

  std::vector<Info> calcs_;  // array of calculated results

};

#endif // EDWARDSTENGSAGE_H
