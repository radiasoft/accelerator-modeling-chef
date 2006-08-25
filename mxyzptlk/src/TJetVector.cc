/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      TJetVector.cc
******                                                                
******  Copyright Universities Research Association, Inc. / Fermilab   
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
*****   and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
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

#include <iosfwd>
#include <complex>
#include <iosetup.h>
#include <GenericException.h>
#include <MathConstants.h>

#include <TJetEnvironment.h>
#include <TJetVector.h>

typedef TJet<std::complex<double> > JetC;

using std::complex;
using std::setprecision;

using FNAL::pcerr;
using FNAL::pcout;

#define CHECKOUT(test,fcn,message)    \
  if( test ) {                        \
    throw( GenericException(          \
             __FILE__, __LINE__,      \
             fcn, message        ) ); \
  }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 template<>
 template<>
 TJetVector<std::complex<double> >::TJetVector(TJetVector<double> const& x):
 _dim(x._dim), _myEnv(x._myEnv) // Note: env implicit conversion
{ 

  
  _comp  = new TJet<std::complex<double> > [ _dim ];

  for ( int i=0; i< _dim; ++i) {

    _comp[i] = x._comp[i];     // implicit type conversion

    CHECKOUT(  _comp[i].Env() != _myEnv , "TJetVector<std::complex<double> >::const TJetVector<double>& x", "Incompatible environments.")

  }

}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
TJetVector<complex<double> > operator*( const TJetVector<complex<double> >& x, const double& c )
{
  return x.operator*( complex<double> (c,0.0) );
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TJetVector<complex<double> > operator*( const double& c, const TJetVector<complex<double> >& x )
{
  return x.operator*( complex<double> (c,0.0) );  

}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
void TJetVector<double>::printCoeffs() const
{
  int i;
  (*pcout) << "\n\nBegin TJetVector<double>::printCoeffs() ......\n"
       << "Dimension: " << _dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << std::endl;
  (*pcout) << "JetVector reference point: " 
       << std::endl;
  for( i = 0; i < _myEnv->numVar(); i++ ) 
    (*pcout) << std::setw(20) << setprecision(12) 
         << _myEnv->refPoint()[i]
         << "\n" << std::endl;

  for ( i = 0; i < _dim; i++ ) {
    (*pcout) << "TJetVector<double>::printCoeffs(): Component " << i << std::endl;
    _comp[i].printCoeffs();
  }
  (*pcout) << "End TJetVector<double>::printCoeffs() ......\n" << std::endl;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
void TJetVector<complex<double> >::printCoeffs() const
{
  int i;
  (*pcout) << "\n\nBegin TJetVector<complex<double> >::printCoeffs() ......\n"
       << "Dimension: " << _dim 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << std::endl;
  (*pcout) << "TJetVector<complex<double> > reference point: " 
       << std::endl;
  for( i = 0; i < _myEnv->numVar(); i++ ) 
    (*pcout) << std::setw(20) << setprecision(12) 
         << real( _myEnv->refPoint()[i] )
         << " + i"
         << std::setw(20) << setprecision(12) 
         << imag( _myEnv->refPoint()[i] )
         << "\n" << std::endl;

  for ( i = 0; i < _dim; i++ ) {
    (*pcout) << "TJetVector<complex<double> >::printCoeffs(): Component " << i << std::endl;
    _comp[i].printCoeffs();
  }
  (*pcout) << "End TJetVector<complex<double> >::printCoeffs() ......\n" << std::endl;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
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


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void TJetVector<complex<double> >::Rotate ( TJetVector<complex<double> >&, double ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, double ) const", 
         "Function does not exist." ) );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
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


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void TJetVector<complex<double> >::Rotate ( TJetVector<complex<double> >&, 
                                                const TJet<complex<double> >& ) const
{
  throw( GenericException( __FILE__, __LINE__, 
         "void JetCVectorRotate ( JetCVector&, const JetC& ) const", 
         "Function does not exist." ) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TJet<double> TJetVector<double>::operator* ( const TJetVector<double>& x ) const
{

  TJet<double> u( _myEnv );
  u = 0.0;
  int i;
  for ( i = 0; i < _dim; i++ ) u += _comp[i] * x._comp[i];
  return u;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TJet<std::complex<double> > TJetVector<std::complex<double> >::operator* ( const TJetVector<std::complex<double> >& x ) const
{

  TJet<std::complex<double> > u( _myEnv );
  u = std::complex<double>(0.0, 0.0);
  int i;
  for ( i = 0; i < _dim; i++ ) u += ( _comp[i] * (real( x._comp[i]) - imag(x._comp[i]))  );
  return u;
}


