/*
 *  File: TuneAdjuster.cc
 *  
 *  Implementation for a sage that adjusts
 *  the tunes given a collection of "correction"
 *  quadrupoles.
 *  
 *  Leo Michelotti
 *  December 17, 1998
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
  lfs.Fast_CS_Calc( &jpr );
 

  int N = this->numberOfCorrectors();
  MatrixD beta      (2,N);
  MatrixD delta_nu  (2,1);
  MatrixD w         (N,1);

  // delta_nu = beta*f*c/4*pi*brho*;
  // w = f*c
  // delta strength_k = w_k
 
  LattFuncSage::lattFunc* ptr;
  bmlnElmnt* q;
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



void TuneAdjuster::eraseAll()
{
  cerr << "\nWARNING: TuneAdjuster::eraseAll is not written" << endl;
}

