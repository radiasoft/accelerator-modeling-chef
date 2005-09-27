/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      BeamlineContext.cc
******  Version:   2.0.1
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
******                                                                
**************************************************************************
*************************************************************************/


#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <GenericException.h>
#include <BmlUtil.h>
#include <slist.h>  // This should not be necessary!!!
#include <BeamlineContext.h>
#include <beamline.h>
#include <BeamlineIterator.h>
// REMOVE: #include "LattFuncSage.h"
#include <ClosedOrbitSage.h>
#include <ChromaticityAdjuster.h>
#include <TuneAdjuster.h>
#include <RefRegVisitor.h>

extern void BeamlineSpitout( int, BeamlineIterator& );

const int BeamlineContext::OKAY = 0;
const int BeamlineContext::NO_TUNE_ADJUSTER = 1;

const double BeamlineContext::_smallClosedOrbitXError   /* [m] */ = 1.0e-9;
const double BeamlineContext::_smallClosedOrbitYError   /* [m] */ = 1.0e-9;
const double BeamlineContext::_smallClosedOrbitNPXError /*[rad]*/ = 1.0e-9;
const double BeamlineContext::_smallClosedOrbitNPYError /*[rad]*/ = 1.0e-9;

using namespace std;

BeamlineContext::BeamlineContext( bool doClone, beamline* x )
: _p_bml(x), _proton( x->Energy() ), _protonBunch(),
  _p_lfs(0), _p_ets(0), _p_covs(0), _p_lbs(0), _p_cos(0), _p_dsps(0),
  _p_ca(0), _p_ta(0),
  _p_co_p(0), _p_disp_p(0), 
  _initialLattFuncPtr(0), 
  _initialDispersionPtr(0),
  _initialCovariancePtr(0),
  _dpp(0.0001),
  _isCloned(doClone),
  _p_bi(0), _p_dbi(0), _p_rbi(0), _p_drbi(0),
  _tunes(0), _eigentunes(0), 
  _p_jp(0), 
  _eps_1(40.0), _eps_2(40.0),
  _normalLattFuncsCalcd(false), 
  _edwardstengFuncsCalcd(false), 
  _momentsFuncsCalcd(false), 
  _LBFuncsCalcd(false), 
  _dispersionFuncsCalcd(false), 
  _dispCalcd(false)
{
  if( x == 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "BeamlineContext::BeamlineContext( bool doClone, beamline* x )", 
           "Invoked with null beamline pointer." ) );
  }

  if( _isCloned ) { _p_bml = (beamline*) (x->Clone()); }

  if( Sage::isRing(_p_bml) ) { this->handleAsRing(); }
  else                       { this->handleAsLine(); }
}

BeamlineContext::BeamlineContext( const BeamlineContext& )
{
  throw( GenericException( __FILE__, __LINE__, 
         "BeamlineContext::BeamlineContext( const BeamlineContext& )", 
         "Calling copy constructor is not allowed." ) );
}

BeamlineContext::~BeamlineContext()
{
  this->reset();

  if( _isCloned && (_p_bml != 0) ) 
  { _p_bml->eliminate(); }
}


void BeamlineContext::reset()
{
  _deleteLFS();
  _deleteETS();
  _deleteCOVS();
  _deleteLBS();
  _deleteDSPS();
  _deleteClosedOrbit();

  if( _p_ca  ) { delete _p_ca; _p_ca = 0; }
  if( _p_ta  ) { delete _p_ta; _p_ta = 0; }

  if( _p_bi   )  { delete _p_bi;   _p_bi   = 0; }
  if( _p_dbi  )  { delete _p_dbi;  _p_dbi  = 0; }
  if( _p_rbi  )  { delete _p_rbi;  _p_rbi  = 0; }
  if( _p_drbi )  { delete _p_drbi; _p_drbi = 0; }

  if( _initialLattFuncPtr ) 
  { delete _initialLattFuncPtr;   _initialLattFuncPtr = 0;   } 
  if( _initialDispersionPtr ) 
  { delete _initialDispersionPtr; _initialDispersionPtr = 0; }

  _proton.setStateToZero();
}


int BeamlineContext::assign( beamline* x )
{
  static bool firstTime = true;
  if( 0 != _p_bml ) 
  { if( firstTime ) 
    { cerr << "\n*** WARNING *** BeamlineContext::assign "
              "invoked illegally."
              "\n*** WARNING *** This message appears only once.\n"
           << endl;
      firstTime = false;
    }
    return 1; 
  }
  
  if( 0 == x ) 
  { return 2;}

  if( _isCloned ) 
  { _p_bml = (beamline*) (x->Clone()); }
  else 
  { _p_bml = x; }

  return 0;
}


void BeamlineContext::accept( ConstBmlVisitor& x ) const
{
  _p_bml->accept(x);
}


void BeamlineContext::accept( BmlVisitor& x )
{
  _p_bml->accept(x);
  this->_deleteLFS();
  if( _p_cos ) { delete _p_cos; _p_cos = 0; }
  if( _p_ca  ) { delete _p_ca; _p_ca = 0;   }
  if( _p_ta  ) { delete _p_ta; _p_ta = 0;   }
}


