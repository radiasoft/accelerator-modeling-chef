/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      FPSolver.cc
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

#include "GenericException.h"
#include "beamline.h"
#include "FPSolver.h"

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
  delete [] jumpScale;
  delete [] zeroScale;
  delete [] l;
}

           /* ------------------------------------- */
           /* ------------------------------------- */

int FPSolver::operator()( Proton* p, const char*, FP_CRITFUNC Crit )
{
  if( dimension != p->State().Dim() ) {
    ostringstream uic;
    uic << "Dimensions are not correct. " << dimension 
        << " != " << p->State().Dim();
    throw( GenericException( __FILE__, __LINE__, 
           "int FPSolver::operator()( Proton* p, const char*, FP_CRITFUNC Crit )",
           uic.str().c_str() ) );
  }

  int i, j;
  char jumpTest, zeroTest;


  // -----------------------------------------------  


  Vector zs( dimension );
  Vector z(4);
  for( i = 0; i < 4; i++ ) z(i) = p->State( l[i] );

  JetProton jpr( *p ); 
  bmLine->propagate( jpr );

  MatrixD MM;
  MM = jpr .State() .Jacobian();

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
    cerr << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
    cerr << "FixedPoint:: Result may not be reliable!! " << endl;

    return 1;
  }

  if( !jumpTest || !zeroTest ) 
    cerr << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  // --- Store closed orbit if desired -------------------------------
  double startLength( 0.0 );
  if( Crit ) {
    zs = p->State();

    dlist_iterator getNext ( *(dlist*) bmLine );
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
  return 0;

}

            /* ------------------------------------- */
           /* ------------------------------------- */

