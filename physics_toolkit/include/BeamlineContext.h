/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      BeamlineContext.h
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
#include <boost/function.hpp>
#include <basic_toolkit/complexAddon.h>
#include <mxyzptlk/Mapping.h>
#include <beamline/ElmPtr.h>
#include <beamline/BmlPtr.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/LatticeFunctions.h>

#include <sqlite/connection.hpp>

template <typename Particle_t>
class TBunch;
typedef TBunch<Particle> ParticleBunch;

class beamline;
class alignmentData;
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


class BeamlineContext {

  public:

    BeamlineContext( Particle const&, BmlPtr bml, std::string const& dbname ="");
   ~BeamlineContext();
 
    sqlite::connection& dbConnection() const;    

    void accept( ConstBmlVisitor& ) const;
    void accept(      BmlVisitor& );

    void                  setInitial( LattFuncs const& );
    LattFuncs const&      getInitial( );

    void writeTree();

    void   computeClosedOrbit( );
    void    computeDispersion( );  
    void computeCourantSnyder( );
    void   computeEdwardsTeng( );
    void    computeCovariance( );
    void    computeEigenmodes( );
    void    computeEigenTunes( );

    double  getHTune();
    double  getVTune();

    MatrixD equilibriumCovariance();
    void setAvgInvariantEmittance( double epsh, double epsv);   // Arguments are in units of pi mm-mr.

    Particle const& getParticle();
    void            setParticle(Particle const& );

    bool isRing() const;
    bool isTreatedAsRing() const;

    void handleAsRing();
    void handleAsLine();

    Mapping const& getOneTurnMap();  // Side effect: calculates closed orbit if not
    bool           onTransClosedOrbit( Particle const& ) const;

    Particle  getRefParticle() const;
    bool      hasReferenceParticle() const;


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

    int replaceElement( ElmPtr oldelm, ElmPtr newelm); // 0 everything went as planned
                                                       // 1 first argument was not found
                                                       // 2 at least one argument was null

    int processElements( boost::function<bool(bmlnElmnt &)> action ); // Returns number of elements processed.

    double getMomentum() const;

    int           countHowManyDeeply()       const;
    alignmentData getAlignmentData( ElmPtr ) const;


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


    // !!! Eventually, these three should become private !!!

    // Status flags
    static const int OKAY;
    static const int NO_TUNE_ADJUSTER;
    static const int NO_CHROMATICITY_ADJUSTER;

  private:
    
    void                 clear() const;

    bool       closed_orbit_ok() const; 
    bool         dispersion_ok() const;  
    bool     courant_snyder_ok() const;
    bool       edwards_teng_ok() const;
    bool         covariance_ok() const;
    bool         eigenmodes_ok() const;

    BeamlineContext( BeamlineContext const&); // forbidden

    BmlPtr                bml_;

    ChromaticityAdjuster* p_ca_;
    TuneAdjuster*         p_ta_;

    // Initial conditions

    LattFuncs             initialLattFunc_;
    bool                  initial_lattfunc_set_;

    double                dpp_;  // value of dp/p used for dispersion calculation. default = 0.0001
    double                eps1_; // invariant emittances in units of pi mm-mr.
    double                eps2_;


    JetParticle           oneturnjp_; 

    //   Used for computing equilibrium covariance  matrix.
    //   Default values: eps_1_ = 40, eps_2_ = 40.
    // Relation to <I> = <aa*>: 
    //   <I> [m]  =  1.0e-6 * ( eps_ [pi mm-mr]/2) / (beta*gamma)
    //   Note: because units are pi mm-mr, I divide by 2, not M_TWOPI
    // I assume that  eps1_ is "mostly horizontal" and eps2_ is
    //   "mostly vertical."


    //   ****************************FIXME !**************************************************************

 public:


    Particle*             particle_;    // we use a ptr here in order to preserve info about the actual particle type     
                                        // BAD !!! public beacsue of RayTrace::_pushParticle()':
    ParticleBunch*        particleBunchPtr_;   // BAD!!!! Needed because of DistributionWidget  

    //   ****************************FIXME !**************************************************************

 private:


            std::string               dbname_;
    mutable sqlite::connection        db_;

    static const double small_x_err;   // [m]   = 1.0e-9 
    static const double small_y_err;   // [m]   = 1.0e-9 
    static const double small_npx_err; // [rad] = 1.0e-9 
    static const double small_npy_err; // [rad] = 1.0e-9 
};

#endif // BEAMLINECONTEXT_H