void BeamlineContext::setClonedFlag( bool x )
{
  _isCloned = x;
}


bool BeamlineContext::getClonedFlag()
{
  return _isCloned;
}


void BeamlineContext::setInitial( const DispersionSage::Info& u )
{
  if( _initialDispersionPtr ) { delete _initialDispersionPtr; }
  _initialDispersionPtr = new DispersionSage::Info(u);
}


void BeamlineContext::setInitial( const DispersionSage::Info* u )
{
  if( _initialDispersionPtr ) { delete _initialDispersionPtr; }
  _initialDispersionPtr = new DispersionSage::Info(*u);
}


void BeamlineContext::getInitial( DispersionSage::Info* u )
{
  if( _initialDispersionPtr ) { 
    throw( GenericException( __FILE__, __LINE__, 
             "BeamlineContext::getInitial( DispersionSage::Info* )",
             "Initial conditions for dispersion are not available." ) );
  }

  (*u) = (*_initialDispersionPtr);
}


void BeamlineContext::setInitial( const CovarianceSage::Info& u )
{
  if( _initialCovariancePtr ) { delete _initialCovariancePtr; }
  _initialCovariancePtr = new CovarianceSage::Info(u);
}


void BeamlineContext::setInitial( const CovarianceSage::Info* u )
{
  if( _initialCovariancePtr ) { delete _initialCovariancePtr; }
  _initialCovariancePtr = new CovarianceSage::Info(*u);
}


void BeamlineContext::getInitial( CovarianceSage::Info* u )
{
  if( _initialCovariancePtr ) { 
    throw( GenericException( __FILE__, __LINE__, 
             "BeamlineContext::getInitial( CovarianceSage::Info* )",
             "Initial conditions for covariance are not available." ) );
  }

  (*u) = (*_initialCovariancePtr);
}


void BeamlineContext::setInitial( const LattFuncSage::lattFunc& u )
{
  if( _initialLattFuncPtr ) { delete _initialLattFuncPtr; }
  _initialLattFuncPtr = new LattFuncSage::lattFunc(u);
}


void BeamlineContext::setInitial( const LattFuncSage::lattFunc* u )
{
  if( _initialLattFuncPtr ) { delete _initialLattFuncPtr; }
  _initialLattFuncPtr = new LattFuncSage::lattFunc(*u);
}


void BeamlineContext::getInitial( LattFuncSage::lattFunc* u )
{
  if( _initialLattFuncPtr ) { 
    throw( GenericException( __FILE__, __LINE__, 
             "BeamlineContext::getInitial( LattFuncSage::lattFunc* )",
             "Initial conditions for uncoupled \n"
             "lattice functions are not available." ) );
  }

  (*u) = (*_initialLattFuncPtr);
}


void BeamlineContext::writeTree()
{
  BeamlineIterator bi( _p_bml );
  BeamlineSpitout( 0, bi );
}


// Beamline Functions

const char* BeamlineContext::name() const
{
  return _p_bml->Name();
}


void BeamlineContext::rename( const char* newname )
{
  _p_bml->Rename( newname );
}


void BeamlineContext::peekAt( double& s, Particle* p ) const
{
  _p_bml->peekAt( s, p );
}


double BeamlineContext::sumLengths() const
{
  double ret = 0.0;
  DeepBeamlineIterator dbi( _p_bml );
  bmlnElmnt* q;
  while( q = dbi++ ) { 
    ret += q->Length(); 
  }
  return ret;
}


int BeamlineContext::setLength( bmlnElmnt* w, double l )
{
  static bool notFound; notFound = true;
  static int ret;       ret = 0;

  DeepBeamlineIterator dbi( _p_bml );
  bmlnElmnt* q;
  while( notFound && (q = dbi++) ) 
  { if( q == w ) 
    { notFound = false;
      // REMOVE: if( l != q->Length() ) 
      // REMOVE: { q->setLength(l);
      // REMOVE:   this->_deleteLFS();
      // REMOVE:   // if( _p_lfs ) { delete _p_lfs; _p_lfs = 0; }  // This is extreme.
      // REMOVE:   if( _p_cos ) { delete _p_cos; _p_cos = 0; }
      // REMOVE:   if( _p_ca  ) { delete _p_ca; _p_ca = 0;   }
      // REMOVE:   if( _p_ta  ) { delete _p_ta; _p_ta = 0;   }
      // REMOVE: }
      if( l != q->Length() ) 
      { q->setLength(l);
        this->reset();  // This is extreme.
      }
    }
  }

  if(notFound) { ret = 1; }

  return ret;
}


int BeamlineContext::setStrength( bmlnElmnt* w, double s )
{
  static bool notFound; notFound = true;
  static int ret;       ret = 0;

  DeepBeamlineIterator dbi( _p_bml );
  bmlnElmnt* q;
  while( notFound && (q = dbi++) ) 
  { if( q == w ) 
    { notFound = false;
      if( s != q->Strength() ) 
      { q->setStrength(s);
        this->_deleteLFS();
        // if( _p_lfs ) { delete _p_lfs; _p_lfs = 0; }  // This is extreme.
        if( _p_cos ) { delete _p_cos; _p_cos = 0; }
        if( _p_ca  ) { delete _p_ca; _p_ca = 0;   }
        if( _p_ta  ) { delete _p_ta; _p_ta = 0;   }
      }
    }
  }

  if(notFound) { ret = 1; }

  return ret;
}


