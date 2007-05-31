/*************************************************************************
**************************************************************************
**************************************************************************
******
******  Basic TOOLKIT:  Low level utility C++ classes.
******
******  File:      TMatrix.cc
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Leo Michelotti                                     
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******             Email: michelotti@fnal.gov                         
****** 
******  Revision (Sep 2005):
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov                                   
******             
******   - reorganized code to support explicit template instantiations
******   - eliminated separate MatrixC class implementation
****** 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>

#include <basic_toolkit/TML.h>
#include <basic_toolkit/MLPtr.h>
#include <basic_toolkit/TMatrix.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>


#ifdef WIN32
#include <Distribution.h> // for drand48
#endif


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
TMatrix<std::complex<double> >::TMatrix( TMatrix<double> const& m){ 

  ml_ = m.ml_; // implicit conversion

  return; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<double> TMatrix<double>::dagger() const {

//  for a real matrix, the Hermitian conjugate and the transpose
//  are the same

  
  MLPtr<double> p ( ml_->transpose() );
  return TMatrix<double>( p );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<std::complex<double> >::dagger() const
{

  MLPtr<std::complex<double> > p ( ml_->dagger() );
  return TMatrix<std::complex<double> >( p );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<double> real( const TMatrix<std::complex<double> >& x )
{


  MLPtr<double> p( real_part(x.ml_ ) ); 
  return TMatrix<double>( p ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<double> imag( const TMatrix<std::complex<double> >& x ) 
{

  MLPtr<double> p( imag_part(x.ml_ ) ); 
  return TMatrix<double>( p ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<double>::eigenValues() const
{
 
  MLPtr<std::complex<double> > p( ml_->eigenValues() ); 
  return  TMatrix<std::complex<double> >( p ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<std::complex<double> >::eigenValues() const 
{

  MLPtr<std::complex<double> > p( ml_->eigenValues() ); 
  return  TMatrix<std::complex<double> >( p ); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<double>::eigenVectors() const
{

  MLPtr<std::complex<double> > p( ml_->eigenVectors() ); 
  return  TMatrix<std::complex<double> >(p); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<std::complex<double> >::eigenVectors() const
{

  MLPtr<std::complex<double> > p( ml_->eigenVectors() ); 
  return  TMatrix<std::complex<double> >(p); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void TMatrix<double>::SVD( TMatrix<double>& U, Vector& W, TMatrix<double>& V) 
{
  ml_->SVD( U.ml_, W, V.ml_);

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<std::complex<double>  > operator*(const TMatrix<std::complex<double>  >& x, const TMatrix<double>& y)
{
 
  TMatrix<std::complex<double> > z = y; //implicit conversion 

  MLPtr<std::complex<double>  > p(  multiply<std::complex<double> >(x.ml_, z.ml_) ); 
  return TMatrix<std::complex<double>  >(p); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<std::complex<double> > operator*(const TMatrix<double>& x, const TMatrix<std::complex<double> >& y)
{

  TMatrix<std::complex<double> > z = x; //implicit conversion 

  MLPtr<std::complex<double>  > p(   multiply<std::complex<double> >(z.ml_, y.ml_) ); 
  return TMatrix<std::complex<double>  >( p ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

/*** static function ***/

template<>
Vector 
TMatrix<double>::backSubstitute(TMatrix<double> const& U, Vector const& W, TMatrix<double> const& V, Vector const& rhs, double threshold) { 

  return TML<double>::backSubstitute(U.ml_, W, V.ml_, rhs, threshold);
 
}


