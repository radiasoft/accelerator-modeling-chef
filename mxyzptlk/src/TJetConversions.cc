/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetConversions.cc
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab   
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
*****  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
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
******  Feb 2005 - Jean-Francois Ostiguy
*****              ostiguy@fnal.gov
******  
******  Efficiency improvements.
******  - new memory management 
******  
******  Sep 2005   ostiguy@fnal.gov
******   
******  -new code based on a single template parameter and 
******   conversion operators. 
******  
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <MathConstants.h>

#include <GenericException.h>
#include <complexAddon.h>
#include <VectorD.h>

#include <TJL.h>
#include <TJL.tcc>

#include <TJetEnvironment.h>
#include <TJetEnvironment.tcc>

#include <TJet.h>
#include <TJet.tcc>

#include <TJetVector.h>
#include <TJetVector.tcc>

#include <TLieOperator.h>
#include <TLieOperator.tcc>

#include <TMapping.h>
#include <TMapping.tcc>


#define CHECKOUT(test,fcn,message)    \
  if( test ) {                        \
    throw( GenericException(          \
             __FILE__, __LINE__,      \
             fcn, message        ) ); \
  }

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

using namespace std;
using FNAL::Complex;


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool EnvEquivalent( const TJetEnvironment<double>* x, 
                    const TJetEnvironment<complex<double> >* y ) 
{
  static int i;
  if( x->_numVar    != y->_numVar    ) return false;
  if( x->_spaceDim  != y->_spaceDim  ) return false;
  if( x->_maxWeight != y->_maxWeight ) return false;
  for( i = 0; i < x->_numVar; i++ ) {
    if( ( x->_refPoint[i] != real( y->_refPoint[i] ) ) ||
        ( imag( y->_refPoint[i] ) != 0.0  )
      )                                return false;
    if( x->_scale[i] != y->_scale[i] ) return false;
  }
  return true;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



bool EnvEquivalent( const TJetEnvironment<complex<double> >* x, 
                    const TJetEnvironment<double>* y ) {
  return EnvEquivalent( y, x );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJetEnvironment<double>::operator TJetEnvironment<complex<double> > () const {

  TJetEnvironment<complex<double> > z; 
   
  z._maxWeight   =  _maxWeight; 
  z._monomial    =  0;
  z._maxTerms    =  _maxTerms; 
  z._numVar      =  _numVar;
  z._spaceDim    =  _spaceDim;
  z._pbok        =  _pbok;  
  z._dof         =  _dof;  
  z._TJLmonomial =  0; 
  z._TJLmml      =  0; 
  z._offset.reconstruct( _offset );

  if( TJet<complex<double> >::workEnvironment() != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<double>::operator TJetEnvironment<complex<double> > ()"
           "Close open TJet<complex<double> > environment before copying." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<double>::operator TJetEnvironment<complex<double> > ()\n"
         << "*** WARNING *** ( const TJetEnvironment<double,complex<double> >& )              \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    z._monomial           = 0;
    z._TJLmonomial        = 0;
    z._TJLmml             = 0;
    z._maxTerms           = 0;
    z._exponent           = 0;
    z._pbok               = 0;
    z._numVar             = 0;
    z._maxWeight          = 0;
    z._spaceDim           = -1;
    z._dof                = 0;            
    z._refPoint           = 0;       
    z._scale              = 0;

    return z;
  }

  int w = z._maxWeight;
  int n = z._numVar;

  if( z._monomial ) { delete [] z._monomial; }
  z._monomial   = new std::complex<double>[ bcfRec( w + n, n ) ];

  if (z._exponent) delete  z._exponent;   z._exponent = new int[ n ];            
  z._pbok    = _pbok;
  z._dof     = _dof;

  if(z._refPoint) delete z._refPoint;  z._refPoint = new complex<double> [ n ];  
  if(z._scale)    delete z._scale;     z._scale    = new double[ n ];            

  z._allZeroes.Reconstruct( n );            

  for( int i = 0; i < n; i++ ) {
   z._refPoint[i]   = complex<double> ( _refPoint[i], 0.0 );
   z._scale[i]      = _scale[i];
   z._allZeroes(i)  = 0;   // ??? Redundant and unnecessary.
  }

  z._buildScratchPads();

  return z;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJetEnvironment<complex<double> >::operator TJetEnvironment<double> () const {
 
  TJetEnvironment<double> x;

  x._maxWeight   =  _maxWeight; 
  x._monomial    =  0;
  x._maxTerms    =  _maxTerms; 
  x._numVar      =  _numVar;
  x._spaceDim    =  _spaceDim;
  x._pbok        =  _pbok;  
  x._dof         =  _dof;  
  x._TJLmonomial =  0; 
  x._TJLmml      =  0; 
  x._offset.reconstruct(_offset); 


  for( int ii = 0; ii < _numVar; ii++ ) {
    if( imag( _refPoint[ii] ) != 0.0 ) {
    throw( TJL<complex<double> >::BadReference ( ii, imag( _refPoint[ii] ), 
                                __FILE__, __LINE__, 
      "TJetEnvironment<complex<double> >::operator TJetEnvironment<double> ()",
      "Cannot copy complex environment with non-zero imaginary part to real one." ) );
    }
  }

  if( TJet<double>::workEnvironment() != 0 ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJetEnvironment<complex<double> >::operator TJetEnvironment<double> ()",
           "Close the open Jet environment before copying." ) );
  }

  if( _numVar == 0 ) {
    cerr << "\n\n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** TJetEnvironment<complex<double> >::operator TJetEnvironment<double> () \n"
         << "*** WARNING ***                                          \n"
         << "*** WARNING *** You are copying a null environment.      \n"
         << "*** WARNING ***                                          \n"
         << endl;

    x._monomial           = 0;
    x._TJLmonomial        = 0;
    x._TJLmml             = 0;
    x._maxTerms           = 0;
    x._exponent           = 0;
    x._pbok               = 0;
    x._numVar             = 0;
    x._maxWeight          = 0;
    x._spaceDim           = -1;
    x._dof                = 0;            
    x._refPoint           = 0;       
    x._scale              = 0;

    return x;
  }

  int w = x._maxWeight;
  int n = x._numVar;

  if( x._monomial ) { delete [] x._monomial; }
  x._monomial   = new double[ bcfRec( w + n, n ) ];

  if(x._exponent) delete x._exponent; x._exponent   = new int[ n ];  
  x._pbok       = _pbok;
  x._dof        = _dof;

  if(x._refPoint) delete x._refPoint;  x._refPoint = new double[ n ]; 
  if(x._scale)    delete x._scale;     x._scale    = new double[ n ]; 

  x._allZeroes.Reconstruct( n ); // not needed ????

  for( int i = 0; i < n; i++ ) {
    x._refPoint[i]  = real( _refPoint[i] );
    x._scale[i]     = _scale[i];
    x._allZeroes(i) = 0;   // ??? Redundant and unnecessary.
  }


  x._buildScratchPads();

  return x;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJetEnvironment<complex<double> >* TJetEnvironment<complex<double> >::CreateEnvFrom( const TJetEnvironment<double>* x )
{
  // Check to see if an appropriate environment already exists

  slist_iterator g( TJetEnvironment<complex<double> >::_environments );
  
  TJetEnvironment<complex<double> >* pje;

  while( pje = (TJetEnvironment<complex<double> >*) g() ) {
    if( EnvEquivalent( x, pje ) ) return pje;
  }
 
  // If not, then create a new TJetEnvironment<complex<double> > ...........


  TJetEnvironment<complex<double> >* pje_new = new TJetEnvironment<complex<double> >( *x ); // conversion operator 

  TJetEnvironment<complex<double> >::_environments.append( pje_new );
 
  return pje_new;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJetEnvironment<double>* TJetEnvironment<double>::CreateEnvFrom( const TJetEnvironment<complex<double> >* z )
{
  // Check to see if an appropriate environment already exists
  slist_iterator g( TJetEnvironment<double>::_environments );

  TJetEnvironment<double>* pje;

  while( pje = ( TJetEnvironment<double>* ) g() ) {
    if( EnvEquivalent( pje, z ) ) return pje;
  }
 
  // If not, then create a new TJetEnvironment<double> ............

  TJetEnvironment<double>* pje_new = new TJetEnvironment<double>( *z );     // conversion operator
  TJetEnvironment<double>::_environments.append( pje_new  );
  return pje_new;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJetEnvironment<double>* TJetEnvironment<double>::CreateEnvFrom( const Vector& x, int order )
{
  // POSTCONDITIONS: The _lastEnv static pointer is not changed.
  //
  // Check to see if an appropriate environment already exists
  int n = x.Dim();
  int i = 0;
  slist_iterator g( TJetEnvironment<double>::_environments );
  TJetEnvironment<double>* pje;
  while( pje = (TJetEnvironment<double>*) g() ) {
    bool passed = ( ( order == pje->_maxWeight       ) &&
                    ( n == pje->_spaceDim            ) && 
                    ( pje->_spaceDim == pje->_numVar )    );
    i = 0;
    while( passed && ( i < n ) ) {
      if( std::abs(pje->_refPoint[i] - x(i)) > 1.0e-8 ) { passed = false; }
      i++;
    }
    if( passed ) { return pje; }
  }
 
  // If not, then create a new TJetEnvironment<double,complex<double> > ...........
  TJetEnvironment<double>* storedEnv = TJet<double>::_lastEnv;

  TJet<double>::BeginEnvironment( order );
  for( int i = 0; i < n; i++ ) {
    new Tcoord<double>( x(i) );
    // Unfortunately, these cannot be deleted because
    // the TJetEnvironment stores the original pointers, 
    // not the copies.
    // This is very bad!!
  }
  pje = TJet<double>::EndEnvironment();

  TJet<double>::_lastEnv = storedEnv;
  return pje;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJLterm<complex<double> >::operator TJLterm<double> () const {
 
  TJLterm<double> x;

  x._index   =  _index;
  x._weight  =  _weight; 
  x._value   =  real( _value );  // this should probably throw if imag != 0
  x._deleted =  _deleted; 


  return x; 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJLterm<double>::operator TJLterm<std::complex<double> > () const {
 

  TJLterm<std::complex<double> > z;

  z._index   =  _index;
  z._weight  =  _weight; 
  z._value   =  std::complex<double>( _value, 0.0 );  
  z._deleted =  _deleted; 


  return z; 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<double>::operator TJet<std::complex<double> >() const { 

  TJetEnvironment<complex<double> >* zenv = TJetEnvironment<complex<double> >::CreateEnvFrom(_jl->_myEnv);

  TJet<std::complex<double> >   z( zenv );
 
  TJL<std::complex<double> >::discardTJL(z._jl);

  z._jl = 0;

  TJL<std::complex<double> >*  zjl = 0;

  if ( TJL<std::complex<double> >::_thePool.empty() ) {

      zjl = new TJL<std::complex<double> >;

  } else {

      zjl = TJL<std::complex<double> >::_thePool.back(); TJL<std::complex<double> >::_thePool.pop_back(); 
  }

  if (zjl->_jltermStoreCapacity < _jl->_jltermStoreCapacity) { 
   
      TJLterm<std::complex<double> >::array_deallocate( zjl->_jltermStore );
      zjl->initStore( _jl->_jltermStoreCapacity);   
  
  };

  zjl->_count    = _jl->_count;   // needed by append function
  zjl->_weight   = _jl->_weight;  // needed by append function
  zjl->_accuWgt  = _jl->_accuWgt;
  zjl->_myEnv    = zenv;
  zjl->_rc       = 1;

 
  dlist_iterator getNext( _jl->_theList );

  TJLterm<double>* p                = 0;
  TJLterm<std::complex<double> >* q = 0;

  while ( p = (TJLterm<double>*) getNext() ) {
    q = new( zjl->storePtr() )  TJLterm<complex<double> > ( *p ); // conversion operator
    q->_deleted    = p->_deleted;// ??     
    q->_weight     = p->_weight; // ??     
    zjl->append(q);
  }
 
  zjl->_count    = _jl->_count;    // gets reset by append function
  zjl->_weight   = _jl->_weight;   // may get reset by append function
  zjl->_accuWgt  = _jl->_accuWgt;
  zjl->_myEnv    = zenv;
  zjl->_rc       = 1;


  z._jl = zjl; 

  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<std::complex<double> >::operator TJet<double> () const {


  TJetEnvironment<double>* xenv = TJetEnvironment<double>::CreateEnvFrom(_jl->_myEnv);

  TJet<double>   x( xenv );
  
  TJL<double>::discardTJL(x._jl);

  x._jl = 0;

  TJL<double>*  xjl = 0;

  if (TJL<double>::_thePool.empty() ) {

      xjl = new TJL<double>;

  } else {

      xjl = TJL<double>::_thePool.back(); TJL<double>::_thePool.pop_back(); 
  }

  if (xjl->_jltermStoreCapacity < _jl->_jltermStoreCapacity) { 
   
      TJLterm<double>::array_deallocate( xjl->_jltermStore );
      xjl->initStore(_jl->_jltermStoreCapacity);   
  
  };

  xjl->_count    = _jl->_count;   // needed by append function
  xjl->_weight   = _jl->_weight;  // needed by append function
  xjl->_accuWgt  = _jl->_accuWgt;
  xjl->_myEnv    = xenv;
  xjl->_rc       = 1;

 
  dlist_iterator getNext( _jl->_theList );

  TJLterm<std::complex<double> >* p = 0;
  TJLterm<double>*                q = 0;

  while ( p = (TJLterm<std::complex<double> >*) getNext() ) {
    q              = new ( xjl->storePtr() )  TJLterm<double> ( *p ); // conversion operator
    q->_deleted    = p->_deleted;// ??     
    q->_weight     = p->_weight; // ??     
    xjl->append(q);
  }
 
  xjl->_count    = _jl->_count;    // gets reset by append function
  xjl->_weight   = _jl->_weight;   // may get reset by append function
  xjl->_accuWgt  = _jl->_accuWgt;
  xjl->_myEnv    = xenv;
  xjl->_rc       = 1;

  x._jl = xjl; 

  return x; 
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<double> real( const TJet<complex<double> >& x ) 
{

  TJet<double>                z;
  TJLterm<complex<double> >*  p;
  TJLterm<double>*            q;
  TJL<complex<double> >*      xPtr;
  TJL<double>*                zPtr;
 
  TJetEnvironment<double>* pje_new 
    = TJetEnvironment<double>::CreateEnvFrom( (TJetEnvironment<complex<double> >*) x.Env() );

  // Proceed ...
  xPtr = x._jl;
  zPtr = z._jl;
 
  dlist_iterator gx( xPtr->_theList );


  // If the argument is void, then return ...
  if( xPtr->_count < 1 ) {
   z = 0.0;
   return z;
  }
 
  // .. otherwise, continue normal operations.
  while((  p = (TJLterm<complex<double> >*) gx()  )) {
   q = new( zPtr->storePtr() ) TJLterm<double>( p->_index, real( p->_value ), pje_new );
   zPtr->addTerm( q );
  }
 
  // Set the maximum accurate _weight.
  zPtr->_accuWgt = xPtr->_accuWgt;
  
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> imag( const TJet<complex<double> >& x ) 
{

  TJet<double>                z;
  TJLterm<complex<double> >*  p;
  TJLterm<double>*            q;
  TJL<complex<double> >*      xPtr;
  TJL<double>*                zPtr;
 
  TJetEnvironment<double>* pje_new = TJetEnvironment<double>::CreateEnvFrom( x->_myEnv );

  // Proceed ...
  xPtr = x._jl;
  zPtr = z._jl;
 
  dlist_iterator gx( xPtr->_theList );


  // If the argument is void, then return ...
  if( xPtr->_count < 1 ) {
   z = 0.0;
   return z;
  }
 
  // .. otherwise, continue normal operations.
  while((  p = (TJLterm<complex<double> >*) gx()  )) {
   q = new ( zPtr->storePtr() ) TJLterm<double>( p->_index, imag( p->_value ), pje_new );
   zPtr->addTerm( q );
  }
 
  // Set the maximum accurate _weight.
  zPtr->_accuWgt = xPtr->_accuWgt;
  
  return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// ||| Mixed arithmetic
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TJet<complex<double> > operator+( const TJet<double>& x, const TJet<complex<double> >& y ) 
{
  return TJet<std::complex<double> >(x) + y;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return  x + TJet<std::complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( const double& x, const TJet<complex<double> >& y ) 
{
  return std::complex<double>(x,0.0) + y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+( const TJet<complex<double> >& x, const double& y ) 
{
  return x + complex<double>(y,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+(const std::complex<double>& x,  const TJet<double>& y ) 
{
  return ( x + TJet<std::complex<double> >(y) );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator+(  const TJet<double>& x, const std::complex<double>& y) 
{
  return ( TJet<std::complex<double> >(x) + y );

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<double>& x, const TJet<complex<double> >& y ) 
{
  return TJet<complex<double> >(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return x - TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const double& x, const TJet<complex<double> >& y ) 
{
  return complex<double>(x,0.0) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<complex<double> >& x, const double& y ) 
{
  return x - complex<double> (y,0.0);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const std::complex<double>& x, const TJet<double>& y ) 
{
  return x - TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator-( const TJet<double>& x,   const std::complex<double>& y)
{
  return TJet<complex<double> >(x) - y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<double>& x, const TJet<complex<double> >& y ) 
{
  return TJet<complex<double> >(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return x*TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const complex<double>& y, const TJet<double>& x ) 
{  
  return y*TJet<complex<double> >(x);
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<double>& x, const complex<double> & y ) 
{  
  return TJet<complex<double> >(x)*y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const double& y, const TJet<complex<double> >& x ) 
{  
  return (complex<double>(y) * x);
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator*( const TJet<complex<double> >& x, const double& y ) 
{  
  return ( x * complex<double>(y) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const TJet<double>& x, const TJet<complex<double> >& y ) 
{

  return TJet<complex<double> >(x)/y;

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const TJet<complex<double> >& x, const TJet<double>& y ) 
{
  return x/TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const complex<double> & x, const TJet<double>& y ) 
{
  return x/TJet<complex<double> >(y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/( const TJet<double>& y, const complex<double> & x ) 
{
  return TJet<complex<double> >(y)/x;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/(const TJet<complex<double> >& x, const double& y)
{
  return x/complex<double> (y);
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > operator/(const double& x, const TJet<complex<double> >& y)
{
  return complex<double> (x)/y;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> fabs( const TJet<double>& x )
{
 double u;

 if( x->_count == 0 ) {
   throw( GenericException( __FILE__, __LINE__, 
          "Jet fabs( const Jet& ) { ",
          "Argument is zero." ) );
 }
 
 if( (u = x.standardPart()) != 0.0 ) 
 {
   if( u > 0.0 ) return x;
   else          return -x;
 }
 else
 {
   throw( GenericException( __FILE__, __LINE__, 
          "Jet fabs( const Jet& ) ",
          "Argument is nilpotent." ) );
 }
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<double> erf( const TJet<double>& z ) 
{
  TJetEnvironment<double>* pje = z.Env();

  TJet<double> series    ( pje );
  TJet<double> oldseries ( pje );
  TJet<double> arg       ( pje );
  TJet<double> term      ( pje );

  static double  den;
  static double  fctr_x;
  static int     counter;

  series        = 1.0;
  oldseries     = 0.0;
  arg           = - z*z;
  den           = 1.0;
  term          = 1.0;
  fctr_x        = 0.0;

  counter = 0;
  while( ( series != oldseries ) || counter++ < pje->_maxWeight ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return (2.0/MATH_SQRTPI)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > erf( const TJet<complex<double> >& z ) 
{
  TJetEnvironment<complex<double> >* pje = z.Env();

  if( ( fabs(imag(z.standardPart())) > 3.9 ) || 
      ( fabs(real(z.standardPart())) > 3.0 ) ) {
    TJet<complex<double> > u( pje );
    u = complex<double> ( 0., 1. )*z;
    u = complex<double> ( 1., 0. ) - exp(u*u)*w(u);
    return u;
  }

  TJet<complex<double> >    series    ( pje );
  TJet<complex<double> >    oldseries ( pje );
  TJet<complex<double> >    arg       ( pje );
  TJet<complex<double> >    term      ( pje );

  static double  den;
  static double  fctr_x;
  static int     counter;

  series        = complex_1;
  oldseries     = complex_0;  // ??? Why necessary?
   arg           = - z*z;
  den           = 1.0;
  term          = complex_1;
  fctr_x        = 0.0;

  counter = 0;
  while( ( series != oldseries ) || counter++ < pje->_maxWeight ) {
    oldseries = series;
    den      += 2.0;
    fctr_x   += 1.0;
    term     *= arg/fctr_x;
    series   += term/den;
  }  

  return complex<double> (2.0/MATH_SQRTPI,0.0)*z*series;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJet<complex<double> > w( const TJet<complex<double> >& z ) 
{
  const complex<double>  mi( 0., -1. );
  double x;
  double y;
  TJet<complex<double> >  answer( z.Env() );
  
  x = real( z.standardPart() );
  y = imag( z.standardPart() );

  if( ( x < 0.0 ) || ( y < 0.0  ) ) {
    throw( GenericException( __FILE__, __LINE__, 
           "TJet<complex<double> > w( const TJet<complex<double> >& ) ",
           "Argument must have positive standard part." ) );
  }

  if( ( x > 6.0 ) || ( y > 6.0 ) ) 
    answer = ( - mi * z * (
                          ( 0.5124242  /( z*z - 0.2752551 )) + 
                          ( 0.05176536 /( z*z - 2.724745  ))
                          ) 
             );

  else if( ( x > 3.9 ) || ( y > 3.0 ) ) 
    answer = ( - mi * z * (
                          ( 0.4613135   /( z*z - 0.1901635 )) + 
                          ( 0.09999216  /( z*z - 1.7844927 )) + 
                          ( 0.002883894 /( z*z - 5.5253437 ))
                          ) 
             );

  else answer = exp( -z*z )*(  1.0 -erf( mi*z ) );

  return answer;
}



// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TJetVector<complex<double> > operator*( const TJetVector<complex<double> >& x, const double& c )
{
  return x.operator*( complex<double> (c,0.0) );
}

TJetVector<complex<double> > operator*( const double& c, const TJetVector<complex<double> >& x )
{
  return x.operator*( complex<double> (c,0.0) );  // it's commutative
}

void TJetVector<double>::printCoeffs() const
{
  int i;
  cout << "\n\nBegin TJetVector<double>::printCoeffs() ......\n"
       << "Dimension: " << _dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << endl;
  cout << "JetVector reference point: " 
       << endl;
  for( i = 0; i < _myEnv->_numVar; i++ ) 
    cout << setw(20) << setprecision(12) 
         << _myEnv->_refPoint[i]
         << "\n" << endl;

  for ( i = 0; i < _dim; i++ ) {
    cout << "TJetVector<double>::printCoeffs(): Component " << i << endl;
    _comp[i].printCoeffs();
  }
  cout << "End TJetVector<double>::printCoeffs() ......\n" << endl;
}


void TJetVector<complex<double> >::printCoeffs() const
{
  int i;
  cout << "\n\nBegin TJetVector<complex<double> >::printCoeffs() ......\n"
       << "Dimension: " << _dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << endl;
  cout << "TJetVector<complex<double> > reference point: " 
       << endl;
  for( i = 0; i < _myEnv->_numVar; i++ ) 
    cout << setw(20) << setprecision(12) 
         << real( _myEnv->_refPoint[i] )
         << " + i"
         << setw(20) << setprecision(12) 
         << imag( _myEnv->_refPoint[i] )
         << "\n" << endl;

  for ( i = 0; i < _dim; i++ ) {
    cout << "TJetVector<complex<double> >::printCoeffs(): Component " << i << endl;
    _comp[i].printCoeffs();
  }
  cout << "End TJetVector<complex<double> >::printCoeffs() ......\n" << endl;
}


void TJetVector<double>::Rotate ( TJetVector<double>& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( v._dim != 3 ),
           "TJetVector<double>::Rotate",
           "Dimension must be 3." )
#endif

  double c, s;
  TJetVector<double> e( 3, 0, _myEnv ), u( 3, 0, _myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v._comp[i] = u._comp[i];
}


void TJetVector<complex<double> >::Rotate ( TJetVector<complex<double> >&, double ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, double ) const", 
         "Function does not exist." ) );
}


void TJetVector<double>::Rotate ( TJetVector<double>& v, 
                                 const TJet<double>& theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((_dim != 3) || ( v._dim != 3 ),
           "TJetVector<double,complex<double> >::Rotate",
           "Dimension must be 3." )
  CHECKOUT((_myEnv != v._myEnv)||(_myEnv != theta.Env()), 
           "TJetVector<double,complex<double> >::Rotate", 
           "Incompatible environments.")
#endif

  TJet<double>       c( _myEnv ), s( _myEnv );
  TJetVector<double> e( 3, 0, _myEnv), u( 3, 0, _myEnv);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v._comp[i] = u._comp[i];
}


void TJetVector<complex<double> >::Rotate ( TJetVector<complex<double> >&, 
                                                const TJet<complex<double> >& ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, const JetC& ) const", 
         "Function does not exist." ) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector TMapping<double>::operator()( const Vector& x ) const
{
 int i = x.Dim();
 if( ( i != _myEnv->_numVar ) || ( i != _dim ) ) {
   throw( GenericException(__FILE__, __LINE__, 
          "Vector TMapping<double>::operator()( const Vector& ) const",
          "Incompatible dimensions." ) );
 }

 Vector z( _dim );

 for( i = 0; i < _myEnv->_spaceDim; i++) {
  z(i) = _comp[i]( x );
 }

 return z;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector TMapping<complex<double> >::operator()( const Vector& x ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "Vector TMapping<complex<double> >::operator()( const Vector& x ) const", 
         "This specialization is meaningless. It should not be invoked." ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMapping<double>::operator TMapping<std::complex<double> > () const {

  TMapping<std::complex<double> > z;

  z._dim   = _dim;
  z._myEnv = TJetEnvironment<std::complex<double> >::CreateEnvFrom(_myEnv);

  for (int i =0; i< _dim; ++i ) {

    z._comp[i] = TJet<complex<double> >( _comp[i] ); // the environment is converted by the conversion operator
    
  }

  return z;

}


