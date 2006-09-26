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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <physics_toolkit/FPSolver.h>
#include <basic_toolkit/iosetup.h>


using FNAL::pcout;
using FNAL::pcerr;


using namespace std;

void mygaussj( MatrixD, int, MatrixD, int );

#define DMAX(a,b) (a>b?a:b)
#define FORALL(q)  for ( q = 0; q < dimension; q++ )


FPinfo::FPinfo( const double& s, const Vector& u ) : state( u.Dim() ) {
  arclength = s;
  state     = u;
}


FPSolver::FPSolver( beamline* bml, int n ) {
  int i;
  dimension = n;
  if( n <= 0 || bml == 0 ) {
    ostringstream uic;
    uic << "Incorrect parameters: bml = " << (int) bml 
        << " n = " << n;
    throw( GenericException( __FILE__, __LINE__, 
           "FPSolver::FPSolver( beamline* bml, int n )",
           uic.str().c_str() ) );
  }

  jumpScale = new double [ n ];
  for( i = 0; i < n; i++ ) jumpScale[i] = 1.0e-9;

  zeroScale = new double [ n ];
  for( i = 0; i < 4; i++ ) zeroScale[i] = 1.0e-9;

  l = new int [4];
  l[0] = 0; l[1] = 1; l[2] = 3; l[3] = 4;
 
  bmLine = bml;
}


FPSolver::~FPSolver() {
  delete [] jumpScale;  jumpScale = 0;
  delete [] zeroScale;  zeroScale = 0;
  delete [] l;          l         = 0;
}

           /* ------------------------------------- */
           /* ------------------------------------- */

int FPSolver::operator()( Particle* p, const char*, FP_CRITFUNC Crit )
{
  if( dimension != p->State().Dim() ) {
    ostringstream uic;
    uic << "Dimensions are not correct. " << dimension 
        << " != " << p->State().Dim();
    throw( GenericException( __FILE__, __LINE__, 
           "int FPSolver::operator()( Particle* p, const char*, FP_CRITFUNC Crit )",
           uic.str().c_str() ) );
  }

  int i, j;
  char jumpTest, zeroTest;


  // -----------------------------------------------  


  Vector zs( dimension );
  Vector z(4);
  for( i = 0; i < 4; i++ ) z(i) = p->State( l[i] );

  JetParticle* jprPtr = p->ConvertToJetParticle();
  bmLine->propagate( *jprPtr );

  MatrixD MM;
  MM = jprPtr->State().Jacobian();

  MatrixD M( 4, 4 );
  for( i = 0; i < 4; i++ ) 
    for( j = 0; j < 4; j++ ) 
      M( i, j ) = MM( l[i], l[j] );

  for( i = 0; i < 4; i++ ) M( i, i ) -= 1.0;
  M = M.inverse();
  
  int iterCount = 0;
  Vector eps(4);
  do {
    bmLine->propagate( *p );

    // --- Has the state gone out of bounds? ---------
    for( i = 0; i < Particle::PSD; i++ ) {
      if( isnan(p->State(i)) ) { 
        (*pcerr) << "FPSolver: *** ERROR *** p->State(" << i << ") is NaN." << endl;
        delete jprPtr; jprPtr = 0;
        return -1; 
      }
    }
      
    for( i = 0; i < 4; i++ ) eps(i) = z(i) - p->State( l[i] );

    // --- Set up the tests --------------------------
    jumpTest = zeroTest = 0;
    for( i = 0; i < 4; i++ ) {
      if((  DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) > zeroScale[i]  )) {
        zeroTest = 1;
        jumpTest = jumpTest || 
          ( 
            ( fabs( eps(i) ) >
            jumpScale[i]*DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) )  
          );
      }
    }    

    if( (!jumpTest) || (!zeroTest) ) break;

    // --- Correct orbit and repeat ------------------
    z = z + M*eps;

    zs = p->State();
    for( i = 0; i < 4; i++ ) zs( l[i] ) = z(i);
    p->setState( zs );

    iterCount++;
  } while ( iterCount < 200 );
  

  // --- Cleaning up -------------------------------------------------
  if( iterCount >= 200 ) {
    (*pcerr) << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
    (*pcerr) << "FixedPoint:: Result may not be reliable!! " << endl;

    delete jprPtr; jprPtr = 0;
    return 1;
  }

  if( !jumpTest || !zeroTest ) 
    (*pcerr) << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  // --- Store closed orbit if desired -------------------------------
  double startLength( 0.0 );
  if( Crit ) {
    zs = p->State();

    dlist_iterator getNext ( *bmLine );
    bmlnElmnt* q;
    while ((  q = (bmlnElmnt*) getNext()  )) {
       q->propagate( *p );
       startLength += q->OrbitLength( *p );
       if( (*Crit)( q ) ) {
         q->dataHook.append( "FPS_orbit", new FPinfo( startLength, p->State() ) );
       }
    }

    p->setState( zs );
  }

  // --- Exit --------------------------------------------------------
  delete jprPtr; jprPtr = 0;
  return 0;

}

            /* ------------------------------------- */
           /* ------------------------------------- */

