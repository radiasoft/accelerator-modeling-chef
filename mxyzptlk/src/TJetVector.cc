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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
TJetVector<std::complex<double> >::TJetVector(TJetVector<double> const& x)
: myEnv_(x.myEnv_) // Note: env implicit conversion
{ 
  comp_ = std::vector<TJet<std::complex<double > > >( x.comp_.begin(),  x.comp_.end() ); 
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
       << ", Weight = " << myEnv_->maxWeight()
	   << ", Max accurate weight = " << begin()->getAccuWgt() // FIXME !!!
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
       << ", Weight = " << myEnv_->maxWeight()
	   << ", Max accurate weight = " << begin()->getAccuWgt()   // FIXME !!
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
 TJetVector<double> TJetVector<double>::Rotate ( TJetVector<double> const& v, double theta ) const
{

  TJetVector<double> e = Unit();

  double c = cos( theta );
  double s = sin( theta );

  return ( c*v ) + ( s*( e^v) ) +
         ( ( ( 1.0 - c )*(e*v) )*e );
}


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<>
TJetVector<double> TJetVector<double>::Rotate( TJetVector<double> const& v, TJet<double> const & theta ) const
{

  TJet<double>       c = cos( theta );
  TJet<double>       s = sin( theta );
  TJetVector<double> e = Unit();

  return  ( c*v ) + ( s*( e^v) ) +
                    ( ( ( 1.0 - c )*(e*v) )*e );
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

