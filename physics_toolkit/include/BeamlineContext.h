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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
******
****** - support for reference counted beamlines/elements/BeamlineContext
****** - Particles/JetParticles members stored by value whenever possible
******
**************************************************************************
*************************************************************************/

#ifndef BEAMLINECONTEXT_H
#define BEAMLINECONTEXT_H

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <basic_toolkit/complexAddon.h>
#include <mxyzptlk/Mapping.h>
#include <physics_toolkit/LattFuncSage.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
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
class quadrupole;
class thinQuad;
class sextupole;
class thinSextupole;

typedef boost::shared_ptr<quadrupole>    QuadrupolePtr;
typedef boost::shared_ptr<thinQuad>      ThinQuadPtr;
typedef boost::shared_ptr<sextupole>     SextupolePtr;
typedef boost::shared_ptr<thinSextupole> ThinSextupolePtr;

class  BeamlineContext;

typedef boost::shared_ptr<BeamlineContext>            BmlContextPtr;
typedef boost::shared_ptr<BeamlineContext const>  ConstBmlContextPtr;


class BeamlineContext
{
  public:

    BeamlineContext( Particle const&, BmlPtr bml );
   ~BeamlineContext();

    int assign(  BmlPtr bml);

    void accept( ConstBmlVisitor& ) const;
    void accept( BmlVisitor& );

    void setInitialDispersion( DispersionSage::Info const& );
    DispersionSage::Info const& getInitialDispersion();

    void setInitialTwiss( LattFuncSage::lattFunc const& );
    LattFuncSage::lattFunc const& getInitialTwiss();

    void setInitialCovariance( CovarianceSage::Info const& );
    CovarianceSage::Info const& getInitialCovariance();

    void writeTree();


    // Beamline methods

    std::string name() const;
    void rename( const char* );
    void peekAt( double& s, Particle const& ) const;
    double sumLengths() const;

    int setLength   ( ElmPtr, double );
    int setStrength ( ElmPtr, double );
    int setAlignment( ElmPtr, alignmentData const& );

    // I really want to get rid of AlignmentData altogether!

    int setAlignment( alignmentData const& u, boost::function<bool(bmlnElmnt const&)> criterion);

    int replaceElement( ElmPtr , ElmPtr );
    // Will replace the first argument with
    // the second. Return values:
    // 0 everything went as planned
    // 1 first argument was not found
    // 2 at least one argument was null
    // 
    // WARNING: Element *a will be deleted if the beamline has
    // been cloned. It is assumed that *b has been created on the
    // heap, and the BeamlineContext takes over ownership.

    int processElements( boost::function<bool(bmlnElmnt &)> action );
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
    alignmentData getAlignmentData( ElmPtr ) const;

    Mapping getOneTurnMap();
    // Returns the one turn map; 
    // Side effect: calculates closed orbit if not
    // already done.
    bool onTransClosedOrbit( const Particle& ) const;

    bool hasReferenceParticle() const;
    void setReferenceParticle( Particle const& );
    int  getReferenceParticle( Particle& ) const;

    // Returns -1 if no reference particle is available.
    Particle const& getParticle();

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

    std::vector<LattFuncSage::lattFunc> const& getTwissArray();
    std::vector<EdwardsTengSage::Info>  const& getETArray();
    std::vector<CovarianceSage::Info>   const& getCovarianceArray();
    std::vector<LBSage::Info>           const& getLBArray();
    std::vector<DispersionSage::Info>   const& getDispersionArray();

    MatrixD equilibriumCovariance();
    MatrixD equilibriumCovariance( double, double );

    // Arguments are the "invariant emittances" -
    //   essentially two scaled action coordinates -
    //   in pi mm-mr units. The first is "mostly horizontal," 
    //   the second, "mostly vertical."
    //   (See notes on eps1_ and eps2_ below.)
    // This routine returns a covariance matrix over
    //   the full phase space, but only the transverse
    //   components should be believed. It assumes that
    //   the longitudinal actions are zero.
    // In the first form, the values of member data
    //   eps1_ and eps2_ are used for the arguments.

    // Adjuster methods

    void addHTuneCorrector( QuadrupolePtr );
    void addHTuneCorrector( ThinQuadPtr );
    void addHTuneCorrector( ElmPtr );          // Used by  GUI. MUST go away ! 

