/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      TuneAdjuster.cc
******  Version:   2.0
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******
****** REVISION HISTORY:
****** 
******  Jan-Mar  2007:  Jean-Francois Ostiguy
******                  ostiguy@fnal.gov
****** 
******  - Modified code to handle smart pointers
******  - use value semantics for matrix members
******  - use new style STL compatible iterators
******  - test for presence of slots moved to standalone function slotFound() 
******
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

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
 *
 */


#include <physics_toolkit/TuneAdjuster.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/iosetup.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/LattFuncSage.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneAdjuster::TuneAdjuster( BmlPtr x ) 
: Sage( x ), f_(), c_(2,1) { }

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneAdjuster::TuneAdjuster( beamline const& x ) 
  : Sage( x ), f_(), c_(2,1) { } 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TuneAdjuster::~TuneAdjuster() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool TuneAdjuster::isaThinQuad( bmlnElmnt const& q ) const
{
  return ( 0 == strcmp( q.Type(), "thinQuad" ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool TuneAdjuster::isaQuad( bmlnElmnt const& q ) const
{
  return ( 0 == strcmp( q.Type(), "quadrupole" ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool TuneAdjuster::isQuadLike(  bmlnElmnt const& q ) const
{
  return ( isaQuad(q) || isaThinQuad(q) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int  TuneAdjuster::numberOfCorrectors() const
{
  return correctors_.size();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD& TuneAdjuster::getControls()
{
  return c_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool TuneAdjuster::slotFound() {

  bool found = false;

  for ( beamline::deep_iterator it  = myBeamlinePtr_->deep_begin();  
                                it != myBeamlinePtr_->deep_end(); ++it ) {


    if( strstr( "CF_rbend|rbend|Slot", (*it)->Type() ) ) {
      found = true;
      break;
    }
  }
  return found;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneAdjuster::addCorrector_private(  ElmPtr x, double a, double b )
{

  MatrixD old_f  =  f_;

  correctors_.push_back(x);
  
  int ncor =  correctors_.size();

  f_ = MatrixD(ncor, 2 );

  for( int i=0;  i < ncor-1; ++i) {
    f_(i,0) = old_f(i,0);
    f_(i,1) = old_f(i,1);
  }

  f_( ncor-1, 0 )    = a;
  f_( ncor-1, 1 )    = b;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneAdjuster::addCorrector( ElmPtr x, double a, double b ) {

  // This form is used by the GUI code. It is not taking advantage of the
  // compiler to perform type checking. It *must* to go away !!!! 

  QuadrupolePtr pquad;
  ThinQuadPtr   ptquad;

  if ( pquad  = boost::dynamic_pointer_cast<quadrupole>(x)  ) 
  {  
     addCorrector( pquad,  a, b );
     return;
  }

  if ( ptquad = boost::dynamic_pointer_cast<thinQuad>(x)    ) 
     addCorrector( ptquad, a, b ); 

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneAdjuster::addCorrector( quadrupole const& x, double a, double b )
{
   addCorrector_private( QuadrupolePtr( x.Clone() ), a, b );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneAdjuster::addCorrector( QuadrupolePtr  x, double a, double b )
{
   addCorrector_private( x, a, b );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneAdjuster::addCorrector( thinQuad const& x, double a, double b )
{
   addCorrector_private( ThinQuadPtr( x.Clone()), a, b );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneAdjuster::addCorrector( ThinQuadPtr x, double a, double b )
{
  addCorrector_private( x, a, b );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int TuneAdjuster::changeTunesBy ( double x, double y, JetParticle const& jp )
{
  double delta_H = x;
  double delta_V = y;

  myBeamlinePtr_->dataHook.eraseAll( "Tunes" );
  myBeamlinePtr_->eraseBarnacles( "Twiss" );

  JetParticle jpr(jp);

  // Calculate current lattice functions
  LattFuncSage lfs( myBeamlinePtr_ );

  myBeamlinePtr_->propagate( jpr );


  // This Fast_CS_Calc does not work if there are Slot's!!!  Take action!

  if( slotFound() ) {
    lfs.Slow_CS_Calc( jpr );
  }
  else {
    lfs.Fast_CS_Calc( jpr);
  }  
  

  int N = numberOfCorrectors();

  MatrixD beta      (2,N);
  MatrixD delta_nu  (2,1);


  LattFuncSage::lattFunc lf;

  for( int j=0; j < N; ++j) {
 
   BarnacleList::iterator it = correctors_[j]->dataHook.find( "Twiss" );

    if( it ==  correctors_[j]->dataHook.end() ) {
      throw( GenericException( __FILE__, __LINE__, 
             "int TuneAdjuster::changeTunesBy ( double x, double y, const JetParticle& jp )", 
             "No lattice functions." ) );
    }

    lf = boost::any_cast<LattFuncSage::lattFunc>(it->info);

    beta(0,j) =   lf.beta.hor;
    beta(1,j) = - lf.beta.ver;
  } // for 
  

  // Adjust tunes and recalculate

  delta_nu(0,0) = delta_H;
  delta_nu(1,0) = delta_V;
 
  double brho = jpr.ReferenceBRho();

  c_         = (4.0*M_PI*brho)*( (beta*f_).inverse() * delta_nu );

  MatrixD w = f_ * c_;
 
  int j=0;
  for( std::vector<ElmPtr>::iterator it = correctors_.begin(); 
                                     it != correctors_.end(); ++it, ++j) 
  {
    if( isQuadLike(**it) ) {
      if( isaThinQuad(**it) ) {
        (*it)->setStrength( (*it)->Strength() + w(j,0) );
      }
      else {
        (*it)->setStrength( (*it)->Strength() + (w(j,0)/(*it)->Length()) );
      }
    }
  }

  // Clean up ...

  myBeamlinePtr_->dataHook.eraseAll( "Tunes" );
  myBeamlinePtr_->eraseBarnacles( "Twiss" );

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int TuneAdjuster::changeHorizontalTuneBy ( double delta_H, JetParticle const& jp )
{

  myBeamlinePtr_->dataHook.eraseAll( "Tunes" );
  myBeamlinePtr_->eraseBarnacles( "Twiss" );

  JetParticle jpr(jp);

  // Calculate current lattice functions
  LattFuncSage lfs( myBeamlinePtr_ );
 
  myBeamlinePtr_->propagate( jpr );


  if( slotFound() ) {
    lfs.Slow_CS_Calc( jpr );
  }
  else {
    lfs.Fast_CS_Calc( jpr );
  }  
  
  // lfs.Fast_CS_Calc( jpr );
  // This Fast_CS_Calc does not work if there are Slot's!!!  Take action!

  int N = correctors_.size();

  MatrixD beta(1,N);
  MatrixD f   (N,1);

  // delta_H = beta*f*c/4*pi*brho*;
  // w = f*c
  // delta strength_k = w_k
 
  LattFuncSage::lattFunc lf;

  for( int j=0; j < N; ++j) 
  {
    f(j,0) = f_(j,0);

    BarnacleList::iterator it = correctors_[j]->dataHook.find( "Twiss" );

    if(  it == correctors_[j]->dataHook.end()  ) {
      throw( GenericException( __FILE__, __LINE__, 
             "int TuneAdjuster::changeHorizontalTuneBy ( double delta_H, ", 
             "No lattice functions." ) );
    }

    lf = boost::any_cast<LattFuncSage::lattFunc>(it->info);

    beta(0,j) = lf.beta.hor;

  }
  
  // Adjust tune 

  double brho = jpr.ReferenceBRho();

  double c = (beta*f)(0,0);
         c = (4.0*M_PI*brho)*( delta_H/c );

  MatrixD w  = c*f;
 
  int j=0;
  for( std::vector<ElmPtr>::iterator it  = correctors_.begin(); 
                                     it != correctors_.end(); ++it, ++j) {

    if( isQuadLike(**it) ) {
      if( isaThinQuad(**it) ) {
        (*it)->setStrength( (*it)->Strength() + w(j,0) );
      }
      else {
        (*it)->setStrength( (*it)->Strength() + (w(j,0)/(*it)->Length()) );
      }
    }
  }

  c_(0,0) = c;
  c_(1,0) = 0.0;

  // Clean up ...

  myBeamlinePtr_->dataHook.eraseAll( "Tunes" );
  myBeamlinePtr_->eraseBarnacles( "Twiss" );

  return 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int TuneAdjuster::changeVerticalTuneBy ( double delta_V, JetParticle const& jp )
{

  myBeamlinePtr_->dataHook.eraseAll( "Tunes" );
  myBeamlinePtr_->eraseBarnacles( "Twiss" );

  
  JetParticle jpr(jp);

  // Calculate current lattice functions
  LattFuncSage lfs( myBeamlinePtr_ );
 
  myBeamlinePtr_->propagate( jpr );

  // Check for Slots


  // Fast_CS_Calc does not work if there are Slot's!!!  Take action!

  if( slotFound() ) {
    lfs.Slow_CS_Calc( jpr );
  }
  else {
    lfs.Fast_CS_Calc( jpr );
  }  
  
  int N = correctors_.size();

  MatrixD beta(1,N);
  MatrixD f   (N,1);

  // delta_V = beta*f*c/4*pi*brho*;
  // w = f*c
  // delta strength_k = w_k
 
  LattFuncSage::lattFunc lf;

  for( int j=0; j < N; ++j) 
  {
    f(j,0) = f_(j,1);

    BarnacleList::iterator it = correctors_[j]->dataHook.find( "Twiss" );

    if( it == correctors_[j]->dataHook.end() ) {
      throw( GenericException( __FILE__, __LINE__, 
             "int TuneAdjuster::changeVerticalTuneBy ( double delta_V, ", 
             "No lattice functions." ) );
    }
    
    lf = boost::any_cast<LattFuncSage::lattFunc>(it->info);
    beta(0,j) = lf.beta.ver;
  }
  
  // Adjust tune 
  double brho = jpr.ReferenceBRho();

  double c = (beta*f)(0,0);
  c = (4.0*M_PI*brho)*( delta_V/c );
  MatrixD w = c*f;
 
  int j=0;
  for( std::vector<ElmPtr>::iterator it = correctors_.begin(); 
                                     it != correctors_.end(); ++it, ++j) {
 
    if( isQuadLike(**it) ) {
      if( isaThinQuad(**it) ) {
        (*it)->setStrength( (*it)->Strength() + w(j,0) );
      }
      else {
        (*it)->setStrength( (*it)->Strength() + (w(j,0)/(*it)->Length()) );
      }
    }
  }

  c_(0,0) = 0.0;
  c_(1,0) = c;

  // Clean up ...

  myBeamlinePtr_->dataHook.eraseAll( "Tunes" );
  myBeamlinePtr_->eraseBarnacles( "Twiss" );

  return 0;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void TuneAdjuster::eraseAll()
{
  (*FNAL::pcerr) << "\nWARNING: TuneAdjuster::eraseAll is not written" 
                 << endl;
}