int FPSolver::operator()( JetProton* p_jpr, const char*, FP_CRITFUNC Crit )
{
  int ret = 0;
  Proton* p = 0;

  if( p_jpr ) 
  {
    p = (Proton*) p_jpr->ConvertToParticle();
    p_jpr->setState( p->State() );  // Resets to identity.
  }
  else 
  {
    cerr << "*** ERROR ***                                          \n"
            "*** ERROR *** FPSolver::operator()(JetProton*, char*)  \n"
            "*** ERROR *** null argument for JetProton*             \n"
            "*** ERROR ***                                          \n"
         << endl;
    ret = 1;
  }

  // :::::::::::::::::::::::::::::::::::::::::::

  if( ret == 0 ) 
  {
    if( dimension != p_jpr->State().Dim() ) {
      cerr << "*** ERROR ***                                         \n"
              "*** ERROR *** FPSolver::operator()(JetProton*, char*) \n"
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
      cerr << "FPSolver: More than 200 Newton's iterations attempted." << endl;
      cerr << "FPSolver: Result may not be reliable!! " << endl;
    }
  
  
    if( !jumpTest || !zeroTest ) 
      cerr << 
        "FPSolver:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;
    
    // --- Store closed orbit if desired -------------------------------
    double startLength( 0.0 );
    if( Crit ) {
      zs = p->State();
    
      dlist_iterator getNext ( *(dlist*) bmLine );
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
  

    // --- Reset JetProton* argument to contain the map ----------------
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
  if(p) delete p;
  return ret;

}

           /* ------------------------------------- */
           /* ------------------------------------- */

int FPSolver::operator()( Proton* p, FP_CRITFUNC Crit )
{
  Vector z = p->State();

  if( dimension != z.Dim() ) {
    ostringstream uic;
    uic << "Dimensions are not correct. "
        << dimension << " != " << z.Dim();
    throw( GenericException( __FILE__, __LINE__, 
           "int FPSolver::operator()( Proton* p, FP_CRITFUNC Crit )",
           uic.str().c_str() ) );
  }

  int i;
  char jumpTest, zeroTest;


  // -----------------------------------------------  


  JetProton jpr( *p ); 
  bmLine->propagate( jpr );

  MatrixD M;
  M = jpr .State() .Jacobian();
  for( i = 0; i < dimension; i++ ) M( i, i ) -= 1.0;
  M = M.inverse();
  
  int iterCount = 0;
  Vector eps;
  do {
    bmLine->propagate( *p );
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
    cerr << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
    cerr << "FixedPoint:: Result may not be reliable!! " << endl;

    return 1;
  }

  if( !jumpTest ) 
    cerr << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  // --- Store closed orbit if desired -------------------------------
  double startLength( 0.0 );
  if( !Crit ) {
    z = p->State();

    dlist_iterator getNext ( *(dlist*) bmLine );
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
  return 0;

}

           /* ------------------------------------- */
           /* ------------------------------------- */

void FPSolver::operator()( JetProton* p, FP_CRITFUNC Crit )
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
      y.setVariable( i, ((Jet__environment*) p->State().Env()) );
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
    cerr << 
      "FixedPoint:: More than 200 Newton's iterations attempted." << endl;
    cerr << "FixedPoint:: Result may not be reliable!! " << endl;
  }

  if( !jumpTest ) 
    cerr << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  // --- Store closed orbit if desired -------------------------------
  double startLength( 0.0 );
  if( !Crit ) {
    Proton* xp;
    xp = (Proton*) ( p->ConvertToParticle() );

    dlist_iterator getNext ( *(dlist*) bmLine );
    bmlnElmnt* q;
    while ((  q = (bmlnElmnt*) getNext()  )) {
       q->propagate( *xp );
       startLength += q->OrbitLength( *xp );
       if( (*Crit)( q ) )
         q->dataHook.append( "FPS_orbit", new FPinfo( startLength, xp->State() ) );
    }

    delete xp;
  }

  // --- Exit --------------------------------------------------------
}

           /* ------------------------------------- */
           /* ------------------------------------- */

void FPSolver::operator()( double* result )
{
  JetProton      p(bmLine->Energy());
  LieOperator   map;
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
  
  Jet       y;
  Mapping   w;
  int     test, jumpTest, zeroTest;
  int      iterCount;
  
  iterCount = 0;
  MatrixD hessian(dimension,dimension);
  MatrixD eps(dimension,1);
  
  MatrixD zzhessian(dimension,dimension);
  MatrixD zzeps(dimension,1);
  MatrixD zzx(dimension);
  

  do{
    FORALL(i) {
      y.setVariable( i, ((Jet__environment*) p.State().Env()) );
      w.SetComponent( i, y );
    }
    p.setState(w);
    bmLine->propagate( p );
    p.getState(w);
    test = 1;
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

    for( i = 0; i < 6; i++ ) {
      for( j = 0; j < 6; j++ ) 
	zzhessian(i,j) = 0.0;
    }
    zzeps = eps;
    zzx = eps;
    zzhessian = hessian;
    mygaussj( zzhessian, 6, zzeps, 1 );
    
    eps = zzeps;
    
    jumpTest = 0;
    FORALL(i) 
      jumpTest = jumpTest || 
        (  
          ( fabs( eps(i,0) ) >
          jumpScale[i]*DMAX(fabs( particleCoord[i] ),fabs(u[i])) )  
        );

    FORALL(i) particleCoord[i] += eps(i,0);

    zeroTest = 0;
    FORALL(i)  {
      zeroTest = zeroTest || 
        ( DMAX(fabs( particleCoord[i] ),fabs( u[i] )) > zeroScale[i] );
    }

    iterCount++;
  } while ( jumpTest && test && (iterCount < 200) && zeroTest );
  
  if( iterCount >= 200 ) {
    cerr << 
      "fixedPoint:: More than 200 Newton's iterations attempted." << endl;
    cerr << "fixedPoint:: Result may not be reliable!! " << endl;
  }
  if( !test ) 
    cerr << 
      "fixedPoint:: Mapping failed after " << iterCount << " iterations."
					   << endl;
  if( !jumpTest ) 
    cerr << 
      "fixedPoint:: Convergence achieved after " << iterCount 
						 << " iterations." << endl;
  FORALL(i) result[i] = particleCoord[i];
}


void FPSolver::eraseAll() {
 dlist_iterator getNext ( *(dlist*) bmLine );
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
