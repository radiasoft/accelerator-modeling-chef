/*
 *  File: ChromaticityAdjuster.cc
 *  
 *  Implementation for a sage that adjusts
 *  chromaticities given a collection of "correction"
 *  sextupoles
 *  
 *  Leo Michelotti
 *  February 1, 1999
 */


#include "ChromaticityAdjuster.h"
#include "LattFuncSage.h"

ChromaticityAdjuster::ChromaticityAdjuster( const beamline* x ) 
: Sage( x ), _numberOfCorrectors(0)
{
  _correctors = 0;
  _f = 0;
}

ChromaticityAdjuster::ChromaticityAdjuster( const beamline& x ) 
: Sage( &x ), _numberOfCorrectors(0)
{
  _correctors = 0;
  _f = 0;
}

ChromaticityAdjuster::~ChromaticityAdjuster() 
{
  if( _correctors ) delete [] _correctors;
  if( _f ) delete _f;
}


void ChromaticityAdjuster::_addCorrector( const bmlnElmnt* x,
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


void ChromaticityAdjuster::addCorrector( const sextupole& x, double a, double b )
{
  this->_addCorrector( &x, a, b );
}

void ChromaticityAdjuster::addCorrector( const sextupole* x, double a, double b )
{
  this->_addCorrector( x, a, b );
}

void ChromaticityAdjuster::addCorrector( const thinSextupole& x, double a, double b )
{
  this->_addCorrector( &x, a, b );
}

void ChromaticityAdjuster::addCorrector( const thinSextupole* x, double a, double b )
{
  this->_addCorrector( x, a, b );
}


int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetProton& jp ) const
{
  int j;
  double delta_H = x;
  double delta_V = y;

  _myBeamlinePtr->dataHook.eraseAll( "Ring" );
  _myBeamlinePtr->eraseBarnacles( "Ring" );

  // 
  JetProton jpr ( jp );   
  JetProton jpr2( jp );  // Necessary????
  JetProton jpr3( jp );  // Necessary????

  // Calculate current lattice functions
  LattFuncSage lfs( _myBeamlinePtr );
 
  _myBeamlinePtr->propagate( jpr );
  lfs.Fast_CS_Calc( &jpr  );
  lfs.Disp_Calc   ( &jpr2 );
 
  int N = this->numberOfCorrectors();
  MatrixD beta      (2,N);
  MatrixD delta_xi  (2,1);
  MatrixD c         (2,1);
  MatrixD w         (N,1);
  MatrixD dsp       (N,1);
  // delta_xi = beta*_f*c
  // w = _f*c
  // delta strength_k = 2 pi * brho * w_k / dps_k
 
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
    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Dispersion" );
    if(!ptr) {
      cout << "No dispersion." << endl;
      exit(1);
    }
    dsp(j,0)  =   ptr->dispersion.hor;
  }
  

  // Adjust tunes and recalculate
  delta_xi(0,0) = delta_H;
  delta_xi(1,0) = delta_V;
 
  c = (beta*(*_f)).inverse() * delta_xi;
  w = (*_f)*c;
 
  double brho = jpr.ReferenceBRho();
  for( j = 0; j < _numberOfCorrectors; j++ ) 
  {
    q = _correctors[j];
    if( _isaThinSextupole(q) ) {
      q->setStrength( q->Strength() + (M_TWOPI*brho*w(j,0)/dsp(j,0)) );
    }
    else {
      q->setStrength( q->Strength() + ((M_TWOPI*brho*w(j,0)/dsp(j,0))/q->Length()) );
    }
  }

 
  // Clean up ...
  // _myBeamlinePtr->dataHook.eraseAll( "Tunes" );
  // _myBeamlinePtr->eraseBarnacles( "Twiss" );
  return 0;
}


void ChromaticityAdjuster::eraseAll()
{
}
