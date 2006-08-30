/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      CovarianceSage.cc
******  Version:   1.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <physics_toolkit/CovarianceSage.h>

using namespace std;

// ============================================================== //
// Global error codes

// Returned by 
// int CovarianceSage::doCalc( JetParticle* ptr_jp, 
//                             MatrixD cov, 
//                             beamline::Criterion& crit )
const int CovarianceSage::OKAY   = 0;
const int CovarianceSage::NOTSQR = 1;
// Argument cov is not a square matrix.
const int CovarianceSage::OVRRUN = 2;
// Dimension of _calcs array was overrun. 
// Indicates too many elements in the beamline
//   and a mismatch with the _calcs array.
const int CovarianceSage::NEGDET = 3;
// Determinant of a sub-covariance matrix is negative.
// Indicates that the argument cov was not a valid
//   covariance matrix.

// ============================================================== //

CovarianceSage::Info::Info() 
: covariance( Particle::PSD, Particle::PSD )
{
  arcLength = -1.0;
  beta.hor  = 0.0;
  beta.ver  = 0.0;
  alpha.hor = 0.0;
  alpha.ver = 0.0;
}


CovarianceSage::Info::Info( const CovarianceSage::Info& x ) 
: covariance( Particle::PSD, Particle::PSD )
{
  arcLength  = x.arcLength;
  beta.hor   = x.beta.hor;
  beta.ver   = x.beta.ver;
  alpha.hor  = x.alpha.hor;
  alpha.ver  = x.alpha.ver;
  covariance = x.covariance;
}


CovarianceSage::Info& CovarianceSage::Info::operator=( const CovarianceSage::Info& x )
{
  if( this != &x ) {
    this->arcLength  = x.arcLength;
    this->covariance = x.covariance;
    this->beta.hor   = x.beta.hor;
    this->beta.ver   = x.beta.ver;
    this->alpha.hor  = x.alpha.hor;
    this->alpha.ver  = x.alpha.ver;
  }

  return *this;
}


// ============================================================== //

CovarianceSage::CovarianceSage( const beamline* x, bool doClone ) 
: Sage( x, doClone ), _calcs(0), _n(0)
{
}


CovarianceSage::CovarianceSage( const beamline& x, bool doClone ) 
: Sage( &x, doClone ), _calcs(0), _n(0)
{
}


CovarianceSage::~CovarianceSage()
{
  _deleteCalcs();
}

// ============================================================== //

