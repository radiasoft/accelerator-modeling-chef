//
// fixedPoint
// Algorithm and initial coding by Leo Michelotti
// Modified by James Holt
//
// FixedPoint( JetProton )
// Change in stopping criteria (Leo Michelotti)
//
// FixedPoint( Proton )
// New algorithm (Leo Michelotti)
//
// TransverseFixedPoint( Proton )
// Transverse phase space only  (Leo Michelotti)
//

#include "beamline.h"
  mygaussj(MatrixD,int,MatrixD,int);

#define DMAX(a,b) (a>b?a:b)

#define FORALL(q)  for ( q = 0; q < dimension; q++ )

           /* ------------------------------------- */
           /* ------------------------------------- */

int TransverseFixedPoint( beamline& bmLine, 
                          Proton*   p, 
                          double*   jumpScale,
                          double*   zeroScale, 
                          int*      l ) 
{
  int dimension = p->State().Dim();
  int i, j;
  char jumpTest, zeroTest;
  char jumpArg = 0;
  char zeroArg = 0;
  char lArg    = 0;


  // --- Conserve the reference point --------------  
  double* oldRefPoint = new double[Jet::oneEnv.NumVar];
  double* newRefPoint = new double[Jet::oneEnv.NumVar];
  Jet  y;
  y.getReference(oldRefPoint);
  for( i = 0; i < dimension; i++ ) newRefPoint[i] = p->State(i);
  for( i = dimension + 1; i < Jet::oneEnv.NumVar; i++ ) 
                                   newRefPoint[i] = oldRefPoint[i];
  Jet::FixReference( newRefPoint );


  // --- If criteria are not provided ... ----------  
  if( !zeroScale ) {
    zeroScale = new double [ 4 ];
    for( i = 0; i < 4; i++ ) zeroScale[i] = 1.0e-9;
    zeroArg = 1;
  }

  if( !jumpScale ) {
    jumpScale = new double [ 4 ];
    for( i = 0; i < 4; i++ ) jumpScale[i] = 1.0e-9;
    jumpArg = 1;
  }

  if( !l ) {
    l = new int [4];
    l[0] = 0; l[1] = 1; l[2] = 3; l[3] = 4;
    lArg = 1;
  }

  // -----------------------------------------------  


  Vector zs( dimension );
  Vector z(4);
  for( i = 0; i < 4; i++ ) z(i) = p->State( l[i] );

  JetProton jpr( *p ); 
  bmLine.propagate( jpr );

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
    cout << "DGN> In FixedPoint: Begin bmLine.propagate(p)" << endl;  // DGN
    bmLine.propagate( *p );
    cout << "DGN> In FixedPoint: End   bmLine.propagate(p)" << endl;  // DGN
    for( i = 0; i < dimension; i++ ) cout << p->State(i) << "  ";     // DGN
    cout << endl;                                                     // DGN
    for( i = 0; i < 4; i++ ) eps(i) = z(i) - p->State( l[i] );

    // --- Set up the tests --------------------------
    jumpTest = 0;
    for( i = 0; i < 4; i++ ) {
      jumpTest = jumpTest || 
        (  
          ( fabs( eps(i) ) >
          jumpScale[i]*DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) )  
        );
    }
    if( !jumpTest ) break;

    zeroTest = 0;
    for( i = 0; i < 4; i++ )  {
      zeroTest = zeroTest || 
        ( DMAX(fabs( z(i) ),fabs( p->State( l[i] ) )) > zeroScale[i] );
    }
    if( !zeroTest ) break;


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

    Jet::FixReference( oldRefPoint );   //restore reference point
    delete [] oldRefPoint;
    delete [] newRefPoint;
    if( zeroArg ) delete [] zeroScale;
    if( jumpArg ) delete [] jumpScale;
    if( lArg )    delete [] l;
    return 1;
  }

  if( !jumpTest || !zeroTest) 
    cout << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  Jet::FixReference( oldRefPoint );   //restore reference point
  delete [] oldRefPoint;
  delete [] newRefPoint;
  if( zeroArg ) delete [] zeroScale;
  if( jumpArg ) delete [] jumpScale;
  if( lArg )    delete [] l;
  return 0;

}

           /* ------------------------------------- */
           /* ------------------------------------- */