void BeamlineContext::setAvgInvariantEmittance( double x, double y )
{
  _eps_1 = std::abs(x);
  _eps_2 = std::abs(y);
  // Note: it is assumed that x and y are
  //       in units of pi mm-mr.
}


double BeamlineContext::getEnergy() const
{
  return _p_bml->Energy();
}


int BeamlineContext::countHowManyDeeply() const
{
  return _p_bml->countHowManyDeeply();
}


int BeamlineContext::setAlignment( bmlnElmnt* v, const alignmentData& u )
{
  static int errCode_nullArg  = 1;
  static int errCode_notFound = 2;
  if( v == 0 )                   { return errCode_nullArg; }
  if( 0 == _p_bml->contains(v) ) { return errCode_notFound; }

  v->setAlignment( u );
  // ??? Check for passive element!!!

  this->_deleteLFS(); // ??? Too conservative
  this->_deleteClosedOrbit();
  return 0;
}


int BeamlineContext::setAlignment( beamline::Criterion& cf, const alignmentData& u )
{
  int ret = 0;

  DeepBeamlineIterator dbi( _p_bml );
  bmlnElmnt* q;
  while((  q = dbi++  )) {
    if( cf(q) ) {
      ret++;
      q->setAlignment( u );
    }
  }

  if( ret != 0 ) {
    this->_deleteLFS(); // ??? Too conservative
    this->_deleteClosedOrbit();
  }

  return ret;
}


int BeamlineContext::replaceElement( bmlnElmnt* a, bmlnElmnt* b )
{
  // Will replace the first argument with
  // the second. Return values:
  // 0 everything went as planned
  // 1 first argument was not found
  // 2 at least one argument was null
  // 
  // WARNING: Element *a will be deleted if the beamline has
  // been cloned. It is assumed that *b has been created on the
  // heap, and the BeamlineContext takes over ownership.

  int ret = _p_bml->replace(a,b);
  if( 0 == ret ) { this->reset(); }
  // This step will not always be necessary.
  return ret;
}


int BeamlineContext::processElements( beamline::Action& cf )
{
  int ret = 0;

  DeepBeamlineIterator dbi( _p_bml );
  bmlnElmnt* q;
  while((  q = dbi++  )) {
    if( 0 == cf(q) ) {
      ret++;
    }
  }

  if( ret != 0 ) {
    this->_deleteLFS(); // ??? Too conservative
    this->_deleteClosedOrbit();
  }

  return ret;
}


alignmentData BeamlineContext::getAlignmentData( const bmlnElmnt* v ) const
{
  static const alignment err1Ret(  137.,  137.,  137. );
  static const alignment err2Ret( -137., -137., -137. );
  if( v == 0 )                   { return err1Ret.getAlignment(); }
  if( 0 == _p_bml->contains(v) ) { return err2Ret.getAlignment(); }

  return v->Alignment();
}


beamline* BeamlineContext::cloneBeamline() const
{
  // Creates new beamline, for which the invoker is responsible.
  return dynamic_cast<beamline*>(_p_bml->Clone());
}


