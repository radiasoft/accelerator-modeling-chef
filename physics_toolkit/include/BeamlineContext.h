/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
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

class Mapping;

#ifndef LATTFUNCSAGE_H
#include "LattFuncSage.h"
#endif
// The way that LattFuncSage.h is currently written,
// it #includes beamline.h.  Bad form.

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
    const LattFuncSage::lattFunc* getLattFuncPtr( int );

    MatrixD equilibriumCovariance( double, double );
    // Arguments are the "invariant emittances" -
    //   essentially two scaled action coordinates -
    //   in pi mm-mr units. The first is "mostly horizontal," 
    //   the second, "mostly vertical."
    // This routine returns a covariance matrix over
    //   the full phase space, but only the transverse
    //   components should be believed. It assumes tat
    //   the longitudinal actions are zero.

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
    ClosedOrbitSage*      _p_cos;
    ChromaticityAdjuster* _p_ca;
    TuneAdjuster*         _p_ta;

    double                _dpp;
    // value of dp/p used for dispersion calculation.
    // by default = 0.0001

    Proton*               _p_co_p; 
    // once created, proton holds initial state for
    // (transverse) closed orbit.
    Proton*               _p_disp_p;
    // save as above, but at reference energy*(1+dp/p)
    JetProton*            _p_jp;
    // once created, its state always contains
    // one traversal of the beamline at the
    // reference energy on the closed orbit.

    // "small" numbers for testing orbit closure
    const static double _smallClosedOrbitXError;
    const static double _smallClosedOrbitYError;
    const static double _smallClosedOrbitNPXError;
    const static double _smallClosedOrbitNPYError;


    LattFuncSage::tunes*  _tunes;

    bool                  _isCloned;
    bool                  _normalLattFuncsCalcd;
    bool                  _dispCalcd;

    BeamlineIterator*            _p_bi;
    DeepBeamlineIterator*        _p_dbi;
    ReverseBeamlineIterator*     _p_rbi;
    DeepReverseBeamlineIterator* _p_drbi;

    void _createLFS();
    void _deleteLFS();
    void _createTunes();
    void _createClosedOrbit();
    void _deleteClosedOrbit();

    bool _onTransClosedOrbit( const Proton& ) const;
};

#endif // BEAMLINECONTEXT_H