int FixedPoint( beamline& bmLine, 
                Proton*   p, 
                double*   jumpScale,
                double*   zeroScale ) 
{
  Vector z = p->State();
  int dimension = z.Dim();
  int i;
  char jumpTest, zeroTest;
  char jumpArg = 0;
  char zeroArg = 0;


  // --- Conserve the reference point --------------  
  double* oldRefPoint = new double[Jet::oneEnv.NumVar];
  double* newRefPoint = new double[Jet::oneEnv.NumVar];
  Jet  y;
  y.getReference(oldRefPoint);
  for( i = 0; i < dimension; i++ ) newRefPoint[i] = p->State(i);
  for( i = dimension + 1; i < Jet::oneEnv.NumVar; i++ ) 
                                   newRefPoint[i] = oldRefPoint[i];
  Jet::FixReference( newRefPoint );


  // --- If criteria are not provided ... ----------  
  if( !zeroScale ) {
    zeroScale = new double [ dimension ];
    for( i = 0; i < dimension; i++ ) zeroScale[i] = 1.0e-9;
    zeroArg = 1;
  }

  if( !jumpScale ) {
    jumpScale = new double [ dimension ];
    for( i = 0; i < dimension; i++ ) jumpScale[i] = 1.0e-9;
    jumpArg = 1;
  }
  // -----------------------------------------------  


  JetProton jpr( *p ); 
  bmLine.propagate( jpr );

  MatrixD M;
  M = jpr .State() .Jacobian();
  for( i = 0; i < dimension; i++ ) M( i, i ) -= 1.0;
  /* DGN */ cout << "DGN> Before inverting: M = \n" << M << endl;
  M = M.inverse();
  
  int iterCount = 0;
  Vector eps;
  do {
    /* DGN */ cout << "DGN> In FixedPoint: Begin bmLine.propagate(p)" << endl;
    bmLine.propagate( *p );
    /* DGN */ cout << "DGN> In FixedPoint: End   bmLine.propagate(p)" << endl;
    /* DGN */ for( i = 0; i < dimension; i++ ) cout << p->State(i) << "  ";
    /* DGN */ cout << endl;
    eps = z - p->State();

    // --- Set up the tests --------------------------
    jumpTest = 0;
    FORALL(i) 
      jumpTest = jumpTest || 
        (  
          ( fabs( eps(i) ) >
          jumpScale[i]*DMAX(fabs( z(i) ),fabs( p->State(i) )) )  
        );
    if( !jumpTest ) break;

    zeroTest = 0;
    FORALL(i)  {
      zeroTest = zeroTest || 
        ( DMAX(fabs( z(i) ),fabs( p->State(i) )) > zeroScale[i] );
    }
    if( !zeroTest ) break;


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

    Jet::FixReference( oldRefPoint );   //restore reference point
    delete [] oldRefPoint;
    delete [] newRefPoint;
    if( zeroArg ) delete [] zeroScale;
    if( jumpArg ) delete [] jumpScale;
    return 1;
  }

  if( !jumpTest ) 
    cout << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  Jet::FixReference( oldRefPoint );   //restore reference point
  delete [] oldRefPoint;
  delete [] newRefPoint;
  if( zeroArg ) delete [] zeroScale;
  if( jumpArg ) delete [] jumpScale;
  return 0;

}

           /* ------------------------------------- */
           /* ------------------------------------- */

void FixedPoint( beamline&  bmLine, 
                 JetProton* p, 
                 double*    jumpScale,
                 double*    zeroScale ) 
{
  int dimension = 6;
  double* oldRefPoint;
  
  char jumpArg = 0;
  char zeroArg = 0;

  double particleCoord[6];
  double u[6];
  int m[6];
  double zero[6];
  int      i,j;
  for(i=0; i<6; i++) {
    particleCoord[i] = 0.0;
    zero[i] = 0.0;
  }
  
  if( !zeroScale ) {
    zeroScale = new double [ dimension ];
    for( i = 0; i < dimension; i++ ) zeroScale[i] = 1.0e-9;
    zeroArg = 1;
  }

  if( !jumpScale ) {
    jumpScale = new double [ dimension ];
    for( i = 0; i < dimension; i++ ) jumpScale[i] = 1.0e-9;
    jumpArg = 1;
  }

  oldRefPoint = new double[Jet::oneEnv.NumVar];
  Jet         y;
  LieOperator w;
  int     jumpTest, zeroTest;
  int      iterCount;
  y.getReference(oldRefPoint);
  
  iterCount = 0;
  MatrixD hessian(dimension,dimension);
  MatrixD eps(dimension,1);
  
  MatrixD zzhessian(dimension,dimension);
  MatrixD zzeps(dimension,1);
  MatrixD zzx(dimension);
  

  LieOperator p_arg_state;
  p->getState( p_arg_state );
  FORALL(i) particleCoord[i] = p_arg_state(i).standardPart();
  FORALL(i) eps(i,0) = 0.0;

  do{
    FORALL(i) particleCoord[i] += eps(i,0);

    Jet::FixReference( particleCoord );   // Reference point will be 
                                          // restored at the end
    w.fixReference();

    FORALL(i) {
      y.setVariable( i );
      w.setVariable( y, i );
    }
    p->setState(w);
    /* DGN */ cout << "DGN> In FixedPoint: Begin bmLine.propagate(p)" << endl;
    bmLine.propagate( *p );
    /* DGN */ cout << "DGN> In FixedPoint: End   bmLine.propagate(p)" << endl;
    p->getState(w);
    w.standardPart( u );
    /* DGN */ for( i = 0; i < 6; i++ ) cout << u[i] << "  ";
    /* DGN */ cout << endl;

    FORALL(i) m[i] = 0;
    FORALL(i) FORALL(j) {
      m[j] = 1;
      hessian(i,j) = w[i].derivative( m );  // This is actually NOT the 
      m[j] = 0;                             // transpose of the hessian. ?????
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
    cout << 
      "FixedPoint:: Convergence achieved after " << iterCount 
                                                 << " iterations." << endl;

  Jet::FixReference( oldRefPoint );   //restore reference point

  delete [] oldRefPoint;
  if( zeroArg ) delete [] zeroScale;
  if( jumpArg ) delete [] jumpScale;
}
