#include <stdlib.h>
#include <iomanip.h>
#include "JetC.h"

// ================================================================
//      Global variables
//

slist JetC::environments;
char  JetC__environment::SkipEnvEqTest = 0;
  // Used by the JetC::EndEnvironment function.

JetC__environment*  JetC::lastEnv = 0;
JetC__environment*  JetC::workEnv = 0;
int                JetC::currentIndex = 0;
slist              JetC::newCoordCs;
slist              JetC::newValues;
FILE              *JetC::scratchFile;
// ??? REMOVE dlist              JetC::coordCPool;
dlist              JetC::coordCPtrs;
#ifdef OBJECT_DEBUG
int                JetC::objectCount = 0;
int                JetC__environment::objectCount = 0;
int                coordC::objectCount = 0;
#endif


// ================================================================
//      External routines
//

extern char nexcom( int, int, int* );  
                                // Computes the next composition
                                //  of an integer into a number of parts.
                                //  Algorithm devised by Herbert Wilf.

extern "C" {
 int bcfRec( int, int );        // Recursive evaluation of binomial
                                //  coefficient.
 int nearestInteger( double );  // Returns the integer nearest to its
                                //  double argument.
}


// ================================================================
//      Implementation of JetC__environment
//      ??? Belongs in JLC.cc

