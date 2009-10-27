/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      BeamlineContext.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******                                                                
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
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
****** REVISION HISTORY
******
****** Mar 2007    ostiguy@fnal.gov
******                                                                
****** - use new-style STL compatible beamline iterators
****** - support for reference counted elements/beamlines
****** - Initialization optimizations
****** - Interface based on Particle/JetParticle references
****** - value-based semantics for Particle/JetParticles whenever possible
****** 
****** July 2007   ostiguy@fnal.gov
******
****** - eliminated all references to beamline::Action functor. 
******
****** June 2008   ostiguy@fnal.gov
****** - refactored implementation taking advantage of embedded database.  
******
**************************************************************************
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <algorithm>
#include <cstdio>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/dbutils.h>
#include <basic_toolkit/GenericException.h>

#include <beamline/beamline_elements.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/TBunch.h>
#include <beamline/LinacCavity.h>
#include <beamline/FramePusher.h>

#include <physics_toolkit/BmlUtil.h>
#include <physics_toolkit/BeamlineContext.h>
#include <physics_toolkit/Optics.h>

#include <sstream>
#include <sqlite/connection.hpp>
#include <sqlite/execute.hpp>

#include <boost/filesystem.hpp>   
#include <boost/filesystem/fstream.hpp> 

extern void      BeamlineSpitout( int, beamline const&, beamline::const_iterator &);
extern beamline* DriftsToSlots( beamline const& argbml );
extern bool      d2S_rbendLike( BmlnElmnt& x);

using namespace std;
using namespace boost;

using FNAL::pcerr;
using FNAL::pcout;


static double const small_x_err   = 1.0e-9;
static double const small_y_err   = 1.0e-9;
static double const small_npx_err = 1.0e-9;
static double const small_npy_err = 1.0e-9;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext::BeamlineContext( Particle const& p, beamline const& bml )
:   beamline(*bml.clone()), particle_(0) ,particleBunchPtr_(0) , 
    initialCSLattFuncs_() , 
    eps1_(40.0), eps2_(40.0), refjp_(p),
    dbname_(),
    db_()
{

   particle_         = p.clone();
   particleBunchPtr_ = new ParticleBunch( *particle_);


  // Initialize the internal particle
  // ----------------------------------

   particle_->setStateToZero();
   particle_->setRefMomentum( Momentum() );

 
   //-------------------------------------------------------------------
   // Determine whether some elements have faces not perpendicular 
   // to the reference trajectory. 
   // 
   // Currently ***disabled*** because DriftsToSlots does not preserve 
   // the hierarchical structure. In the future, this should be done
   // automatically.
   //-------------------------------------------------------------------

   bool convert_drifts_to_slots = false;

#if 1
   for (beamline::deep_iterator it  = deep_begin();  
                                it != deep_end(); ++it ) {

     if ( convert_drifts_to_slots = !(*it )->hasStandardFaces() ) break;
   }

   
    if ( convert_drifts_to_slots ) { 
      //DriftsToSlots( *this ); }       
      std::cout << "contains slots ! " << std::endl;
    }

#endif

   //-------------------------------------------------------------------
   // create and initialize the database
   //-------------------------------------------------------------------

   //-------------------------------------------------------------------
   // need a good portable way to generate unique tmp file names
   // on win32, it seems that the file may be created in the root directory
   // which is not generally writable
   // -----------------------------------------------------------------    
#ifdef _WIN32
   std::ostringstream address; 
   address << std::hex << this;
   dbname_ = string("./") + Name()+  address.str() + string(".db"); }  
#else

   dbname_ =   tempnam("/tmp", "chefdb" );  

   //std::ostringstream address; 
   //address << std::hex << this;
   //dbname_ =  string("./") + Name()+  address.str() + string(".db");  

   db_  =   shared_ptr<sqlite::connection>(new sqlite::connection(dbname_));
#endif

   Optics::initdb( *db_ );

   if(  isRing() ) { handleAsRing(); }
   else            { handleAsLine(); }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext::BeamlineContext( BeamlineContext const& o)
