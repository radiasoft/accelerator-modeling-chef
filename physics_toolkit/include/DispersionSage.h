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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#ifndef DISPERSIONSAGE_H
#define DISPERSIONSAGE_H

#include "Sage.h"

class DispersionSage : public Sage
{
public: 

  // Substructs
  struct GlobalInfo : BarnacleData
  {
    struct Tunes {
      double hor;
      double ver;
    } tune;
    struct Chrs {
      double hor;
      double ver;
    } chromaticity;

    GlobalInfo();
    GlobalInfo( const GlobalInfo&  );
    ~GlobalInfo() {}
    GlobalInfo& operator=( const GlobalInfo& );
  };

  struct Info : BarnacleData 
  {
    double arcLength;
    struct Dis {
      double hor;
      double ver;
    } dispersion;
    struct DDis {
      double hor;
      double ver;
    } dPrime;

    Info();
    Info( const Info&  );
    ~Info() {}
    Info& operator=( const Info& );
  };

  struct Options {
    bool onClosedOrbit;
    // 
    Options();
    Options( const Options& x );
    ~Options() {}
    Options& operator=( const Options& x );
  } flags;

  static const Options defaultFlags;


  // Constructors and the destructor
  DispersionSage( const beamline*, bool = false );
  DispersionSage( const beamline&, bool = false );
  // Second argument is used by class Sage
  // to control cloning. (See Sage.h)
  ~DispersionSage();


  // Public member functions
  int doCalc( JetParticle*, beamline::Criterion& = beamline::yes );
  int fullCalc( JetParticle*, beamline::Criterion& = beamline::yes );
      // Two equivalent functions. doCalc is a name alias for fullCalc.
      //   It provides compatability with other Sages.
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

  DispersionSage::GlobalInfo getGlobalInfo() const;
  const DispersionSage::Info* getInfoPtr() const;
      // Returns the information calculated at the of
      //   the beamline. For a ring, this will be the
      //   dispersion at its beginning. If nothing has
      //   been calculated, return value is zero.
  const DispersionSage::Info* getInfoPtr( int ) const;
      // Returns the information calculated for the
      //   ith element in the line.
  bool checkInfoPtr() const;
      // Checks whether the address of the Info
      //   stored on the last beamline element
      //   matches the pointer stored in the _calcs
      //   array
  bool checkInfoPtr( int ) const;
      // Checks whether the address of the Info
      //   stored on the kth beamline element
      //   matches the pointer stored in the _calcs
      //   array

  void eraseAll();

  void   set_dpp( double );
  double get_dpp() const;

  void setIgnoreErrors( bool );

  // Error identifiers
  static const int DONE;
  static const int TOO_MANY_VECTORS;
  static const int IMPOSSIBILITY;

 private:
  double _dpp;    // used for dispersion calculations
  Info** _calcs;  // array of calculated results;
                  //   the objects stored on
                  //   the individual beamline elements
  int    _n;      // size of the _calcs array
  GlobalInfo _lr;

  void _finishConstructor();
  void _deleteCalcs();
  bool _ignoreErrors;
};


inline DispersionSage::GlobalInfo DispersionSage::getGlobalInfo() const
{
  return _lr;
}


inline double DispersionSage::get_dpp() const
{
  return _dpp;
}

inline void DispersionSage::setIgnoreErrors( bool x )
{
  _ignoreErrors = x;
}

#endif // DISPERSIONSAGE_H
