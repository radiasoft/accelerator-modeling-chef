/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TVector.tcc
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
******  Copyright  Universities Research Association, Inc.    
******             All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Revision History:
******
******  Dec 2005   Jean-Francois Ostiguy
******             ostiguy@fnal.gov
******  - misc optimizations
******  Mar 2006  ostiguy@fnal.gov 
******  - templated version
******  Oct 2006   ostiguy@fnal.gov
******  - completely new implementation based on std::vector<>
******  Mar 2007 ostiguy@fnal.gov 
******  - exposed iterator interface
******  Aug 2008 ostiguy@fnal.gov
******  - added support for outer product operator
******                                                               
**************************************************************************
*************************************************************************/

#include <stdlib.h>

#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/GenericException.h>

#include <iomanip>
#include <vector>
#include <algorithm>
#include <functional>
 
using namespace std;


// ================================================================
//      Globals
//

template<typename T>
OutputFormat* TVector<T>::defOFPtr_ = 0;

template<typename T>
void TVector<T>::setDefaultFormat( OutputFormat const& x )
{
  if( ( (OutputFormat*) 0 ) != TVector<T>::defOFPtr_ ) {
    delete TVector<T>::defOFPtr_;
  }
  defOFPtr_ = new OutputFormat( x );
}

// ================================================================
//      Exception handlers
//

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ================================================================
//      Constructors and the destructor ...
//