JetC__environment::JetC__environment() 
{
  monomial           = 0;
  JLCmonomial        = 0;
  exponent           = 0;
  expCode            = 0;
  PBOK               = 0;
  numPaths           = 0;
  NumVar             = 0;
  MaxWeight          = 0;
  SpaceDim           = -1;
  dof                = 0;            
  refPoint           = 0;       
  scale              = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC__environment::JetC__environment( JetC__environment& x )
: MaxWeight( x.MaxWeight ), 
  NumVar( x.NumVar ), 
  SpaceDim( x.SpaceDim )
{
  if( JetC::workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** JetC__environment::JetC__environment       \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** You must close the open environment      \n"
         << "*** ERROR *** before attempting to copy an environment.\n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  if( NumVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** JetC__environment::JetC__environment       \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLCmonomial        = 0;
    exponent           = 0;
    expCode            = 0;
    PBOK               = 0;
    numPaths           = 0;
    NumVar             = 0;
    MaxWeight          = 0;
    SpaceDim           = -1;
    dof                = 0;            
    refPoint           = 0;       
    scale              = 0;

    return;
  }

  int w = MaxWeight;
  int n = NumVar;
  int s = SpaceDim;
  int i, j;

  j = bcfRec( w + n, n );

  monomial   = new Complex[ j ];
  JLCmonomial = new JetC   [ j ];
  JetC__environment* stupid = &x;
  for( i = 0; i < j; i++ ) JLCmonomial[i].Reconstruct( stupid );
  exponent   = new int[ n ];
  expCode    = new char[ w + n ];
  PBOK       = x.PBOK;

  numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  dof                = x.dof;

  refPoint = new Complex[ n ];
  scale = new double[ n ];
  AllZeroes.Reconstruct( n );

  for( i = 0; i < n; i++ ) {
   refPoint[i] = x.refPoint[i];
   scale[i] = x.scale[i];
   AllZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }

  myCoords = x.myCoords;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}
 
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC__environment::~JetC__environment() 
{
  if( monomial   ) delete [] monomial;
  if( JLCmonomial ) delete [] JLCmonomial;
  if( exponent   ) delete [] exponent;
  if( expCode    ) delete [] expCode;
  if( numPaths   ) delete    numPaths;
  if( refPoint   ) delete [] refPoint;
  if( scale      ) delete [] scale;

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetC__environment& JetC__environment::operator=( const JetC__environment& x )
{

  // The first line is useful for the JetC::EndEnvironment function.
  if( JetC__environment::SkipEnvEqTest == 1 ) 
      JetC__environment::SkipEnvEqTest == 0;

  else if( JetC::workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** JetC__environment::operator=              \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** You must close the open environment      \n"
         << "*** ERROR *** before attempting to assign.             \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  MaxWeight = x.MaxWeight;
  NumVar    = x.NumVar;
  SpaceDim  = x.SpaceDim;

  if( NumVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** JetC__environment::operator=              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLCmonomial         = 0;
    exponent           = 0;
    expCode            = 0;
    PBOK               = 0;
    numPaths           = 0;
    NumVar             = 0;
    MaxWeight          = 0;
    SpaceDim           = -1;
    dof                = 0;            
    refPoint           = 0;       
    scale              = 0;

    return *this;
  }

  int w = MaxWeight;
  int n = NumVar;
  int s = SpaceDim;
  int i, j;

  if( monomial ) delete [] monomial;
  monomial   = new Complex[ bcfRec( w + n, n ) ];

  if( JLCmonomial ) delete [] JLCmonomial;
  JLCmonomial = new JetC[ bcfRec( w + n, n ) ];

  if( exponent ) delete [] exponent;
  exponent   = new int[ n ];

  if( expCode ) delete [] expCode;
  expCode    = new char[ w + n ];

  PBOK       = x.PBOK;

  if( numPaths ) delete  numPaths;
  numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  dof                = x.dof;

  if( refPoint ) delete [] refPoint;
  refPoint = new Complex[ n ];

  if( scale ) delete [] scale;
  scale = new double[ n ];

  AllZeroes.Reconstruct( n );
  
  for( i = 0; i < n; i++ ) {
   refPoint[i] = x.refPoint[i];
   scale[i] = x.scale[i];
   AllZeroes(i) = 0;   // ??? Redundant and unnecessary
  }

  myCoords = x.myCoords;

  return *this;
}


char JetC__environment::operator==( const JetC__environment& x ) const
{
  int i;
  if( x.NumVar != NumVar ) return 0;
  if( x.SpaceDim != SpaceDim ) return 0;
  if( x.MaxWeight != MaxWeight ) return 0;
  for( i = 0; i < NumVar; i++ ) {
    if( refPoint[i] != x.refPoint[i] ) return 0;
    if( scale[i] != x.scale[i] ) return 0;
  }
  return 1;  
}


char JetC__environment::operator!=( const JetC__environment& x ) const
{
  return !operator==( x );
}


ostream& 
operator<<( ostream& os, const JetC__environment* x )
{
  int i;
  os << x->NumVar << endl;
  os << x->SpaceDim << endl;
  for( i = 0; i < x->NumVar; i++ ) os << setprecision(30) 
                                      << x->refPoint[i] << endl;
  for( i = 0; i < x->NumVar; i++ ) os << setprecision(30) 
                                      << x->scale[i] << endl;
  os << x->MaxWeight << endl;
  os << x->PBOK << endl;
  return os;
}

istream& 
operator>>( istream& is, JetC__environment* x )
{
  static slist envs;
  slist_iterator getNext( envs );
  JetC__environment* pje;
  int i(0), j(0);

  // --- Test to see if x was previously read
  while((  pje = (JetC__environment*) getNext()  ))
    if( *pje == *x ) {
      if( x ) delete x;
      x = pje;
      return is;
    }
  
  // --- x was not previously read
  pje = new JetC__environment;
  is >> pje->NumVar;
  is >> pje->SpaceDim;

  if( pje->NumVar < pje->SpaceDim ) {
    cerr << "\n\n"
         << "*** ERROR ***                              \n"
         << "*** ERROR *** istream operator>>           \n"
         << "*** ERROR ***                              \n"
         << "*** ERROR *** JetC__enviroment dimensions   \n"
         << "*** ERROR *** are wrong.                   \n"
         << "*** ERROR ***                              \n"
         << endl;
    exit(1);
  }
  
  pje->refPoint = new Complex[ pje->NumVar ];
  pje->scale = new double[ pje->NumVar ];
  for( i = 0; i < x->NumVar; i++ ) is >> pje->refPoint[i];
  for( i = 0; i < x->NumVar; i++ ) is >> pje->scale[i];

  is >> pje->MaxWeight;
  is >> pje->PBOK;


  // --- Calculations, just as in JetC::EndEnvironment
  int n = pje->NumVar;
  int w = pje->MaxWeight;
  int s = pje->SpaceDim;

  pje->exponent   = new int[ n ];
  pje->expCode    = new char[ w + n ];
  pje->monomial   = new Complex[ bcfRec( w + n, n ) ];
  pje->JLCmonomial = new JetC[ bcfRec( w + n, n ) ];

  pje->AllZeroes.Reconstruct(n);
  for( i = 0; i < n; i++ ) pje->AllZeroes(i) = 0;

  // Load the numPaths array with binomial coefficients;
  // required by Wilf's algorithm for ranking monomials.
  pje->numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*(pje->numPaths))( i, j-1 ) = bcfRec( i + j - 1, i );

  // Initialize the coordinates
  // ??? HOW ???

  // --- Finished
  envs.append( pje );
  return is;
}

//***************************************************************
//***************************************************************
//***************************************************************

JetC::JetC( JetC__environment* pje ) 
: stacked(0) 
{
 jl = new JLC( pje );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetC::JetC( const Complex& x, JetC__environment* p_je ) 
: stacked(0) 
{
 jl = new JLC( x, p_je );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetC::JetC( const JetC& x ) 
{
 stacked = x.stacked;
 jl = x.jl;
 ( jl->rc )++;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetC::~JetC() 
{
 if( --(jl->rc) == 0 ) delete jl;

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

void JetC::Reconstruct()
{
 // Combines destructor and constructor functions.
 // Use when initializing a static JetC variable.
 static JetC__environment* pje;
 pje = jl->myEnv;
 if( --(jl->rc) == 0 ) delete jl;
 jl = new JLC( pje );
 stacked = 0;
}

void JetC::Reconstruct( JetC__environment* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static JetC variable.
 if( --(jl->rc) == 0 ) delete jl;
 jl = new JLC( pje );
 stacked = 0;
}


JetC__environment* JetC::Env() const
{
  return jl->myEnv;
}

void JetC::Env( JetC__environment* pje ) const
{
  pje = jl->myEnv;
}


void JetC::getReference( Complex* r ) 
{
 jl->getReference( r );
}

void JetC::setVariable( const Complex& x,
                       const int& j, 
                             JetC__environment* pje )
{
 if( jl == NULL ) jl = new JLC( pje );
 jl->setVariable( x, j, pje );  // !! Alters the environment!
}

void JetC::setVariable( const int& j, 
                             JetC__environment* pje ) 
{
 if( jl == NULL ) jl = new JLC( pje );
 jl->setVariable( j, pje );
}

JetC& JetC::operator=( const JetC& x ) 
{
 if( this != &x ) {
  if( --(jl->rc) == 0 ) delete jl;
#ifndef FORBIDSTACKED
  if( x.stacked ) {
   jl = x.jl;
   (jl->rc)++;
  }
  else { 
#endif
   jl = new JLC( x->myEnv );
   *jl = *(x.jl);
#ifndef FORBIDSTACKED
  }
#endif
 }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

JetC& JetC::DeepCopy( const JetC& x ) 
{
 if( this != &x ) {
  if( --(jl->rc) == 0 ) delete jl;
  jl = new JLC( x->myEnv );
  *jl = *(x.jl);
 }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

JetC& JetC::operator=( const Complex& x ) 
{
 static JetC__environment* pje;
 pje = jl->myEnv;

 if( --(jl->rc) == 0 ) delete jl;
 jl = new JLC(pje);
 *(jl) = x;
 stacked = 0;
 return *this; 
}

istream& operator>>( istream& is,  JetC& x ) 
{
 if( --(x.jl->rc) == 0 ) delete x.jl;
 x.jl = new JLC;
 return operator>>( is, *(x.jl) );
}

ostream& operator<<( ostream& os, JetC& x ) 
{
 return operator<<( os, *(x.jl) );
}



//***************************************************************
//***************************************************************
//***************************************************************
//
//      New initialization routines.
//      Must be called first by any program in order to establish the
//      number of (scalar) variables, n,
//      maximum weight of derivatives, w,
//      dimension of phase space, s,
//      default reference point, r.

void
JetC::BeginEnvironment( int w ) 
{
  if( workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** JetC::BeginEnvironment                    \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** You cannot open a new environment        \n"
         << "*** ERROR *** until you close an old one.              \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  workEnv = new JetC__environment;
  workEnv->MaxWeight = w;
  currentIndex = 0;
}


void
JetC::Parameters()
{
  if( workEnv->PBOK == 1 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** JetC::Parameters                          \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** You can only call this once for an       \n"
         << "*** ERROR *** open environment.                        \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }
  workEnv->PBOK = 1;
  workEnv->SpaceDim = currentIndex;
}


JetC__environment*
JetC::EndEnvironment( Complex* scl )
{
  if( currentIndex == 0 ) {
    delete workEnv;
    workEnv = 0;
    return 0;
  }

  coordC*  p;
  Complex* q;
  int     i, j;

  if((  ( currentIndex     != newCoordCs.size() )  ||  
        ( newCoordCs.size() != newValues.size() )  )) {
    cerr << "\n\n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** JetC::EndEnvironment               \n"
         << "*** ERROR *** A horrible, inexplicable error    \n"
         << "*** ERROR *** has occurred. This is beyond      \n"
         << "*** ERROR *** the realm of human understanding. \n"
         << "*** ERROR *** Please consult an exorcist.       \n"
         << "*** ERROR ***                                   \n"
         << endl;
    exit(1);
  }

  workEnv->NumVar = currentIndex;
  if( !workEnv->PBOK ) workEnv->SpaceDim = currentIndex;
  workEnv->dof = workEnv->SpaceDim / 2;
  if( workEnv->PBOK && ( 2*workEnv->dof != workEnv->SpaceDim ) ) {
    cerr << "\n\n"
         << "*** WARNING ***                                 \n"
         << "*** WARNING *** JetC::EndEnvironment()           \n"
         << "*** WARNING *** Phase space has odd dimension.  \n"
         << "*** WARNING *** I hope you know what your       \n"
         << "*** WARNING *** are doing, but I doubt it.      \n"
         << "*** WARNING ***                                 \n"
         << endl;
  }
  
  int w = workEnv->MaxWeight;
  int n = workEnv->NumVar;
  int s = workEnv->SpaceDim;
  int bcfr = bcfRec( w + n, n );

  workEnv->exponent   = new int[ n ];
  workEnv->expCode    = new char[ w + n ];
  workEnv->monomial   = new Complex [ bcfr ];
  workEnv->JLCmonomial = new JetC    [ bcfr ];
  for( i = 0; i < bcfr; i++ ) workEnv->JLCmonomial[i].Reconstruct( workEnv );

  // The reference point is set.
  workEnv->refPoint = new Complex[ n ];
  slist_iterator GetNextValue( newValues );
  i = 0;
  while( ( q = (Complex*) GetNextValue() ) != 0 )
    workEnv->refPoint[i++] = *q;
  if( i != n ) {
    cerr << "\n\n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** JetC::EndEnvironment               \n"
         << "*** ERROR *** An unbelievably hideous error     \n"
         << "*** ERROR *** has occurred.                     \n"
         << "*** ERROR *** " << i << " != " << n << "        \n"
         << "*** ERROR ***               This is beyond      \n"
         << "*** ERROR *** the realm of human understanding. \n"
         << "*** ERROR ***                                   \n"
         << endl;
    exit(1);
  }
 
  workEnv->AllZeroes.Reconstruct(n);
  for( i = 0; i < n; i++ ) workEnv->AllZeroes(i) = 0;
 
  // Load the numPaths array with binomial coefficients;
  // required by Wilf's algorithm for ranking monomials.
  workEnv->numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*(workEnv->numPaths))( i, j-1 ) = bcfRec( i + j - 1, i );

  // Initialize the coordinates
  i = 0;
  while((  p = (coordC*) newCoordCs.get()  )) {
    q = (Complex*) newValues.get();
    p->setVariable( *q, p->Index(), workEnv );  
       // This alters the environment!
    workEnv->myCoords.append( p );
    delete q;
  }
  
  // Set the scale array
  workEnv->scale = new double[ workEnv->NumVar ];
  if( scl )  
    for( i = 0; i < workEnv->NumVar; i++ ) workEnv->scale[i] = abs(scl[i]);
  else 
    for( i = 0; i < workEnv->NumVar; i++ ) workEnv->scale[i] = 1.0;


  // Reset for the next environment and exit.
  JetC__environment::SkipEnvEqTest = 1;
  lastEnv = workEnv;

  JetC::environments.append( workEnv );
  workEnv = 0;        // This does NOT delete the environment.
  newCoordCs.clear();  // Should be unnecessary.
  newValues.clear();  // Should be unnecessary.
  currentIndex = 0;

  return lastEnv;
}


void JetC::EnlargeEnvironment( JetC__environment* pje )
{
  int i;

  // Like JetC::BeginEnvironment
  if( workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** JetC::EnlargeEnvironment                  \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** You cannot enlarge an environment        \n"
         << "*** ERROR *** until all environments are closed.       \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  workEnv = new JetC__environment;
  workEnv->MaxWeight = pje->MaxWeight;
  currentIndex = 0;  

  // Creating new coordinates ...
  coordC* p_coordC;
  for( i = 0; i < pje->NumVar; i++ ) 
    p_coordC = new coordC( pje->refPoint[ i ] );
    // currentIndex is automatically increased by coordC::coordC
    // ??? REMOVE workEnv->myCoordCs.append( new coordC( pje->refPoint[ i ] ) );

  // Like JetC::Parameters()
  if( pje->PBOK != 0 ) {
    workEnv->PBOK = pje->PBOK;
    workEnv->SpaceDim = pje->SpaceDim;
  }
}


//***************************************************************
//***************************************************************
//***************************************************************
//
//    Implementation of class coordC
//
 
coordC::coordC( Complex x ) : JetC() {
 
 if( !workEnv ) {
   cerr << "\n\n"
        << "*** ERROR ***                                          \n"
        << "*** ERROR *** coordC::coordC                             \n"
        << "*** ERROR ***                                          \n"
        << "*** ERROR *** You cannot declare a coordinate before   \n"
        << "*** ERROR *** opening an environment.                  \n"
        << "*** ERROR ***                                          \n"
        << "*** ERROR *** Use JetC::BeginEnvironment()              \n"
        << "*** ERROR ***                                          \n"
        << endl;
   exit(1);
 }

 jl->myEnv = workEnv;
 index = currentIndex++;
 
 coordCPtrs.append( this ); // ??? Should coordCPtrs be removed?
 newCoordCs.append( this );
 newValues.append( new Complex( x ) );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coordC::~coordC() {
 // ??? REMOVE int* n;
 char c;
 // ??? REMOVE n = new int( index );
 // ??? REMOVE JetC::coordCPool.insert( n );
 c = JetC::coordCPtrs.remove( this );

#ifdef OBJECT_DEBUG
  objectCount--;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coordC::coordC( const coordC&  ) {
 printf( "\n*** ERROR ***                                       \n" );
 printf(   "*** ERROR *** coordC::coordC( coordC& )                \n" );
 printf(   "*** ERROR *** Coordinate copy constructor called.   \n" );
 printf(   "*** ERROR ***                                       \n" );
 exit(1);

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void coordC::operator=( const Complex& x ) {   // DANGER: Be careful!
  setVariable( x, index, jl->myEnv );          // This alters the environment!
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coordC& coordC::operator=( const coordC& ) {
 printf( "\n*** ERROR ***                                       \n" );
 printf(   "*** ERROR *** coordC::operator=                      \n" );
 printf(   "*** ERROR *** Attempt made to change the value      \n" );
 printf(   "*** ERROR *** of a coordinate.                      \n" );
 printf(   "*** ERROR ***                                       \n" );
 if( 3 + 8 == 11 ) exit(1);  // To avoid compiler warnings.
 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coordC& coordC::operator=( const JetC& ) {
 printf( "\n*** ERROR ***                                       \n" );
 printf(   "*** ERROR *** coordC::operator=                      \n" );
 printf(   "*** ERROR *** Attempt made to change the value      \n" );
 printf(   "*** ERROR *** of a coordinate.                      \n" );
 printf(   "*** ERROR ***                                       \n" );
 if( 3 + 8 == 11 ) exit(1);  // To avoid compiler warnings.
 return *this;
}


//***************************************************************
//***************************************************************
//***************************************************************
//
//      Functions which rank and unrank monomials.  Developed from
//      algorithms provided by Nijenhuis and Wilf, Combinatorial
//      Algorithms. (2nd ed.)
 
void JetC__environment::monoCode() {
 int c;
 int i, j;
 j = 0;
 for( i = 0; i < NumVar; i++ ) {
   for( c = 0; c < exponent[i]; c++ ) expCode[j++] = 1;
   expCode[j++] = 0;
   }
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetC__environment::monoDecode() {
 int c;
 int i, j;
 j = 0;
 for( i = 0; i < NumVar; i++ ) {
   c = 0;
   while( expCode[j++] ) ++c;
   exponent[i] = c;
   }
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

int JetC__environment::monoRank() {
 //  This function assumes monoCode has been run,
 //  so that the path is appropriately coded into expCode.
 char edge;
 int w, n;
 int rank;
 int zeroCount;
 int i;
 
 i = 0;
 rank = 0;
 w = 0;
 n = 1;
 
 if((  edge = expCode[i++]  )) zeroCount = 0;
 else                          zeroCount = 1;
 
 while( zeroCount < NumVar ) {
 
   if( edge ) {
     w++;
     if( ( w > 0 ) && ( n > 1 ) )  rank += (*numPaths)(w, n-2);
     // ??? REMOVE: if( ( w > 0 ) && ( n > 1 ) )  rank += numPaths[w][n-1];
     }
   else  n++;
 
   if( !( edge = expCode[i++] ) ) zeroCount++;
 
   }
 
 rank += ( w * (*numPaths)(w, n-1) ) / n;
 
 return rank;
}

