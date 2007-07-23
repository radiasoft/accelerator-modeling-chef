/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                                                
******  File:      DispersionSage.h
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
******                                                                
**************************************************************************
*************************************************************************/

#ifndef DISPERSIONSAGE_H
#define DISPERSIONSAGE_H

#include <physics_toolkit/Sage.h>

class DispersionSage : public Sage {

public: 

  // Substructs

  struct GlobalInfo {

    GlobalInfo(); 

    struct Tunes {
      double hor;
      double ver;
    } tune;
    struct Chrs {
      double hor;
      double ver;
    } chromaticity;

  };

  struct Info {

    Info();

    double arcLength;
    struct Clort {
      double hor;
      double ver;
    } closedOrbit;
    struct Dis {
      double hor;
      double ver;
    } dispersion;
    struct DDis {
      double hor;
      double ver;
    } dPrime;

  };

  struct Options {
    Options();
    bool onClosedOrbit;   // Default: false
  };

  Options flags;

  static const Options defaultFlags;


  // Constructors and the destructor

  DispersionSage( BmlPtr );
  DispersionSage( beamline const& );


  // Public member functions

  int doCalc( JetParticle& );

  // int fadCalc( const JetParticle*, beamline::Criterion& = beamline::yes ) const;
      // Assumes no vertical dispersion.
      // Uses the 5x5 matrix formalism that 
      //   everyone knows and loves, ignoring the
      //   closed orbit. 
      // Attaches the lattRing "Ring" to the beamline
      //   but no information to the beamline elements.
      // The purpose is to provide a quick, first order
      //   calculation of dispersion at the beginning of the
      //   beamline.
      // By the way, "FAD" stands for "Fast and Dirty."

  int pushCalc( Particle const&, Info const& );
      // Assumes the data in the second argument are
      //   initial conditions and "pushes" a dispersion
      //   calculation down the beamline.

  int pushCalc2( const JetParticle&, Info const& );

  DispersionSage::GlobalInfo getGlobalInfo() const;

  DispersionSage::Info const& getInfo() const;
      // Returns the information calculated at the of
      //   the beamline. For a ring, this will be the
      //   dispersion at its beginning. If nothing has
      //   been calculated, return value is zero.

  std::vector<DispersionSage::Info> const& getDispersionArray() const;

      // Returns the information calculated for the
      //   ith element in the line.

  void eraseAll();

  void   set_dpp( double );
  double get_dpp() const;

  DispersionSage::Options get_options();
  void set_options( DispersionSage::Options const& );

  void setIgnoreErrors( bool );

  // Error identifiers

  static const int DONE;
  static const int TOO_MANY_VECTORS;
  static const int IMPOSSIBILITY;

 private:

  DispersionSage(DispersionSage const&);

  double              dpp_;    // used for dispersion calculations
  std::vector<Info>   calcs_;  // array of calculated results;
                               //   the objects stored on
                               //   the individual beamline elements
  GlobalInfo          lr_;

  bool                ignoreErrors_;


};

#endif // DISPERSIONSAGE_H