    void addVTuneCorrector( QuadrupolePtr );
    void addVTuneCorrector( ThinQuadPtr );
    void addVTuneCorrector( ElmPtr );          // Used by  GUI. MUST go away ! 
 
    int changeTunesBy( double, double );

    void addHChromCorrector( SextupolePtr );
    void addHChromCorrector( ThinSextupolePtr );
    void addHChromCorrector( ElmPtr );         // Used by  GUI. MUST go away ! 

    void addVChromCorrector( SextupolePtr );
    void addVChromCorrector( ThinSextupolePtr );
    void addVChromCorrector( ElmPtr );         // Used by  GUI. MUST go away ! 

    int changeChromaticityBy( double, double );

    // Iterator functions

    ConstBmlPtr cheatBmlPtr() const;

    // Stream functions
    friend std::ostream& operator<<( std::ostream&, const BeamlineContext& );
    friend std::istream& operator>>( std::istream&,       BeamlineContext& );

    // !!! Eventually, these three should become private !!!

    // Status flags
    static const int OKAY;
    static const int NO_TUNE_ADJUSTER;
    static const int NO_CHROMATICITY_ADJUSTER;

  private:

    BeamlineContext( BeamlineContext const&); // forbidden

    BmlPtr                p_bml_;

    LattFuncSage*         p_lfs_;
    EdwardsTengSage*      p_ets_;
    CovarianceSage*       p_covs_;
    LBSage*               p_lbs_;
    ClosedOrbitSage*      p_cos_;
    DispersionSage*       p_dsps_;
    ChromaticityAdjuster* p_ca_;
    TuneAdjuster*         p_ta_;

    // Initial conditions
    LattFuncSage::lattFunc initialLattFunc_;
    DispersionSage::Info   initialDispersion_;
    CovarianceSage::Info   initialCovariance_;

    double                dpp_; // value of dp/p used for dispersion calculation. default = 0.0001
    double                eps1_, eps2_;
    // Average invariant emittances in units of pi mm-mr.
    //   Used for computing equilibrium covariance  matrix.
    //   Default values: eps_1_ = 40, eps_2_ = 40.
    // Relation to <I> = <aa*>: 
    //   <I> [m]  =  1.0e-6 * ( eps_ [pi mm-mr]/2) / (beta*gamma)
    //   Note: because units are pi mm-mr, I divide by 2, not M_TWOPI
    // I assume that  eps1_ is "mostly horizontal" and eps2_ is
    //   "mostly vertical."

 public:
    Particle*             particle_;    // we use a ptr here in order to preserve info about the actual particle type     
                                        // BAD !!! public beacsue of RayTrace::_pushParticle()':
    ParticleBunch*        particleBunchPtr_;   // BAD!!!! Needed because of DistributionWidget  
 private:

    Particle               co_part_; 
    // once created, holds initial state for
    // (transverse) closed orbit.

    Particle               disp_part_;
    // same as above, but at reference energy*(1+dp/p)

    JetParticle            jetparticle_;

    // once created, its state always contains
    // one traversal of the beamline at the
    // reference energy on the closed orbit.

    // "small" numbers for testing orbit closure
 
    static const double smallClosedOrbitXError;
    static const double smallClosedOrbitYError;
    static const double smallClosedOrbitNPXError;
    static const double smallClosedOrbitNPYError;


    LattFuncSage::tunes      tunes_;
    EdwardsTengSage::Tunes   eigentunes_;

    // Status flags

    bool normalLattFuncsCalcd_;
    bool edwardstengFuncsCalcd_;
    bool dispersionFuncsCalcd_;
    bool momentsFuncsCalcd_;
    bool LBFuncsCalcd_;
    bool dispCalcd_;

    bool hasRefParticle_;
    bool closed_orbit_computed_;
    bool tunes_computed_;
    bool eigentunes_computed_;
    bool initial_lattfunc_set_;
    bool initial_dispersion_set_;
    bool initial_covariance_set_;

    // Operations
 
    void createTunes();
    void createLFS();
    void deleteLFS();
    void createETS();
    void deleteETS();
    void createLBS();
    void deleteLBS();
    void createCOVS();
    void deleteCOVS();
    void createDSPS();
    void deleteDSPS();
    void createEigentunes();
    void createClosedOrbit();
    void deleteClosedOrbit();

};

#endif // BEAMLINECONTEXT_H