:   beamline( *o.beamline::clone()  ), 
    particle_(o.particle_->clone() ) ,
    particleBunchPtr_(0) , 
         initialCSLattFuncs_( o.initialCSLattFuncs_) , 
    eps1_(o.eps1_), 
    eps2_(o.eps2_), 
    refjp_( o.refjp_ ),
    dbname_ (),
    db_()
{
   //-------------------------------------------------------------------
   // create and initialize the database
   //-------------------------------------------------------------------
   
#ifdef _WIN32
   std::ostringstream address; 
   address << std::hex << this;
   dbname_ =  "./" + Name()+ address.str() + ".db";  
   db_     =  shared_ptr<sqlite::connection>( new  sqlite::connection(dbname_) );
#else
   dbname_ =   tempnam("/tmp", "chefdb" ); 
   db_     =   shared_ptr<sqlite::connection>(new sqlite::connection(dbname_));
#endif

   Optics::initdb( *db_ );

   if(  isRing() ) { handleAsRing(); }
   else            { handleAsLine(); }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext* BeamlineContext::clone() const
{ 
  return new BeamlineContext(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext::~BeamlineContext()
{

 boost::filesystem::path dbpathname(dbname_ ); 
 //boost::filesystem::remove( dbpathname );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::saveDatabase( std::string const& dbname) const
{
  std::string sql("BEGIN IMMEDIATE TRANSACTION");
  sqlite::execute(*db_, sql, true );

  boost::filesystem::path to_path(dbname ); 
  if ( boost::filesystem::exists(to_path) ) boost::filesystem::remove( to_path);
  
  boost::filesystem::copy_file( dbname_, dbname);

  sql = "ROLLBACK";
  sqlite::execute(*db_, sql, true );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::setParticle( Particle const& p) 
{ 
  particle_ = p.clone();
  clear();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const*  BeamlineContext::dbname() const
{ 
  return dbname_.c_str();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setInitialCS( CSLattFuncs const& u )
{
  initialCSLattFuncs_       = u;
  clear();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setInitialCS4D( CSLattFuncs4D const& u )
{
  initialCS4DLattFuncs_       = u;
  clear();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs const& BeamlineContext::getInitialCS()
{
  if  ( !initialCSLattFuncs_.defined() && isTreatedAsRing() ) { periodicCourantSnyder2D(); }
  return initialCSLattFuncs_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs4D const& BeamlineContext::getInitialCS4D()
{
  return initialCS4DLattFuncs_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping const& BeamlineContext::getOneTurnMap()
{
  return refjp_.state();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::hasReferenceParticle() const
{
  return true; //FIXME  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::writeTree()
{
  beamline::const_iterator it =  begin();
  BeamlineSpitout( 0, *this, it );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setAvgInvariantEmittance( double x, double y )
{
  // Note: it is assumed that x and y are
  //       in units of pi mm-mr.

   eps1_ = std::abs(x);
   eps2_ = std::abs(y);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::onTransClosedOrbit( Particle const& p ) const
{

  Particle probe(p);
  propagate( probe );

  if(    ( abs( p.x()   - probe.x()  ) <  small_x_err   )
      && ( abs( p.y()   - probe.y()  ) <  small_y_err   )
      && ( abs( p.npx() - probe.npx()) <  small_npx_err )
      && ( abs( p.npy() - probe.npy()) <  small_npy_err ) ) 
  {
    return true;
  }

  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle const& BeamlineContext::getRefParticle() const
{
  return *particle_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle const& BeamlineContext::getParticle()
{
  return *particle_; /// WHAT IS THIS FOR ????
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::periodicReferenceOrbit()
{
  if( reference_orbit_ok() )  return; 

   refjp_ = Optics::find_closed_orbit( (*db_), *this, JetParticle(*particle_) );  
   Particle p(refjp_);
   registerReference(p);
   Optics::orbit( (*db_), *this,  p);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::propagateReferenceOrbit()
{

   if( reference_orbit_ok() )  return; 

   Particle p(*particle_); 

   Vector& state = p.state();

   state[0] =  initialCSLattFuncs_.orbit.x;
   state[3] =  initialCSLattFuncs_.orbit.xp;

   state[1] =  initialCSLattFuncs_.orbit.y;
   state[4] =  initialCSLattFuncs_.orbit.yp;

   state[2] =  initialCSLattFuncs_.orbit.cdt;
   state[5] =  initialCSLattFuncs_.orbit.ndp;

   refjp_ = JetParticle(p);

   registerReference(p);

   Optics::orbit( (*db_), *this, p );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector BeamlineContext::chromaticity() const
{
     Vector eta0 = Optics::periodicDispersion( (*db_), refjp_ );  
     Vector chromas = Optics::chromaticity( (*db_), *this, refjp_, eta0 );

  return chromas;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double BeamlineContext::getHTune()
{
  computeEigenTunes();
  return Optics::getHTune( (*db_ ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getVTune()
{
  computeEigenTunes();
  return Optics::getVTune( (*db_ ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::computeEigenTunes()
{

  if( !reference_orbit_ok() ) {
    try {
      periodicReferenceOrbit(); 
    }
    catch( GenericException const& ge ) {
      throw ge;
    }
  }
 
  // FIXME: this should not be called if the tunes have already been computed.

  Optics::tunes( (*db_), refjp_ );  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  BeamlineContext::periodicCourantSnyder2D()
{

  if ( !reference_orbit_ok() )  { periodicReferenceOrbit(); }

  if ( courant_snyder2d_ok() ) return;
  
   if ( isTreatedAsRing() ){
     CSLattFuncs lf = initialCSLattFuncs_ = Optics::periodicCourantSnyder2D( (*db_), refjp_ );  
     Optics::propagateCourantSnyder2D( (*db_), *this, refjp_, lf );
   }  
   else {
      throw GenericException( __FILE__, __LINE__, 
           "BeamlineContext::periodicCourantSnyder2D()", 
           "Beamline is not a ring. ");
   }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::propagateCourantSnyder2D()
{
 if ( !reference_orbit_ok() ) { propagateReferenceOrbit(); }

 Optics::propagateCourantSnyder2D( (*db_), *this, refjp_,  initialCSLattFuncs_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::periodicEdwardsTeng()
{
  if ( edwards_teng_ok() ) return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::propagateEdwardsTeng()
{
  if ( edwards_teng_ok() ) return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::periodicCovariance()
{
  if ( covariance_ok() ) return;
  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::propagateCovariance()
{
  if ( covariance_ok() ) return;
  return;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::periodicCourantSnyder4D()
{
  try {

    if ( !reference_orbit_ok() ) { periodicReferenceOrbit(); }

    if ( isTreatedAsRing() ){
      if ( courant_snyder4d_ok() ) return;
      CSLattFuncs4D lf = Optics::periodicCourantSnyder4D( (*db_), refjp_ );  
      Optics::propagateCourantSnyder4D( (*db_), *this, refjp_, lf );
    }  
    else { 
      throw GenericException( __FILE__, __LINE__, 
           "BeamlineContext::periodicCourantSnyder4D()", 
           "Beamline is not a ring. ");
    }
  }// try
  
  catch( GenericException const& ge ) {
       throw ge;
  }


}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::propagateCourantSnyder4D()
{

  if ( courant_snyder4d_ok() )  return;

  if ( !reference_orbit_ok() ) { propagateReferenceOrbit(); }

  try {

    CSLattFuncs4D lf(initialCSLattFuncs_);

    // if( !reference_orbit_ok() ) { periodicReferenceOrbit(); }
 
    Optics::propagateCourantSnyder4D( (*db_), *this, refjp_, CSLattFuncs4D(initialCSLattFuncs_) );

  }// try
  
  catch( GenericException const& ge ) {
       throw ge;
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BeamlineContext::periodicDispersion()
{
  
   if ( !reference_orbit_ok() )  { periodicReferenceOrbit(); }

   Particle p(*particle_); 

   if ( isTreatedAsRing() ){
     Vector eta0 = Optics::periodicDispersion( (*db_), refjp_ );  
     Optics::propagateDispersion( (*db_), *this, refjp_, eta0 );
   }  
   else {

      Vector eta0 = initialCSLattFuncs_.dispersion.eta;  
      Optics::propagateDispersion( (*db_), *this, refjp_, eta0 );

      // throw GenericException( __FILE__, __LINE__, 
      //   "BeamlineContext::periodicDispersion()", 
      //   "Beamline is not a ring (periodic).");
   }
  

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BeamlineContext::propagateDispersion()
{
     if ( !reference_orbit_ok() )  { propagateReferenceOrbit(); }

     Vector eta0 = initialCSLattFuncs_.dispersion.eta;  
     Optics::propagateDispersion( (*db_), *this, refjp_, eta0 );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD BeamlineContext::equilibriumCovariance()
{
  // FIXME : need to normalize w/r to eps1_, eps2_ ;
  return equilibriumCovariance();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BeamlineContext::clear()
{
  Optics:: cleardb(*db_);
  refjp_ = JetParticle(*particle_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  BeamlineContext::reference_orbit_ok() const
{
  return Optics::reference_orbit_ok( *db_);
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     BeamlineContext::dispersion_ok() const
{  
  return Optics::dispersion_ok( *db_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     BeamlineContext::eigenvectors_ok() const
{  
  return Optics::eigenvectors_ok( *db_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    BeamlineContext::courant_snyder2d_ok() const
{
  return Optics::courant_snyder2d_ok( *db_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool        BeamlineContext::edwards_teng_ok() const
{
  return Optics::edwards_teng_ok( *db_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::covariance_ok() const
{
  return Optics::covariance_ok( *db_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  BeamlineContext::courant_snyder4d_ok() const
{
  return Optics::courant_snyder4d_ok( *db_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::handleAsRing()
{ 
  setLineMode( beamline::ring );
  clear();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::handleAsLine() 
{ 
  setLineMode( beamline::line );
  clear();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::isTreatedAsRing() const
{
  return (beamline::ring == getLineMode() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::isTreatedAsLine() const
{
  return (beamline::ring == getLineMode() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::isRing() const
{ 
  // This test will return true for pathologically short lines
  //   like a beamline consisting of a single 1 mm drift.

  double const gtol  = std::abs(0.005); // = 5 mm  
  double const atol  = std::abs(0.001); // = 1 mrad

  FramePusher fp;
  accept( fp );

  // Check the point of return

  Vector r = fp.getFrame().getOrigin();

  for( int i = 0; i < 3; ++i) {
    if( gtol < std::abs(r[i]) ) { return false; }
  }

  // Check the angle of return
  //   Tolerance is hardwired to 1 milliradian
  MatrixD fv = fp.getFrame().getAxes();

  for( int i=0; i < 3; ++i ) {
    for( int j=0; j < 3; ++j ) {
      if( i != j ) {
        if( atol < std::abs(fv[i][j]) ) { return false; }
      }
    }
  }
  
  return true;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::arclength() const
{
  return Optics::arclength(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::gamma() const
{
  return Optics::gamma(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::beta_x() const
{
  return Optics::beta_x(dbname_);
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::beta_y() const
{  
  return Optics::beta_y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::alpha_x() const
{
  return Optics::alpha_x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::alpha_y() const
{
  return Optics::alpha_y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::psi_x() const
{
  return Optics::psi_x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::psi_y() const
{
  return Optics::psi_y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::beta_1x() const
{
  return Optics::beta_1x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::beta_1y() const
{
  return Optics::beta_1y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::alpha_1x() const
{
  return Optics::alpha_1x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::alpha_1y() const
{
  return Optics::alpha_1y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::beta_2x() const
{
  return Optics::beta_2x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::beta_2y() const
{
  return Optics::beta_2y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::alpha_2x() const
{
  return Optics::alpha_2x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::alpha_2y() const
{
  return Optics::alpha_2y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::eta_x() const
{  
  return Optics::eta_x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::eta_y() const
{
  return Optics::eta_y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::etap_x() const
{
   return Optics::etap_x(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> BeamlineContext::etap_y() const
{     
  return Optics::etap_y(dbname_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