Mapping BeamlineContext::getOneTurnMap()
{
  // REMOVE: if( ( 0 == _p_cos ) || ( 0 == _p_jp ) ) {
  if( ( 0 == _p_co_p ) || ( 0 == _p_jp ) ) {
    try {
      this->_createClosedOrbit(); 
    }
    catch( const GenericException& ge ) {
      this->_deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }

  if( 0 == _p_jp ) {
    cerr << "\n*** ERROR *** " << __FILE__ << ", line " << __LINE__ << ": "
         << "\n*** ERROR *** Mapping BeamlineContext::getOneTurnMap() const"
            "\n*** ERROR *** Unable to calculate closed orbit correctly."
            "\n*** ERROR *** Will return identity map. Your calculations"
            "\n*** ERROR *** will be wrong."
         << endl;
    Mapping idMap("id");
    return idMap;
  }
  
  return _p_jp->State();
}


const beamline* BeamlineContext::cheatBmlPtr() const
{
  return _p_bml;
}



// Sage operations

void BeamlineContext::_deleteLFS()
{
  if( 0 != _p_lfs )
  {
    _p_lfs->eraseAll();
    if( _tunes ) { 
      delete _tunes; 
      _tunes = 0;
    }
    delete _p_lfs;
    _p_lfs = 0;

    _normalLattFuncsCalcd = false;
    _dispCalcd = false;
  }
}


void BeamlineContext::_createLFS()
{
  this->_deleteLFS();
  _p_lfs = new LattFuncSage( _p_bml, false );
}


void BeamlineContext::_deleteETS()
{
  if( 0 != _p_ets )
  {
    _p_ets->eraseAll();
    if( _eigentunes ) { 
      delete _eigentunes; 
      _eigentunes = 0;
    }
    delete _p_ets;
    _p_ets = 0;

    _edwardstengFuncsCalcd = false;
  }
}


void BeamlineContext::_createETS()
{
  this->_deleteETS();
  _p_ets = new EdwardsTengSage( _p_bml, false );
}


void BeamlineContext::_deleteLBS()
{
  if( 0 != _p_lbs )
  {
    _p_lbs->eraseAll();
    delete _p_lbs;
    _p_lbs = 0;

    _LBFuncsCalcd = false;
  }
}


void BeamlineContext::_createLBS()
{
  this->_deleteLBS();
  _p_lbs = new LBSage( _p_bml, false );
}


void BeamlineContext::_deleteCOVS()
{
  if( 0 != _p_covs )
  {
    _p_covs->eraseAll();
    if( _eigentunes ) { 
      delete _eigentunes; 
      _eigentunes = 0;
    }
    delete _p_covs;
    _p_covs = 0;

    _momentsFuncsCalcd = false;
  }
}


void BeamlineContext::_createCOVS()
{
  this->_deleteCOVS();
  _p_covs = new CovarianceSage( _p_bml, false );
}


void BeamlineContext::_deleteDSPS()
{
  if( 0 != _p_dsps )
  {
    _p_dsps->eraseAll();
    delete _p_dsps;
    _p_dsps = 0;

    _dispersionFuncsCalcd = false;
  }
}


void BeamlineContext::_createDSPS()
{
  this->_deleteDSPS();
  _p_dsps = new DispersionSage( _p_bml, false );
}


bool BeamlineContext::onTransClosedOrbit( const Proton& arg ) const
{
  Proton probe( arg );
  
  _p_bml->propagate( probe );

  if(    ( fabs( arg.get_x()   - probe.get_x()  ) < _smallClosedOrbitXError   )
      && ( fabs( arg.get_y()   - probe.get_y()  ) < _smallClosedOrbitYError   )
      && ( fabs( arg.get_npx() - probe.get_npx()) < _smallClosedOrbitNPXError )
      && ( fabs( arg.get_npy() - probe.get_npy()) < _smallClosedOrbitNPYError ) ) 
  {
    return true;
  }

  return false;
}


bool BeamlineContext::hasReferenceProton() const
{
  return (0 != _p_co_p);
}


void BeamlineContext::setReferenceProton( const Proton& x )
{
  this->reset();
  _p_co_p = new Proton(x);
}


Proton BeamlineContext::getReferenceProton() const
{
  if( 0 != _p_co_p ) { 
    return (*_p_co_p); 
  }
  else {
    return Proton( this->getEnergy() );
  }
}


void BeamlineContext::_createClosedOrbit()
{
  // Instantiates the closed orbit Proton and JetProton.
  // The JetProton is on the closed orbit and its environment
  // is centered on the closed orbit. Its state corresponds
  // to the one turn map.
  
  if( !(this->isTreatedAsRing()) ) {
    ostringstream uic;
    uic  <<   "Cannot find closed orbit: line is not considered a ring.";
    throw( GenericException( __FILE__, __LINE__, 
           "void BeamlineContext::_createClosedOrbit()", 
           uic.str().c_str() ) );
  }

  // Eliminate previous information, if necessary
  _deleteClosedOrbit();

  _p_co_p   = new Proton( _p_bml->Energy() );
  _proton.SetReferenceEnergy( _p_co_p->ReferenceEnergy());
  _proton.setState( _p_co_p->State() );

  if( (this->onTransClosedOrbit( *_p_co_p )) ) 
  {
    // Instantiate _p_jp on the closed orbit
    // and propagate it once.
    _p_jp = new JetProton( *_p_co_p );
    _p_bml->propagate( *_p_jp );
  }
  else 
  {
    // Instantiate _p_jp 
    // and use a ClosedOrbitSage
    delete _p_co_p;
    _p_co_p = 0;

    _p_jp = new JetProton( _p_bml->Energy() );
    _p_cos = new ClosedOrbitSage( _p_bml, false );
    int err;

    if( 0 != ( err = _p_cos->findClosedOrbit( _p_jp ) ) ) {
      delete _p_jp; 
      _p_jp = 0;
      delete _p_cos;
      _p_cos = 0;
      ostringstream uic;
      uic  << "Closed orbit calculation exited with error "
           << err;
      throw( GenericException( __FILE__, __LINE__, 
             "void BeamlineContext::_createClosedOrbit()", 
             uic.str().c_str() ) );
    }

    _p_co_p = (dynamic_cast<Proton*>(_p_jp->ConvertToParticle()));
    _proton.setState( _p_co_p->State() );
    _proton.SetReferenceEnergy( _p_co_p->ReferenceEnergy());
  }


  // As a final step, register the closed orbit proton
  //   to initialize the reference times correctly
  //   in the elements.
  Proton dummyProton( *_p_co_p );
  RefRegVisitor registrar( dummyProton );
  _p_bml->accept( registrar );
  
 
  // If necessary, create a new Jet environment, 
  // centered on the closed orbit, for the Jet proton.
  Jet__environment* storedEnv = Jet::_lastEnv;
  Jet__environment* pje = Jet__environment::CreateEnvFrom( _p_co_p->State(), 
                                              storedEnv->_maxWeight );
  // ... Note: this method does not reset Jet::_lastEnv;
  // ...       thus the (possible) necessity of the next line.
  Jet::_lastEnv = pje;
  delete _p_jp;
  _p_jp = new JetProton( *_p_co_p );
  _p_bml->propagate( *_p_jp );


  // Before returning, restore Jet::_lastEnv
  Jet::_lastEnv = storedEnv;
}


void BeamlineContext::_deleteClosedOrbit()
{
  if( _p_cos ) {
    delete _p_cos;
    _p_cos = 0;
  }

  if( _p_co_p ) {
    delete _p_co_p;
    _p_co_p = 0;
  }

  if( _p_jp ) {
    delete _p_jp;
    _p_jp = 0;

    // !!! if( 1 < Jet::environments.size() ) {
    // !!!   Jet__environment* q = (Jet__environment*) Jet::environments.get();
    // !!!   delete q;
    // !!!   Jet::lastEnv = (Jet__environment*) Jet::environments.firstPtr();
    // !!! }
    // !!! or
    // !!! Jet::popEnvironment();  <-- preferred!
  }
}


void BeamlineContext::_createTunes()
{
  // First, 
  // REMOVE: if( ( 0 == _p_cos ) || ( 0 == _p_jp ) ) {
  if( ( 0 == _p_co_p ) || ( 0 == _p_jp ) ) {
    try {
      _createClosedOrbit(); 
    }
    catch( const GenericException& ge ) {
      _deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }

  // At this point, _p_jp's state is that after one-turn on the
  // closed orbit.


  // A little paranoia never hurt.
  if( 0 == _p_lfs ) {
    this->_createLFS();
  }
  

  int lfs_result = _p_lfs->TuneCalc( _p_jp, false );
  if( lfs_result != 0 ) {
    ostringstream uic;
    uic  << "Something went wrong while calculating tune: error no. " 
         << lfs_result;
    throw( GenericException( __FILE__, __LINE__, 
           "void BeamlineContext::_createTunes()", 
           uic.str().c_str() ) );
  }
    
  if( _tunes == 0 ) {
    _tunes = new LattFuncSage::tunes( *( dynamic_cast<LattFuncSage::tunes*>
                                         (_p_bml->dataHook.find( "Tunes" )) 
                                       ) 
                                    );
  }
  else {
    *_tunes = *( dynamic_cast<LattFuncSage::tunes*>(_p_bml->dataHook.find( "Tunes" )) );
  }

  _p_bml->dataHook.eraseAll( "Tunes" );
}


double BeamlineContext::getHorizontalFracTune()
{
  if( 0 == _p_lfs ) {
    this->_createLFS();
  }
  
  if( 0 == _tunes ) {
    this->_createTunes();
  }

  return _tunes->hor;
}


double BeamlineContext::getVerticalFracTune()
{
  if( 0 == _p_lfs ) {
    this->_createLFS();
  }
  
  if( 0 == _tunes ) {
    this->_createTunes();
  }

  return _tunes->ver;
}


void BeamlineContext::_createEigentunes()
{
  // REMOVE: if( ( 0 == _p_cos ) || ( 0 == _p_jp ) ) {
  if( ( 0 == _p_co_p ) || ( 0 == _p_jp ) ) {
    try {
      _createClosedOrbit(); 
    }
    catch( const GenericException& ge ) {
      _deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }
  // At this point, _p_jp's state is that after one-turn on the
  //   closed orbit. It's environment is centered on the closed
  //   orbit and may be out of synch with the current environment.

  if( _eigentunes == 0 ) {
    _eigentunes = new EdwardsTengSage::Tunes;
  }
					     
  int ets_result = EdwardsTengSage::eigenTuneCalc( *_p_jp, *_eigentunes );

  if( 0 != ets_result ) {
    ostringstream uic;
    uic  << "Error number " << ets_result
         << " returned by EdwardsTengSage::eigenTuneCalc.";
    throw( GenericException( __FILE__, __LINE__, 
           "void BeamlineContext::_createEigenTunes()", 
           uic.str().c_str() ) );
  }
}


double BeamlineContext::getHorizontalEigenTune()
{
  if( 0 == _eigentunes ) {
    this->_createEigentunes();
  }
  return _eigentunes->hor;
}


double BeamlineContext::getVerticalEigenTune()
{
  if( 0 == _eigentunes ) {
    this->_createEigentunes();
  }
  return _eigentunes->ver;
}


const LattFuncSage::lattFunc* BeamlineContext::getLattFuncPtr( int i )
{
  if( 0 == _p_lfs ) {
    this->_createLFS();
  }
  
  if( !_normalLattFuncsCalcd ) 
  {
    if( this->isTreatedAsRing() ) 
    {
      if( 0 == _tunes ) {
        this->_createTunes();
      }

      if( !_normalLattFuncsCalcd ) {
        _p_lfs->NewSlow_CS_Calc( _p_jp );
        _normalLattFuncsCalcd = true;
      }

      if( !_dispCalcd ) {
        _p_lfs->NewDisp_Calc( _p_jp, true );
        _dispCalcd = true;
      }
    }

    // If the line is not treated as periodic, do the following:
    else
    {
      if( 0 != _initialLattFuncPtr ) {
        int errorFlag = _p_lfs->pushCalc( _proton, *_initialLattFuncPtr );
        _normalLattFuncsCalcd = ( 0 == errorFlag );
      }
      else {
        _normalLattFuncsCalcd = false;
        throw( GenericException( __FILE__, __LINE__, 
               "BeamlineContext::getLattFuncPtr( int i )", 
               "You must first provide initial conditions"
               "\nfor a non-periodic line." ) );
      }
    }
  }

  return (_p_lfs->get_lattFuncPtr(i));
}


const EdwardsTengSage::Info* BeamlineContext::getETFuncPtr( int i )
{
  if( 0 == _p_ets ) {
    this->_createETS();
  }
  
  if( 0 == _eigentunes ) {
    this->_createEigentunes();
  }

  if( !_edwardstengFuncsCalcd ) {
    // This is inefficient! _p_jp is already on the closed
    //   orbit, which means it has traversed the beamline.
    //   This information should have been preserved.

    // Preserve/reset the current Jet environment
    Jet__environment*  storedEnv  = Jet::_lastEnv;
    JetC__environment* storedEnvC = JetC::_lastEnv;
    Jet::_lastEnv = (Jet__environment*) (_p_jp->State().Env());
    JetC::_lastEnv = JetC__environment::CreateEnvFrom( Jet::_lastEnv );

    JetParticle* ptr_arg = _p_jp->Clone();
    Mapping id( "identity" );
    ptr_arg->setState( id );
    int errorFlag = _p_ets->doCalc( ptr_arg, beamline::yes );
    _edwardstengFuncsCalcd = ( 0 == errorFlag );
    delete ptr_arg;

    // Restore current environment
    Jet::_lastEnv = storedEnv;
    JetC::_lastEnv = storedEnvC;
  }

  if( _edwardstengFuncsCalcd ) { return (_p_ets->get_ETFuncPtr(i)); }
  else                         { return 0;                          }
}


const CovarianceSage::Info* BeamlineContext::getCovFuncPtr( int i )
{
  if( 0 == _p_covs ) {
    this->_createCOVS();
  }
  
  if( this->isTreatedAsRing() ) {
    if( 0 == _eigentunes ) {
      this->_createEigentunes();
    }
  }

  if( !_momentsFuncsCalcd ) 
  { 
    int j;
    const int n = Particle::PSD;
    MatrixD covariance(n,n);
    Jet__environment*  storedEnv = 0;
    JetC__environment* storedEnvC = 0;
    JetParticle* ptr_arg = 0;
    coord** coordPtr = 0;

    if( this->isTreatedAsRing() ) {
      covariance = this->equilibriumCovariance( _eps_1, _eps_2 );

      storedEnv  = Jet::_lastEnv;
      storedEnvC = JetC::_lastEnv;
      Jet::_lastEnv = (Jet__environment*) (_p_jp->State().Env());
      JetC::_lastEnv = JetC__environment::CreateEnvFrom( Jet::_lastEnv );

      ptr_arg = _p_jp->Clone();
    }
    else {
      if( 0 != _initialCovariancePtr ) {
        covariance = _initialCovariancePtr->covariance;

        coordPtr = new coord* [n];

        // Preserve the current Jet environment
        storedEnv  = Jet::_lastEnv;
        storedEnvC = JetC::_lastEnv;

        // Create a new Jet environment
        double scale[n];
        //   scale is probably no longer needed ... oh, well ...
        Jet::BeginEnvironment( 1 );
        for( j = 0; j < n; j++ ) {
          scale[j] = 0.001;
          coordPtr[j] = new coord( _proton.State(j) );
        }
        JetC::_lastEnv = JetC__environment::CreateEnvFrom( Jet::EndEnvironment( scale ) );

        ptr_arg = _proton.ConvertToJetParticle();
      }

      else {
        _momentsFuncsCalcd = false;
        throw( GenericException( __FILE__, __LINE__, 
               "BeamlineContext::getCovFuncPtr", 
               "You must first provide initial conditions"
               "\nfor a non-periodic line." ) );
      }
    }

    Mapping id( "identity" );
    ptr_arg->setState( id );

    int errorFlag = _p_covs->doCalc( ptr_arg, covariance, beamline::yes );
    _momentsFuncsCalcd = ( 0 == errorFlag );

    // Clean up before leaving 
    delete ptr_arg;
    if( coordPtr ) {
      for( j = 0; j < n; j++ ) { delete coordPtr[j]; }
      delete [] coordPtr;
    }

    // Restore current environment
    Jet::_lastEnv = storedEnv;
    JetC::_lastEnv = storedEnvC;
  }

  return (_p_covs->getInfoPtr(i));
}


const LBSage::Info* BeamlineContext::getLBFuncPtr( int i )
{
  if( 0 == _p_lbs ) {
    this->_createLBS();
  }
  
  // REMOVE: if( ( 0 == _p_cos ) || ( 0 == _p_jp ) ) {
  if( ( 0 == _p_co_p ) || ( 0 == _p_jp ) ) {
    try {
      _createClosedOrbit(); 
    }
    catch( const GenericException& ge ) {
      _deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }
  // At this point, _p_jp's state is that after one-turn on the
  //   closed orbit. It's environment is centered on the closed
  //   orbit and may be out of synch with the current environment.


  if( !_LBFuncsCalcd ) {
    // Preserve current Jet environment
    //   and reset to that of *_p_jp
    Jet__environment*  storedEnv  = Jet::_lastEnv;
    JetC__environment* storedEnvC = JetC::_lastEnv;
    Jet::_lastEnv = (Jet__environment*) (_p_jp->State().Env());
    JetC::_lastEnv = JetC__environment::CreateEnvFrom( Jet::_lastEnv );

    _LBFuncsCalcd = ( 0 == _p_lbs->doCalc( _p_jp, beamline::yes ) );

    // Restore current environment
    Jet::_lastEnv = storedEnv;
    JetC::_lastEnv = storedEnvC;
  }

  if( _LBFuncsCalcd ) { return (_p_lbs->getInfoPtr(i)); }
  else                { return 0;                       }
}


const DispersionSage::Info* BeamlineContext::getDispersionPtr( int i )
{
  if( 0 == _p_dsps ) {
    this->_createDSPS();
  }
  
  if( !_dispersionFuncsCalcd ) 
  { 
    if( this->isTreatedAsRing() ) {
      // REMOVE: if( ( 0 == _p_cos ) || ( 0 == _p_jp ) ) {
      if( ( 0 == _p_co_p ) || ( 0 == _p_jp ) ) {
        try {
          this->_createClosedOrbit(); 
        }
        catch( const GenericException& ge ) {
          this->_deleteDSPS(); 
          this->_deleteClosedOrbit();
          throw ge;
        }
      }

      // Preserve/reset the current Jet environment
      Jet__environment*  storedEnv  = Jet::_lastEnv;
      JetC__environment* storedEnvC = JetC::_lastEnv;
      Jet::_lastEnv = (Jet__environment*) (_p_jp->State().Env());
      JetC::_lastEnv = JetC__environment::CreateEnvFrom( Jet::_lastEnv );

      // DispersionSage::Options newOptions;
      // newOptions.onClosedOrbit = true;
      // _p_dsps->set_options( newOptions );
    
      _p_dsps->flags.onClosedOrbit = true;
      int errorFlag = _p_dsps->doCalc( _p_jp, beamline::yes );
      _dispersionFuncsCalcd = ( 0 == errorFlag );

      // Restore current environment
      Jet::_lastEnv = storedEnv;
      JetC::_lastEnv = storedEnvC;
    }

    // If the line is not treated as periodic, do the following:
    else {
      if( 0 != _initialDispersionPtr ) {
        int errorFlag = _p_dsps->pushCalc( _proton, *_initialDispersionPtr );
        _dispersionFuncsCalcd = ( 0 == errorFlag );
      }
      else {
        _dispersionFuncsCalcd = false;
        throw( GenericException( __FILE__, __LINE__, 
               "BeamlineContext::getDispersionPtr( int i )", 
               "You must first provide initial conditions"
               "\nfor a non-periodic line." ) );
      }
    }
  }

  if( _dispersionFuncsCalcd ) { return (_p_dsps->getInfoPtr(i)); }
  else                        { return 0;                        }
}



MatrixD BeamlineContext::equilibriumCovariance()
{
  return this->equilibriumCovariance( _eps_1, _eps_2 );
}


MatrixD BeamlineContext::equilibriumCovariance( double eps_1, double eps_2 )
{
  // eps_1 and eps_2 are two "invariant emittances"
  //   in units of pi mm-mr.
  // I assume that eps_1 is "mostly horizontal" and eps_2 is
  //   "mostly vertical."

  int i=0, j=0;
  const double mm_mr = 1.0e-6;

  // If necessary ...
  // REMOVE: if( ( 0 == _p_cos ) || ( 0 == _p_jp ) ) {
  if( ( 0 == _p_co_p ) || ( 0 == _p_jp ) ) {
    try {
      _createClosedOrbit(); 
    }
    catch( const GenericException& ge ) {
      _deleteClosedOrbit(); // Almost certainly not necessary.
      throw ge;
    }
  }

  double betaGamma = _p_jp->ReferenceBeta() * _p_jp->ReferenceGamma();

  // Convert to action, in meters (crazy units)
  double I1 = ( std::abs( eps_1 )/betaGamma ) * mm_mr / 2.0;
  double I2 = ( std::abs( eps_2 )/betaGamma ) * mm_mr / 2.0;

  int x   = Particle::_x();
  int y   = Particle::_y();
  int cdt = Particle::_cdt();
  int xp  = Particle::_xp(); 
  int yp  = Particle::_yp(); 
  int dpp = Particle::_dpop();

  int n = Particle::PSD;

  MatrixD aa(n,n);
  aa(x,x)     = I1;
  aa(xp,xp)   = I1;
  aa(y,y)     = I2;
  aa(yp,yp)   = I2;

  MatrixC E(n,n);
  E = ( _p_jp->State() ).Jacobian().eigenVectors();
  BmlUtil::normalize( E );

  MatrixD cov(n,n);
  cov = real(E*aa*E.dagger());

  return cov;
}


// Adjuster methods

int BeamlineContext::addHTuneCorrector( const bmlnElmnt* x ) 
{
  if( 0 == _p_ta ) {
    _p_ta = new TuneAdjuster( _p_bml, false );
  }
    
  if( 0 == strcmp( x->Type(), "quadrupole" ) ) {
    _p_ta->addCorrector( dynamic_cast<const quadrupole*>(x), 1.0, 0.0 );
  }  
  else if( 0 == strcmp( x->Type(), "thinQuad"   ) ) {
    _p_ta->addCorrector( dynamic_cast<const thinQuad*>(x), 1.0, 0.0 );
  }  
  else {
    return 1;
  }

  return 0;
}

int BeamlineContext::addVTuneCorrector( const bmlnElmnt* x ) 
{
  if( 0 == _p_ta ) {
    _p_ta = new TuneAdjuster( _p_bml, false );
  }
    
  if( 0 == strcmp( x->Type(), "quadrupole" ) ) {
    _p_ta->addCorrector( dynamic_cast<const quadrupole*>(x), 0.0, 1.0 );
  }  
  else if( 0 == strcmp( x->Type(), "thinQuad"   ) ) {
    _p_ta->addCorrector( dynamic_cast<const thinQuad*>(x), 0.0, 1.0 );
  }  
  else {
    return 1;
  }

  return 0;
}


int BeamlineContext::changeTunesBy( double dnuh, double dnuv )
{
  if( 0 == _p_ta ) {
    return NO_TUNE_ADJUSTER;
  }

  JetProton jp( _p_bml->Energy() );
  _p_ta->changeTunesBy( dnuh, dnuv, jp );

  _deleteLFS();
  return OKAY;
}


// Iterator functions

int BeamlineContext::beginIterator()
{
  if(_p_rbi||_p_dbi||_p_drbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_bi) { _p_bi->reset(); }
  else      { _p_bi = new BeamlineIterator(_p_bml); }

  return 0;
}


int BeamlineContext::beginDeepIterator()
{
  if(_p_bi||_p_rbi||_p_drbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_dbi) { _p_dbi->reset(); }
  else       { _p_dbi = new DeepBeamlineIterator(_p_bml); }

  return 0;
}


int BeamlineContext::beginReverseIterator()
{
  if(_p_bi||_p_dbi||_p_drbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_rbi) { _p_rbi->reset(); }
  else       { _p_rbi = new ReverseBeamlineIterator(_p_bml); }

  return 0;
}


int BeamlineContext::beginDeepReverseIterator()
{
  if(_p_bi||_p_rbi||_p_dbi)
  { 
    cerr << "*** WARNING *** "
         << __FILE__ 
         << ", line "
         << __LINE__
         << ": Only one BeamlineContext::Iterator at a time." 
         << endl;
    return -1;
  }

  if(_p_drbi) { _p_drbi->reset(); }
  else        { _p_drbi = new DeepReverseBeamlineIterator(_p_bml); }

  return 0;
}


void BeamlineContext::endIterator()
{
  if(_p_bi) 
  { delete _p_bi;
    _p_bi = 0;
  }
}


void BeamlineContext::endDeepIterator()
{
  if(_p_dbi) 
  { delete _p_dbi;
    _p_dbi = 0;
  }
}


void BeamlineContext::endReverseIterator()
{
  if(_p_rbi) 
  { delete _p_rbi;
    _p_rbi = 0;
  }
}


void BeamlineContext::endDeepReverseIterator()
{
  if(_p_drbi) 
  { delete _p_drbi;
    _p_drbi = 0;
  }
}


const bmlnElmnt* BeamlineContext::i_next()
{
  if(_p_bi) { return (*_p_bi)++; }
  else      { return 0; }
  return 0;
}


const bmlnElmnt* BeamlineContext::di_next()
{
  if(_p_dbi) { return (*_p_dbi)++; }
  else       { return 0; }
  return 0;
}


const bmlnElmnt* BeamlineContext::ri_next()
{
  if(_p_rbi) { return (*_p_rbi)++; }
  else       { return 0; }
  return 0;
}


const bmlnElmnt* BeamlineContext::dri_next()
{
  if(_p_drbi) { return (*_p_drbi)++; }
  else        { return 0; }
  return 0;
}


void BeamlineContext::i_reset()
{
  _p_bi->reset();
}


void BeamlineContext::di_reset()
{
  _p_dbi->reset();
}


void BeamlineContext::ri_reset()
{
  _p_rbi->reset();
}


void BeamlineContext::dri_reset()
{
  _p_drbi->reset();
}


ostream& operator<<( ostream& os, const BeamlineContext& x )
{
  os << "Begin BeamlineContext" << endl;
  if( x._p_bml ) 
  { os << "_p_bml" << endl;
    os << *(x._p_bml);
  }
  os << "End BeamlineContext" << endl;
  return os;
}


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
    if( 0 == strcmp( charBuffer, "_p_bml" ) )
    {
      is >> *(x._p_bml);
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