int FPSolver::operator()( JetParticle* p_jpr, const char*, FP_CRITFUNC Crit )
{
  int ret = 0;
  Particle* p = 0;

  if( p_jpr ) 
  {
    p = p_jpr->ConvertToParticle();
    p_jpr->setState( p->State() );  // Resets to identity.
  }
  else 
  {
    (*pcerr) << "*** ERROR ***                                          \n"
            "*** ERROR *** FPSolver::operator()(JetParticle*, char*)  \n"
            "*** ERROR *** null argument for JetParticle*             \n"
            "*** ERROR ***                                          \n"
         << endl;
    ret = 1;
  }

  // :::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) 
  {
    if( dimension != p_jpr->State().Dim() ) {
      (*pcerr) << "*** ERROR ***                                         \n"
              "*** ERROR *** FPSolver::operator()(JetParticle*, char*) \n"
              "*** ERROR *** Dimensions are not correct.             \n"
              "*** ERROR ***  " << dimension << " != " << p->State().Dim() << "\n"
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
  
    Vector zs( dimension );
    Vector z(4);
    for( i = 0; i < 4; i++ ) z(i) = p->State( l[i] );
  
    bmLine->propagate( (*p_jpr) );
  
    MatrixD MM;
    MM = p_jpr->State().Jacobian();
  
    MatrixD M( 4, 4 );
    for( i = 0; i < 4; i++ ) 
      for( j = 0; j < 4; j++ ) 
        M( i, j ) = MM( l[i], l[j] );
  
    for( i = 0; i < 4; i++ ) M( i, i ) -= 1.0;
    M = M.inverse();
    
    int iterCount = 0;
    Vector eps(4);
    do {
      bmLine->propagate( *p );

      // --- Has the state gone out of bounds? ---------
      for( i = 0; i < Particle::PSD; i++ ) {
        if( isnan(p->State(i)) ) { 
          (*pcerr) << "FPSolver: *** ERROR *** p->State(" << i << ") is NaN." << endl;
          if(p) { delete p; p = 0; }
          return -1; 
        }
      }
      
      for( i = 0; i < 4; i++ ) eps(i) = z(i) - p->State( l[i] );
  
      // --- Set up the tests --------------------------
      jumpTest = zeroTest = 0;
      for( i = 0; i < 4; i++ ) {
        if((  DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) > zeroScale[i]  )) {
          zeroTest = 1;
          jumpTest = jumpTest || 
            ( 
              ( fabs( eps(i) ) >
              jumpScale[i]*DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) )  
            );
        }
      }    
  
      if( (!jumpTest) || (!zeroTest) ) break;
  
      // --- Correct orbit and repeat ------------------
      z = z + M*eps;
  
      zs = p->State();
      for( i = 0; i < 4; i++ ) zs( l[i] ) = z(i);
      p->setState( zs );
  
      iterCount++;
    } while ( iterCount < 200 );
    
  
    if( iterCount >= 200 ) {
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
      zs = p->State();
    
      dlist_iterator getNext ( *bmLine );
      bmlnElmnt* q;
      while ((  q = (bmlnElmnt*) getNext()  )) {
         q->propagate( *p );
         startLength += q->OrbitLength( *p );
         if( (*Crit)( q ) ) {
           q->dataHook.append( "FPS_orbit", new FPinfo( startLength, p->State() ) );
         }
      }
    
      p->setState( zs );
    }
  

    // --- Reset JetParticle* argument to contain the map ----------------
    // --- on the closed orbit.                         ----------------

    zs( p->cdtIndex() ) = 0.0;
    p_jpr->setState( zs );
    bmLine->propagate( (*p_jpr) );
    Mapping oneTurn( p_jpr->State() );
    for( i = 0; i < dimension; i++ ) {
      oneTurn(i) += ( zs(i) - oneTurn(i).standardPart() );
    }
    p_jpr->setState( oneTurn );

  } // End block: if( ret == 0 )


  // --- Exit --------------------------------------------------------
  if(p) { delete p; p = 0; }
  return ret;
}

           /* ------------------------------------- */
           /* ------------------------------------- */

