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
****** - refactored implementation based on embedded database.  
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <algorithm>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/dbutils.h>
#include <basic_toolkit/GenericException.h>

#include <beamline/beamline_elements.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/RefRegVisitor.h>
#include <beamline/LinacCavity.h>

#include <physics_toolkit/BmlUtil.h>
#include <physics_toolkit/BeamlineContext.h>
#include <physics_toolkit/ChromaticityAdjuster.h>
#include <physics_toolkit/TuneAdjuster.h>
#include <physics_toolkit/Optics.h>

#include <sstream>
#include <sqlite/connection.hpp>

#include <boost/filesystem.hpp>   

extern void      BeamlineSpitout( int, beamline const&, beamline::const_iterator &);
extern beamline* DriftsToSlots( beamline const& argbml );
extern bool      d2S_rbendLike( bmlnElmnt& x);

using namespace std;
using namespace boost;

using FNAL::pcerr;
using FNAL::pcout;

const int BeamlineContext::OKAY                     = 0;
const int BeamlineContext::NO_TUNE_ADJUSTER         = 1;
const int BeamlineContext::NO_CHROMATICITY_ADJUSTER = 2;

double const BeamlineContext::small_x_err   = 1.0e-9;
double const BeamlineContext::small_y_err   = 1.0e-9;
double const BeamlineContext::small_npx_err = 1.0e-9;
double const BeamlineContext::small_npy_err = 1.0e-9;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext::BeamlineContext( Particle const& p, beamline const& bml )
:   beamline(*bml.Clone()), particle_(0) ,particleBunchPtr_(0) , 
    p_ca_(0) , p_ta_(0) , 
    initialLattFunc_() , dpp_(0.0001),
    eps1_(40.0), eps2_(40.0), refjp_(p),
    dbname_ (),
    db_()
{

   particle_         = p.Clone();
   particleBunchPtr_ = new ParticleBunch( *particle_);


  // Initialize the internal particle
  // ----------------------------------

   particle_->setStateToZero();
   particle_->SetReferenceMomentum( Momentum() );

 
   //-------------------------------------------------------------------
   // Determine whether some elements have faces not perpendicular 
   // to the reference trajectory. 
   // 
   // Currently ***disabled*** because DriftsToSlots does not preserve 
   // the hierarchical structure. In the future, this should be done
   // automatically.
   //-------------------------------------------------------------------

   bool convert_drifts_to_slots = false;
   for (beamline::deep_iterator it  = deep_begin();  
                                it != deep_end(); ++it ) {

     
     if ( convert_drifts_to_slots = d2S_rbendLike( **it ) ) break;
   }

   // ***** if ( convert_drifts_to_slots ) { DriftsToSlots( *this ); }       


   //-------------------------------------------------------------------
   // create and initialize the database
   //-------------------------------------------------------------------
   
   std::ostringstream address; 
   address << std::hex << this;

   dbname_ =  "./" + Name()+ address.str() + ".db";  
   db_     =  shared_ptr<sqlite::connection>( new  sqlite::connection(dbname_) );
   Optics::initdb( *db_ );

   if(  isRing() ) { handleAsRing(); }
   else            { handleAsLine(); }


}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BeamlineContext::~BeamlineContext()
{
  if( particleBunchPtr_ ) { delete particleBunchPtr_; particleBunchPtr_ = 0; }

  boost::filesystem::path dbpathname(dbname_ ); 
  boost::filesystem::remove( dbpathname );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setParticle( Particle const& p) 
{ 
  particle_ = p.Clone();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::setInitial( CSLattFuncs const& u )
{
  initialLattFunc_       = u;
  initial_lattfunc_set_  = true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs const& BeamlineContext::getInitial()
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

Mapping const& BeamlineContext::getOneTurnMap()
{
  return refjp_.State();
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

double BeamlineContext::sumLengths() const
{
  double sum = 0.0;

  for (beamline::const_deep_iterator it  =  deep_begin(); 
                                     it !=  deep_end(); ++it ) {
   sum += (*it)->Length(); 
  }
  return sum;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setElmLength( ElmPtr w, double l )
{
  bool   found = false;

  for ( beamline::deep_iterator it  =  deep_begin();
	it != deep_end(); ++it ) {

    if ( (*it) !=  w ) continue;
 
    if ( l != (*it)->Length() ) { (*it)->setLength(l); clear(); } // This is extreme.
  }

  return found ? 0 : 1; 

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setElmStrength( ElmPtr w, double s )
{

  bool found  = false;
  int  ret    = 0;


  for ( beamline::deep_iterator it  = deep_begin();
	it != deep_end(); ++it ) {

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
  // Note: it is assumed that x and y are
  //       in units of pi mm-mr.

   eps1_ = std::abs(x);
   eps2_ = std::abs(y);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setElmAlignment( ElmPtr v, alignmentData const& u )
{
  static const int errCode_nullArg  = 1;
  static const int errCode_notFound = 2;

  if( !v ) { 
     return errCode_nullArg; 
  }

  beamline::deep_iterator it; 
  if( (it = std::find(deep_begin(), deep_end(), v )) == deep_end() ) { 
   return errCode_notFound; 
  }

  v->setAlignment( u );

  clear();

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::setElmAlignment( alignmentData const& u, boost::function<bool(bmlnElmnt const&)> criterion)
{
  int ret = 0;

  for ( beamline::deep_iterator it  = deep_begin();
	it != deep_end(); ++it ) {

    if ( !criterion( **it )  ) continue;

    ++ret;
    (*it)->setAlignment( u );
   
  }

  clear();

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::processElements( boost::function<bool(bmlnElmnt &)> action )
{

  int ret = 0;

  for ( beamline::deep_iterator it  = deep_begin();
	it != deep_end(); ++it ) {

    if ( !action( **it) )   continue;
    
    ++ret;
   
  }

   clear();

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

  int ret = 0;

  beamline::deep_iterator it = std::find( deep_begin(),  deep_end(), a );
  
  if( it == deep_end() ) { 
    clear(); ret = 1; 
  } 
  else {
    (*it) = b;
  }

  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData BeamlineContext::getElmAlignmentData( ElmPtr v ) const
{
  static const alignment err1Ret(  137.,  137.,  137. );
  static const alignment err2Ret( -137., -137., -137. );

  if( !v  ) { return err1Ret.getAlignment(); }
  
  const_deep_iterator it = deep_begin(); 

  if( (it = std::find(it,  deep_end(), v )) == deep_end() ) { 
    return err2Ret.getAlignment(); 
  }

  return v->Alignment();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BeamlineContext::onTransClosedOrbit( Particle const& p ) const
{

  Particle probe(p);
  propagate( probe );

  if(    ( abs( p.get_x()   - probe.get_x()  ) <  small_x_err   )
      && ( abs( p.get_y()   - probe.get_y()  ) <  small_y_err   )
      && ( abs( p.get_npx() - probe.get_npx()) <  small_npx_err )
      && ( abs( p.get_npy() - probe.get_npy()) <  small_npy_err ) ) 
  {
    return true;
  }

  return false;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Particle BeamlineContext::getRefParticle() const
{

  Particle p( *particle_);
  // FIXME :: set the state of the particle !
  return p;

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

   Particle p(*particle_); 

   RefRegVisitor( ( Particle(p) ) ).visit(*this) ;

   refjp_ = Optics::find_closed_orbit( (*db_), *this, JetParticle(p) );  
   
   Optics::orbit( (*db_), *this, Particle(refjp_) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::propagateReferenceOrbit()
{

   if( reference_orbit_ok() )  return; 

  
   Particle p(*particle_); 

   Vector& state = p.State();

   state[0] =  initialLattFunc_.reference_orbit.x;
   state[3] =  initialLattFunc_.reference_orbit.xp;
   state[1] =  initialLattFunc_.reference_orbit.y;
   state[4] =  initialLattFunc_.reference_orbit.yp;

   RefRegVisitor( ( Particle(p) ) ).visit(*this) ;

   Optics::orbit( (*db_), *this, p );

   refjp_ = JetParticle(p);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double BeamlineContext::getHTune()
{
  return Optics::getHTune( (*db_ ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BeamlineContext::getVTune()
{
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
   

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  BeamlineContext::periodicCourantSnyder2D()
{

  if ( !reference_orbit_ok() )  { periodicReferenceOrbit(); }

  if ( courant_snyder2d_ok() ) return;
  
   if ( isTreatedAsRing() ){
     CSLattFuncs lf = initialLattFunc_ = Optics::periodicCourantSnyder2D( (*db_), refjp_ );  
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

 Optics::propagateCourantSnyder2D( (*db_), *this, refjp_,  initialLattFunc_ );
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

    CSLattFuncs4D lf(initialLattFunc_);

    // if( !reference_orbit_ok() ) { periodicReferenceOrbit(); }
 
    Optics::propagateCourantSnyder4D( (*db_), *this, refjp_, CSLattFuncs4D(initialLattFunc_) );

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

      Vector eta0 = initialLattFunc_.dispersion.eta;  
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

     Vector eta0 = initialLattFunc_.dispersion.eta;  
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

void BeamlineContext::addHTuneCorrector( ElmPtr x ) 
{
  //if( !p_ta_ ) p_ta_ = new TuneAdjuster( *this, (*db_) );
  //p_ta_->addCorrector( x, 1.0, 0.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addHTuneCorrector( QuadrupolePtr x ) 
{
  //if( !p_ta_ ) p_ta_ = new TuneAdjuster( bml_, (*db_) );
    
  //p_ta_->addCorrector( x, 1.0, 0.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addHTuneCorrector( ThinQuadPtr x ) 
{
  //if( !p_ta_ ) p_ta_ = new TuneAdjuster( bml_, (*db_) );
    
  //p_ta_->addCorrector( x, 1.0, 0.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addVTuneCorrector( ElmPtr x ) 
{
  //if( !p_ta_ ) p_ta_ = new TuneAdjuster( bml_, (*db_) );
    
  //p_ta_->addCorrector( x, 0.0, 1.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addVTuneCorrector( QuadrupolePtr x ) 
{
  //if( !p_ta_ ) p_ta_ = new TuneAdjuster( bml_ , (*db_));
    
  //p_ta_->addCorrector( x, 0.0, 1.0 );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addVTuneCorrector( ThinQuadPtr x ) 
{
  //if( !p_ta_ ) p_ta_ = new TuneAdjuster( bml_, (*db_) );
    
  //p_ta_->addCorrector( x, 0.0, 1.0 );
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::changeTunesBy( double dnuh, double dnuv )
{
  //if( !p_ta_ ) return NO_TUNE_ADJUSTER;
 

  //Particle  dummyParticle( *particle_);
  //dummyParticle.setStateToZero();
  //dummyParticle.SetReferenceMomentum( Momentum() );
  //JetParticle  jp(dummyParticle);

  //p_ta_->changeTunesBy( dnuh, dnuv, jp );

  //clear();

  //return OKAY;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addHChromCorrector( ElmPtr x ) 
{
  //if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( bml_, (*db_) );
    
  // p_ca_->addCorrector( x, 1.0, 0.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addHChromCorrector( SextupolePtr x ) 
{
  //if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( bml_, (*db_) );
    
   //p_ca_->addCorrector( x, 1.0, 0.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addHChromCorrector( ThinSextupolePtr x ) 
{
  //if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( bml_, (*db_) );
    
  // p_ca_->addCorrector( x, 1.0, 0.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BeamlineContext::addVChromCorrector( ElmPtr x ) 
{
  //if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( bml_, (*db_));
    
  // p_ca_->addCorrector( x, 0.0, 1.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addVChromCorrector( SextupolePtr x ) 
{
  //if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( bml_, (*db_));
    
  // p_ca_->addCorrector( x, 0.0, 1.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BeamlineContext::addVChromCorrector( ThinSextupolePtr x ) 
{
  //if( !p_ca_ ) p_ca_ = new ChromaticityAdjuster( bml_, (*db_));
    
  // p_ca_->addCorrector( x, 0.0, 1.0 );
   
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int BeamlineContext::changeChromaticityBy( double dh, double dv )
{
  //if( ! p_ca_ ) return NO_CHROMATICITY_ADJUSTER;
 

  //Particle dummyParticle( *particle_);
  //dummyParticle.setStateToZero();
  //dummyParticle.SetReferenceMomentum( Momentum() );
  //JetParticle  jp(dummyParticle);

  //p_ca_->changeChromaticityBy( dh, dv, jp );

  //clear();

  //return OKAY;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sqlite::connection& BeamlineContext::dbConnection() const    
{
  return (*db_);
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
 return Sage::isRing(*this);
}
