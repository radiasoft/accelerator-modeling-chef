#ifdef __VISUAL_CPP__
#include <iomanip>
using std::setprecision;
#else
#include <stdlib.h>
#include <iomanip.h>
#endif

#include "Jet.h"

#define PREPFORCHANGE(jl)  if(((jl)->rc)>1){--((jl)->rc);(jl) = new JL(jl);}

// ================================================================
//      Global variables
//

#define DEFSCALE  0.001

slist Jet::environments;
char  Jet__environment::SkipEnvEqTest = 0;
  // Used by the Jet::EndEnvironment function.

Jet__environment*  Jet::lastEnv = 0;
Jet__environment*  Jet::workEnv = 0;
int                Jet::currentIndex = 0;
slist              Jet::newCoords;
slist              Jet::newValues;
FILE              *JL::scratchFile;
// ??? REMOVE dlist              Jet::coordPool;
dlist              Jet::coordPtrs;
#ifdef OBJECT_DEBUG
int                Jet::objectCount = 0;
int                Jet__environment::objectCount = 0;
int                coord::objectCount = 0;
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
//      Implementation of Jet__environment
//      ??? Belongs in JL.cc

Jet__environment::Jet__environment() 
{
  monomial           = 0;
  JLmonomial         = 0;
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

Jet__environment::Jet__environment( Jet__environment& x )
: MaxWeight( x.MaxWeight ), 
  NumVar( x.NumVar ), 
  SpaceDim( x.SpaceDim )
{
  if( Jet::workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Jet__environment::Jet__environment       \n"
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
         << "*** WARNING *** Jet__environment::Jet__environment       \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLmonomial         = 0;
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
  int i, j;

  j = bcfRec( w + n, n );

  monomial   = new double[ j ];
  JLmonomial = new Jet   [ j ];
  Jet__environment* stupid = &x;
  for( i = 0; i < j; i++ ) JLmonomial[i].Reconstruct( stupid );
  exponent   = new int[ n ];
  expCode    = new char[ w + n ];
  PBOK       = x.PBOK;

  numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ )
    for( j = 1; j <= n; j++ )
      (*numPaths)( i, j-1 ) = bcfRec( i + j - 1, i );

  dof                = x.dof;

  refPoint = new double[ n ];
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

Jet__environment::~Jet__environment() 
{
  if( monomial   ) delete [] monomial;
  if( JLmonomial ) delete [] JLmonomial;
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

Jet__environment& Jet__environment::operator=( const Jet__environment& x )
{

  // The first line is useful for the Jet::EndEnvironment function.
  if( Jet__environment::SkipEnvEqTest == 1 ) 
      Jet__environment::SkipEnvEqTest = 0;

  else if( Jet::workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Jet__environment::operator=              \n"
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
         << "*** WARNING *** Jet__environment::operator=              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    monomial           = 0;
    JLmonomial         = 0;
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
  int i, j;

  if( monomial ) delete [] monomial;
  monomial   = new double[ bcfRec( w + n, n ) ];

  if( JLmonomial ) delete [] JLmonomial;
  JLmonomial = new Jet[ bcfRec( w + n, n ) ];

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
  refPoint = new double[ n ];

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


char Jet__environment::operator==( const Jet__environment& x ) const
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


char Jet__environment::operator!=( const Jet__environment& x ) const
{
  return !operator==( x );
}


ostream& 
operator<<( ostream& os, const Jet__environment* x )
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
operator>>( istream& is, Jet__environment* x )
{
  static slist envs;
  slist_iterator getNext( envs );
  Jet__environment* pje;
  int i(0), j(0);

  // --- Test to see if x was previously read
  while((  pje = (Jet__environment*) getNext()  ))
    if( *pje == *x ) {
      if( x ) delete x;
      x = pje;
      return is;
    }
  
  // --- x was not previously read
  pje = new Jet__environment;
  is >> pje->NumVar;
  is >> pje->SpaceDim;

  if( pje->NumVar < pje->SpaceDim ) {
    cerr << "\n\n"
         << "*** ERROR ***                              \n"
         << "*** ERROR *** istream operator>>           \n"
         << "*** ERROR ***                              \n"
         << "*** ERROR *** Jet__enviroment dimensions   \n"
         << "*** ERROR *** are wrong.                   \n"
         << "*** ERROR ***                              \n"
         << endl;
    exit(1);
  }
  
  pje->refPoint = new double[ pje->NumVar ];
  pje->scale = new double[ pje->NumVar ];
  for( i = 0; i < x->NumVar; i++ ) is >> pje->refPoint[i];
  for( i = 0; i < x->NumVar; i++ ) is >> pje->scale[i];

  is >> pje->MaxWeight;
  is >> pje->PBOK;


  // --- Calculations, just as in Jet::EndEnvironment
  int n = pje->NumVar;
  int w = pje->MaxWeight;

  pje->exponent   = new int[ n ];
  pje->expCode    = new char[ w + n ];
  pje->monomial   = new double[ bcfRec( w + n, n ) ];
  pje->JLmonomial = new Jet[ bcfRec( w + n, n ) ];

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

Jet::Jet( Jet__environment* pje ) 
{
 jl = new JL( pje );
 constIterPtr = 0;
 iterPtr     = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

Jet::Jet( const double& x, Jet__environment* p_je ) 
{
 jl = new JL( x, p_je );
 constIterPtr = 0;
 iterPtr     = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

Jet::Jet( const Jet& x ) 
{
 jl = x.jl;
 ( jl->rc )++;

 constIterPtr = 0;  // The iterators are NOT copied.
 iterPtr     = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

Jet::~Jet() 
{
 if( 0 != iterPtr ) {
   delete iterPtr;
 }
 if( 0 != constIterPtr ) {
   delete constIterPtr;
 }
 if( --(jl->rc) == 0 ) {
   delete jl;
 }

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

void Jet::Reconstruct()
{
 // Combines destructor and constructor functions.
 // Use when initializing a static Jet variable.
 static Jet__environment* pje;
 pje = jl->myEnv;
 if( --(jl->rc) == 0 ) delete jl;
 jl = new JL( pje );
}

void Jet::Reconstruct( Jet__environment* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static Jet variable.
 if( --(jl->rc) == 0 ) delete jl;
 jl = new JL( pje );
}


void Jet::setEnvTo( const Jet& x )
{
  if( jl == NULL ) 
  {
    jl = new JL( x.jl->myEnv );
  }
  else if( jl->myEnv != x.jl->myEnv ) 
  {
    PREPFORCHANGE(jl);
    jl->myEnv = x.jl->myEnv;
  }
}


void Jet::setEnvTo( const Jet__environment* pje )
{
  if( jl == NULL ) 
  {
    jl = new JL( pje );
  }
  else if( jl->myEnv != pje ) 
  {
    PREPFORCHANGE(jl);
    jl->myEnv = (Jet__environment*) pje;
  }
}


void Jet::getReference( double* r ) const
{
 jl->getReference( r );
}

void Jet::setVariable( const double& x,
                       const int& j, 
                             Jet__environment* pje )
{
 if( jl == NULL ) {
   jl = new JL( pje );
 }
 else {
   PREPFORCHANGE(jl)
 }
 jl->setVariable( x, j, pje );  // !! Alters the environment!
}

void Jet::setVariable( const double& x,
                       const int& j )
{
 if( jl == NULL ) {
   cerr << "\n\n"
        << "*** ERROR ***                               \n"
        << "*** ERROR *** Jet::setVariable( int, double )    \n"
        << "*** ERROR ***                               \n"
        << "*** ERROR *** jl not initialized?????       \n"
        << "*** ERROR ***                               \n"
        << endl;
   exit(1);
 }
 else {
   PREPFORCHANGE(jl)
 }
 jl->setVariable( x, j, jl->myEnv );  // !! Alters the environment!
}

void Jet::setVariable( const int& j, 
                             Jet__environment* pje ) 
{
 if( jl == NULL ) {
   jl = new JL( pje );
 }
 else {
   PREPFORCHANGE(jl)
 }
 jl->setVariable( j, pje );
}

void Jet::setVariable( const int& j )
{
 if( jl == NULL ) {
   cerr << "\n\n"
        << "*** ERROR ***                               \n"
        << "*** ERROR *** Jet::setVariable( int )       \n"
        << "*** ERROR ***                               \n"
        << "*** ERROR *** jl not initialized?????       \n"
        << "*** ERROR ***                               \n"
        << endl;
   exit(1);
 }
 else {
   PREPFORCHANGE(jl)
 }
 jl->setVariable( j, jl->myEnv );
}

Jet& Jet::operator=( const Jet& x ) 
{
 if( jl != x.jl ) {
   if( --(jl->rc) == 0 ) delete jl;
   jl = x.jl;
   (jl->rc)++;
 }
 return *this; 
}

Jet& Jet::DeepCopy( const Jet& x ) 
{
 if( this != &x ) {
  if( --(jl->rc) == 0 ) delete jl;
  jl = new JL( x->myEnv );
  *jl = *(x.jl);
 }
 return *this; 
}

Jet& Jet::operator=( const double& x ) 
{
 static Jet__environment* pje;
 pje = jl->myEnv;

 if( --(jl->rc) == 0 ) delete jl;
 jl = new JL(pje);
 *(jl) = x;
 return *this; 
}

istream& operator>>( istream& is,  Jet& x ) 
{
 if( --(x.jl->rc) == 0 ) delete x.jl;
 x.jl = new JL;
 return operator>>( is, *(x.jl) );
}

ostream& operator<<( ostream& os, const Jet& x ) 
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
Jet::BeginEnvironment( int w ) 
{
  if( workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Jet::BeginEnvironment                    \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** You cannot open a new environment        \n"
         << "*** ERROR *** until you close an old one.              \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  workEnv = new Jet__environment;
  workEnv->MaxWeight = w;
  currentIndex = 0;
}


void
Jet::Parameters()
{
  if( workEnv->PBOK == 1 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Jet::Parameters                          \n"
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


Jet__environment*
Jet::EndEnvironment( double* scl )
{
  if( currentIndex == 0 ) {
    delete workEnv;
    workEnv = 0;
    return 0;
  }

  coord*  p;
  double* q;
  int     i, j;

  if((  ( currentIndex     != newCoords.size() )  ||  
        ( newCoords.size() != newValues.size() )  )) {
    cerr << "\n\n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** Jet::EndEnvironment               \n"
         << "*** ERROR *** A horrible, inexplicable error    \n"
         << "*** ERROR *** has occurred. This is beyond      \n"
         << "*** ERROR *** the realm of human understanding. \n"
         << "*** ERROR *** Please consult an exorcist.       \n"
         << "*** ERROR ***                                   \n";

    cerr << "*** ERROR *** currentIndex:      " << currentIndex     << endl;
    cerr << "*** ERROR *** newCoords.size() : " << newCoords.size() << endl;
    cerr << "*** ERROR *** newValues.size() : " << newValues.size() << endl;
    cerr << "*** ERROR ***                                   \n"
         << endl;
    exit(1);
  }

  workEnv->NumVar = currentIndex;
  if( !workEnv->PBOK ) workEnv->SpaceDim = currentIndex;
  workEnv->dof = workEnv->SpaceDim / 2;
  if( workEnv->PBOK && ( 2*workEnv->dof != workEnv->SpaceDim ) ) {
    cerr << "\n\n"
         << "*** WARNING ***                                 \n"
         << "*** WARNING *** Jet::EndEnvironment()           \n"
         << "*** WARNING *** Phase space has odd dimension.  \n"
         << "*** WARNING *** I hope you know what your       \n"
         << "*** WARNING *** are doing, but I doubt it.      \n"
         << "*** WARNING ***                                 \n"
         << endl;
  }
  
  int w = workEnv->MaxWeight;
  int n = workEnv->NumVar;
  int bcfr = bcfRec( w + n, n );

  workEnv->exponent   = new int[ n ];
  workEnv->expCode    = new char[ w + n ];
  workEnv->monomial   = new double [ bcfr ];
  workEnv->JLmonomial = new Jet    [ bcfr ];
  for( i = 0; i < bcfr; i++ ) workEnv->JLmonomial[i].Reconstruct( workEnv );

  // The reference point is set.
  workEnv->refPoint = new double[ n ];
  slist_iterator GetNextValue( newValues );
  i = 0;
  while( ( q = (double*) GetNextValue() ) != 0 )
    workEnv->refPoint[i++] = *q;
  if( i != n ) {
    cerr << "\n\n"
         << "*** ERROR ***                                   \n"
         << "*** ERROR *** Jet::EndEnvironment               \n"
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
  while((  p = (coord*) newCoords.get()  )) {
    q = (double*) newValues.get();
    p->setVariable( *q, p->Index(), workEnv );  
       // This alters the environment!
    workEnv->myCoords.append( p );
    delete q;
  }
  
  // Set the scale array
  workEnv->scale = new double[ workEnv->NumVar ];
  if( scl )  
    for( i = 0; i < workEnv->NumVar; i++ ) workEnv->scale[i] = fabs(scl[i]);
  else 
    for( i = 0; i < workEnv->NumVar; i++ ) workEnv->scale[i] = DEFSCALE;


  // Reset for the next environment and exit.
  Jet__environment::SkipEnvEqTest = 1;
  lastEnv = workEnv;

  Jet::environments.append( workEnv );
  workEnv = 0;        // This does NOT delete the environment.
  newCoords.clear();  // Should be unnecessary.
  newValues.clear();  // Should be unnecessary.
  currentIndex = 0;

  return lastEnv;
}


void Jet::EnlargeEnvironment( Jet__environment* pje )
{
  int i;

  // Like Jet::BeginEnvironment
  if( workEnv != 0 ) {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Jet::EnlargeEnvironment                  \n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** You cannot enlarge an environment        \n"
         << "*** ERROR *** until all environments are closed.       \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }

  workEnv = new Jet__environment;
  workEnv->MaxWeight = pje->MaxWeight;
  currentIndex = 0;  

  // Creating new coordinates ...
  coord* p_coord;
  for( i = 0; i < pje->NumVar; i++ ) 
    p_coord = new coord( pje->refPoint[ i ] );
    // currentIndex is automatically increased by coord::coord
    // ??? REMOVE workEnv->myCoords.append( new coord( pje->refPoint[ i ] ) );

  // Like Jet::Parameters()
  if( pje->PBOK != 0 ) {
    workEnv->PBOK = pje->PBOK;
    workEnv->SpaceDim = pje->SpaceDim;
  }
}


//***************************************************************
//***************************************************************
//***************************************************************
//
//    Implementation of class coord
//
 
coord::coord( double x ) 
: Jet( workEnv ) {
 
 if( !workEnv ) {
   cerr << "\n\n"
        << "*** ERROR ***                                          \n"
        << "*** ERROR *** coord::coord                             \n"
        << "*** ERROR ***                                          \n"
        << "*** ERROR *** You cannot declare a coordinate before   \n"
        << "*** ERROR *** opening an environment.                  \n"
        << "*** ERROR ***                                          \n"
        << "*** ERROR *** Use Jet::BeginEnvironment()              \n"
        << "*** ERROR ***                                          \n"
        << endl;
   exit(1);
 }

 // ??? REMOVE: jl->myEnv = workEnv;
 index = currentIndex++;
 
 coordPtrs.append( this ); // ??? Should coordPtrs be removed?
 newCoords.append( this );
 newValues.append( new double( x ) );

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coord::~coord() {
 // ??? REMOVE int* n;
 char c;
 // ??? REMOVE n = new int( index );
 // ??? REMOVE Jet::coordPool.insert( n );
 c = Jet::coordPtrs.remove( this );

#ifdef OBJECT_DEBUG
  objectCount--;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coord::coord( const coord&  ) {
 printf( "\n*** ERROR ***                                       \n" );
 printf(   "*** ERROR *** coord::coord( coord& )                \n" );
 printf(   "*** ERROR *** Coordinate copy constructor called.   \n" );
 printf(   "*** ERROR ***                                       \n" );
 exit(1);

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void coord::operator=( const double& x ) {   // DANGER: Be careful!
  setVariable( x, index, this->Env() );      // This alters the environment!
  // ??? REMOVE: setVariable( x, index, jl->myEnv );        // This alters the environment!
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coord& coord::operator=( const coord& ) {
 printf( "\n*** ERROR ***                                       \n" );
 printf(   "*** ERROR *** coord::operator=                      \n" );
 printf(   "*** ERROR *** Attempt made to change the value      \n" );
 printf(   "*** ERROR *** of a coordinate.                      \n" );
 printf(   "*** ERROR ***                                       \n" );
 if( 3 + 8 == 11 ) exit(1);  // To avoid compiler warnings.
 return *this;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coord& coord::operator=( const Jet& ) {
 printf( "\n*** ERROR ***                                       \n" );
 printf(   "*** ERROR *** coord::operator=                      \n" );
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
 
void Jet__environment::monoCode() {
 int c;
 int i, j;
 j = 0;
 for( i = 0; i < NumVar; i++ ) {
   for( c = 0; c < exponent[i]; c++ ) expCode[j++] = 1;
   expCode[j++] = 0;
   }
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Jet__environment::monoDecode() {
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

int Jet__environment::monoRank() {
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



//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//    Iteration routines
// 

void Jet::resetConstIterator() 
{
  if( 0 == constIterPtr ) {
    constIterPtr = new dlist_iterator( *(dlist*) jl );
  }
  else {
    delete constIterPtr;
    constIterPtr = new dlist_iterator( *(dlist*) jl );
    // Note: jl may have changed.
  }
}

JLterm Jet::stepConstIterator()  const
{
  if( constIterPtr ) {
    return JLterm( (JLterm*) (constIterPtr->operator()()) );
  }
  else {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Jet::stepConstIterator                   \n"
         << "*** ERROR *** You must first resetConstIterator        \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }
}

void Jet::resetIterator()
{
  if( 0 == iterPtr ) {
    iterPtr = new dlist_iterator( *(dlist*) jl );
  }
  else {
    delete iterPtr;
    iterPtr = new dlist_iterator( *(dlist*) jl );
    // Note: jl may have changed.
  }
}

JLterm* Jet::stepIterator()
{
  PREPFORCHANGE(jl)
  // This has to be done each time, since jl could have
  // been given to a new Jet since the last invocation.

  if( iterPtr ) {
    return (JLterm*) (iterPtr->operator()());
  }
  else {
    cerr << "\n\n"
         << "*** ERROR ***                                          \n"
         << "*** ERROR *** Jet::stepIterator                        \n"
         << "*** ERROR *** You must first resetIterator             \n"
         << "*** ERROR ***                                          \n"
         << endl;
    exit(1);
  }
}
