/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      JetC.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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

#include <iomanip>

#include "JetC.h"

#define PREPFORCHANGE(jl)  if(((jl)->rc)>1){--((jl)->rc);(jl) = new JLC(jl);}

using namespace std;
using FNAL::Complex;

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
FILE              *JLC::scratchFile;
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
    throw( JLC::GenericException( __FILE__, __LINE__, 
           "JetC__environment::JetC__environment( JetC__environment& )",
           "Close open environment before copying." ) );
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
      JetC__environment::SkipEnvEqTest = 0;

  else if( JetC::workEnv != 0 ) {
    throw( JLC::GenericException( __FILE__, __LINE__, 
           "JetC__environment& JetC__environment::operator=( const JetC__environment& )",
           "Close open environment before assigning." ) );
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
operator<<( ostream& os, const JetC__environment& x )
{
  int i;
  os << x.NumVar << endl;
  os << x.SpaceDim << endl;
  for( i = 0; i < x.NumVar; i++ ) os << setprecision(30) 
                                      << x.refPoint[i] << endl;
  for( i = 0; i < x.NumVar; i++ ) os << setprecision(30) 
                                      << x.scale[i] << endl;
  os << x.MaxWeight << endl;
  os << x.PBOK << endl;
  return os;
}

istream& streamIn( istream& is, JetC__environment** x )
{
  static slist envs;
  slist_iterator getNext( envs );
  JetC__environment* pje;
  int i(0), j(0);

  pje = new JetC__environment;
  is >> pje->NumVar;
  is >> pje->SpaceDim;

  if( pje->NumVar < pje->SpaceDim ) {
    throw( JLC::GenericException( __FILE__, __LINE__, 
           "istream& streamIn( istream&, JetC__environment** )",
           "JetC__enviroment dimensions are wrong." ) );
  }
  
  pje->refPoint = new Complex[ pje->NumVar ];
  pje->scale = new double[ pje->NumVar ];
  for( i = 0; i < pje->NumVar; i++ ) is >> pje->refPoint[i];
  for( i = 0; i < pje->NumVar; i++ ) is >> pje->scale[i];

  is >> pje->MaxWeight;
  is >> pje->PBOK;


  // --- Calculations, just as in JetC::EndEnvironment
  int n = pje->NumVar;
  int w = pje->MaxWeight;

  pje->exponent   = new int[ n ];
  pje->expCode    = new char[ w + n ];
  pje->monomial   = new Complex[ bcfRec( w + n, n ) ];
  pje->JLCmonomial = new JetC[ bcfRec( w + n, n ) ];

  pje->AllZeroes.Reconstruct(n);
  for( i = 0; i < n; i++ ) pje->AllZeroes(i) = 0;

  // Load the numPaths array with binomial coefficients;
  // required by Wilf's algorithm for ranking monomials.
  pje->numPaths = new MatrixI( w+1, n );
  for( i = 0; i <= w; i++ ) {
    for( j = 1; j <= n; j++ ) {
      (*(pje->numPaths))( i, j-1 ) = bcfRec( i + j - 1, i );
    }
  }

  // Initialize the coordinates
  // ??? HOW ???

  // --- Test to see if environment was already read
  JetC__environment* q;
  bool found( false );
  while((  !found && ( q = (JetC__environment*) getNext()  ) )) 
  {
    if( *pje == *q ) 
    {
      delete pje;
      pje = q;
      found = true;
    }
  }
  if( !found ) {
    envs.append( pje );
  }

  // --- Finished
  *x = pje;
  return is;
}

//***************************************************************
//***************************************************************
//***************************************************************

JetC::JetC( JetC__environment* pje ) 
{
 jl = new JLC( pje );
 constIterPtr = 0;
 iterPtr      = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetC::JetC( const Complex& x, JetC__environment* p_je ) 
{
 jl = new JLC( x, p_je );
 constIterPtr = 0;
 iterPtr      = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetC::JetC( const JetC& x ) 
{
 jl = x.jl;
 ( jl->rc )++;

 constIterPtr = 0;  // Iterators are NOT copied.
 iterPtr      = 0;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

JetC::~JetC() 
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

void JetC::Reconstruct()
{
 // Combines destructor and constructor functions.
 // Use when initializing a static JetC variable.
 static JetC__environment* pje;
 pje = jl->myEnv;
 if( --(jl->rc) == 0 ) delete jl;
 jl = new JLC( pje );
}

void JetC::Reconstruct( JetC__environment* pje )
{
 // Combines destructor and constructor functions.
 // Use when initializing a static JetC variable.
 if( --(jl->rc) == 0 ) delete jl;
 jl = new JLC( pje );
}


void JetC::setEnvTo( const JetC& x )
{
  if( jl == NULL ) 
  {
    jl = new JLC( x.jl->myEnv );
  }
  else if( jl->myEnv != x.jl->myEnv ) 
  {
    PREPFORCHANGE(jl);
    jl->myEnv = x.jl->myEnv;
  }
}


void JetC::setEnvTo( const JetC__environment* pje )
{
  if( jl == NULL ) 
  {
    jl = new JLC( pje );
  }
  else if( jl->myEnv != pje ) 
  {
    PREPFORCHANGE(jl);
    jl->myEnv = (JetC__environment*) pje;
  }
}


void JetC::getReference( Complex* r ) const 
{
 jl->getReference( r );
}

void JetC::setVariable( const Complex& x,
                       const int& j, 
                             JetC__environment* pje )
{
 if( jl == NULL ) {
   jl = new JLC( pje );
 }
 else {
   PREPFORCHANGE(jl)
 }
 jl->setVariable( x, j, pje );  // !! Alters the environment!
}

void JetC::setVariable( const int& j, 
                             JetC__environment* pje ) 
{
 if( jl == NULL ) {
   jl = new JLC( pje );
 }
 else {
   PREPFORCHANGE(jl)
 }
 jl->setVariable( j, pje );
}

JetC& JetC::operator=( const JetC& x ) 
{
 if( jl != x.jl ) {
   if( --(jl->rc) == 0 ) delete jl;
   jl = x.jl;
   (jl->rc)++;
 }
 return *this; 
}

JetC& JetC::DeepCopy( const JetC& x ) 
{
 if( this != &x ) {
  if( --(jl->rc) == 0 ) delete jl;
  jl = new JLC( x->myEnv );
  *jl = *(x.jl);
 }
 return *this; 
}

JetC& JetC::operator=( const Complex& x ) 
{
 static JetC__environment* pje;
 pje = jl->myEnv;

 if( --(jl->rc) == 0 ) delete jl;
 jl = new JLC(pje);
 *(jl) = x;
 return *this; 
}

istream& operator>>( istream& is,  JetC& x ) 
{
 if( --(x.jl->rc) == 0 ) delete x.jl;
 x.jl = new JLC;
 return operator>>( is, *(x.jl) );
}

ostream& operator<<( ostream& os, const JetC& x ) 
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

void JetC::BeginEnvironment( int w ) 
{
  if( workEnv != 0 ) {
    throw( JLC::GenericException( __FILE__, __LINE__, 
           "void JetC::BeginEnvironment( int )",
           "Cannot open two environments simultaneously. Close first." ) );
  }

  workEnv = new JetC__environment;
  workEnv->MaxWeight = w;
  currentIndex = 0;
}


void JetC::Parameters()
{
  if( workEnv->PBOK ) {
   throw( JLC::GenericException( __FILE__, __LINE__, 
          "void JetC::Parameters()",
          "Can only be called once per (open) environment." ) );
  }
  workEnv->PBOK = 1;
  workEnv->SpaceDim = currentIndex;
}


JetC__environment* JetC::EndEnvironment( Complex* scl )
{
  if( currentIndex == 0 ) {
    delete workEnv;
    workEnv = 0;
    return 0;
  }

  coordC*  p;
  Complex* q;
  int     i, j;

  if((  ( currentIndex      != newCoordCs.size())  ||  
        ( newCoordCs.size() != newValues.size() )  )) {
    throw( JLC::HorribleException( 
           currentIndex, newCoordCs.size(), newValues.size(),
           __FILE__, __LINE__, 
           "JetC__environment* JetC::EndEnvironment( Complex* )",
           "" ) );
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
    throw( JLC::HideousException(i, n, __FILE__, __LINE__, 
             "JetC__environment* JetC::EndEnvironment( Complex* )", 
             "" ) );
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
    p->Reconstruct( workEnv );
    // This reconstruction is not strictly necessary
    // when the data is represented by a list - which
    // can be empty. It is needed by other representations,
    // and is, in any case, safe.
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
    throw( JLC::GenericException( __FILE__, __LINE__, 
           "void JetC::EnlargeEnvironment( JetC__environment* )",
           "Close open environment before invoking this function." ) );
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
  if( pje->PBOK ) {
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
 
coordC::coordC( Complex x ) 
: JetC( workEnv ) {
 
 if( !workEnv ) {
   throw( JLC::GenericException( __FILE__, __LINE__, 
          "coordC::coordC( Complex ) ",
          "Use JetC::BeginEnvironment() to open an environment first." ) );
 }

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
 throw( JLC::GenericException( __FILE__, __LINE__, 
        "coordC::coordC( const coordC& )",
        "Coordinate copy constructor called; this is forbidden." ) );

#ifdef OBJECT_DEBUG
  objectCount++;
#endif
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void coordC::operator=( const Complex& x ) {   // DANGER: Be careful!
  setVariable( x, index, this->Env() );        // This alters the environment!
  // ??? REMOVE: setVariable( x, index, jl->myEnv );          // This alters the environment!
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coordC& coordC::operator=( const coordC& ) {
 throw( JLC::GenericException( __FILE__, __LINE__, 
        "coordC& coordC::operator=( const coordC& )",
        "It is forbidden to change the value of a coordinate." ) );
 return *this; // Never executed, of course.
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

coordC& coordC::operator=( const JetC& ) {
 throw( JLC::GenericException( __FILE__, __LINE__, 
        "coordC& coordC::operator=( const JetC& )",
        "It is forbidden to change the value of a coordinate." ) );
 return *this;  // Never executed, of course.
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

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//
//    Iteration routines
// 

void JetC::resetConstIterator()
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

JLCterm JetC::stepConstIterator()  const
{
  if( constIterPtr ) {
    return JLCterm( (JLCterm*) (constIterPtr->operator()()) );
  }
  else {
    throw( JLC::GenericException( __FILE__, __LINE__, 
           "JLCterm JetC::stepConstIterator()  const",
           "You must first resetConstIterator." ) );
  }
}

void JetC::resetIterator()
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

JLCterm* JetC::stepIterator()
{
  PREPFORCHANGE(jl)
  // This has to be done each time, since jl could have
  // been given to a new JetC since the last invocation.

  if( iterPtr ) {
    return (JLCterm*) (iterPtr->operator()());
  }
  else {
    throw( JLC::GenericException( __FILE__, __LINE__, 
           "JLCterm* JetC::stepIterator()",
           "You must first resetIterator." ) );
  }
}
