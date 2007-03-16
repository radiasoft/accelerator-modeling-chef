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
******  Dec 2006 - Jean-Francois Ostiguy 
******             ostiguy@fnal
******    
******  - interface based on Particle& rather than ptrs. 
******    Stack allocated local Particle objects.
******  - changes to accomodate new boost::any based Barnacle objects.
******  - use new style STL-compatible beamline iterators
******  - calcs_ array is now an STL vector. LF are now returned by 
******    returning a const reference to the entire vector.
******  - misc cleanup.  
******                                                               
****** REVISION HISTORY
******
****** Mar 2007        ostiguy@fnal.gov
****** -efficiency improvements
****** -use new-style STL-compatible beamline iterators
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/CovarianceSage.h>

using namespace std;

// ============================================================== //
// Global error codes

// Returned by 
// int CovarianceSage::doCalc( JetParticle& ptr_jp, 
//                             MatrixD cov, 
//                             beamline::Criterion& crit )
const int CovarianceSage::OKAY   = 0;
const int CovarianceSage::NOTSQR = 1;
// Argument cov is not a square matrix.
const int CovarianceSage::OVRRUN = 2;
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CovarianceSage::Info& CovarianceSage::Info::operator=( const CovarianceSage::Info& x )
{
  if( this == &x )  return *this;

   arcLength  = x.arcLength;
   covariance = x.covariance;
   beta.hor   = x.beta.hor;
   beta.ver   = x.beta.ver;
   alpha.hor  = x.alpha.hor;
   alpha.ver  = x.alpha.ver;

  return *this;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CovarianceSage::CovarianceSage( BmlPtr x ) 
: Sage( x)
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CovarianceSage::CovarianceSage( beamline const& x) 
: Sage( x )
{}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int CovarianceSage::doCalc( JetParticle& jp, MatrixD const& cov, beamline::Criterion& crit )
{
  // PRECONDITION:   The JetParticle must be on the closed
  //                 orbit with the identity mapping for its state.
  //                 Its Jet environment's reference point 
  //                 should be the closed orbit. It is not reset.
  //                 cov is the covariance matrix in particle
  //                 coordinates at the beginning of the line. It need not
  //                 be the equilibrium covariance matrix.
  // POSTCONDITIONS: The JetParticle has the one-turn
  //                 mapping for its state.
  //                * Every element in the beamline that
  //          NO!   * matches the criterion has attached a Barnacle
  //                * labelled CovarianceSage that possesses and
  //                * CovarianceSage::Info data struct.
  //                 The beamline has attached to it
  //                 a Barnacle labelled eigentunes that possesses
  //                 an CovarianceSage::Tunes data struct. 
 
  // This routine has been partially copied from
  //   TevatronOptics/calcs/ellipse/ellipse_3.cc



  // A little paranoia check.
  int r = cov.rows();
  if( cov.cols() != r ) {
    *errorStreamPtr_ 
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

  const int x  = Particle::xIndex(); 
  const int y  = Particle::yIndex(); 
  const int xp = Particle::npxIndex();
  const int yp = Particle::npyIndex();


  // Clear the calcs_ array and delete its contents

  calcs_.clear();

  // Go through the line element by element

  double s          = 0.0;
  double normalizer = 0.0;
  Particle co_particle(jp);

  CovarianceSage::Info info;

  Matrix lcov    = cov;
  Matrix M;
  
  for (beamline::deep_iterator it = myBeamlinePtr_->deep_begin(); 
                               it != myBeamlinePtr_->deep_end(); ++it) 
  {
 

    (*it)->propagate( jp );
    s += (*it)->OrbitLength( co_particle );
  
    M           =  jp.State().Jacobian();
    lcov        =  M * cov * M.transpose();

    info.arcLength  = s;
    info.covariance = lcov;

    // ... "Horizontal" lattice functions

    normalizer = (lcov(x,x)*lcov(xp,xp)) - (lcov(x,xp)*lcov(xp,x));

    if( normalizer <= 0.0 ) {
      *errorStreamPtr_ 
           << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
           << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
           << "\n***ERROR*** Determinant of (projected) \"horizontal\" covariance matrix = "
           << normalizer
           << " <= 0.0."
           << endl;
      return NEGDET;
    }
    normalizer = 1.0/sqrt(normalizer);

    info.beta.hor  =   normalizer*lcov(x,x);
    info.alpha.hor = - normalizer*lcov(x,xp);

    // ... "Vertical" lattice functions

    normalizer = (lcov(y,y)*lcov(yp,yp)) - (lcov(y,yp)*lcov(yp,y));

    if( normalizer <= 0.0 ) {
      *errorStreamPtr_ 
           << "\n***ERROR*** File: " << __FILE__ << "  Line: " << __LINE__
           << "\n***ERROR*** int CovarianceSage::doCalc( JetParticle*, MatrixD, beamline::Criterion& )"
           << "\n***ERROR*** Determinant of (projected) \"vertical\" covariance matrix = "
           << normalizer
           << " <= 0.0."
           << endl;
      return NEGDET;
    }

    normalizer = 1.0/sqrt(normalizer);

    info.beta.ver  =   normalizer*lcov(y,y);
    info.alpha.ver = - normalizer*lcov(y,yp);

    calcs_.push_back(info);
   
  }

  // Finished

  return OKAY;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


std::vector<CovarianceSage::Info> const& CovarianceSage::getCovarianceArray()
{
  return calcs_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CovarianceSage::eraseAll() 
{
  myBeamlinePtr_->dataHook.eraseAll( "CovarianceSage" );

  for ( beamline::deep_iterator it  = myBeamlinePtr_->deep_begin();  
                                it != myBeamlinePtr_->deep_end(); ++it )
  { 
    (*it)->dataHook.eraseAll( "CovarianceSage" );
  }

}

