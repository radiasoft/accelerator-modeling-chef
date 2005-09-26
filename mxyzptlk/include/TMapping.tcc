/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      TMapping.cc
******  Version:   1.0
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
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
******  Revision History:
******
******  May 2005 - Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******
******            -Efficiency improvements. 
******            - new memory management
******
******  Sept 2005   ostiguy@fnal.gov
******
******            - new code based on a single template parameter
******              instead of two. Mixed mode now handled
******              using conversion operators.
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TMAPPING_TCC
#define TMAPPING_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <GenericException.h>
#include <iosetup.h>
#include <TMapping.h>
#include <TJL.h>

using namespace std;
using FNAL::pcout;
using FNAL::pcerr;

#ifndef MX_MAXITER
#define MX_MAXITER  100     // Maximum number of iterations allowed
                            //   in iterative routines
#endif  // MX_MAXITER


//    Globals |||||||||||||||||||||||||||||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

using namespace std;

//    Constructors and destructors    |||||||||||||||||||||||||||
//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( int n,
                                  const TJet<T>* pj,
                                  TJetEnvironment<T>* pje )
: TJetVector<T>( n, pj, pje )
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( const TMapping<T>& x ) 
: TJetVector<T>( (const TJetVector<T>&) x ) 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( const TJetVector<T>& x ) 
: TJetVector<T>( x ) 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::TMapping<T>( const char*, TJetEnvironment<T>* pje  ) 
: TJetVector<T>( pje->_spaceDim, 0, pje )
{
 int i, s;
 
 if( pje->_spaceDim == 0 ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T>::TMapping<T>( char*, TJetEnvironment<T>* ) ",
          "Phase space has dimension 0." ) );
 }
 
 s = pje->_spaceDim;
