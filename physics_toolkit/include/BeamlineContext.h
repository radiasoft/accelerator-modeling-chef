/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      BeamlineContext.h
******  Version:   2.2
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
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
******                                          
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
******                                                                
**************************************************************************
*************************************************************************/

#ifndef BEAMLINECONTEXT_H
#define BEAMLINECONTEXT_H

#include <iostream>
#include <basic_toolkit/complexAddon.h>
#include <mxyzptlk/Mapping.h>
#include <physics_toolkit/LattFuncSage.h>
#include <beamline/Particle.h>
#include <physics_toolkit/EdwardsTengSage.h>
#include <physics_toolkit/CovarianceSage.h>
#include <physics_toolkit/LBSage.h>
#include <physics_toolkit/DispersionSage.h>


class beamline;
class ClosedOrbitSage;
class ChromaticityAdjuster;
class TuneAdjuster;
class ConstBmlVisitor;
class BmlVisitor;
class bmlnElmnt;
class BeamlineIterator;
class DeepBeamlineIterator;
class ReverseBeamlineIterator;
class DeepReverseBeamlineIterator;


class BeamlineContext
{
  public:
    BeamlineContext( const Particle&, beamline* = 0, bool = false );
    BeamlineContext( const BeamlineContext& );
    // If the bool argument is true, the
    // beamline is cloned, and its copy is
    // owned by the context; 
    // if false, it is not cloned.
    ~BeamlineContext();

    int assign( /* const */ beamline* );

    void accept( ConstBmlVisitor& ) const;
    void accept( BmlVisitor& );
    void setClonedFlag( bool );
    bool getClonedFlag();

    void setInitial( const DispersionSage::Info& );
    void setInitial( const DispersionSage::Info* );
    void getInitial( DispersionSage::Info* );
    void setInitial( const LattFuncSage::lattFunc& );
    void setInitial( const LattFuncSage::lattFunc* );
    void getInitial( LattFuncSage::lattFunc* );
    void setInitial( const CovarianceSage::Info& );
    void setInitial( const CovarianceSage::Info* );
    void getInitial( CovarianceSage::Info* );

    void writeTree();


    // Beamline methods
    const char* name() const;
    void rename( const char* );
    void peekAt( double& s, const Particle& ) const;
    double sumLengths() const;

    int setLength   ( bmlnElmnt*, double );
    int setStrength ( bmlnElmnt*, double );
    int setAlignment( bmlnElmnt*, const alignmentData& );
    // I really want to get rid of AlignmentData altogether!
    int setAlignment( beamline::Criterion&, const alignmentData& );

    int replaceElement( bmlnElmnt*, bmlnElmnt* );
    // Will replace the first argument with
    // the second. Return values:
    // 0 everything went as planned
    // 1 first argument was not found
    // 2 at least one argument was null
    // 
    // WARNING: Element *a will be deleted if the beamline has
    // been cloned. It is assumed that *b has been created on the
    // heap, and the BeamlineContext takes over ownership.

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
    bool onTransClosedOrbit( const Particle& ) const;

    bool hasReferenceParticle() const;
    void setReferenceParticle( const Particle& );
    int  getReferenceParticle( Particle& ) const;
    // Returns -1 if no reference particle is available.
    const Particle& getParticle();

    Vector getParticleState();
    void   loadParticleStateInto( Vector& );
    double getParticle_x   ();
    double getParticle_y   ();
    double getParticle_cdt ();
    double getParticle_npx ();
    double getParticle_npy ();
    double getParticle_ndp ();

    void setParticleState( const Vector& );
    void setParticle_x   ( double );
    void setParticle_y   ( double );
    void setParticle_cdt ( double );
    void setParticle_npx ( double );
    void setParticle_npy ( double );
    void setParticle_ndp ( double );

    bool isRing() const;
    bool isTreatedAsRing() const;
    void handleAsRing();
    void handleAsLine();

    void reset();
    // This is a drastic function which almost acts
    //   like a destructor. Effectively, all information
    //   about previous calculations is eliminated.

    // Sage methods ... and others
    double getHorizontalFracTune();
    double getVerticalFracTune();
    double getHorizontalEigenTune();
    double getVerticalEigenTune();
    const LattFuncSage::lattFunc* getLattFuncPtr( int );
    const EdwardsTengSage::Info* getETFuncPtr( int );
    const CovarianceSage::Info* getCovFuncPtr( int );
    const LBSage::Info* getLBFuncPtr( int );
    const DispersionSage::Info* getDispersionPtr( int );

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
    int addHChromCorrector( const bmlnElmnt* );
    int addVChromCorrector( const bmlnElmnt* );
    int changeChromaticityBy( double, double );
    // The method will check to make certain that
    // the argument is either a sextupole or a 
    // thinSextupole.


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
    friend std::ostream& operator<<( std::ostream&, const BeamlineContext& );
    friend std::istream& operator>>( std::istream&,       BeamlineContext& );