template<typename T> 
TVector<T>::TVector()
 : theVector_(), ofPtr_(0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
TVector<T>::TVector( int dimension, T value )
 : theVector_( dimension, value ), ofPtr_(0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
TVector<T>::TVector( TVector const& x )
 : theVector_( x.theVector_), ofPtr_( x.ofPtr_ ) 
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
template <typename Iterator_t> 
TVector<T>::TVector( Iterator_t first,  Iterator_t last)
 : theVector_(first, last), ofPtr_(0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>::TVector( TMatrix<T> const& x )
: ofPtr_(0)
{


  if( std::min( x.rows(), x.cols() ) != 1 ) {

  throw(  GenericException( __FILE__, __LINE__, "TVector& TVector( TMatrix const & x )", "Incompatible dimensions."  ) );
 
  }

  if( x.rows() == 1 ) {
    for (int i=0; i < x.cols(); ++i) { 
         theVector_.push_back( x[0][i] );
    }
    return;
  }
 
  for (int i=0; i < x.rows(); ++i) {
         theVector_.push_back( x[i][0] );
  }
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
void TVector<T>::clear()
{
  theVector_.clear();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>::TVector::~TVector()
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Algebraic functions ...
template<typename T>
TVector<T>& TVector<T>::operator= ( TVector const& x )
{
   if( this == &x )  return *this;
  
    theVector_ = x.theVector_;
    ofPtr_     = x.ofPtr_;

    return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator+ ( TVector<T> const& x ) const
{

  TVector z(x);
  std::transform( begin(), end(),  x.begin(),  z.begin(),  plus<T>() ); 
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>& TVector<T>::operator+= ( TVector const& x )
{
 std::transform( begin(), end(),  x.begin(),  begin(), plus<T>() ); 
 return (*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TVector<T> TVector<T>::operator- ( TVector const& x ) const
{

 TVector z( Dim() ); 
 std::transform( begin(), end(),  x.begin(),  z.begin(), minus<T>() ); 
 return z;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>& TVector<T>::operator-= ( TVector<T> const& x )
{
 
  std::transform( begin(), end(),  x.begin(),  begin(), minus<T>() ); 
  return (*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> operator- ( TVector<T> const& x ) // unary minus
{
  TVector<T> z( x );
  std::transform( z.begin(), z.end(), z.begin(), std::negate<T>() ); 
  return z;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>  operator* ( TVector<T> const& x,  T const& c ) 
{
  TVector<T> z( x );
  std::transform( z.begin(), z.end(), z.begin(), std::bind2nd( std::multiplies<T>(), c ) ); 
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> operator* ( T const& c, TVector<T> const& x )
{
  return x*c;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>&  TVector<T>::operator*=( T const& c ) 
{
 std::transform( begin(), end(), begin(), std::bind2nd( std::multiplies<T>(), c) ); 
 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>  TVector<T>::operator/ ( T const& c ) const
{
 TVector z( *this );
 std::transform( z.begin(), z.end(), z.begin(), std::bind2nd( std::divides<T>(), c) ); 
 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>& TVector<T>::operator/=( T const& c ) 
{
  std::transform( begin(), end(), begin(), std::bind2nd( std::divides<T>(), c) ); 
  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator^ ( TVector<T> const& x ) const
{
  TVector<T> z( 3 );
  z.theVector_[ 0 ] = theVector_[ 1 ] * x.theVector_[ 2 ] - theVector_[ 2 ] * x.theVector_[ 1 ];
  z.theVector_[ 1 ] = theVector_[ 2 ] * x.theVector_[ 0 ] - theVector_[ 0 ] * x.theVector_[ 2 ];
  z.theVector_[ 2 ] = theVector_[ 0 ] * x.theVector_[ 1 ] - theVector_[ 1 ] * x.theVector_[ 0 ];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// Boolean functions ...

template<typename T>
bool TVector<T>::operator== ( TVector<T> const& x ) const
{
  if( Dim() != x.Dim() ) return 0;

  return theVector_ == x.theVector_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::operator!= ( TVector<T> const& x ) const
{
  return !( *this == x  );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::IsNull() const
{

  for ( const_iterator it = begin(); it != end(); ++it ) {
    if ( (*it) != T()  ) return false; 
  }

  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<double> TVector<T>::Abs() const
{
 TVector<double> z( Dim() );

 TVector<double>::iterator itz  = z.begin(); 
             const_iterator it  =  begin(); 

  for ( ; it !=  end(); ++it, ++itz ) {
    (*itz) = std::abs(*it);
  }

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::Unit () const
{
  double x = Norm();
  TVector<T> z( Dim() );

  const_iterator it  =   begin(); 
        iterator itz = z.begin(); 

  for ( ; it !=  end(); ++it, ++itz ) {
    (*itz) = (*it)/x;
  }

  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::IsUnit() const
{
  return (std::abs( Norm() - 1.0 ) < 1.0e-10 );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TVector<T> TVector<T>::Rotate ( TVector<T> const& v, double const& theta ) const
{

  TVector<T> e  = Unit();
 
  T c = cos( theta );
  T s = sin( theta );

  return    ( c*v )+ ( s*( e^v) )
                   + ( ( ( 1.0 - c )*(e*v) )*e );
 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( std::ostream & os, TVector<T> const& v ) 
{
  OutputFormat* q;
  q = v.ofPtr_;
  if( ( (OutputFormat*) 0 ) == q  ) {
    q = TVector<T>::defOFPtr_;
  }

  typename TVector<T>::const_iterator it = v.begin();

  if( ( (OutputFormat*) 0 ) == q  ) {
    os << "( ";
 
    for ( ; it !=  v.theVector_.end()-1; ++it) {
      os << (*it) << ", ";
    }
    os << (*it)<< " )";
    return os;
  }

  else {
    os << "( ";
    for ( ; it !=  v.theVector_.end()-1; ++it) {
      os << setw(q->width) 
         << setprecision(q->precision) 
         << resetiosflags((std::_Ios_Fmtflags) (014002 | 04)) 
         << setiosflags((std::_Ios_Fmtflags) (q->flags)) 
//         << resetiosflags((014002 | 04)) 
//         << setiosflags((q->flags)) 
         << (*it) << ", "
         << q->getPadding();
    }
    os << setw(q->width) 
       << setprecision(q->precision) 
       << resetiosflags((std::_Ios_Fmtflags) (014002 | 04)) 
       << setiosflags((std::_Ios_Fmtflags) (q->flags)) 
//       << resetiosflags((014002 | 04)) 
//       << setiosflags((q->flags)) 
       <<  (*it) << " )";

    return os;
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::iterator TVector<T>::begin()
{
 return theVector_.begin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_iterator TVector<T>::begin()  const
{
   return theVector_.begin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::iterator  TVector<T>::end()
{
    return theVector_.end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_iterator TVector<T>::end() const
{
  return theVector_.end();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::reverse_iterator TVector<T>::rbegin()
{
   return theVector_.rbegin();  
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_reverse_iterator TVector<T>::rbegin() const
{
  return theVector_.rbegin(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::reverse_iterator TVector<T>::rend() 
{
   return theVector_.rend(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_reverse_iterator TVector<T>::rend()  const
{
    return theVector_.rend(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TMatrix<T> operator%( TVector<T> const& lhs,       TVector<T> const& rhs) 
{
  //-----------------------
  // vector outer product 
  //-----------------------

  typedef typename TVector<T>::const_iterator const_iterator; 

  TMatrix<T> mtrx( lhs.Dim(), rhs.Dim() );
 
  int i=-1, j=-1;

  for ( const_iterator itl = lhs.begin(); itl != lhs.end(); ++itl ) {   
    for ( const_iterator itr = rhs.begin(); itr != rhs.end(); ++itr ) {    
      mtrx[++i][++j] = (*itl)*(*itr); 
    }
  }
  
  return mtrx;

}