(this->_myEnv) = pje;

 for( i = 0; i < s; i++ ) (this->_comp)[i].setVariable( i, pje );
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T>::~TMapping<T>() 
{
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::operator()( const TMapping<T>& x ) const
{
 if( x._dim != (this->_myEnv)->_numVar ) {
   throw( GenericException(__FILE__, __LINE__, 
          "TMapping<T> TMapping<T>::operator()( const TMapping<T>& ) const",
          "Incompatible dimensions." ) );
 }

 int i;
 TMapping<T> z( (this->_dim), 0, x._myEnv );

 for( i = 0; i < (this->_myEnv)->_spaceDim; i++) {
  z._comp[i] = (this->_comp)[i]( x );
 }

   return z;
}

//    |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::operator*( const TMapping<T>& x ) const
{
  return operator()( x );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void operator*=( TMapping<T>& x, const TMapping<T>& y ) {
 x = x*y;
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMatrix<T> TMapping<T>::Jacobian() const 
{
 int            i, j;
 int            nv = (this->_myEnv)->_numVar;   // ??? Is this right?
 int*           d = new int[ nv ];
 TMatrix<T>    M( (this->_dim), nv, ((T) 0.0) );

 for( i = 0; i < nv; i++  ) d[i] = 0;
 for( j = 0; j < nv; j++ ) {
  d[j] = 1;
  for( i = 0; i < (this->_dim); i++ ) M( i, j ) = (this->_comp)[i].derivative( d );
  d[j] = 0;
 }

 delete [] d;
 return M;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::Inverse() const 
{ 
 if( (this->_myEnv)->_spaceDim != (this->_dim) ) {
  throw( typename TJL<T>::BadDimension( (this->_myEnv)->_spaceDim, (this->_dim), __FILE__, __LINE__, 
         "Mapping Mapping::Inverse() const ",
         "Phase space dimensions do not match." ) );
 }

 int        i, j;
 int        nv   = (this->_myEnv)->_numVar;
 char       t1;
 char*      t2   = new char    [(this->_dim)];
 char       t3;
 T* ref  = new T[nv];
 TJLterm<T>**   p    = new TJLterm<T>* [(this->_dim)];
 for( j = 0; j < (this->_dim); j++ ) p[j] = 0;


 // If zero maps to zero, return result .....................
 t1 = 1;    // Indicates reference point is the origin.
            // Test is one _spaceDim coordinates only.
 for( i = 0; i < (this->_dim); i++ ) 
   if( (this->_myEnv)->_refPoint[i] != 0.0 ) {
     t1 = 0;
     break;
   }

 for( j = 0; j < (this->_dim); j++ )
   t2[j] = ( (this->_comp)[j].standardPart() == 0.0 );

 t3 = t1;
 if( t3 ) for( j = 0; j < (this->_dim); j++ ) t3 &= t2[j];

 if( t3 ) {
   delete [] t2;
   delete [] ref;
   delete [] p;
   return _epsInverse( (this->_myEnv) );
 }


 // Otherwise, create a new TJetEnvironment<T> .........................
 TJetEnvironment<T>* pje_new = new TJetEnvironment<T>( *(this->_myEnv) );
 TJetEnvironment<T>* pje;

 for( i = 0; i < (this->_dim); i++ ) pje_new->_refPoint[i] = (this->_comp)[i].standardPart();

 // ... Check to see if it already exists
 slist_iterator g( TJet<T>::_environments );
 char found = 0;

 while( pje = (TJetEnvironment<T>*) g() )
   if( *pje == *pje_new ) {
     found = 1;
     break;
   }

 if( found ) {
   delete pje_new;
   pje_new = pje;
 }
 else TJet<T>::_environments.append( pje_new );


 // Construct an idempotent 
 // and compute its inverse. ..................................
 TMapping<T> z( (this->_dim), 0, (this->_myEnv) );
 z = *this;   // ??? Deep copy needed here.

 // ... Temporarily zero the reference point 
 for( i = 0; i < (this->_dim); i++ ) pje_new->_refPoint[i] = 0.0;
 z._myEnv = pje_new;

 for( j = 0; j < (this->_dim); j++ ) {
   if( !t2[j] ) { 
     p[j] = z._comp[j].get();
     z._comp[j].addTerm( new( z._comp[j].storePtr() ) TJLterm<T>( z._myEnv->_allZeroes, 0.0, z._myEnv ) );
     // ??? This last line should not be necessary.
   }
 }

 if( !t1 ) 
   for( j = 0; j < (this->_dim); j++ )        // A little paranoia
     z._comp[j].setEnvTo( z._myEnv );  // never hurt.

 z = z._epsInverse( z._myEnv );


 // Make final adjustments before returning. .....................
 for( i = 0; i < (this->_dim); i++ ) 
   pje_new->_refPoint[i] = (this->_comp)[i].standardPart();

 // ... A little test ...
 z.standardPart( ref );
 for( j = 0; j < (this->_dim); j++ ) 
   if( ref[j] != 0.0 ) {
    (*pcout) << "*** WARNING ***                                              \n"
            "*** WARNING *** TJet<T>::Inverse()                       \n"
            "*** WARNING *** ref[" << j << "] = " << ref[j] << "          \n"
            "*** WARNING ***                                              \n"
         << endl;
   }

 // ... Add the correct image point ...

 if( !t1 ) {
  for( j = 0; j < z._dim; j++ ) 
   z._comp[j].addTerm( new ( z._comp[j].storePtr() ) TJLterm<T>( z._myEnv->_allZeroes, 
                                   (this->_myEnv)->_refPoint[j], 
                                   z._myEnv
                                 ) );
 }


 // Clean up and leave
 delete [] t2;
 delete [] ref;

 for( j = 0; j < (this->_dim); j++ )  if ( p[j] )  p[j]->_deleted = true; // p[j] seems to be 0 sometimes
                                                                       // Should this check be necessary ?  
                                                                                 
 delete [] p;

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TMapping<T> TMapping<T>::_epsInverse( TJetEnvironment<T>* pje ) const 
{
 TMapping<T>  z( (this->_dim), 0, pje );
 TMapping<T>  id( "ident", pje );
 TMapping<T>  v( (this->_dim), 0, pje );
 int  i = 0;
 TMatrix<T> M( (this->_dim), (this->_dim), 0.0 );

 if( (this->_dim) == (this->_myEnv)->_numVar ) M = Jacobian().inverse();
 else                         M = Jacobian().Square().inverse();

 z = M*id;
 v = ( operator()(z) - id );
 while ( ( i++ < MX_MAXITER ) && ( ( v - v.filter(0,1)) != ((T) 0.0) ) ) {
               // This assumes linear is handled well enough
               // by the TMatrix<T> routine.  
  z = z - M*v;
  v = ( operator()(z) - id );
 }

 if( i >= MX_MAXITER ) {
  (*pcerr) << "\n\n"
       << "*** WARNING ***                                            \n" 
          "*** WARNING *** TMapping<T>& TMapping<T>::_epsInverse()                     \n" 
          "*** WARNING *** Over " << MX_MAXITER << " iterations used. \n"
          "*** WARNING *** result may be incorrect.                   \n" 
          "*** WARNING ***                                            \n" 
       << endl;
 }

 return z;
}


#endif // TMAPPING_TCC
