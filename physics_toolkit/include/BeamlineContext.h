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
****** Aug 2008           ostiguy@fnal.gov
****** - completely rewritten version using embedded database. 
******
**************************************************************************
*************************************************************************/
#ifndef BEAMLINECONTEXT_H
#define BEAMLINECONTEXT_H

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <mxyzptlk/Mapping.h>
#include <beamline/ElmPtr.h>
#include <beamline/BmlPtr.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/LatticeFunctions.h>
#include <beamline/beamline.h>

#include <sqlite/connection.hpp>

template <typename Particle_t>
class TBunch;
typedef TBunch<Particle> ParticleBunch;

class beamline;
class alignmentData;
class ChromaticityAdjuster;
class TuneAdjuster;
class ConstBmlVisitor;
class BmlVisitor;

class BmlnElmnt;
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


class BeamlineContext: public beamline {

 public: 

    BeamlineContext( Particle const&, beamline const& bml );
   ~BeamlineContext();

    BeamlineContext* clone() const;

    char const*  dbname() const;

    void saveDatabase( std::string const& dbname) const;

    void                  setInitialCS( CSLattFuncs const& );
    CSLattFuncs const&    getInitialCS( );

    void                  setInitialCS4D( CSLattFuncs4D const& );
    CSLattFuncs4D const&  getInitialCS4D( );

    void handleAsRing();
    void handleAsLine();
    bool isTreatedAsRing() const;
    bool isTreatedAsLine() const;
    bool isRing() const; 
   
    void processElements( boost::function<void(ElmPtr)> action );

    void writeTree();

    void computeEigenTunes( );

    void  periodicReferenceOrbit( );
    void  periodicDispersion( );  
    void  periodicCourantSnyder2D( );
    void  periodicCourantSnyder4D();
    void  periodicEdwardsTeng( );
    void  periodicCovariance( );

    void  propagateReferenceOrbit( );
    void  propagateDispersion( );  
    void  propagateCourantSnyder2D( );
    void  propagateCourantSnyder4D();
    void  propagateEdwardsTeng( );
    void  propagateCovariance( );

    std::vector<double>  arclength ( ) const;
    std::vector<double>       gamma( ) const;
    std::vector<double>     beta_x ( ) const;
    std::vector<double>     beta_y ( ) const;
    std::vector<double>    alpha_x ( ) const;
    std::vector<double>    alpha_y ( ) const;
    std::vector<double>      psi_x ( ) const;
    std::vector<double>      psi_y ( ) const;
    std::vector<double>    beta_1x ( ) const;
    std::vector<double>    beta_1y ( ) const;
    std::vector<double>   alpha_1x ( ) const;
    std::vector<double>   alpha_1y ( ) const;
    std::vector<double>    beta_2x ( ) const;
    std::vector<double>    beta_2y ( ) const;
    std::vector<double>   alpha_2x ( ) const;
    std::vector<double>   alpha_2y ( ) const;
    std::vector<double>      eta_x ( ) const;
    std::vector<double>      eta_y ( ) const;
    std::vector<double>     etap_x ( ) const;
    std::vector<double>     etap_y ( ) const;


    double  getHTune();
    double  getVTune();

    MatrixD equilibriumCovariance();
    void setAvgInvariantEmittance( double epsh, double epsv);   // Arguments are in units of pi mm-mr.

    Particle const& getParticle();
    void            setParticle(Particle const& );


    Mapping const& getOneTurnMap();  // Side effect: calculates closed orbit if not
    bool           onTransClosedOrbit( Particle const& ) const;

    Particle const& getRefParticle()       const;
    bool            hasReferenceParticle() const;


    Vector   chromaticity() const;

    void     clear();

  private:
    

    bool              is_linac() const;
    bool    reference_orbit_ok() const; 
    bool         dispersion_ok() const;  
    bool   courant_snyder2d_ok() const;
    bool   courant_snyder4d_ok() const;
    bool       edwards_teng_ok() const;
    bool         covariance_ok() const;
    bool       eigenvectors_ok() const;



    CSLattFuncs           initialCSLattFuncs_;
    CSLattFuncs4D         initialCS4DLattFuncs_;

    double                eps1_; // invariant emittances in units of pi mm-mr.
    double                eps2_;


    JetParticle           refjp_;  // reference jet particle (for a ring, 1-turn map;  for a line, identity map) 

    //   Used for computing equilibrium covariance  matrix.
    //   Default values: eps_1_ = 40, eps_2_ = 40.
    // Relation to <I> = <aa*>: 
    //   <I> [m]  =  1.0e-6 * ( eps_ [pi mm-mr]/2) / (beta*gamma)
    //   Note: because units are pi mm-mr, I divide by 2, not M_TWOPI
    // I assume that  eps1_ is "mostly horizontal" and eps2_ is
    //   "mostly vertical."


    //   ****************************FIXME !**************************************************************

 public:


    Particle*             particle_;            // we use a ptr here in order to preserve info about the actual particle type     
                                                // BAD !!! public because of RayTrace::_pushParticle()':
    ParticleBunch*        particleBunchPtr_;    // BAD!!!! Needed because of DistributionWidget  

    //   ****************************FIXME !**************************************************************

 private:

            std::string                          dbname_;
    mutable boost::shared_ptr<sqlite::connection>    db_;

    BeamlineContext( BeamlineContext const& );

};

#endif // BEAMLINECONTEXT_H