int FPSolver::operator()( Particle* p, FP_CRITFUNC Crit )
{
  Vector z = p->State();

  if( dimension != z.Dim() ) {
    ostringstream uic;
    uic << "Dimensions are not correct. "
        << dimension << " != " << z.Dim();
    throw( GenericException( __FILE__, __LINE__, 
           "int FPSolver::operator()( Particle* p, FP_CRITFUNC Crit )",
           uic.str().c_str() ) );
  }

  int i;
  char jumpTest, zeroTest;


  // -----------------------------------------------  


  JetParticle* jprPtr = p->ConvertToJetParticle();
  bmLine->propagate( *jprPtr );

  MatrixD M;
  M = jprPtr->State().Jacobian();
  for( i = 0; i < dimension; i++ ) M( i, i ) -= 1.0;
  M = M.inverse();
  
  int iterCount = 0;
  Vector eps;
  do {
    bmLine->propagate( *p );

    // --- Has the state gone out of bounds? ---------
    for( i = 0; i < Particle::PSD; i++ ) {
      if( isnan(p->State(i)) ) { 
        (*pcerr) << "FPSolver: *** ERROR *** p->State(" << i << ") is NaN." << endl;
        delete jprPtr; jprPtr = 0;
        return -1; 
      }
    }
      
    eps = z - p->State();

    // --- Set up the tests --------------------------
    jumpTest = zeroTest = 0;
    FORALL(i) {
      if((  DMAX(fabs( z(i) ),fabs( p->State(i) )) > zeroScale[i]  )) {
        zeroTest = 1;
        jumpTest = jumpTest || 
          ( 
            ( fabs( eps(i) ) >
            jumpScale[i]*DMAX(fabs( z(i) ),fabs( p->State(i) )) )  
          );
      }
    }    

    if( (!jumpTest) || (!zeroTest) ) break;

    // --- Correct orbit and repeat ------------------
    z = z + M*eps;
    p->setState( z );

    iterCount++;
  } while ( iterCount < 200 );
  

  // --- Cleaning up -------------------------------------------------
  if( iterCount >= 200 ) {
    (*pcerr) << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
    (*pcerr) << "FixedPoint:: Result may not be reliable!! " << endl;
    delete jprPtr; jprPtr = 0;
    return 1;
  }

  if( !jumpTest ) 
    (*pcerr) << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  // --- Store closed orbit if desired -------------------------------
  double startLength( 0.0 );
  if( !Crit ) {
    z = p->State();

    dlist_iterator getNext ( *bmLine );
    bmlnElmnt* q;
    while ((  q = (bmlnElmnt*) getNext()  )) {
       q->propagate( *p );
       startLength += q->OrbitLength( *p );
       if( (*Crit)( q ) )
         q->dataHook.append( "FPS_orbit", new FPinfo( startLength, p->State() ) );
    }

    p->setState( z );
  }

  // --- Exit --------------------------------------------------------
  delete jprPtr; jprPtr = 0;
  return 0;
}

           /* ------------------------------------- */
           /* ------------------------------------- */