int CovarianceSage::doCalc( JetParticle* ptr_jp, MatrixD cov, beamline::Criterion& crit )
{
  // PRECONDITION:   The JetParticle must be on the closed
  //   orbit with the identity mapping for its state.
  //                 Its Jet environment's reference point 
  //   should be the closed orbit. It is not reset.
  //                 cov is the covariance matrix in particle
  //   coordinates at the beginning of the line. It need not
  //   be the equilibrium covariance matrix.
  // POSTCONDITIONS: The JetParticle has the one-turn
  //   mapping for its state.
  //                 Every element in the beamline that
  //   matches the criterion has attached a Barnacle
  //   labelled CovarianceSage that possesses and
  //   CovarianceSage::Info data struct.
  //                 The beamline has attached to it
  //   a Barnacle labelled eigentunes that possesses
  //   an CovarianceSage::Tunes data struct. 
 
  // This routine has been partially copied from
  //   TevatronOptics/calcs/ellipse/ellipse_3.cc

  // A little paranoia check.
  int r = cov.rows();
  if( cov.cols() != r ) {
    *_errorStreamPtr 
         << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
         << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
         << "\n***ERROR*** Argument covariance matrix is "
         << r
         << "x"
         << (cov.cols())
         << ", not square."
         << endl;
    return NOTSQR;
  }

  int i, j;

  const int x  = Particle::_x(); 
  const int y  = Particle::_y(); 
  const int xp = Particle::_xp();
  const int yp = Particle::_yp();

  // Symmetrize the matrix, just to be on the safe side.
  // cov = ( cov + cov.transpose() )/2.0;

  // Create the _calcs array
  _deleteCalcs();
  _n = _arrayPtr->size();
  _calcs = new Info* [_n];
  for( i = 0; i < _n; i++ ) { _calcs[i] = 0;  }


  Particle* p_co_p = ptr_jp->ConvertToParticle();  
  // MUST BE DELETED BEFORE RETURNING


  CovarianceSage::Info* infoPtr = 0;
  MatrixD initialCov( cov );
  Mapping map( ptr_jp->State() );
  MatrixD M( map.Jacobian() );

  MatrixD localCov( cov.rows(), cov.cols() );
  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* q;
  double s, normalizer;
  s = 0;
  i = 0;


  // Go through the line element by element
  while((  q = dbi++  )) {
    q->propagate( *ptr_jp );
    s += q->OrbitLength( *p_co_p );

    map = ptr_jp->State();
    M = map.Jacobian();
    cov = M * initialCov * M.transpose();

    infoPtr = new CovarianceSage::Info;
    if( i < _n ) {
      _calcs[i++] = infoPtr;
    }
    else {
      *_errorStreamPtr 
           << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
           << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
           << "\n***ERROR*** Overran dimension ("
           << _n
           << ")of _calc array."
           << "\n***ERROR*** Calculation is terminating."
           << endl;
      delete p_co_p;
      delete infoPtr;
      return OVRRUN;
    }

    // Push local data into the infoPtr
    infoPtr->arcLength = s;
    infoPtr->covariance = cov;

    // ... "Horizontal" lattice functions
    normalizer = (cov(x,x)*cov(xp,xp)) - (cov(x,xp)*cov(xp,x));
    if( normalizer <= 0.0 ) {
      *_errorStreamPtr 
           << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
           << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
           << "\n***ERROR*** Determinant of (projected) \"horizontal\" covariance matrix = "
           << normalizer
           << " <= 0.0."
           << endl;
      delete p_co_p;
      return NEGDET;
    }
    normalizer = 1.0/sqrt(normalizer);
    infoPtr->beta.hor  =   normalizer*cov(x,x);
    infoPtr->alpha.hor = - normalizer*cov(x,xp);

    // ... "Vertical" lattice functions
    normalizer = (cov(y,y)*cov(yp,yp)) - (cov(y,yp)*cov(yp,y));
    if( normalizer <= 0.0 ) {
      *_errorStreamPtr 
           << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
           << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
           << "\n***ERROR*** Determinant of (projected) \"vertical\" covariance matrix = "
           << normalizer
           << " <= 0.0."
           << endl;
      delete p_co_p;
      return NEGDET;
    }
    normalizer = 1.0/sqrt(normalizer);
    infoPtr->beta.ver  =   normalizer*cov(y,y);
    infoPtr->alpha.ver = - normalizer*cov(y,yp);
  }


  // Finished
  delete p_co_p;
  return OKAY;
}


// ============================================================== //

const CovarianceSage::Info* CovarianceSage::getInfoPtr( int j )
{
  if( 0 == _calcs || j < 0 || _n <= j ) { return 0; }
  return _calcs[j];
}


// ============================================================== //

void CovarianceSage::eraseAll() 
{
  _myBeamlinePtr->dataHook.eraseAll( "CovarianceSage" );

  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* be;
  while((  be = dbi++  )) {
    be->dataHook.eraseAll( "CovarianceSage" );
  }

  _deleteCalcs();
}

// ============================================================== //

void CovarianceSage::_deleteCalcs()
{
  if( 0 != _calcs ) {
    for( int i = 0; i < _n; i++ ) 
    { if( 0 != _calcs[i] ) {delete _calcs[i];} }
    delete [] _calcs;
    _calcs = 0;
    _n = 0;
  }
}

// ============================================================== //
