/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      ChromaticityAdjuster.cc
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

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


#include "GenericException.h"
#include "ChromaticityAdjuster.h"
#include "LattFuncSage.h"

using namespace std;

ChromaticityAdjuster::ChromaticityAdjuster( const beamline* x, bool doClone ) 
: Sage( x, doClone ), _numberOfCorrectors(0), _c(2,1)
{
  _correctors = 0;
  _f = 0;
}

ChromaticityAdjuster::ChromaticityAdjuster( const beamline& x, bool doClone ) 
: Sage( &x, doClone ), _numberOfCorrectors(0), _c(2,1)
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


int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetProton& jp )
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
  // lfs.Fast_CS_Calc( &jpr  );
  lfs.Slow_CS_Calc( &jpr  );
  lfs.Disp_Calc   ( &jpr2 );
 
  int N = this->numberOfCorrectors();
  MatrixD beta      (2,N);
  MatrixD delta_xi  (2,1);
  MatrixD w         (N,1);
  double  dsp;
  // delta_xi = beta*_f*c
  // w = _f*c
  // delta strength_k = 2 pi * brho * w_k / dps_k
 
  LattFuncSage::lattFunc* ptr;
  bmlnElmnt* q; 
  for( j = 0; j < N; j++ ) 
  {
    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Dispersion" );
    if(!ptr) {
      throw( GenericException( __FILE__, __LINE__, 
             "int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetProton& jp )", 
             "No dispersion information found." ) );
    }
    dsp       =   ptr->dispersion.hor;

    ptr = (LattFuncSage::lattFunc*) _correctors[j]->dataHook.find( "Twiss" );
    if(!ptr) {
      throw( GenericException( __FILE__, __LINE__, 
             "int ChromaticityAdjuster::changeChromaticityBy ( double x, double y, const JetProton& jp )", 
             "Lattice functions not yet calculated." ) );
    }
    beta(0,j) =   ptr->beta.hor * dsp;
    beta(1,j) = - ptr->beta.ver * dsp;
  }
  

  // Adjust tunes and recalculate
  delta_xi(0,0) = delta_H;
  delta_xi(1,0) = delta_V;
 
  double brho = jpr.ReferenceBRho();
  _c = (M_TWOPI*brho)*( (beta*(*_f)).inverse() * delta_xi );
  w = (*_f)*_c;
 
  for( j = 0; j < _numberOfCorrectors; j++ ) 
  {
    q = _correctors[j];
    if( _isaThinSextupole(q) ) {
      q->setStrength( q->Strength() + w(j,0) );
    }
    else {
      q->setStrength( q->Strength() + (w(j,0)/q->Length()) );
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