void FPSolver::operator()( JetParticle* p, FP_CRITFUNC Crit )
{
  dimension = 6;
  
  double particleCoord[6];
  double u[6];
  int m[6];
  double zero[6];
  int      i,j;
  for(i=0; i<6; i++) {
    particleCoord[i] = 0.0;
    zero[i] = 0.0;
  }
  
  Jet         y;
  Mapping     w;
  int     jumpTest, zeroTest;
  int      iterCount;
  
  iterCount = 0;
  MatrixD hessian(dimension,dimension);
  MatrixD eps(dimension,1);
  
  MatrixD zzhessian(dimension,dimension);
  MatrixD zzeps(dimension,1);
  MatrixD zzx(dimension);
  

  Mapping p_arg_state;
  p->getState( p_arg_state );
  FORALL(i) particleCoord[i] = p_arg_state(i).standardPart();
  FORALL(i) eps(i,0) = 0.0;

  do{
    FORALL(i) particleCoord[i] += eps(i,0);

    FORALL(i) {
      y.setVariable( i, p->State().Env() );
      w.SetComponent( i, y );
    }
    p->setState(w);
    bmLine->propagate( *p );
    p->getState(w);
    w.standardPart( u );

    FORALL(i) m[i] = 0;
    FORALL(i) FORALL(j) {
      m[j] = 1;
      hessian(i,j) = w(i).derivative( m );
      m[j] = 0;                           
    }

    FORALL(i) {
      hessian(i,i) -= 1.0;
      eps(i,0) = particleCoord[i] - u[i];
    }


    // --- Set up the tests and correct orbit --------------------------
    jumpTest = 0;
    FORALL(i) 
      jumpTest = jumpTest || 
        (  
          ( fabs( eps(i,0) ) >
          jumpScale[i]*DMAX(fabs( particleCoord[i] ),fabs(u[i])) )  
        );

    zeroTest = 0;
    FORALL(i)  {
      zeroTest = zeroTest || 
        ( DMAX(fabs( particleCoord[i] ),fabs( u[i] )) > zeroScale[i] );
    }


    // --- Iterative step ----------------------------------------------
    for( i = 0; i < 6; i++ ) {
      for( j = 0; j < 6; j++ ) 
        zzhessian(i,j) = 0.0;
    }
    zzeps = eps;
    zzx = eps;
    zzhessian = hessian;
    mygaussj( zzhessian, 6, zzeps, 1 );
    
    eps = zzeps;
    
    iterCount++;
  } while ( jumpTest && (iterCount < 200) && zeroTest );
  


  // --- Cleaning up -------------------------------------------------
  if( iterCount >= 200 ) {
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
  if( !Crit ) {
    Particle* xp = p->ConvertToParticle();

    dlist_iterator getNext ( *bmLine );
    bmlnElmnt* q;
    while ((  q = (bmlnElmnt*) getNext()  )) {
       q->propagate( *xp );
       startLength += q->OrbitLength( *xp );
       if( (*Crit)( q ) )
         q->dataHook.append( "FPS_orbit", new FPinfo( startLength, xp->State() ) );
    }

    delete xp; xp = 0;
  }

  // --- Exit --------------------------------------------------------
}

           /* ------------------------------------- */
           /* ------------------------------------- */

// REMOVE: void FPSolver::operator()( double* result )
// REMOVE: {
// REMOVE:   JetProton      p(bmLine->Energy());
// REMOVE:   LieOperator   map;
// REMOVE:   dimension = 6;
// REMOVE:   
// REMOVE:   double particleCoord[6];
// REMOVE:   double u[6];
// REMOVE:   int m[6];
// REMOVE:   double zero[6];
// REMOVE:   int      i,j;
// REMOVE:   for(i=0; i<6; i++) {
// REMOVE:     particleCoord[i] = 0.0;
// REMOVE:     zero[i] = 0.0;
// REMOVE:   }
// REMOVE:   
// REMOVE:   Jet       y;
// REMOVE:   Mapping   w;
// REMOVE:   int     test, jumpTest, zeroTest;
// REMOVE:   int      iterCount;
// REMOVE:   
// REMOVE:   iterCount = 0;
// REMOVE:   MatrixD hessian(dimension,dimension);
// REMOVE:   MatrixD eps(dimension,1);
// REMOVE:   
// REMOVE:   MatrixD zzhessian(dimension,dimension);
// REMOVE:   MatrixD zzeps(dimension,1);
// REMOVE:   MatrixD zzx(dimension);
// REMOVE:   
// REMOVE: 
// REMOVE:   do{
// REMOVE:     FORALL(i) {
// REMOVE:       y.setVariable( i,  p.State().Env() );
// REMOVE:       w.SetComponent( i, y );
// REMOVE:     }
// REMOVE:     p.setState(w);
// REMOVE:     bmLine->propagate( p );
// REMOVE:     p.getState(w);
// REMOVE:     test = 1;
// REMOVE:     w.standardPart( u );
// REMOVE: 
// REMOVE:     // --- Has the state gone out of bounds? ---------
// REMOVE:     for( i = 0; i < 6; i++ ) {
// REMOVE:       if( isnan(u[i]) ) { 
// REMOVE:         (*pcerr) << "fixedPoint:: p->State(" << i << ") is NaN." << endl;
// REMOVE:         return; 
// REMOVE:       }
// REMOVE:     }
// REMOVE:       
// REMOVE:     FORALL(i) m[i] = 0;
// REMOVE:     FORALL(i) FORALL(j) {
// REMOVE:       m[j] = 1;
// REMOVE:       hessian(i,j) = w(i).derivative( m );
// REMOVE:       m[j] = 0;                           
// REMOVE:     }
// REMOVE: 
// REMOVE:     FORALL(i) {
// REMOVE:       hessian(i,i) -= 1.0;
// REMOVE:       eps(i,0) = particleCoord[i] - u[i];
// REMOVE:     }
// REMOVE: 
// REMOVE:     for( i = 0; i < 6; i++ ) {
// REMOVE:       for( j = 0; j < 6; j++ ) 
// REMOVE: 	zzhessian(i,j) = 0.0;
// REMOVE:     }
// REMOVE:     zzeps = eps;
// REMOVE:     zzx = eps;
// REMOVE:     zzhessian = hessian;
// REMOVE:     mygaussj( zzhessian, 6, zzeps, 1 );
// REMOVE:     
// REMOVE:     eps = zzeps;
// REMOVE:     
// REMOVE:     jumpTest = 0;
// REMOVE:     FORALL(i) 
// REMOVE:       jumpTest = jumpTest || 
// REMOVE:         (  
// REMOVE:           ( fabs( eps(i,0) ) >
// REMOVE:           jumpScale[i]*DMAX(fabs( particleCoord[i] ),fabs(u[i])) )  
// REMOVE:         );
// REMOVE: 
// REMOVE:     FORALL(i) particleCoord[i] += eps(i,0);
// REMOVE: 
// REMOVE:     zeroTest = 0;
// REMOVE:     FORALL(i)  {
// REMOVE:       zeroTest = zeroTest || 
// REMOVE:         ( DMAX(fabs( particleCoord[i] ),fabs( u[i] )) > zeroScale[i] );
// REMOVE:     }
// REMOVE: 
// REMOVE:     iterCount++;
// REMOVE:   } while ( jumpTest && test && (iterCount < 200) && zeroTest );
// REMOVE:   
// REMOVE:   if( iterCount >= 200 ) {
// REMOVE:     (*pcerr) << 
// REMOVE:       "fixedPoint:: More than 200 Newton's iterations attempted." << endl;
// REMOVE:     (*pcerr) << "fixedPoint:: Result may not be reliable!! " << endl;
// REMOVE:   }
// REMOVE:   if( !test ) 
// REMOVE:     (*pcerr) << 
// REMOVE:       "fixedPoint:: Mapping failed after " << iterCount << " iterations."
// REMOVE: 					   << endl;
// REMOVE:   if( !jumpTest ) 
// REMOVE:     (*pcout) << 
// REMOVE:       "fixedPoint:: Convergence achieved after " << iterCount 
// REMOVE: 						 << " iterations." << endl;
// REMOVE:   FORALL(i) result[i] = particleCoord[i];
// REMOVE: }


void FPSolver::eraseAll() {
 dlist_iterator getNext ( *bmLine );
 bmlnElmnt*     be;
 while((  be = (bmlnElmnt*) getNext()  )) 
  be->dataHook.eraseAll( "FPS_orbit" );
}

double& FPSolver::JumpScale( int i ) { 
  if( i >= 0 && i < dimension ) return jumpScale[i];
  else {
    ostringstream uic;
    uic << "integer argument out of range. Either " 
        << i << " < 0 or " << dimension << " <= " << i;
    throw( GenericException( __FILE__, __LINE__, 
           "double& FPSolver::JumpScale( int i )",
           uic.str().c_str() ) );
  }

  return jumpScale[i];  // This line will never be reached.
                        // It is here to keep a compiler happy.
}

double& FPSolver::ZeroScale( int i ) { 
  if( i >= 0 && i < dimension ) return zeroScale[i];
  else {
    ostringstream uic;
    uic << "integer argument out of range. Either " 
        << i << " < 0 or " << dimension << " <= " << i;
    throw( GenericException( __FILE__, __LINE__, 
           "double& FPSolver::ZeroScale( int i )",
           uic.str().c_str() ) );
  }

  return zeroScale[i];  // This line will never be reached.
                        // It is here to keep a compiler happy.
}
