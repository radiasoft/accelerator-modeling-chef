/*
 *  File: TuneAdjuster.cc
 *  
 *  Implementation for a sage that adjusts
 *  the tunes given a collection of "correction"
 *  quadrupoles.
 *  
 *  Leo Michelotti
 *  December 17, 1998
 *
 *  Put in check for Slots and use of new 
 *  LattFuncSage functions.
 *
 *  Leo Michelotti
 *  December 16, 1999
 *
 *  Added functions to control only horizontal
 *  or vertical tunes.
 *
 *  Leo Michelotti
 *  January 19, 2001
 *
 */


#include "TuneAdjuster.h"
#include "LattFuncSage.h"

TuneAdjuster::TuneAdjuster( const beamline* x ) 
: Sage( x ), _numberOfCorrectors(0), _c(2,1)
{
  _correctors = 0;
  _f = 0;
}

TuneAdjuster::TuneAdjuster( const beamline& x ) 
: Sage( &x ), _numberOfCorrectors(0), _c(2,1)
{
  _correctors = 0;
  _f = 0;
}

TuneAdjuster::~TuneAdjuster() 
{
  if( _correctors ) delete [] _correctors;
  if( _f ) delete _f;
}


void TuneAdjuster::_addCorrector( const bmlnElmnt* x,
                                  double a, double b )
{
  int         i;
  bmlnElmnt** oldCorrectors = 0;
  MatrixD*    old_f = 0;

  if( _numberOfCorrectors > 0 ) 
  {
    oldCorrectors = new bmlnElmnt* [ _numberOfCorrectors ];
    old_f = new MatrixD( _numberOfCorrectors, 2 );

    for( i = 0; i < _numberOfCorrectors; i++ ) 
    {
      oldCorrectors[i] = _correctors[i];

      (*old_f)(i,0) = (*_f)(i,0);
      (*old_f)(i,1) = (*_f)(i,1);
    }

    delete [] _correctors;
    _correctors = 0;   // Not really necessary.
    delete _f;
    _f = 0;            // Not really necessary.
  }

  _numberOfCorrectors++;
  _correctors = new bmlnElmnt* [ _numberOfCorrectors ];
  _f = new MatrixD( _numberOfCorrectors, 2 );
  for( i = 0; i < _numberOfCorrectors-1; i++ ) {
    _correctors[i] = oldCorrectors[i];
    (*_f)(i,0) = (*old_f)(i,0);
    (*_f)(i,1) = (*old_f)(i,1);
  }
  _correctors[ _numberOfCorrectors-1 ] = (bmlnElmnt*) x;
  (*_f)( _numberOfCorrectors-1, 0 )    = a;
  (*_f)( _numberOfCorrectors-1, 1 )    = b;


  if( oldCorrectors ) delete [] oldCorrectors;
  if( old_f )         delete old_f;
}


void TuneAdjuster::addCorrector( const quadrupole& x, double a, double b )
{
  this->_addCorrector( &x, a, b );
}

void TuneAdjuster::addCorrector( const quadrupole* x, double a, double b )
{
  this->_addCorrector( x, a, b );
}

void TuneAdjuster::addCorrector( const thinQuad& x, double a, double b )
{
  this->_addCorrector( &x, a, b );
}

void TuneAdjuster::addCorrector( const thinQuad* x, double a, double b )
{
  this->_addCorrector( x, a, b );
}


int TuneAdjuster::changeTunesBy ( double x, double y, const JetProton& jp )
{
  int j;
  double delta_H = x;
  double delta_V = y;

  _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  _myBeamlinePtr->eraseBarnacles( "Twiss" );

  // 
  JetProton jpr ( jp );   
  JetProton jpr2( jp );  // Necessary????
  JetProton jpr3( jp );  // Necessary????

  // Calculate current lattice functions
  LattFuncSage lfs( _myBeamlinePtr );
 
  _myBeamlinePtr->propagate( jpr );

  // Check for Slots
  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* q;
  char slotFound = 0;
  while((  q = dbi++  )) {
    if( strstr( "CF_rbend|rbend|Slot", q->Type() ) ) {
      slotFound = 1;
      break;
    }
  }
  dbi.reset();

  if( slotFound ) {
    lfs.Slow_CS_Calc( &jpr );
  }
  else {
    lfs.Fast_CS_Calc( &jpr );
  }  
  
  // lfs.Fast_CS_Calc( &jpr );
  // This Fast_CS_Calc does not work if there are Slot's!!!  Take action!

  int N = this->numberOfCorrectors();
  MatrixD beta      (2,N);
  MatrixD delta_nu  (2,1);
  MatrixD w         (N,1);

  // delta_nu = beta*f*c/4*pi*brho*;
  // w = f*c
  // delta strength_k = w_k
 
  LattFuncSage::lattFunc* ptr;

  for( j = 0; j < N; j++ ) 
  {
    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Twiss" );
    if(!ptr) {
      cout << "No lattice functions." << endl;
      exit(1);
    }
    beta(0,j) =   ptr->beta.hor;
    beta(1,j) = - ptr->beta.ver;
  }
  

  // Adjust tunes and recalculate
  delta_nu(0,0) = delta_H;
  delta_nu(1,0) = delta_V;
 
  double brho = jpr.ReferenceBRho();
  _c = (4.0*M_PI*brho)*( (beta*(*_f)).inverse() * delta_nu );
  w = (*_f)*_c;
 
  for( j = 0; j < _numberOfCorrectors; j++ ) 
  {
    q = _correctors[j];
    if( _isQuadLike(q) ) {
      if( _isaThinQuad(q) ) {
        q->setStrength( q->Strength() + w(j,0) );
      }
      else {
        q->setStrength( q->Strength() + (w(j,0)/q->Length()) );
      }
    }
  }


  // Clean up ...
  _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  _myBeamlinePtr->eraseBarnacles( "Twiss" );
  return 0;
}


