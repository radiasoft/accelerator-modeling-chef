#include "beamline.h"
#include "FPSolver.h"

mygaussj( MatrixD, int, MatrixD, int );

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
    cerr << "*** ERROR ***                            \n"
         << "*** ERROR *** FPSolver::FPSolver         \n"
         << "*** ERROR *** Incorrect parameters:      \n"
         << "*** ERROR *** bml = " << (int) bml << "  \n"
         << "*** ERROR *** n   = " <<       n   << "  \n"
         << "*** ERROR ***                            \n"
         << endl;
    exit(1);
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
    cerr << "*** ERROR ***                                         \n"
            "*** ERROR *** FPSolver::operator()(Proton*, char*)    \n"
            "*** ERROR *** Dimensions are not correct.             \n"
            "*** ERROR ***  " << dimension << " != " << p->State().Dim() << "\n"
            "*** ERROR ***                                         \n"
         << endl;
    exit(1);
  }

  int i, j;
  char jumpTest, zeroTest;


  // --- Conserve the reference point --------------  
  // ??? REMOVE double* oldRefPoint = new double[Jet::oneEnv.NumVar];
  // ??? REMOVE double* newRefPoint = new double[Jet::oneEnv.NumVar];
  // ??? REMOVE Jet  y;
  // ??? REMOVE y.getReference(oldRefPoint);
  // ??? REMOVE for( i = 0; i < dimension; i++ ) newRefPoint[i] = p->State(i);
  // ??? REMOVE for( i = dimension + 1; i < Jet::oneEnv.NumVar; i++ )
  // ??? REMOVE                                  newRefPoint[i] = oldRefPoint[i];
  // ??? REMOVE Jet::FixReference( newRefPoint );


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

    // ??? REMOVE // --- Set up the tests --------------------------
    // ??? REMOVE jumpTest = 0;
    // ??? REMOVE for( i = 0; i < 4; i++ ) {
    // ??? REMOVE   jumpTest = jumpTest || 
    // ??? REMOVE     (  
    // ??? REMOVE       ( fabs( eps(i) ) >
    // ??? REMOVE       jumpScale[i]*DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) )  
    // ??? REMOVE     );
    // ??? REMOVE }
    // ??? REMOVE if( !jumpTest ) break;
    // ??? REMOVE 
    // ??? REMOVE zeroTest = 0;
    // ??? REMOVE for( i = 0; i < 4; i++ )  {
    // ??? REMOVE   zeroTest = zeroTest || 
    // ??? REMOVE     ( DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) > zeroScale[i] );
    // ??? REMOVE }
    // ??? REMOVE if( !zeroTest ) break;
    // ??? REMOVE 
    // ??? REMOVE 
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

    // ??? REMOVE Jet::FixReference( oldRefPoint );   //restore reference point
    // ??? REMOVE delete [] oldRefPoint;
    // ??? REMOVE delete [] newRefPoint;
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
  // ??? REMOVE Jet::FixReference( oldRefPoint );   //restore reference point
  // ??? REMOVE delete [] oldRefPoint;
  // ??? REMOVE delete [] newRefPoint;
  return 0;

}

           /* ------------------------------------- */
           /* ------------------------------------- */

int FPSolver::operator()( Proton* p, FP_CRITFUNC Crit )
{
  Vector z = p->State();

  if( dimension != z.Dim() ) {
    cerr << "*** ERROR ***                                         \n"
            "*** ERROR *** FPSolver::operator()(Proton*)           \n"
            "*** ERROR *** Dimensions are not correct.             \n"
            "*** ERROR ***  " << dimension << " != " << z.Dim() << "\n"
            "*** ERROR ***                                         \n"
         << endl;
    exit(1);
  }

  int i;
  char jumpTest, zeroTest;


  // --- Conserve the reference point --------------  
  // ??? REMOVE double* oldRefPoint = new double[Jet::oneEnv.NumVar];
  // ??? REMOVE double* newRefPoint = new double[Jet::oneEnv.NumVar];
  // ??? REMOVE Jet  y;
  // ??? REMOVE y.getReference(oldRefPoint);
  // ??? REMOVE for( i = 0; i < dimension; i++ ) newRefPoint[i] = p->State(i);
  // ??? REMOVE for( i = dimension + 1; i < Jet::oneEnv.NumVar; i++ ) 
  // ??? REMOVE      newRefPoint[i] = oldRefPoint[i];
  // ??? REMOVE Jet::FixReference( newRefPoint );


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

    // ??? REMOVE  // --- Set up the tests --------------------------
    // ??? REMOVE  jumpTest = 0;
    // ??? REMOVE  FORALL(i) 
    // ??? REMOVE    jumpTest = jumpTest || 
    // ??? REMOVE      (  
    // ??? REMOVE        ( fabs( eps(i) ) >
    // ??? REMOVE        jumpScale[i]*DMAX(fabs( z(i) ),fabs( p->State(i) )) )  
    // ??? REMOVE      );
    // ??? REMOVE  if( !jumpTest ) break;
    // ??? REMOVE  
    // ??? REMOVE  zeroTest = 0;
    // ??? REMOVE  FORALL(i)  {
    // ??? REMOVE    zeroTest = zeroTest || 
    // ??? REMOVE      ( DMAX(fabs( z(i) ),fabs( p->State(i) )) > zeroScale[i] );
    // ??? REMOVE  }
    // ??? REMOVE  if( !zeroTest ) break;
    // ??? REMOVE  
    // ??? REMOVE  
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

    // ??? REMOVE Jet::FixReference( oldRefPoint );   //restore reference point
    // ??? REMOVE delete [] oldRefPoint;
    // ??? REMOVE delete [] newRefPoint;
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
  // ??? REMOVE Jet::FixReference( oldRefPoint );   //restore reference point
  // ??? REMOVE delete [] oldRefPoint;
  // ??? REMOVE delete [] newRefPoint;
  return 0;

}

           /* ------------------------------------- */
           /* ------------------------------------- */

