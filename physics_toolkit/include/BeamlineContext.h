/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   2.0
******                                    
******  File:      BeamlineContext.h
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef BEAMLINECONTEXT_H
#define BEAMLINECONTEXT_H

#ifndef _CPP_IOSTREAM
#include <iostream>
#endif

#include "complexAddon.h"

class beamline;
class ClosedOrbitSage;
class ChromaticityAdjuster;
class TuneAdjuster;
class ConstBmlVisitor;
class BmlVisitor;
class Particle;
class JetProton;
class bmlnElmnt;
class BeamlineIterator;
class DeepBeamlineIterator;
class ReverseBeamlineIterator;
class DeepReverseBeamlineIterator;

template<typename T1, typename T2> 
class TMapping;
typedef TMapping<double,FNAL::Complex> Mapping;
// class Mapping;

#ifndef LATTFUNCSAGE_H
#include "LattFuncSage.h"
#endif
// The way that LattFuncSage.h is currently written,
// it #includes bmlnElmnt.h.  Bad form.

#ifndef EDWARDSTENGSAGE_H
#include "EdwardsTengSage.h"
#endif

#ifndef COVARIANCESAGE_H
#include "CovarianceSage.h"
#endif

class BeamlineContext
{
  public:
    BeamlineContext( bool = false, beamline* = 0 );
    BeamlineContext( const BeamlineContext& );
    // If first argument is true, the
    // beamline is cloned, and therefore
    // owned by the context.
    ~BeamlineContext();

    int assign( /* const */ beamline* );

    void accept( ConstBmlVisitor& ) const;
    void accept( BmlVisitor& );
    void setClonedFlag( bool );
    bool getClonedFlag();

    void writeTree();


    // Beamline methods
    const char* name() const;
    void peekAt( double& s, Particle* = 0 ) const;
    double sumLengths() const;

    int setLength   ( bmlnElmnt*, double );
    int setStrength ( bmlnElmnt*, double );
    int setAlignment( bmlnElmnt*, const alignmentData& );
    // I really want to get rid of AlignmentData altogether!

    int setAlignment( beamline::Criterion&, const alignmentData& );
    int processElements( beamline::Action& );
    // Returns number of elements processed.

    void setAvgInvariantEmittance( double, double );
    // Sets the parameters used to calculate
    //   an equilibrium covariance matrix.
    // Arguments are in units of pi mm-mr.
    //   The first is "mostly horizontal"; 
    //   the second, "mostly vertical."
    // If not set, default values are: (40,40).

    double getEnergy() const;
    int countHowManyDeeply() const;
    alignmentData getAlignmentData( const bmlnElmnt* ) const;
    beamline* cloneBeamline() const;
    // Creates new beamline, for which the invoker is responsible.
    Mapping getOneTurnMap();
    // Returns the one turn map; 
    // Side effect: calculates closed orbit if not
    // already done.


    // Sage methods ... and others
    double getHorizontalFracTune();
    double getVerticalFracTune();
    double getHorizontalEigenTune();
    double getVerticalEigenTune();
    const LattFuncSage::lattFunc* getLattFuncPtr( int );
    const EdwardsTengSage::Info* getETFuncPtr( int );
    const CovarianceSage::Info* getCovFuncPtr( int );

    MatrixD equilibriumCovariance();
    MatrixD equilibriumCovariance( double, double );
    // Arguments are the "invariant emittances" -
    //   essentially two scaled action coordinates -
    //   in pi mm-mr units. The first is "mostly horizontal," 
    //   the second, "mostly vertical."
    //   (See notes on _eps_1 and _eps_2 below.)
    // This routine returns a covariance matrix over
    //   the full phase space, but only the transverse
    //   components should be believed. It assumes that
    //   the longitudinal actions are zero.
    // In the first form, the values of member data
    //   _eps_1 and _eps_2 are used for the arguments.

    // Adjuster methods
    int addHTuneCorrector( const bmlnElmnt* );
    int addVTuneCorrector( const bmlnElmnt* );
    int changeTunesBy( double, double );
    // The method will check to make certain that
    // the argument is either a quadrupole or a 
    // thinQuad.


    // Iterator functions
    int beginIterator();
    int beginDeepIterator();
    int beginReverseIterator();
    int beginDeepReverseIterator();

    void endIterator();
    void endDeepIterator();
    void endReverseIterator();
    void endDeepReverseIterator();

    const bmlnElmnt* i_next();
    const bmlnElmnt* di_next();
    const bmlnElmnt* ri_next();
    const bmlnElmnt* dri_next();

    void i_reset();
    void di_reset();
    void ri_reset();
    void dri_reset();

    const beamline* cheatBmlPtr() const;

    // Stream functions
    friend ostream& operator<<( ostream&, const BeamlineContext& );
    friend istream& operator>>( istream&,       BeamlineContext& );

    Proton                _proton;

    // Status flags
    static const int OKAY;
    static const int NO_TUNE_ADJUSTER;

  private:
    beamline*             _p_bml;
    LattFuncSage*         _p_lfs;
    EdwardsTengSage*      _p_ets;
    CovarianceSage*       _p_covs;
    ClosedOrbitSage*      _p_cos;
    ChromaticityAdjuster* _p_ca;
    TuneAdjuster*         _p_ta;

    double                _dpp;
    // value of dp/p used for dispersion calculation.
    // by default = 0.0001
    double                _eps_1, _eps_2;
    // Average invariant emittances in units of pi mm-mr.
    //   Used for computing equilibrium covariance  matrix.
    //   Default values: _eps_1 = 40, _eps_2 = 40.
    // Relation to <I> = <aa*>: 
    //   <I> [m]  =  1.0e-6 * (_eps [pi mm-mr]/2) / (beta*gamma)
    //   Note: because units are pi mm-mr, I divide by 2, not M_TWOPI
    // I assume that _eps_1 is "mostly horizontal" and _eps_2 is
    //   "mostly vertical."
    Proton*               _p_co_p; 
    // once created, proton holds initial state for
    // (transverse) closed orbit.
    Proton*               _p_disp_p;
    // same as above, but at reference energy*(1+dp/p)
    JetProton*            _p_jp;
    // once created, its state always contains
    // one traversal of the beamline at the
    // reference energy on the closed orbit.

    // "small" numbers for testing orbit closure
    const static double _smallClosedOrbitXError;
    const static double _smallClosedOrbitYError;
    const static double _smallClosedOrbitNPXError;
    const static double _smallClosedOrbitNPYError;


    LattFuncSage::tunes*     _tunes;
    EdwardsTengSage::Tunes*  _eigentunes;

    // Status flags
    bool                  _isCloned;
    bool                  _normalLattFuncsCalcd;
    bool                  _edwardstengFuncsCalcd;
    bool                  _momentsFuncsCalcd;
    bool                  _dispCalcd;

    // Iterators
    BeamlineIterator*            _p_bi;
    DeepBeamlineIterator*        _p_dbi;
    ReverseBeamlineIterator*     _p_rbi;
    DeepReverseBeamlineIterator* _p_drbi;

    // Operations
    void _createTunes();
    void _createLFS();
    void _deleteLFS();
    void _createETS();
    void _deleteETS();
    void _createCOVS();
    void _deleteCOVS();
    void _createEigentunes();
    void _createClosedOrbit();
    void _deleteClosedOrbit();

    bool _onTransClosedOrbit( const Proton& ) const;
};

#endif // BEAMLINECONTEXT_H