int TuneAdjuster::changeHorizontalTuneBy ( double delta_H, 
                                           const JetProton& jp )
{
  int j;

  _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  _myBeamlinePtr->eraseBarnacles( "Twiss" );

  // 
  JetProton jpr ( jp );   
  JetProton jpr2( jp );  // Necessary????
  JetProton jpr3( jp );  // Necessary????

  // Calculate current lattice functions
  LattFuncSage lfs( _myBeamlinePtr );
 
  _myBeamlinePtr->propagate( jpr );

  // Check for Slots
  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* q;
  char slotFound = 0;
  while((  q = dbi++  )) {
    if( strstr( "CF_rbend|rbend|Slot", q->Type() ) ) {
      slotFound = 1;
      break;
    }
  }
  dbi.reset();

  if( slotFound ) {
    lfs.Slow_CS_Calc( &jpr );
  }
  else {
    lfs.Fast_CS_Calc( &jpr );
  }  
  
  // lfs.Fast_CS_Calc( &jpr );
  // This Fast_CS_Calc does not work if there are Slot's!!!  Take action!

  int N = this->numberOfCorrectors();
  MatrixD beta(1,N);
  MatrixD w   (N,1);
  MatrixD f   (N,1);
  double  c;

  // delta_H = beta*f*c/4*pi*brho*;
  // w = f*c
  // delta strength_k = w_k
 
  LattFuncSage::lattFunc* ptr;

  for( j = 0; j < N; j++ ) 
  {
    f(j,0) = (*_f)(j,0);

    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Twiss" );
    if(!ptr) {
      cout << "No lattice functions." << endl;
      exit(1);
    }
    beta(0,j) = ptr->beta.hor;
  }
  
  // Adjust tune 
  double brho = jpr.ReferenceBRho();
  c = (beta*f)(0,0);
  c = (4.0*M_PI*brho)*( delta_H/c );
  w = c*f;
 
  for( j = 0; j < _numberOfCorrectors; j++ ) 
  {
    q = _correctors[j];
    if( _isQuadLike(q) ) {
      if( _isaThinQuad(q) ) {
        q->setStrength( q->Strength() + w(j,0) );
      }
      else {
        q->setStrength( q->Strength() + (w(j,0)/q->Length()) );
      }
    }
  }

  _c(0,0) = c;
  _c(1,0) = 0.0;

  // Clean up ...
  _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  _myBeamlinePtr->eraseBarnacles( "Twiss" );
  return 0;
}


int TuneAdjuster::changeVerticalTuneBy ( double delta_V, 
                                         const JetProton& jp )
{
  int j;

  _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  _myBeamlinePtr->eraseBarnacles( "Twiss" );

  // 
  JetProton jpr ( jp );   
  JetProton jpr2( jp );  // Necessary????
  JetProton jpr3( jp );  // Necessary????

  // Calculate current lattice functions
  LattFuncSage lfs( _myBeamlinePtr );
 
  _myBeamlinePtr->propagate( jpr );

  // Check for Slots
  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* q;
  char slotFound = 0;
  while((  q = dbi++  )) {
    if( strstr( "CF_rbend|rbend|Slot", q->Type() ) ) {
      slotFound = 1;
      break;
    }
  }
  dbi.reset();

  if( slotFound ) {
    lfs.Slow_CS_Calc( &jpr );
  }
  else {
    lfs.Fast_CS_Calc( &jpr );
  }  
  
  // lfs.Fast_CS_Calc( &jpr );
  // This Fast_CS_Calc does not work if there are Slot's!!!  Take action!

  int N = this->numberOfCorrectors();
  MatrixD beta(1,N);
  MatrixD w   (N,1);
  MatrixD f   (N,1);
  double  c;

  // delta_V = beta*f*c/4*pi*brho*;
  // w = f*c
  // delta strength_k = w_k
 
  LattFuncSage::lattFunc* ptr;

  for( j = 0; j < N; j++ ) 
  {
    f(j,0) = (*_f)(j,1);

    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Twiss" );
    if(!ptr) {
      cout << "No lattice functions." << endl;
      exit(1);
    }
    beta(0,j) = ptr->beta.ver;
  }
  
  // Adjust tune 
  double brho = jpr.ReferenceBRho();
  c = (beta*f)(0,0);
  c = (4.0*M_PI*brho)*( delta_V/c );
  w = c*f;
 
  for( j = 0; j < _numberOfCorrectors; j++ ) 
  {
    q = _correctors[j];
    if( _isQuadLike(q) ) {
      if( _isaThinQuad(q) ) {
        q->setStrength( q->Strength() + w(j,0) );
      }
      else {
        q->setStrength( q->Strength() + (w(j,0)/q->Length()) );
      }
    }
  }


  _c(0,0) = 0.0;
  _c(1,0) = c;

  // Clean up ...
  _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  _myBeamlinePtr->eraseBarnacles( "Twiss" );
  return 0;
}



void TuneAdjuster::eraseAll()
{
  cerr << "\nWARNING: TuneAdjuster::eraseAll is not written" << endl;
}

