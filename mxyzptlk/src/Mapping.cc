#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      Mapping.cc
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


#include "Mapping.h"
#include "JL.h"
using std::cout;


#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER


//    Globals |||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifdef OBJECT_DEBUG
int Mapping::objectCount = 0;
#endif


using namespace std;

//    Constructors and destructors    |||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping::Mapping( const int& n,
          const Jet* pj,
                Jet__environment* pje )
: JetVector( n, pj, pje )
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping::Mapping( const Mapping& x ) 
: JetVector( (JetVector&) x ) 
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping::Mapping( const JetVector& x ) 
: JetVector( x ) 
{
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping::Mapping( char*, Jet__environment* pje  ) 
: JetVector( pje->SpaceDim, 0, pje )
{
 int i, s;
 
 if( pje->SpaceDim == 0 ) {
   cerr << "\n*** ERROR: Mapping::Mapping called " << endl;
   cerr << "but phase space has dimension 0.\n" << endl;
   exit(0);
 }
 
 // if( !pje ) {
 //   cerr << "\n*** Mapping::Mapping( char* ) ERROR *** " << endl;
 //   cerr << "No environment defined." << endl;
 //   exit(0);
 // }
 
 s = pje->SpaceDim;
 myEnv = pje;

 for( i = 0; i < s; i++ ) comp[i].setVariable( i, pje );

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping::~Mapping() 
{
#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping& Mapping::operator=( const Mapping& x )
{ 
  return (Mapping&) ( ( (JetVector*) this )->operator=( (JetVector&) x ) ); 
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector
Mapping::operator()( const Vector& x ) const
{
 static int i;

 i = x.Dim();
 if( ( i != myEnv->NumVar ) || ( i != dim ) ) {
   cerr << "\n\n"
        << "*** ERROR ***                                  \n"
        << "*** ERROR *** Mapping::operator()( Mapping )   \n"
        << "*** ERROR ***                                  \n"
        << "*** ERROR *** Incompatible dimensions.         \n"
        << "*** ERROR ***                                  \n"
        << endl;
   exit(1);
 }

 Vector z( dim );

 for( i = 0; i < myEnv->SpaceDim; i++) {
  z(i) = comp[i]( x );
 }

 return z;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping 
Mapping::operator()( const Mapping& x ) const
{
 if( x.dim != myEnv->NumVar ) {
   cerr << "\n\n"
        << "*** ERROR ***                                  \n"
        << "*** ERROR *** Mapping::operator()( Mapping )           \n"
        << "*** ERROR ***                                  \n"
        << "*** ERROR *** Incompatible dimensions.         \n"
        << "*** ERROR ***                                  \n"
        << endl;
   exit(1);
 }

 int i;
 Mapping z( dim, 0, x.myEnv );

 for( i = 0; i < myEnv->SpaceDim; i++) {
  z.comp[i] = comp[i]( (JetVector&) x );
 }

 return z;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping Mapping::operator*( const Mapping& x ) const
{
  return operator()( x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void operator*=( Mapping& x, const Mapping& y ) {
 x = x*y;
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MatrixD 
Mapping::Jacobian() const 
{
 int            i, j;
 int            nv = myEnv->NumVar;   // ??? Is this right?
 int*           d = new int[ nv ];
 MatrixD        M( dim, nv, 0.0 );

 for( i = 0; i < nv; i++  ) d[i] = 0;
 for( j = 0; j < nv; j++ ) {
  d[j] = 1;
  for( i = 0; i < dim; i++ ) M( i, j ) = comp[i].derivative( d );
  d[j] = 0;
 }

 delete [] d;
 return M;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping 
Mapping::Inverse() const 
{ 
 if( myEnv->SpaceDim != dim ) {
  cerr << "*** ERROR ***                                     \n"
          "*** ERROR *** Mapping::Inverse                        \n"
          "*** ERROR ***                                     \n"
          "*** ERROR *** Dimensions do not match correctly.  \n"
          "*** ERROR *** myEnv->SpaceDim = " 
                                     << myEnv->SpaceDim << " \n"
          "*** ERROR *** dim = " << dim << "                 \n"
          "*** ERROR ***                                     \n"
       << endl;
  exit(1);
 }

 int        i, j;
 int        nv   = myEnv->NumVar;
 char       t1;
 char*      t2   = new char    [dim];
 char       t3;
 double*    ref  = new double  [nv];
 JLterm**   p    = new JLterm* [dim];
 for( j = 0; j < dim; j++ ) p[j] = 0;


 // If zero maps to zero, return result .....................
 t1 = 1;    // Indicates reference point is the origin.
            // Test is one SpaceDim coordinates only.
 for( i = 0; i < dim; i++ ) 
   if( myEnv->refPoint[i] != 0.0 ) {
     t1 = 0;
     break;
   }

 for( j = 0; j < dim; j++ )
   t2[j] = ( comp[j].standardPart() == 0.0 );

 t3 = t1;
 if( t3 ) for( j = 0; j < dim; j++ ) t3 &= t2[j];

 if( t3 ) {
   delete [] t2;
   delete [] ref;
   delete [] p;
   return EpsInverse( myEnv );
 }


 // Otherwise, create a new Jet__environment .........................
 Jet__environment* pje_new = new Jet__environment( *myEnv );
 Jet__environment* pje;

 for( i = 0; i < dim; i++ ) pje_new->refPoint[i] = comp[i].standardPart();

 // ... Check to see if it already exists
 slist_iterator g( Jet::environments );
 char found = 0;

 while( pje = (Jet__environment*) g() )
   if( *pje == *pje_new ) {
     found = 1;
     break;
   }

 if( found ) {
   delete pje_new;
   pje_new = pje;
 }
 else Jet::environments.append( pje_new );


 // Construct an idempotent 
 // and compute its inverse. ..................................
 Mapping z( dim, 0, myEnv );
 z = *this;   // ??? Deep copy needed here.

 // ... Temporarily zero the reference point 
 for( i = 0; i < dim; i++ ) pje_new->refPoint[i] = 0.0;
 z.myEnv = pje_new;
 for( j = 0; j < dim; j++ ) {
   if( !t2[j] ) { 
     p[j] = z.comp[j].get();
     z.comp[j].addTerm( new JLterm( z.myEnv->AllZeroes, 0.0, z.myEnv ) );
     // ??? This last line should not be necessary.
   }
 }

 if( !t1 ) 
   for( j = 0; j < dim; j++ )        // A little paranoia
     z.comp[j].setEnvTo( z.myEnv );  // never hurt.

 z = z.EpsInverse( z.myEnv );


 // Make final adjustments before returning. .....................
 for( i = 0; i < dim; i++ ) 
   pje_new->refPoint[i] = comp[i].standardPart();

 // ... A little test ...
 z.standardPart( ref );
 for( j = 0; j < dim; j++ ) 
   if( ref[j] != 0.0 ) {
    cout << "*** WARNING ***                                              \n"
            "*** WARNING *** Jet::Inverse()                               \n"
            "*** WARNING *** ref[" << j << "] = " << ref[j] << "          \n"
            "*** WARNING ***                                              \n"
         << endl;
   }

 // ... Add the correct image point ...
 if( !t1 ) {
  for( j = 0; j < z.dim; j++ ) 
   z.comp[j].addTerm( new JLterm( z.myEnv->AllZeroes, 
                                  myEnv->refPoint[j], 
                                  z.myEnv
                                ) 
                    );
 }


 // Clean up and leave
 delete [] t2;
 delete [] ref;
 for( j = 0; j < dim; j++ ) delete p[j];
 delete [] p;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Mapping 
Mapping::EpsInverse( Jet__environment* pje ) const 
{
 Mapping  z( dim, 0, pje );
 Mapping  id( "ident", pje );
 Mapping  v( dim, 0, pje );
 int  i = 0;
 MatrixD M( dim, dim, 0.0 );

 if( dim == myEnv->NumVar ) M = Jacobian().inverse();
 else              M = Jacobian().Square().inverse();

 z = M*id;
 v = ( operator()(z) - id );
 while ( ( i++ < MX_MAXITER ) && ( ( v - v.filter(0,1)) != 0.0 ) ) {
               // This assumes linear is handled well enough
               // by the MatrixD routine.  
  z = z - M*v;
  v = ( operator()(z) - id );
 }

 if( i >= MX_MAXITER ) {
  cerr << "\n\n"
       << "*** WARNING ***                                            \n" 
          "*** WARNING *** Mapping& Mapping::EpsInverse()                     \n" 
          "*** WARNING *** Over " << MX_MAXITER << " iterations used. \n"
          "*** WARNING *** result may be incorrect.                   \n" 
          "*** WARNING ***                                            \n" 
       << endl;
 }

 return z;
}