void FPSolver::operator()( JetProton* p, FP_CRITFUNC Crit )
{
  dimension = 6;
  // ??? REMOVE double* oldRefPoint;
  
  double particleCoord[6];
  double u[6];
  int m[6];
  double zero[6];
  int      i,j;
  for(i=0; i<6; i++) {
    particleCoord[i] = 0.0;
    zero[i] = 0.0;
  }
  
  // ??? REMOVE oldRefPoint = new double[Jet::oneEnv.NumVar];
  Jet         y;
  Mapping     w;
  int     jumpTest, zeroTest;
  int      iterCount;
  // ??? REMOVE y.getReference(oldRefPoint);
  
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

    // ??? REMOVE Jet::FixReference( particleCoord );   // Reference point will be 
                                          // restored at the end
    // ??? REMOVE w.fixReference();

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
  // ??? REMOVE Jet::FixReference( oldRefPoint );   //restore reference point

  // ??? REMOVE delete [] oldRefPoint;
}

           /* ------------------------------------- */
           /* ------------------------------------- */

void FPSolver::operator()( double* result )
{
  JetProton      p(bmLine->Energy());
  LieOperator   map;
  dimension = 6;
  // ??? REMOVE double* oldRefPoint;
  
  double particleCoord[6];
  double u[6];
  int m[6];
  double zero[6];
  // ??? REMOVE double zeroScale[6];
  int      i,j;
  for(i=0; i<6; i++) {
    particleCoord[i] = 0.0;
    zero[i] = 0.0;
    // ??? REMOVE zeroScale[i] = ZEROSCALE;
  }
  
  // ??? REMOVE static double zeroScale [] = { 1.0e-9, 1.0e-9, 1.0e-6, 
  // ??? REMOVE                                1.0e-9, 1.0e-9, 1.0e-6
  // ??? REMOVE                              };
  // ??? REMOVE static double jumpScale [] = { 1.0e-9, 1.0e-9, 0.01,
  // ??? REMOVE                                1.0e-9, 1.0e-9, 0.01
  // ??? REMOVE                              };

  // ??? REMOVE oldRefPoint = new double[Jet::oneEnv.NumVar];
  Jet       y;
  Mapping   w;
  int     test, jumpTest, zeroTest;
  int      iterCount;
  // ??? REMOVE double   changeNorm, scaleNorm;
  // ??? REMOVE y.getReference(oldRefPoint);
  
  iterCount = 0;
  MatrixD hessian(dimension,dimension);
  MatrixD eps(dimension,1);
  
  MatrixD zzhessian(dimension,dimension);
  MatrixD zzeps(dimension,1);
  MatrixD zzx(dimension);
  

  do{
    // ??? REMOVE Jet::FixReference( particleCoord );   // Reference point will be 
					  // restored at the end
    // ??? REMOVE w.fixReference();

    FORALL(i) {
      y.setVariable( i, p.State().Env() );
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
    
    // ??? REMOVE changeNorm = 0.0;
    // ??? REMOVE FORALL(i) changeNorm +=  eps(i,0) * eps(i,0) ;
    // ??? REMOVE changeNorm = sqrt( changeNorm );
    // ??? REMOVE scaleNorm = 0.0;
    // ??? REMOVE FORALL(i) scaleNorm +=  particleCoord[i] * particleCoord[i];
    // ??? REMOVE scaleNorm = sqrt( scaleNorm );
    // ??? REMOVE if( changeNorm <= (5.0e-11)*scaleNorm ) jumpTest = 0;
    // ??? REMOVE else jumpTest = 1;

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
  // ??? REMOVE Jet::FixReference( oldRefPoint );   //restore reference point
  // ??? REMOVE delete [] oldRefPoint;
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
    cerr << "*** ERROR ***                                    \n" 
         << "*** ERROR *** FPSolver::JumpScale                \n" 
	 << "*** ERROR *** integer argument out of range      \n" 
	 << "*** ERROR *** Either " << i << " < 0 or          \n" 
	 << "*** ERROR *** " << dimension << " <= " << i << " \n" 
	 << "*** ERROR ***                                    \n" 
	 << endl;
    if( 1 + 1 == 2 ) exit(1);
  }
}

double& FPSolver::ZeroScale( int i ) { 
  if( i >= 0 && i < dimension ) return zeroScale[i];
  else {
    cerr << "*** ERROR ***                                    \n" 
         << "*** ERROR *** FPSolver::zeroScale                \n" 
	 << "*** ERROR *** integer argument out of range      \n" 
	 << "*** ERROR *** Either " << i << " < 0 or          \n" 
	 << "*** ERROR *** " << dimension << " <= " << i << " \n" 
	 << "*** ERROR ***                                    \n" 
	 << endl;
    if( 1 + 1 == 2 ) exit(1);
  }
}
