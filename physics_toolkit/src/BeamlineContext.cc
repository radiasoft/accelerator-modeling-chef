/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      BeamlineContext.cc
******  Version:   2.2
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
****** - stack variables for Particle/JetParticles whenever possible
****** 
****** July 2007   ostiguy@fnal.gov
******
****** - eliminated all references to beamline::Action functor. 
******
**************************************************************************
*************************************************************************/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>

#include <beamline/beamline_elements.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/RefRegVisitor.h>

#include <physics_toolkit/BmlUtil.h>
#include <physics_toolkit/BeamlineContext.h>
#include <physics_toolkit/ClosedOrbitSage.h>
#include <physics_toolkit/ChromaticityAdjuster.h>
#include <physics_toolkit/TuneAdjuster.h>

extern void BeamlineSpitout( int, beamline::const_iterator &);

using namespace std;
using namespace boost;

using FNAL::pcerr;
using FNAL::pcout;

const int BeamlineContext::OKAY                     = 0;
const int BeamlineContext::NO_TUNE_ADJUSTER         = 1;
const int BeamlineContext::NO_CHROMATICITY_ADJUSTER = 2;

const double BeamlineContext::smallClosedOrbitXError   /* [m] */ = 1.0e-9;
const double BeamlineContext::smallClosedOrbitYError   /* [m] */ = 1.0e-9;
const double BeamlineContext::smallClosedOrbitNPXError /*[rad]*/ = 1.0e-9;
const double BeamlineContext::smallClosedOrbitNPYError /*[rad]*/ = 1.0e-9;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext::BeamlineContext( Particle const& w, BmlPtr x )
:   p_bml_(x)
  , particle_(0)
  , particleBunchPtr_(0)
  , p_lfs_(0)
  , p_ets_(0)
  , p_covs_(0)
  , p_lbs_(0)
  , p_cos_(0)
  , p_dsps_(0)
  , p_ca_(0)
  , p_ta_(0)
  , co_part_(w)
  , disp_part_(w)
  , initialLattFunc_()
  , initialDispersion_()
  , initialCovariance_()
  , dpp_(0.0001)
  , jetparticle_(w)
  , eps1_(40.0)
  , eps2_(40.0)
  , normalLattFuncsCalcd_(false)
  , edwardstengFuncsCalcd_(false)
  , momentsFuncsCalcd_(false)
  , LBFuncsCalcd_(false)
  , dispersionFuncsCalcd_(false)
  , dispCalcd_(false)
  , closed_orbit_computed_(false)
  , tunes_computed_(false)
  , eigentunes_computed_(false)
  , initial_lattfunc_set_(false)
  , initial_dispersion_set_(false)
  , initial_covariance_set_(false)
{

  particle_ = w.Clone();

  // Try to instantiate _theSamplePtr
  // --------------------------------
 
  if( typeid(w) == typeid(Proton) ) {
    particleBunchPtr_ = new ParticleBunch( *particle_);
  }
  else if( typeid(w) == typeid(Positron) ) {
    particleBunchPtr_ = new ParticleBunch( *particle_);
  }
  else {
    throw GenericException( __FILE__, __LINE__, 
           "BeamlineContext::BeamlineContext( Particle w&, BmlPtr )", 
           "*** SORRY ***"
           "\n*** SORRY *** This version of BeamlinContext only handles"
           "\n*** SORRY *** positrons and protons, principally because."
           "\n*** SORRY *** this version of ParticleBunch is restricted"
           "\n*** SORRY *** to those classes."
           "\n*** SORRY ***"  );
  }


  // If that succeeds, then continue 
  // -------------------------------

  if( !x ) {
    throw( GenericException( __FILE__, __LINE__, 
           "BeamlineContext::BeamlineContext( Particle const& w, BmlPtr x )", 
           "Invoked with null beamline pointer." ) );
  }


  // Reinitialize the internal particle
  // ----------------------------------

   particle_->setStateToZero();
   particle_->SetReferenceEnergy( p_bml_->Energy() );


   if( Sage::isRing( p_bml_) ) { handleAsRing(); }
   else                        { handleAsLine(); }
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext::~BeamlineContext()
{
 
  reset();

  if( particleBunchPtr_ ) { delete particleBunchPtr_; particleBunchPtr_ = 0; }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::reset()
{

  deleteLFS();
  deleteETS();
  deleteCOVS();
  deleteLBS();
  deleteDSPS();
  deleteClosedOrbit();

  if( p_ca_   ) { delete  p_ca_;  p_ca_ = 0; }
  if( p_ta_   ) { delete  p_ta_;  p_ta_ = 0; }

  particle_->setStateToZero();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::assign( BmlPtr x )
{
  static bool firstTime = true;
  if( p_bml_ ) 
  { if( firstTime ) 
    { (*pcerr) << "\n*** WARNING *** BeamlineContext::assign "
              "invoked illegally."
              "\n*** WARNING *** This message appears only once.\n"
           << endl;
      firstTime = false;
    }
    return 1; 
  }
  
  if( !x ) { return 2;}


  p_bml_ = x;

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::isRing() const 
{ 
  return Sage::isRing(p_bml_);                       
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::isTreatedAsRing() const 
{ 
  return (beamline::ring == p_bml_->getLineMode() ); 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::handleAsRing()          
{ 
  p_bml_->setLineMode( beamline::ring );             
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::handleAsLine() 
{ 
  p_bml_->setLineMode( beamline::line );             
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector BeamlineContext::getParticleState()    
{ 
  return particle_->State();                          
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::loadParticleStateInto( Vector& s ) 
{ 
  s = particle_->State();                  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getParticle_x()       
{ 
 return   particle_->get_x();                        
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getParticle_y()       
{ 
  return  particle_->get_y();                         
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getParticle_cdt()     
{ 
  return  particle_->get_cdt();                       
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getParticle_npx()     
{ 
  return particle_->get_npx();                        
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getParticle_npy()     
{ 
  return particle_->get_npy();                        
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getParticle_ndp()     
{ 
  return  particle_->get_ndp();                       
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticle_x( double u ) 
{ 
  particle_->set_x(u);                              
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticle_y( double u ) 
{ 
  particle_->set_y(u);                              
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticle_cdt( double u ) 
{ 
  particle_->set_cdt(u);                          
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticle_npx( double u ) 
{ 
  particle_->set_npx(u);                          
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticle_npy( double u ) 
{ 
  particle_->set_npy(u);                          
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticle_ndp( double u ) 
{ 
  particle_->set_ndp(u);                          
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::accept( ConstBmlVisitor& x ) const
{
   p_bml_->accept(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::accept( BmlVisitor& x )
{
  p_bml_->accept(x);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setInitialDispersion( DispersionSage::Info const& u )
{
  initialDispersion_      = u;
  initial_dispersion_set_ = true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


DispersionSage::Info const& BeamlineContext::getInitialDispersion()
{

 if( !initial_dispersion_set_ ) { 
    throw( GenericException( __FILE__, __LINE__, 
             "BeamlineContext::getInitial( DispersionSage::Info* )",
             "Initial conditions for dispersion are not available." ) );
  }

  return initialDispersion_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setInitialCovariance( CovarianceSage::Info const& u )
{

  initialCovariance_       = u;
  initial_covariance_set_  = true;  
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CovarianceSage::Info const& BeamlineContext::getInitialCovariance()
{

 if( !initial_covariance_set_ ) { 
    throw( GenericException( __FILE__, __LINE__, 
             "BeamlineContext::getInitial( CovarianceSage::Info* )",
             "Initial conditions for covariance are not available." ) );
  }

  return initialCovariance_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setInitialTwiss( LattFuncSage::lattFunc const& u )
{
  initialLattFunc_      = u;
  initial_lattfunc_set_ = true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LattFuncSage::lattFunc const& BeamlineContext::getInitialTwiss()
{

 if( !initial_lattfunc_set_ ) { 
    throw( GenericException( __FILE__, __LINE__, 
             "BeamlineContext::getInitial( LattFuncSage::lattFunc* )",
             "Initial conditions for uncoupled \n"
             "lattice functions are not available." ) );
  }
  return initialLattFunc_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::writeTree()
{
  beamline::const_iterator it =  p_bml_->begin();
  BeamlineSpitout( 0, it );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Beamline Functions

std::string BeamlineContext::name() const
{
  return p_bml_->Name();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::rename( const char* newname )
{
   p_bml_->rename( newname );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::peekAt( double& s, const Particle& p ) const
{
   p_bml_->peekAt( s, p );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::sumLengths() const
{
  double ret = 0.0;
  

  for (beamline::const_deep_iterator it  =  boost::static_pointer_cast<const beamline>(p_bml_)->deep_begin(); 
                                     it !=  boost::static_pointer_cast<const beamline>(p_bml_)->deep_end(); ++it ) {
   ret += (*it)->Length(); 
  }
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setLength( ElmPtr w, double l )
{
  bool      found = false;
  int       ret   = 0;

  for ( beamline::deep_iterator it  =  p_bml_->deep_begin();
	it != p_bml_->deep_end(); ++it ) {

    if ( (*it) !=  w ) continue;
 
    if ( l != (*it)->Length() ) { (*it)->setLength(l); reset(); } // This is extreme.
  }

  return found ? ret : 1; 

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setStrength( ElmPtr w, double s )
{

  bool found  = false;
  int  ret    = 0;


  for ( beamline::deep_iterator it  = p_bml_->deep_begin();
	it != p_bml_->deep_end(); ++it ) {

    if( (*it) != w )  continue;

    if( s != (*it)->Strength() ) { 
        (*it)->setStrength(s);
      }
  }

  return found ? ret : 1;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setAvgInvariantEmittance( double x, double y )
{
   eps1_ = std::abs(x);
   eps2_ = std::abs(y);
  // Note: it is assumed that x and y are
  //       in units of pi mm-mr.
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getEnergy() const
{
  return p_bml_->Energy();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::countHowManyDeeply() const
{
  return  p_bml_->countHowManyDeeply();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setAlignment( ElmPtr v, alignmentData const& u )
{
  static const int errCode_nullArg  = 1;
  static const int errCode_notFound = 2;

  if( v == 0 )                   { return errCode_nullArg; }
  if( 0 == p_bml_->contains(v) ) { return errCode_notFound; }

  v->setAlignment( u );
  // ??? Check for passive element!!!

  deleteLFS(); // ??? Too conservative
  deleteClosedOrbit();

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setAlignment( alignmentData const& u, boost::function<bool(bmlnElmnt const&)> criterion)
{
  int ret = 0;

  for ( beamline::deep_iterator it  = p_bml_->deep_begin();
	it != p_bml_->deep_end(); ++it ) {

    if ( !criterion( **it )  ) continue;

    ++ret;
    (*it)->setAlignment( u );
   
  }

  if( ret ) {
    deleteLFS(); 
    deleteClosedOrbit();
  }

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::processElements( boost::function<bool(bmlnElmnt &)> action )
{

  int ret = 0;

  for ( beamline::deep_iterator it  = p_bml_->deep_begin();
	it != p_bml_->deep_end(); ++it ) {

    if ( !action( **it) )   continue;
    
    ++ret;
   
  }

  if( ret ) {
    deleteLFS(); // ??? Too conservative
    deleteClosedOrbit();
  }

  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::replaceElement( ElmPtr a, ElmPtr b )
{
  // Will replace the first argument with
  // the second. Return values:
  // 0 everything went as planned
  // 1 first argument was not found
  // 2 at least one argument was null
  // 

  int ret = p_bml_->replace(a,b);

  if( 0 == ret ) { reset(); }

  // This step will not always be necessary.

  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData BeamlineContext::getAlignmentData( ElmPtr v ) const
{
  static const alignment err1Ret(  137.,  137.,  137. );
  static const alignment err2Ret( -137., -137., -137. );

  if( v == 0 )                   { return err1Ret.getAlignment(); }
  if( 0 == p_bml_->contains(v) ) { return err2Ret.getAlignment(); }

  return v->Alignment();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping BeamlineContext::getOneTurnMap()
{

  if( !closed_orbit_computed_ ) {
    try { createClosedOrbit(); 
    }
    catch( GenericException const& ge ) {
      deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }

  closed_orbit_computed_ = true;

  return jetparticle_.State();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ConstBmlPtr BeamlineContext::cheatBmlPtr() const
{

  return p_bml_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Sage operations

void BeamlineContext::deleteLFS()
{


  if( p_lfs_ ) { p_lfs_->eraseAll(); delete p_lfs_; p_lfs_ = 0; }

  normalLattFuncsCalcd_ = false;
  dispCalcd_            = false;
  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::createLFS()
{
  if (  p_lfs_) deleteLFS();
  p_lfs_ = new LattFuncSage( p_bml_ );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::deleteETS()
{
  if( p_ets_ ) { p_ets_->eraseAll(); delete p_ets_; p_ets_ = 0; }
  edwardstengFuncsCalcd_ = false;
  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::createETS()
{
  deleteETS();
  p_ets_ = new EdwardsTengSage( p_bml_);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::deleteLBS()
{

  if( p_lbs_ ) { p_lbs_->eraseAll(); delete p_lbs_; p_lbs_ = 0; }
  LBFuncsCalcd_ = false;
  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext:: createLBS()
{
  if (!p_lbs_) deleteLBS();
  p_lbs_ = new LBSage( p_bml_ );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::deleteCOVS()
{
  if( p_covs_ ) { p_covs_->eraseAll(); delete p_covs_;  p_covs_ = 0; }
  momentsFuncsCalcd_ = false;
 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::createCOVS()
{
  deleteCOVS();
  p_covs_ = new CovarianceSage( p_bml_);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::deleteDSPS()
{
  if( p_dsps_ ) { p_dsps_->eraseAll(); delete p_dsps_; p_dsps_ = 0; }
  dispersionFuncsCalcd_ = false;
 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::createDSPS()
{
  if ( !p_dsps_) deleteDSPS();
  p_dsps_ = new DispersionSage( p_bml_);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::onTransClosedOrbit( Particle const& arg ) const
{

  Particle probe(arg);
  
  p_bml_->propagate( probe );

  if(    ( fabs( arg.get_x()   - probe.get_x()  ) <  smallClosedOrbitXError   )
      && ( fabs( arg.get_y()   - probe.get_y()  ) <  smallClosedOrbitYError   )
      && ( fabs( arg.get_npx() - probe.get_npx()) <  smallClosedOrbitNPXError )
      && ( fabs( arg.get_npy() - probe.get_npy()) <  smallClosedOrbitNPYError ) ) 
  {
    return true;
  }

  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::hasReferenceParticle() const
{
  return hasRefParticle_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setReferenceParticle( Particle const& x )
{
   reset();

   co_part_        = x;
   hasRefParticle_ = true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::getReferenceParticle( Particle& x ) const
{

  if (hasRefParticle_) { x = co_part_;  return 0; } 

  (*pcerr) << "\n*** WARNING *** "
              "\n*** WARNING *** No reference particle is established,"
              "\n*** WARNING *** so none is being returned."
              "\n*** WARNING *** "
             << endl;
  return -1;
 

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle const& BeamlineContext::getParticle()
{
 
  return *particle_;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticleState( Vector const& s )
{
   particle_->setState(s);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::createClosedOrbit()
{
  // Instantiates the closed orbit Particle and JetParticle.
  // The JetParticle is on the closed orbit and its environment
  // is centered on the closed orbit. Its state corresponds
  // to the one turn map.
  
  if( !isTreatedAsRing() ) {
    ostringstream uic;
    uic  <<   "Cannot find closed orbit: line is not considered a ring.";
    throw( GenericException( __FILE__, __LINE__, 
           "void BeamlineContext::_createClosedOrbit()", 
           uic.str().c_str() ) );
  }

  // Eliminate previous information, if necessary
   deleteClosedOrbit();
   particle_->SetReferenceEnergy( p_bml_->Energy() );
   particle_->setStateToZero();
   co_part_ = *particle_;

  if( onTransClosedOrbit( co_part_ ) ) 
  {
    // Instantiate jetparticle_ on the closed orbit
    // and propagate it once.

    co_part_        = *particle_;
    jetparticle_    = JetParticle( co_part_);
    p_bml_->propagate( jetparticle_ );
  }
  else 
  {
    // Instantiate jetparticle_ 
    // and use a ClosedOrbitSage

    jetparticle_ = JetParticle( *particle_);
    p_cos_   = new ClosedOrbitSage( p_bml_ );

    int err;

    if( ( err = p_cos_->findClosedOrbit( jetparticle_ ) ) ) {

      delete p_cos_; p_cos_ = 0;

      ostringstream uic;
      uic  << "Closed orbit calculation exited with error "
           << err;
      throw( GenericException( __FILE__, __LINE__, 
             "void BeamlineContext::_createClosedOrbit()", 
             uic.str().c_str() ) );
    }


    co_part_ = Particle(jetparticle_);

  }


  // As a final step, register the closed orbit particle
  //   to initialize the reference times correctly
  //   in the elements.

  Particle  dummyParticle( co_part_ );
  RefRegVisitor registrar( dummyParticle);
  p_bml_->accept( registrar );
  
 
  // If necessary, create a new Jet environment, 
  // centered on the closed orbit, for the JetParticle.

  Jet__environment_ptr storedEnv = Jet__environment::getLastEnv();
  Jet__environment_ptr pje = Jet__environment::makeJetEnvironment( storedEnv->maxWeight(), co_part_.State() ); //????????
  // ... Note: this method does not reset Jet::_lastEnv;
  // ...       thus the (possible) necessity of the next line.
  Jet__environment::setLastEnv(pje);

  jetparticle_ = JetParticle(co_part_);

  p_bml_->propagate( jetparticle_ );


  // Before returning, restore Jet__environment::_lastEnv
  Jet__environment::setLastEnv(storedEnv);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::deleteClosedOrbit()
{
  if( p_cos_  ) { delete p_cos_; p_cos_ = 0; }
  closed_orbit_computed_ = false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::createTunes()
{


  if( !closed_orbit_computed_) {
    try {
      createClosedOrbit(); 
    }
    catch( GenericException const& ge ) {
      deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }
  
  closed_orbit_computed_ = true;

  // At this point, jet_part's state is that after one-turn on the
  // closed orbit.


  // A little paranoia never hurt.
  if( !p_lfs_ ) createLFS();
  
  int lfs_result = p_lfs_->TuneCalc( jetparticle_, false );
  if( lfs_result != 0 ) {
    ostringstream uic;
    uic  << "Something went wrong while calculating tune: error no. " 
         << lfs_result;
    throw( GenericException( __FILE__, __LINE__, 
           "void BeamlineContext::createTunes()", 
           uic.str().c_str() ) );
  }
    
  tunes_ = LattFuncSage::tunes( any_cast<LattFuncSage::tunes>( p_bml_->dataHook.find("Tunes")->info ) );

  p_bml_->dataHook.eraseAll( "Tunes" );
  
  tunes_computed_ = true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getHorizontalFracTune()
{
  if( !p_lfs_ ) createLFS();

  if( !tunes_computed_ ) { createTunes(); tunes_computed_ = true; }

  return tunes_.hor;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getVerticalFracTune()
{
  if( !p_lfs_ ) createLFS();
 
  if( !tunes_computed_ ) { createTunes(); tunes_computed_ = true; }

  return tunes_.ver;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::createEigentunes()
{

  if( !closed_orbit_computed_ ) {
    try {
      createClosedOrbit(); 
    }
    catch( GenericException const& ge ) {
      deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }
  // At this point, jet_part's state is that after one-turn on the
  //   closed orbit. It's environment is centered on the closed
  //   orbit and may be out of synch with the current environment.


  int ets_result = EdwardsTengSage::eigenTuneCalc( jetparticle_, eigentunes_ );

  if( 0 != ets_result ) {
    ostringstream uic;
    uic  << "Error number " << ets_result
         << " returned by EdwardsTengSage::eigenTuneCalc.";
    throw( GenericException( __FILE__, __LINE__, 
           "void BeamlineContext::_createEigenTunes()", 
           uic.str().c_str() ) );
  }

  eigentunes_computed_ = true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getHorizontalEigenTune()
{
  if( ! eigentunes_computed_ ) createEigentunes();
  return eigentunes_.hor;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getVerticalEigenTune()
{
  if( !eigentunes_computed_ ) createEigentunes();
  return eigentunes_.ver;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<LattFuncSage::lattFunc> const&  BeamlineContext::getTwissArray()
{
  if( !p_lfs_ ) createLFS();
 
  
  if( !normalLattFuncsCalcd_ ) 
  {
    if( isTreatedAsRing() ) 
    {
      if( !tunes_computed_ ) createTunes();
      

      if( !normalLattFuncsCalcd_ ) {
        p_lfs_->NewSlow_CS_Calc( jetparticle_ );
        normalLattFuncsCalcd_ = true;
      }

      if( !dispCalcd_ ) {
        p_lfs_->NewDisp_Calc( jetparticle_, true );
        dispCalcd_ = true;
      }
    }

    // If the line is not treated as periodic, do the following:
    else
    {

      if( initial_lattfunc_set_) {

        int errorFlag = p_lfs_->pushCalc( *particle_, initialLattFunc_);
        normalLattFuncsCalcd_ = ( 0 == errorFlag );
      }
      else {
        normalLattFuncsCalcd_ = false;
        throw( GenericException( __FILE__, __LINE__, 
               "BeamlineContext::getTwissArray()", 
               "You must first provide initial conditions"
               "\nfor a non-periodic line." ) );
      }
    }
  }

  return p_lfs_->getTwissArray();
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<EdwardsTengSage::Info> const&  BeamlineContext::getETArray( )
{

  if( !p_ets_ ) createETS();
  
  if( !eigentunes_computed_ ) createEigentunes();


  if( !edwardstengFuncsCalcd_ ) {
    // This is inefficient! jetparticle_ is already on the closed
    //   orbit, which means it has traversed the beamline.
    //   This information should have been preserved.

    // Preserve/reset the current Jet environment

    EnvPtr<double>                 storedEnv  = Jet__environment::getLastEnv();
    EnvPtr<std::complex<double> >  storedEnvC = JetC__environment::getLastEnv();

     Jet__environment::setLastEnv( jetparticle_.State().Env() );
    JetC__environment::setLastEnv( Jet__environment::getLastEnv() ) ; // implicit conversion 

    JetParticle tmp_jetpart(jetparticle_);
    Mapping id( "identity" );
    tmp_jetpart.setState( id );
    int errorFlag = p_ets_->doCalc(tmp_jetpart );
 
    edwardstengFuncsCalcd_ = ( 0 == errorFlag ); // SHOULD THROW EXCEPTION IF THIS FAILS

    // Restore current environment
    Jet__environment::setLastEnv(storedEnv);
    JetC__environment::setLastEnv(storedEnvC);
  }

  return p_ets_->getETArray(); 

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<CovarianceSage::Info> const&  BeamlineContext::getCovarianceArray()
{
  if( !p_covs_ ) createCOVS();
  
  if( isTreatedAsRing() ) {
    if( !eigentunes_computed_ ) createEigentunes();
  }

  if( !momentsFuncsCalcd_ ) { 
    int j;

    const int n = Particle::PSD;
    MatrixD covariance(n,n);

    EnvPtr<double>                  storedEnv;  // null 
    EnvPtr<std::complex<double> >   storedEnvC; // null

    JetParticle tmp_jetpart(jetparticle_);

    coord** coordPtr = 0;

    if( isTreatedAsRing() ) { 
    
      covariance = equilibriumCovariance( eps1_, eps2_ );

      storedEnv  = Jet__environment::getLastEnv();
      storedEnvC = JetC__environment::getLastEnv();
      Jet__environment::setLastEnv(  jetparticle_.State().Env() );
      JetC__environment::setLastEnv( Jet__environment::getLastEnv() ); //implicit conversion operator

    }
    else {

       if( initial_covariance_set_) {

        covariance = initialCovariance_.covariance;

        coordPtr = new coord* [n];

        // Preserve the current Jet environment
        storedEnv  = Jet__environment::getLastEnv();
        storedEnvC = JetC__environment::getLastEnv();

        // Create a new Jet environment
        double scale[n];
        for( int j = 0; j < n; j++ ) {
          scale[j] = 0.001;
        } 
        //   scale is probably no longer needed ... oh, well ...

        Jet__environment::BeginEnvironment( 1 );

        for( int j = 0; j < n; j++ ) {
          coordPtr[j] = new coord( particle_->State(j) );
        }
        JetC__environment::setLastEnv( Jet__environment::EndEnvironment(scale) ); // implicit conversion 

      }

      else {
        momentsFuncsCalcd_ = false;
        throw( GenericException( __FILE__, __LINE__, 
               "BeamlineContext::getCovFuncPtr", 
               "You must first provide initial conditions"
               "\nfor a non-periodic line." ) );
      }
    }



    Mapping id( "identity" );
    tmp_jetpart.setState( id );

    int errorFlag = p_covs_->doCalc( tmp_jetpart, covariance);
    momentsFuncsCalcd_ = ( 0 == errorFlag );

    // Clean up before leaving 

    if( coordPtr ) {
      for( j = 0; j < n; j++ ) { delete coordPtr[j]; }
      delete [] coordPtr;
    }

    // Restore current environment
    Jet__environment::setLastEnv( storedEnv );
    JetC__environment::setLastEnv(storedEnvC );
  }

  return (p_covs_->getCovarianceArray() );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<LBSage::Info> const& BeamlineContext::getLBArray()
{
  if( !p_lbs_ ) createLBS();
  
  if( !closed_orbit_computed_ ) {
    try {
      createClosedOrbit(); 
    }
    catch( GenericException const& ge ) {
      deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }
  // At this point,  jetparticle_'s state is that after one-turn on the
  //   closed orbit. It's environment is centered on the closed
  //   orbit and may be out of synch with the current environment.


  if( ! LBFuncsCalcd_ ) {
    // Preserve current Jet environment
    //   and reset to that of jetparticle_

    EnvPtr<double>                storedEnv  = Jet__environment::getLastEnv();
    EnvPtr<std::complex<double> > storedEnvC = JetC__environment::getLastEnv();

    Jet__environment::setLastEnv ( jetparticle_.State().Env() );
    JetC__environment::setLastEnv( Jet__environment::getLastEnv() ); // implicit conversion 

    LBFuncsCalcd_ = ( 0 == p_lbs_->doCalc( jetparticle_ ) );

    // Restore current environment
    Jet__environment::setLastEnv( storedEnv );
    JetC__environment::setLastEnv( storedEnvC );
  }

  return  p_lbs_->getLBArray();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<DispersionSage::Info> const&  BeamlineContext::getDispersionArray()
{
  if( !p_dsps_ ) createDSPS();
  
  if( !dispersionFuncsCalcd_ ) 
  { 
    if( isTreatedAsRing() ) {

      if( !closed_orbit_computed_ ) {
        try
        { 
           createClosedOrbit(); 
        }
        catch( GenericException const& ge ) {
          deleteDSPS(); 
          deleteClosedOrbit();
          throw ge;
        }
      }

      // Preserve/reset the current Jet environment

      EnvPtr<double>   storedEnv                = Jet__environment::getLastEnv();
      EnvPtr<std::complex<double> >  storedEnvC = JetC__environment::getLastEnv();
 
      Jet__environment::setLastEnv (  jetparticle_.State().Env() );
      JetC__environment::setLastEnv ( Jet__environment::getLastEnv() ); //implicit conversion 

    
      p_dsps_->flags.onClosedOrbit = true;
      int errorFlag = p_dsps_->doCalc( jetparticle_ );
      dispersionFuncsCalcd_ = ( 0 == errorFlag );

      // Restore current environment
      Jet__environment::setLastEnv( storedEnv );
      JetC__environment::setLastEnv( storedEnvC );

    } // ring

    // If the line is not treated as periodic, do the following:
    else {
    
        int errorFlag = p_dsps_->pushCalc( *particle_, initialDispersion_ );
        dispersionFuncsCalcd_ = ( 0 == errorFlag );

//else {
//      _dispersionFuncsCalcd = false;
//      throw( GenericException( __FILE__, __LINE__, 
//             "BeamlineContext::getDispersionPtr( int i )", 
//             "You must first provide initial conditions"
//             "\nfor a non-periodic line." ) );
//     }
  
     } 
  }

  return p_dsps_->getDispersionArray();  
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD BeamlineContext::equilibriumCovariance()
{
  return equilibriumCovariance( eps1_, eps2_ );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD BeamlineContext::equilibriumCovariance( double eps1, double eps2 )
{
  // eps1 and eps2 are two "invariant emittances"
  //   in units of pi mm-mr.
  // I assume that eps1_ is "mostly horizontal" and eps2_ is
  //   "mostly vertical."

  int i=0, j=0;
  const double mm_mr = 1.0e-6;


  if( ! closed_orbit_computed_ ) {
    try {
       createClosedOrbit(); 
    }
    catch( GenericException const& ge ) {
      deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }

  double betaGamma = jetparticle_.ReferenceBeta() * jetparticle_.ReferenceGamma();

  // Convert to action, in meters (crazy units)
  double I1 = ( std::abs( eps1_ )/betaGamma ) * mm_mr / 2.0;
  double I2 = ( std::abs( eps2_ )/betaGamma ) * mm_mr / 2.0;

  int x   = Particle::xIndex();
  int y   = Particle::yIndex();
  int cdt = Particle::cdtIndex();
  int xp  = Particle::npxIndex(); 
  int yp  = Particle::npyIndex(); 
  int dpp = Particle::ndpIndex();

  int n = Particle::PSD;

  MatrixD aa(n,n);
  aa(x,x)     = I1;
  aa(xp,xp)   = I1;
  aa(y,y)     = I2;
  aa(yp,yp)   = I2;

  MatrixC E(n,n);
  E = ( jetparticle_.State() ).Jacobian().eigenVectors();
  BmlUtil::normalize( E );

  MatrixD cov(n,n);
  cov = real(E*aa*E.dagger());

  return cov;
}


// Adjuster methods

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addHTuneCorrector( ElmPtr x ) 
{
  if( !p_ta_ ) p_ta_ = new TuneAdjuster( p_bml_ );
    
  p_ta_->addCorrector( x, 1.0, 0.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addHTuneCorrector( QuadrupolePtr x ) 
{
  if( !p_ta_ ) p_ta_ = new TuneAdjuster( p_bml_ );
    
  p_ta_->addCorrector( x, 1.0, 0.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addHTuneCorrector( ThinQuadPtr x ) 
{
  if( !p_ta_ ) p_ta_ = new TuneAdjuster( p_bml_ );
    
  p_ta_->addCorrector( x, 1.0, 0.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addVTuneCorrector( ElmPtr x ) 
{
  if( !p_ta_ ) p_ta_ = new TuneAdjuster( p_bml_ );
    
  p_ta_->addCorrector( x, 0.0, 1.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addVTuneCorrector( QuadrupolePtr x ) 
{
  if( !p_ta_ ) p_ta_ = new TuneAdjuster( p_bml_ );
    
  p_ta_->addCorrector( x, 0.0, 1.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addVTuneCorrector( ThinQuadPtr x ) 
{
  if( !p_ta_ ) p_ta_ = new TuneAdjuster( p_bml_ );
    
  p_ta_->addCorrector( x, 0.0, 1.0 );
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::changeTunesBy( double dnuh, double dnuv )
{
  if( !p_ta_ ) return NO_TUNE_ADJUSTER;
 

  Particle  dummyParticle( *particle_);
  dummyParticle.setStateToZero();
  dummyParticle.SetReferenceEnergy( p_bml_->Energy() );
  JetParticle  jp(dummyParticle);

  p_ta_->changeTunesBy( dnuh, dnuv, jp );

  deleteLFS();

  return OKAY;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addHChromCorrector( ElmPtr x ) 
{
   if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( p_bml_ );
    
    p_ca_->addCorrector( x, 1.0, 0.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addHChromCorrector( SextupolePtr x ) 
{
   if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( p_bml_ );
    
    p_ca_->addCorrector( x, 1.0, 0.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addHChromCorrector( ThinSextupolePtr x ) 
{
   if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( p_bml_ );
    
    p_ca_->addCorrector( x, 1.0, 0.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addVChromCorrector( ElmPtr x ) 
{
   if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( p_bml_);
    
    p_ca_->addCorrector( x, 0.0, 1.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addVChromCorrector( SextupolePtr x ) 
{
   if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( p_bml_);
    
    p_ca_->addCorrector( x, 0.0, 1.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addVChromCorrector( ThinSextupolePtr x ) 
{
   if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( p_bml_);
    
    p_ca_->addCorrector( x, 0.0, 1.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::changeChromaticityBy( double dh, double dv )
{
  if( ! p_ca_ ) return NO_CHROMATICITY_ADJUSTER;
 

  Particle dummyParticle( *particle_);
  dummyParticle.setStateToZero();
  dummyParticle.SetReferenceEnergy( p_bml_->Energy() );
  JetParticle  jp(dummyParticle);

  p_ca_->changeChromaticityBy( dh, dv, jp );

  deleteLFS();

  return OKAY;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<( ostream& os, const BeamlineContext& x )
{
  os << "Begin BeamlineContext" << endl;
  if( x.p_bml_ ) 
  { os << "p_bml_" << endl;
    os << *(x.p_bml_);
  }
  os << "End BeamlineContext" << endl;
  return os;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& operator>>( istream& is, BeamlineContext& x )
{

  static char charBuffer[128];
  is.getline( charBuffer, 128, '\n' );
  if( 0 != strcmp( charBuffer, "Begin BeamlineContext" ) ) 
  { 
    throw( GenericException( __FILE__, __LINE__, 
           "istream& operator>>( istream& is, BeamlineContext& x )", 
           "Expected beginning of BeamlineContext." ) );
  }

  is.getline( charBuffer, 128, '\n' );
  while( 0 != strcmp( charBuffer, "End BeamlineContext" ) )
  { 
    if( 0 == strcmp( charBuffer, "p_bml_" ) )
    {
      is >> *(x.p_bml_);
    }

    else 
    { 
      ostringstream uic;
      uic  << "Unrecognized keyword " << charBuffer;
      throw( GenericException( __FILE__, __LINE__, 
             "istream& operator>>( istream& is, BeamlineContext& x )", 
             uic.str().c_str() ) );
    }

    is.getline( charBuffer, 128, '\n' );
  }

  return is;
}
