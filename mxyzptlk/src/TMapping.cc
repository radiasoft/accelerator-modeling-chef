/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.cc
******  Version:   1.0
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
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

#include "TMapping.h"
#include "TJL.h"

using namespace std;

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER


//    Globals |||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

using namespace std;
using FNAL::Complex;

//    Constructors and destructors    |||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMapping<T1,T2>::TMapping<T1,T2>( short int n,
                                  const TJet<T1,T2>* pj,
                                  TJetEnvironment<T1,T2>* pje )
: TJetVector<T1,T2>( n, pj, pje )
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMapping<T1,T2>::TMapping<T1,T2>( const TMapping<T1,T2>& x ) 
: TJetVector<T1,T2>( (TJetVector<T1,T2>&) x ) 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMapping<T1,T2>::TMapping<T1,T2>( const TJetVector<T1,T2>& x ) 
: TJetVector<T1,T2>( x ) 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMapping<T1,T2>::TMapping<T1,T2>( char*, TJetEnvironment<T1,T2>* pje  ) 
: TJetVector<T1,T2>( pje->SpaceDim, 0, pje )
{
 int i, s;
 
 if( pje->SpaceDim == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T1,T2>::TMapping<T1,T2>( char*, TJetEnvironment<T1,T2>* ) ",
          "Phase space has dimension 0." ) );
 }
 
 s = pje->SpaceDim;
 _myEnv = pje;

 for( i = 0; i < s; i++ ) comp[i].setVariable( i, pje );
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMapping<T1,T2>::~TMapping<T1,T2>() 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMapping<T1,T2> TMapping<T1,T2>::operator()( const TMapping<T1,T2>& x ) const
{
 if( x.dim != _myEnv->_numVAr ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T1,T2> TMapping<T1,T2>::operator()( const TMapping<T1,T2>& ) const",
          "Incompatible dimensions." ) );
 }

 int i;
 TMapping<T1,T2> z( dim, 0, x._myEnv );

 for( i = 0; i < _myEnv->SpaceDim; i++) {
  z.comp[i] = comp[i]( (TJetVector<T1,T2>&) x );
 }

 return z;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMapping<T1,T2> TMapping<T1,T2>::operator*( const TMapping<T1,T2>& x ) const
{
  return operator()( x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
void operator*=( TMapping<T1,T2>& x, const TMapping<T1,T2>& y ) {
 x = x*y;
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T1, typename T2>
TMatrix<T1> TMapping<T1,T2>::Jacobian() const 
{
 int            i, j;
 int            nv = _myEnv->_numVAr;   // ??? Is this right?
 int*           d = new int[ nv ];
 TMatrix<T1>    M( dim, nv, ((T1) 0.0) );

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

template<typename T1, typename T2>
TMapping<T1,T2> TMapping<T1,T2>::Inverse() const 
{ 
 if( _myEnv->SpaceDim != dim ) {
  throw( TJL<T1,T2>::BadDimension( _myEnv->SpaceDim, dim, __FILE__, __LINE__, 
         "Mapping Mapping::Inverse() const ",
         "Phase space dimensions do not match." ) );
 }

 int        i, j;
 int        nv   = _myEnv->_numVAr;
 char       t1;
 char*      t2   = new char    [dim];
 char       t3;
 Complex*   ref  = new Complex [nv];
 TJLterm<T1,T2>**   p    = new TJLterm<T1,T2>* [dim];
 for( j = 0; j < dim; j++ ) p[j] = 0;


 // If zero maps to zero, return result .....................
 t1 = 1;    // Indicates reference point is the origin.
            // Test is one SpaceDim coordinates only.
 for( i = 0; i < dim; i++ ) 
   if( _myEnv->_refPoint[i] != 0.0 ) {
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
   return _epsInverse( _myEnv );
 }


 // Otherwise, create a new TJetEnvironment<T1,T2> .........................
 TJetEnvironment<T1,T2>* pje_new = new TJetEnvironment<T1,T2>( *_myEnv );
 TJetEnvironment<T1,T2>* pje;

 for( i = 0; i < dim; i++ ) pje_new->_refPoint[i] = comp[i].standardPart();

 // ... Check to see if it already exists
 slist_iterator g( TJet<T1,T2>::environments );
 char found = 0;

 while( pje = (TJetEnvironment<T1,T2>*) g() )
   if( *pje == *pje_new ) {
     found = 1;
     break;
   }

 if( found ) {
   delete pje_new;
   pje_new = pje;
 }
 else TJet<T1,T2>::environments.append( pje_new );


 // Construct an idempotent 
 // and compute its inverse. ..................................
 TMapping<T1,T2> z( dim, 0, _myEnv );
 z = *this;   // ??? Deep copy needed here.

 // ... Temporarily zero the reference point 
 for( i = 0; i < dim; i++ ) pje_new->_refPoint[i] = 0.0;
 z._myEnv = pje_new;
 for( j = 0; j < dim; j++ ) {
   if( !t2[j] ) { 
     p[j] = z.comp[j].get();
     z.comp[j].addTerm( new TJLterm<T1,T2>( z._myEnv->AllZeroes, complex_0, z._myEnv ) );
     // ??? This last line should not be necessary.
   }
 }

 if( !t1 ) 
   for( j = 0; j < dim; j++ )        // A little paranoia
     z.comp[j].setEnvTo( z._myEnv );  // never hurt.

 z = z._epsInverse( z._myEnv );


 // Make final adjustments before returning. .....................
 for( i = 0; i < dim; i++ ) 
   pje_new->_refPoint[i] = comp[i].standardPart();

 // ... A little test ...
 z.standardPart( ref );
 for( j = 0; j < dim; j++ ) 
   if( ref[j] != 0.0 ) {
    cout << "*** WARNING ***                                              \n"
            "*** WARNING *** TJet<T1,T2>::Inverse()                       \n"
            "*** WARNING *** ref[" << j << "] = " << ref[j] << "          \n"
            "*** WARNING ***                                              \n"
         << endl;
   }

 // ... Add the correct image point ...
 if( !t1 ) {
  for( j = 0; j < z.dim; j++ ) 
   z.comp[j].addTerm( new TJLterm<T1,T2>( z._myEnv->AllZeroes, 
                                   _myEnv->_refPoint[j], 
                                   z._myEnv
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

template<typename T1, typename T2>
TMapping<T1,T2> TMapping<T1,T2>::_epsInverse( const TJetEnvironment<T1,T2>* pje ) const 
{
 TMapping<T1,T2>  z( dim, 0, pje );
 TMapping<T1,T2>  id( "ident", pje );
 TMapping<T1,T2>  v( dim, 0, pje );
 int  i = 0;
 TMatrix<T1> M( dim, dim, complex_0 );
 static Complex CZero( 0., 0. );

 if( dim == _myEnv->_numVAr ) M = Jacobian().inverse();
 else                         M = Jacobian().Square().inverse();

 z = M*id;
 v = ( operator()(z) - id );
 while ( ( i++ < MX_MAXITER ) && ( ( v - v.filter(0,1)) != CZero ) ) {
               // This assumes linear is handled well enough
               // by the TMatrix<T1> routine.  
  z = z - M*v;
  v = ( operator()(z) - id );
 }

 if( i >= MX_MAXITER ) {
  cerr << "\n\n"
       << "*** WARNING ***                                            \n" 
          "*** WARNING *** TMapping<T1,T2>& TMapping<T1,T2>::_epsInverse()                     \n" 
          "*** WARNING *** Over " << MX_MAXITER << " iterations used. \n"
          "*** WARNING *** result may be incorrect.                   \n" 
          "*** WARNING ***                                            \n" 
       << endl;
 }

 return z;
}

