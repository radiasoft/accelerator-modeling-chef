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

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif
#ifndef MAP_HXX
#include "Mapping"
#endif
#ifndef MATHCONS_H
#include "MathConstants.h"
#endif
#ifndef SAGE_H
#include "Sage.h"
#endif

class EdwardsTengSage : public Sage
{
public:
  struct Info : BarnacleData 
  {
    double arcLength;
    Mapping map;
    Mapping mapInv;
    struct {
     double hor;
     double ver;
    } beta;
    struct {
     double hor;
     double ver;
    } alpha;
    double phi;
    MatrixD D;
    MatrixC EV;

    Info();
    Info( const Info& );
    ~Info(){}
  };

  struct Tunes : BarnacleData 
  {
    double hor;
    double ver;

    Tunes() : hor(0.0), ver(0.0) {}
    Tunes( const Tunes& x ) : hor(x.hor), ver(x.ver) {}
    ~Tunes() {}
    
    Tunes& operator=( const Tunes& x )
    { if( this != &x ) {hor = x.hor;ver = x.ver;} return *this; }
  };

private:
  static double _csH, _csV, _snH, _snV;
  static Mapping* _theMapPtr;
  static int _attachETFuncs( bmlnElmnt* );

  Info** _calcs;  // array of calculated results
  int    _n;      // size of the _calcs array

  void _deleteCalcs();

public:
  EdwardsTengSage( const beamline*, bool = false );
  EdwardsTengSage( const beamline&, bool = false );
  ~EdwardsTengSage();

  // vestigial: need to retain???
  int doCalc( JetParticle*, beamline::Criterion& = beamline::yes ); 
      // PRECONDITION: The JetParticle must be on the closed
      //   orbit with the identity mapping for its state.
      //               Its Jet environment's reference point 
      //   should be the closed orbit. It is not reset.
      // POSTCONDITION: The JetParticle has the one-turn
      //   mapping for its state.
      // If default value is used for second argument, 
      //   information is attached to all elements.

  static int eigenTuneCalc( const JetParticle&, Tunes& );
  // PRECONDITION: The JetParticle is on the closed
  //   orbit. Its state is the one-turn mapping
  //   for an environment centered on the closed orbit.

  const EdwardsTengSage::Info* get_ETFuncPtr( int );

  void eraseAll();
};

#endif // EDWARDSTENGSAGE_H