    // !!! Eventually, these three should become private !!!
    Particle*             _particlePtr;
    Particle*             _facadeParticlePtr;
    ParticleBunch*        _particleBunchPtr;

    // Status flags
    static const int OKAY;
    static const int NO_TUNE_ADJUSTER;
    static const int NO_CHROMATICITY_ADJUSTER;

  private:
    beamline*             _p_bml;
    LattFuncSage*         _p_lfs;
    EdwardsTengSage*      _p_ets;
    CovarianceSage*       _p_covs;
    LBSage*               _p_lbs;
    ClosedOrbitSage*      _p_cos;
    DispersionSage*       _p_dsps;
    ChromaticityAdjuster* _p_ca;
    TuneAdjuster*         _p_ta;

    // Initial conditions
    LattFuncSage::lattFunc* _initialLattFuncPtr;
    DispersionSage::Info*   _initialDispersionPtr;
    CovarianceSage::Info*   _initialCovariancePtr;

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
    Particle*             _p_co_p; 
    // once created, holds initial state for
    // (transverse) closed orbit.
    Particle*             _p_disp_p;
    // same as above, but at reference energy*(1+dp/p)
    JetParticle*          _p_jp;
    // once created, its state always contains
    // one traversal of the beamline at the
    // reference energy on the closed orbit.

    // "small" numbers for testing orbit closure
    static const double _smallClosedOrbitXError;
    static const double _smallClosedOrbitYError;
    static const double _smallClosedOrbitNPXError;
    static const double _smallClosedOrbitNPYError;


    LattFuncSage::tunes*     _tunes;
    EdwardsTengSage::Tunes*  _eigentunes;

    // Status flags
    bool _isCloned;
    bool _normalLattFuncsCalcd;
    bool _edwardstengFuncsCalcd;
    bool _dispersionFuncsCalcd;
    bool _momentsFuncsCalcd;
    bool _LBFuncsCalcd;
    bool _dispCalcd;

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
    void _createLBS();
    void _deleteLBS();
    void _createCOVS();
    void _deleteCOVS();
    void _createDSPS();
    void _deleteDSPS();
    void _createEigentunes();
    void _createClosedOrbit();
    void _deleteClosedOrbit();
};


inline bool BeamlineContext::isRing() const
{
  return (Sage::isRing(_p_bml));
}


inline bool BeamlineContext::isTreatedAsRing() const
{
  return ( beamline::ring == _p_bml->getLineMode() );
}


inline void BeamlineContext::handleAsRing()
{
  _p_bml->setLineMode( beamline::ring );
}


inline void BeamlineContext::handleAsLine()
{
  _p_bml->setLineMode( beamline::line );
}


inline Vector BeamlineContext::getParticleState()
{
  return (_particlePtr->State());
}


inline void BeamlineContext::loadParticleStateInto( Vector& s )
{
  s = _particlePtr->State();
}


inline double BeamlineContext::getParticle_x()
{
  return (_particlePtr->get_x());
}


inline double BeamlineContext::getParticle_y()
{
  return (_particlePtr->get_y());
}


inline double BeamlineContext::getParticle_cdt()
{
  return (_particlePtr->get_cdt());
}


inline double BeamlineContext::getParticle_npx()
{
  return (_particlePtr->get_npx());
}


inline double BeamlineContext::getParticle_npy()
{
  return (_particlePtr->get_npy());
}


inline double BeamlineContext::getParticle_ndp()
{
  return (_particlePtr->get_ndp());
}


inline void BeamlineContext::setParticle_x( double u )
{
  _particlePtr->set_x(u);
}


inline void BeamlineContext::setParticle_y( double u )
{
  _particlePtr->set_y(u);
}


inline void BeamlineContext::setParticle_cdt( double u )
{
  _particlePtr->set_cdt(u);
}


inline void BeamlineContext::setParticle_npx( double u )
{
  _particlePtr->set_npx(u);
}


inline void BeamlineContext::setParticle_npy( double u )
{
  _particlePtr->set_npy(u);
}


inline void BeamlineContext::setParticle_ndp( double u )
{
  _particlePtr->set_ndp(u);
}


#endif // BEAMLINECONTEXT_H
