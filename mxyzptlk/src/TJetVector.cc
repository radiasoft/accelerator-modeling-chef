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


#include <iosfwd>
#include <complex>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/MathConstants.h>

#include <mxyzptlk/TJetEnvironment.h>
#include <mxyzptlk/TJetVector.h>

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
TJetVector<std::complex<double> >::TJetVector(TJetVector<double> const& x)
: myEnv_(x.myEnv_) // Note: env implicit conversion
{ 
  comp_ = std::vector<TJet<std::complex<double > > >( x.comp_.begin(),  x.comp_.end() ); 
  CHECKOUT(  comp_[0].Env() != myEnv_ , "TJetVector<std::complex<double> >::const TJetVector<double>& x", "Incompatible environments.")
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
       << "Dimension: " << comp_.size() 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << std::endl;
  (*pcout) << "JetVector reference point: " 
       << std::endl;
  for( i = 0; i < myEnv_->numVar(); i++ ) 
    (*pcout) << std::setw(20) << setprecision(12) 
         << myEnv_->refPoint()[i]
         << "\n" << std::endl;

  for ( i = 0; i < comp_.size(); i++ ) {
    (*pcout) << "TJetVector<double>::printCoeffs(): Component " << i << std::endl;
    comp_[i].printCoeffs();
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
       << "Dimension: " << comp_.size() 
       << ", Weight = " << Weight()
       << ", Max accurate weight = " << AccuWgt() 
       << std::endl;
  (*pcout) << "TJetVector<complex<double> > reference point: " 
       << std::endl;
  for( i = 0; i < myEnv_->numVar(); i++ ) 
    (*pcout) << std::setw(20) << setprecision(12) 
         << real( myEnv_->refPoint()[i] )
         << " + i"
         << std::setw(20) << setprecision(12) 
         << imag( myEnv_->refPoint()[i] )
         << "\n" << std::endl;

  for ( i = 0; i < comp_.size(); i++ ) {
    (*pcout) << "TJetVector<complex<double> >::printCoeffs(): Component " << i << std::endl;
    comp_[i].printCoeffs();
  }
  (*pcout) << "End TJetVector<complex<double> >::printCoeffs() ......\n" << std::endl;
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
void TJetVector<double>::Rotate ( TJetVector<double>& v, double theta ) const
{
#ifndef NOCHECKS
  CHECKOUT((comp_.size() != 3) || ( v.comp_.size() != 3 ),
           "TJetVector<double>::Rotate",
           "Dimension must be 3." )
#endif

  double c, s;
  TJetVector<double> e( TVector<double>(3), myEnv_ ), u( Vector(3), myEnv_);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v.comp_[i] = u.comp_[i];
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
  CHECKOUT((comp_.size() != 3) || ( v.comp_.size() != 3 ),
           "TJetVector<double,complex<double> >::Rotate",
           "Dimension must be 3." )
  CHECKOUT((myEnv_ != v.myEnv_)||(myEnv_ != theta.Env()), 
           "TJetVector<double,complex<double> >::Rotate", 
           "Incompatible environments.")
#endif

  TJet<double>       c( myEnv_ ), s( myEnv_ );
  TJetVector<double> e( Vector(3), myEnv_), u( Vector(3), myEnv_);

  e = Unit();
  c = cos( theta );
  s = sin( theta );
  u = ( c*v ) +
      ( s*( e^v) ) +
      ( ( ( 1.0 - c )*(e*v) )*e );
  for ( int i = 0; i < 3; i++ ) v.comp_[i] = u.comp_[i];
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
TJet<double> TJetVector<double>::operator* ( TJetVector<double> const& x ) const
{

  TJet<double> u( myEnv_);

  for ( int i=0; i < comp_.size(); ++i) {
      u += ( comp_[i] * x.comp_[i] );
  }
  return u;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
TJet<std::complex<double> > TJetVector<std::complex<double> >::operator* ( TJetVector<std::complex<double> > const& x ) const
{

  TJet<std::complex<double> > u( myEnv_);

  for ( int i=0; i < comp_.size(); ++ i) {
      u += ( comp_[i] * (real( x.comp_[i]) - imag(x.comp_[i]))  );
  }
  return u;
}

