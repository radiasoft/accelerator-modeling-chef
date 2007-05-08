/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      FPSolver.cc
******  Version:   1.5
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
******  REVISION HISTORY:
******
******  Jan - Mar 2007   ostiguy@fnal
******
******  - interface based on Particle& ./JetParticle& rather than ptrs.
******  - use stack variables for local Particle/JetParticles   
******  - object initialization optimizations
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/FPSolver.h>
#include <basic_toolkit/iosetup.h>


using FNAL::pcout;
using FNAL::pcerr;


using namespace std;

void mygaussj( MatrixD&, int, MatrixD&, int );
void mygaussj( MatrixD&, int, VectorD&);

#define FORALL(q)  for ( int q = 0; q < dimension_; q++ )


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FPinfo::FPinfo( double const& s, Vector const& u )
: arclength(s), state( u ) { }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FPSolver::FPSolver( BmlPtr bml, int n ) : dimension_(n) {

  if( n <= 0 ||  !bml ) {
    ostringstream uic;
    uic << "Incorrect parameters: bml = " << (int) bml.get() 
        << " n = " << n;
    throw( GenericException( __FILE__, __LINE__, 
           "FPSolver::FPSolver( beamline* bml, int n )",
           uic.str().c_str() ) );
  }

  jumpScale_ = new double [ n ];
  for( int i = 0; i < n; i++ ) jumpScale_[i] = 1.0e-9;

  zeroScale_ = new double [ n ];
  for( int i = 0; i < 4; i++ ) zeroScale_[i] = 1.0e-9;

  l_ = new int [4];
  l_[0] = 0; l_[1] = 1; l_[2] = 3; l_[3] = 4;
 
  bmLine_ = bml;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FPSolver::~FPSolver() {
  delete [] jumpScale_;  jumpScale_ = 0;
  delete [] zeroScale_;  zeroScale_ = 0;
  delete [] l_;          l_         = 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int FPSolver::operator()( Particle& p, const char*, FP_CRITFUNC Crit )
{
  if( dimension_ != p.State().Dim() ) {
    ostringstream uic;
    uic << "Dimension_s are not correct. " << dimension_ 
        << " != " << p.State().Dim();
    throw( GenericException( __FILE__, __LINE__, 
           "int FPSolver::operator()( Particle& p, const char*, FP_CRITFUNC Crit )",
           uic.str().c_str() ) );
  }

  bool jumpTest, zeroTest;


  // -----------------------------------------------  


  Vector zs( dimension_ );
  Vector z(4);
  for( int i = 0; i < 4; i++ ) z(i) = p.State( l_[i] );

  JetParticle jpr(p);
  bmLine_->propagate( jpr);

  MatrixD MM;
  MM = jpr.State().Jacobian();

  MatrixD M( 4, 4 );
  for( int i = 0; i < 4; i++ ) 
    for( int j = 0; j < 4; j++ ) 
      M( i, j ) = MM( l_[i], l_[j] );

  for( int i = 0; i < 4; i++ ) M( i, i ) -= 1.0;
  M = M.inverse();
  
  int iterCount = 0;
  Vector eps(4);
  do {
    bmLine_->propagate( p );

    // --- Has the state gone out of bounds? ---------
    for( int i = 0; i < Particle::PSD; i++ ) {
      if( isnan(p.State(i)) ) { 
        (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
        (*pcerr) << "FPSolver: *** ERROR *** p.State(" << i << ") is NaN." << endl;
        return -1; 
      }
    }
      
    for( int i = 0; i < 4; i++ ) eps(i) = z(i) - p.State( l_[i] );

    // --- Set up the tests --------------------------
    jumpTest = zeroTest = false;
    for( int i = 0; i < 4; i++ ) {
      if((  std::max(std::abs( z(i) ),std::abs( p.State( l_[i] ) )) > zeroScale_[i]  )) {
        zeroTest = true;
        jumpTest = jumpTest || 
          ( 
            ( std::abs( eps(i) ) >
            jumpScale_[i]*std::max(std::abs( z(i) ),std::abs( p.State( l_[i] ) )) )  
          );
      }
    }    

    if( (!jumpTest) || (!zeroTest) ) break;

    // --- Correct orbit and repeat ------------------
    z = z + M*eps;

    zs = p.State();
    for( int i = 0; i < 4; i++ ) zs( l_[i] ) = z(i);
    p.setState( zs );

  } while ( ++iterCount < 200 );
  

  // --- Cleaning up -------------------------------------------------
  if( iterCount >= 200 ) {
      (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
    (*pcerr) << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
      (*pcerr) << "FixedPoint:: Result may not be reliable!! " << endl;

    return 1;
  }

  if( !jumpTest || !zeroTest ) {
      (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
    (*pcerr) << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;
  } 
  // --- Store closed orbit if desired -------------------------------
  double startLength( 0.0 );
  if( Crit ) {
    zs = p.State();

    for (beamline::iterator it = bmLine_->begin();  it != bmLine_->end(); ++it ) {
       (*it)->propagate( p );
       startLength += (*it)->OrbitLength( p );
       if( (*Crit)( *it ) ) {
         (*it)->dataHook.append( Barnacle("FPS_orbit", FPinfo(startLength, p.State()) ) );
       }
    }

    p.setState( zs );
  }

  // --- Exit --------------------------------------------------------

  return 0;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int FPSolver::operator()( JetParticle& jpr, const char*, FP_CRITFUNC Crit )
{
   int ret = 0;

   jpr.setState( Mapping("Identity", jpr.State().Env() ) );  // Resets to identity.

   Particle p(jpr);

  // :::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) 
  {
    if( dimension_ != jpr.State().Dim() ) {
      (*pcerr) << "*** ERROR ***                                         \n"
              "*** ERROR *** FPSolver::operator()(JetParticle&, char*) \n"
              "*** ERROR *** Dimensions are not correct.             \n"
              "*** ERROR ***  " << dimension_ << " != " << jpr.State().Dim() << "\n"
              "*** ERROR ***                                         \n"
           << endl;
      ret = 2;
    }
  }

  // :::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) 
  {
    int i, j;
    char jumpTest, zeroTest;
  
    Vector zs( dimension_ );
    Vector z(4);
    for( i = 0; i < 4; i++ ) z(i) = p.State( l_[i] );
  
    bmLine_->propagate( jpr );
  
    MatrixD MM = jpr.State().Jacobian();
  
    MatrixD M( 4, 4 );
    for( i = 0; i < 4; i++ ) 
      for( j = 0; j < 4; j++ ) 
        M( i, j ) = MM( l_[i], l_[j] );
  
    for( i = 0; i < 4; i++ ) M( i, i ) -= 1.0;
    M = M.inverse();
    
    int iterCount = 0;
    Vector eps(4);
    do {

      bmLine_->propagate( p );

      // --- Has the state gone out of bounds? ---------
      for( i = 0; i < Particle::PSD; i++ ) {
        if( isnan(p.State(i)) ) { 
          (*pcerr) << "FPSolver (lineno: " << __LINE__ << " )  : *** ERROR *** p.State(" << i << ") is NaN." << endl;
          return -1; 
        }
      }
      
      for( i = 0; i < 4; i++ ) eps(i) = z(i) - p.State( l_[i] );
  
      // --- Set up the tests --------------------------
      jumpTest = zeroTest = 0;
      for( i = 0; i < 4; i++ ) {
        if((  std::max(std::abs( z(i) ),std::abs( p.State( l_[i] ) )) > zeroScale_[i]  )) {
          zeroTest = 1;
          jumpTest = jumpTest || 
            ( 
              ( std::abs( eps(i) ) >
              jumpScale_[i]*std::max(std::abs( z(i) ),std::abs( p.State( l_[i] ) )) )  
            );
        }
      }    
  
      if( (!jumpTest) || (!zeroTest) ) break;
  
      // --- Correct orbit and repeat ------------------
      z = z + M*eps;
  
      zs = p.State();
      for( i = 0; i < 4; i++ ) zs( l_[i] ) = z(i);
      p.setState( zs );
  
      iterCount++;
    } while ( iterCount < 200 );
    
  
    if( iterCount >= 200 ) {
      (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
      (*pcerr) << "FPSolver: More than 200 Newton's iterations attempted." << endl;
      (*pcerr) << "FPSolver: Result may not be reliable!! " << endl;
    }
  
  
    if( !jumpTest || !zeroTest ) 
      (*pcerr) << 
        "FPSolver:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;
    
    // --- Store closed orbit if desired -------------------------------
    double startLength( 0.0 );
    if( Crit ) {
      zs = p.State();
    
      for (beamline::iterator it = bmLine_->begin();  it != bmLine_->end(); ++it ) {
 
        (*it)->propagate( p );
         startLength += (*it)->OrbitLength( p );
         if( (*Crit)( *it ) ) {
           (*it)->dataHook.append( Barnacle("FPS_orbit", FPinfo(startLength, p.State() ) ) );
         }
      }
    
      p.setState( zs );
    }
  

    // --- Reset JetParticle* argument to contain the map ----------------
    // --- on the closed orbit.                         ----------------

    zs( p.cdtIndex() ) = 0.0;
    jpr.setState( zs );
    bmLine_->propagate( (jpr) );
    Mapping oneTurn( jpr.State() );
    for( int i = 0; i < dimension_; i++ ) {
      oneTurn(i) += ( zs(i) - oneTurn(i).standardPart() );
    }
    jpr.setState( oneTurn );

  } // End block: if( ret == 0 )


  // --- Exit --------------------------------------------------------

  return ret;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


int FPSolver::operator()( Particle& p, FP_CRITFUNC Crit )
{
  Vector z = p.State();

  if( dimension_ != z.Dim() ) {
    ostringstream uic;
    uic << "Dimensions are not correct. "
        << dimension_ << " != " << z.Dim();
    throw( GenericException( __FILE__, __LINE__, 
           "int FPSolver::operator()( Particle& p, FP_CRITFUNC Crit )",
           uic.str().c_str() ) );
  }

  int i;
  char jumpTest, zeroTest;


  // -----------------------------------------------  


  JetParticle jpr(p);
  bmLine_->propagate( jpr );

  MatrixD M;
  M = jpr.State().Jacobian();
  for( int i = 0; i < dimension_; i++ ) M( i, i ) -= 1.0;
  M = M.inverse();
  
  int iterCount = 0;
  Vector eps;
  do {
    bmLine_->propagate( p );

    // --- Has the state gone out of bounds? ---------
    for( int i = 0; i < Particle::PSD; i++ ) {
      if( isnan(p.State(i)) ) { 
        (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
        (*pcerr) << "FPSolver: *** ERROR *** p.State(" << i << ") is NaN." << endl;
        return -1; 
      }
    }
      
    eps = z - p.State();

    // --- Set up the tests --------------------------
    jumpTest = zeroTest = 0;
    FORALL(i) {
      if((  std::max(std::abs( z(i) ),std::abs( p.State(i) )) > zeroScale_[i]  )) {
        zeroTest = 1;
        jumpTest = jumpTest || 
          ( 
            ( std::abs( eps(i) ) >
            jumpScale_[i]*std::max(std::abs( z(i) ),std::abs( p.State(i) )) )  
          );
      }
    }    

    if( (!jumpTest) || (!zeroTest) ) break;

    // --- Correct orbit and repeat ------------------
    z = z + M*eps;
    p.setState( z );

    iterCount++;
  } while ( iterCount < 200 );
  

  // --- Cleaning up -------------------------------------------------
  if( iterCount >= 200 ) {
      (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
      (*pcerr) << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
      (*pcerr) << "FixedPoint:: Result may not be reliable!! " << endl;
    return 1;
  }

  if( !jumpTest ) 
    (*pcerr) << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  // --- Store closed orbit if desired -------------------------------
  double startLength( 0.0 );
  if( !Crit ) {
    z = p.State();

    for (beamline::iterator it = bmLine_->begin();  it != bmLine_->end(); ++it ) {
       (*it)->propagate( p );
       startLength += (*it)->OrbitLength( p );
       if( (*Crit)( *it )  )
         (*it)->dataHook.append( Barnacle("FPS_orbit", FPinfo( startLength, p.State() ) ) );
    }

    p.setState( z );
  }

  // --- Exit --------------------------------------------------------
  return 0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void FPSolver::operator()( JetParticle& p, FP_CRITFUNC Crit )
{
  dimension_ = 6;
  
  IntArray m(dimension_, 0);

  IntArray zero(dimension_,0);
 
  Jet     y;
  int     jumpTest, zeroTest;
  int      iterCount;
  
  iterCount = 0;

  MatrixD hessian(dimension_,dimension_);
  Vector eps(dimension_);
  
  MatrixD zzhessian(dimension_,dimension_);

  Vector zzeps(dimension_);
  Vector zzx(dimension_);

  Mapping w(dimension_);

  Vector particleCoord = p.getState().standardPart();

  FORALL(i) eps[i] = 0.0;

  do{

    FORALL(i) particleCoord[i] += eps[i];

    FORALL(i) {
      y.setVariable( i, p.State().Env() );
      w.SetComponent( i, y );
    }

    p.setState(w);
 
    bmLine_->propagate( p );
   
    w = p.State(); // get a copy of the state


    Vector u = w.standardPart();

    FORALL(i) m[i] = 0;
    FORALL(i) FORALL(j) {
      m[j] = 1;
      hessian(i,j) = w(i).derivative( m );
      m[j] = 0;                           
    }

    FORALL(i) {
      hessian(i,i) -= 1.0;
      eps[i] = particleCoord[i] - u[i];
    }


    // --- Set up the tests and correct orbit --------------------------
    jumpTest = 0;
    FORALL(i) 
      jumpTest = jumpTest || 
        (  
          ( std::abs( eps[i] ) >
          jumpScale_[i]*std::max(std::abs( particleCoord[i] ),std::abs(u[i])) )  
        );

    zeroTest = 0;
    FORALL(i)  {
      zeroTest = zeroTest || 
        ( std::max(std::abs( particleCoord[i] ),std::abs( u[i] )) > zeroScale_[i] );
    }


    // --- Iterative step ----------------------------------------------
    for( int i=0; i < dimension_; ++i ) {
      for( int j=0; j < dimension_; ++j ) 
        zzhessian(i,j) = 0.0;
    }
    zzeps = eps;
    zzx   = eps;
    zzhessian = hessian;
    mygaussj( zzhessian, dimension_, zzeps);
    
    eps = zzeps;
    
    ++iterCount;
  } while ( jumpTest && (iterCount < 200) && zeroTest );
  


  // --- Cleaning up -------------------------------------------------
  if( iterCount >= 200 ) {
      (*pcerr) << __FILE__ << " line no " << __LINE__ << std::endl;  
      (*pcerr) << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
      (*pcerr) << "FixedPoint:: Result may not be reliable!! " << endl;
  }

  if( !jumpTest ) 
    (*pcerr) << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  // --- Store closed orbit if desired -------------------------------

  double startLength( 0.0 );

  if( !Crit ) return;  

  Particle xp(p);

  for (beamline::iterator it = bmLine_->begin();  it != bmLine_->end(); ++it ) {

       (*it)->propagate( xp );
       startLength += (*it)->OrbitLength( xp );

       if( ! (*Crit)( *it ) ) continue;

       (*it)->dataHook.append( Barnacle( "FPS_orbit", new FPinfo( startLength, xp.State() ) ) );
  }


  // --- Exit --------------------------------------------------------
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void FPSolver::eraseAll() {
 for (beamline::iterator it = bmLine_->begin();  it != bmLine_->end(); ++it ) {
  (*it)->dataHook.eraseAll( "FPS_orbit" );
 }
}
 
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double& FPSolver::JumpScale( int i ) { 
  if( i >= 0 && i < dimension_ ) return jumpScale_[i];
  else {
    ostringstream uic;
    uic << "integer argument out of range. Either " 
        << i << " < 0 or " << dimension_ << " <= " << i;
    throw( GenericException( __FILE__, __LINE__, 
           "double& FPSolver::JumpScale( int i )",
           uic.str().c_str() ) );
  }

  return jumpScale_[i];  // This line will never be reached.
                         // It is here to keep a compiler happy.
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double& FPSolver::ZeroScale( int i ) { 

  if( i >= 0 && i < dimension_ ) return zeroScale_[i];
  else {
    ostringstream uic;
    uic << "integer argument out of range. Either " 
        << i << " < 0 or " << dimension_ << " <= " << i;
    throw( GenericException( __FILE__, __LINE__, 
           "double& FPSolver::ZeroScale( int i )",
           uic.str().c_str() ) );
  }

  return zeroScale_[i];  // This line will never be reached.
                        // It is here to keep a compiler happy.
}
