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
******
******  Mar 2006   
******  - templated version
******
******  Oct 2006   ostiguy@fnal.gov
******  - new implementation based on std::vector<>
******
******  Mar 2007 ostiguy@fnal.gov 
******  - exposed iterator interface
******                                                                        
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TVECTOR_TCC
#define TVECTOR_TCC

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/GenericException.h>

#include <iomanip>
#include <vector>
 
using namespace std;


// ================================================================
//      Globals
//

template<typename T>
OutputFormat* TVector<T>::m_defOFPtr = 0;

template<typename T>
void TVector<T>::setDefaultFormat( OutputFormat const& x )
{
  if( ( (OutputFormat*) 0 ) != TVector<T>::m_defOFPtr ) {
    delete TVector<T>::m_defOFPtr;
  }
  m_defOFPtr = new OutputFormat( x );
}

// ================================================================
//      Exception handlers
//

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// #define NOCHECKS

#define CHECKOUT(test,fcn,message)                       \
  if( test ) {                                           \
    throw( GenericException( __FILE__, __LINE__, fcn, message ) ); \
  }


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ================================================================
//      Constructors and the destructor ...
//

template<typename T>
TVector<T>::TVector( int n, T const* x, OutputFormat* fmtPtr )
: m_theVector(n,T()), m_ofPtr(fmtPtr)
{

  CHECKOUT(n <= 0, "TVector<T>::TVector", "Dimension must be positive.")


  if (!x) return; 
  std::copy(x, x+n, m_theVector.begin());   

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
TVector<T>::TVector( TVector const& x )
 : m_theVector( x.m_theVector), m_ofPtr( x.m_ofPtr ) 
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>::TVector( const_iterator first,  const_iterator last)
 : m_theVector(first, last), m_ofPtr(0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>::TVector( TMatrix<T> const& x ): m_ofPtr(0)
{


  if( std::min( x.rows(), x.cols() ) != 1 ) {

  throw(  GenericException( __FILE__, __LINE__, "TVector& TVector( TMatrix const & x )", "Incompatible dimensions."  ) );
 
  }

  if( x.rows() == 1 ) {
    for (int i=0; i < x.cols(); ++i) { 
         m_theVector.push_back( x(0,i) );
    }
    return;
  }
 
  for (int i=0; i < x.rows(); ++i) {
         m_theVector.push_back( x(i,0) );
  }
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T> 
void TVector<T>::reset()
{
  m_theVector.clear();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>::TVector::~TVector()
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
void TVector<T>::Set( const T* x )
{
  int n = m_theVector.size();
  
  std::copy(x, x+n, m_theVector.begin() ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
T const& TVector<T>::operator() ( int i ) const
{
  if ( ( 0 <= i ) && ( i < Dim() ) ) { return m_theVector[i]; }
  else {
    throw( GenericException( __FILE__, __LINE__, "T TVector<T>::operator() ( int i ) const", "Index out of range.") );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
T& TVector<T>::operator() ( int i )
{
  if ( ( 0 <= i ) && ( i < Dim() ) ) { return m_theVector[i]; }
  else {
    throw( GenericException( __FILE__, __LINE__, "T& TVector<T>::operator() ( int i )", "Index out of range.") );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Algebraic functions ...
template<typename T>
TVector<T>& TVector<T>::operator= ( TVector const& x )
{
   if( this == &x )  return *this;
  
    m_theVector = x.m_theVector;
    m_ofPtr     = x.m_ofPtr;

    return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator+ ( TVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT( Dim() != x.Dim(), "TVector<T>::operator+", "Incompatible dimensions.")
#endif

  TVector z( Dim() );

  typename std::template vector<T>::const_iterator it  = m_theVector.begin(); 
  typename std::template vector<T>::const_iterator itx = x.m_theVector.begin(); 
  typename std::template vector<T>::iterator itz = z.m_theVector.begin(); 

  for (  ; it !=  m_theVector.end(); ++it, ++itx, ++itz) {
    (*itz) = (*it) + (*itx);     
  }
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>& TVector<T>::operator+= ( TVector const& x )
{
#ifndef NOCHECKS
  CHECKOUT( Dim() != x.Dim(), "TVector::operator+=", "Incompatible dimensions.")
#endif

  typename std::vector<T>::iterator it         =   m_theVector.begin(); 
  typename std::vector<T>::const_iterator itx = x.m_theVector.begin(); 

  for (  ; it !=  m_theVector.end(); ++it, ++itx) {
    (*it) += (*itx);     
  }

 return (*this);

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


template<typename T>
TVector<T> TVector<T>::operator- ( TVector const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT( Dim() != x.Dim(), "TVector::operator-", "Incompatible dimensions.")
#endif
  
  TVector<T> z( Dim() );

  typename std::vector<T>::const_iterator it  =   m_theVector.begin(); 
  typename std::vector<T>::const_iterator itx = x.m_theVector.begin(); 
  typename std::vector<T>::iterator itz = z.m_theVector.begin(); 

  for (  ; it !=  m_theVector.end(); ++it, ++itx, ++itz) {
    (*itz) = (*it) - (*itx);     
  }
  return z;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>& TVector<T>::operator-= ( TVector<T> const& x )
{
#ifndef NOCHECKS
  CHECKOUT( Dim() != x.Dim(), "TVector<T> TVector<T>::operator-=", "Incompatible dimensions.")
#endif

  typename std::vector<T>::iterator it        =   m_theVector.begin(); 
  typename std::vector<T>::const_iterator itx = x.m_theVector.begin(); 

  for (  ; it !=  m_theVector.end(); ++it, ++itx) {
    (*it) -= (*itx);     
  }
 
  return (*this);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> operator- ( TVector<T> const& x ) // unary minus
{
  TVector<T> z( x.Dim() );

  typename std::vector<T>::const_iterator itx = x.m_theVector.begin(); 
  typename std::vector<T>::iterator itz = z.m_theVector.begin(); 

  for ( itx = x.m_theVector.begin(); itx !=  x.m_theVector.end(); ++itx, ++itz ) {
    (*itz) = - (*itx);
  }

  return z;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>  operator* ( TVector<T> const& x,  T const& c ) 
{
  TVector<T> z( x.Dim() );

  typename std::vector<T>::const_iterator itx  =   x.m_theVector.begin(); 
  typename std::vector<T>::iterator itz  =   z.m_theVector.begin(); 

  for (  ; itx !=  x.m_theVector.end(); ++itx, ++itz ) {
    (*itz) = (*itx)*c;     
  }

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
TVector<T> TVector<T>::operator*=( T const& c ) {

 typename std::vector<T>::iterator it   =     m_theVector.begin();
 
  for (  ; it !=  m_theVector.end(); ++it) {
    (*it) *= c;     
  }

 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T>  TVector<T>::operator/ ( T const& c ) const
{
 TVector z( Dim() );

  typename std::vector<T>::const_iterator it   =     m_theVector.begin(); 
  typename std::vector<T>::iterator itz  =   z.m_theVector.begin(); 

  for (  ; it !=  m_theVector.end(); ++it, ++itz) {
    (*itz) = (*it)/c;     
  }

 return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator/=( T const& c ) {

 typename std::vector<T>::iterator it   =     m_theVector.begin();
 
  for (  ; it !=  m_theVector.end(); ++it) {
    (*it) /= c;     
  }

 return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
TVector<T> TVector<T>::operator^ ( TVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(( Dim() != 3) || ( x.Dim() != 3 ),
           "TVector<T>::operator^",
           "Operator defined only for dimension=3." )
#endif

  TVector<T> z( 3 );
  z.m_theVector[ 0 ] = m_theVector[ 1 ] * x.m_theVector[ 2 ] - m_theVector[ 2 ] * x.m_theVector[ 1 ];
  z.m_theVector[ 1 ] = m_theVector[ 2 ] * x.m_theVector[ 0 ] - m_theVector[ 0 ] * x.m_theVector[ 2 ];
  z.m_theVector[ 2 ] = m_theVector[ 0 ] * x.m_theVector[ 1 ] - m_theVector[ 1 ] * x.m_theVector[ 0 ];
  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// Boolean functions ...

template<typename T>
bool TVector<T>::operator== ( TVector<T> const& x ) const
{
  if( Dim() != x.Dim() ) return 0;

  return m_theVector == x.m_theVector;
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
bool TVector<T>::operator< ( TVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(Dim() != x.Dim(), "TVector<T>::operator<", "Dimensions incompatible.")
#endif

  typename std::vector<T>::const_iterator it  =   m_theVector.begin(); 
  typename std::vector<T>::const_iterator itx = x.m_theVector.begin(); 

  for (; it != m_theVector.end() ; ++it, ++itx ) { 
    if ( !( (*it) < (*itx) ) ) return false;
  }
  return true;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::operator<= (  TVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT( Dim() != x.Dim(), "TVector<T>::operator<=", "Dimensions incompatible.")
#endif

  typename std::vector<T>::const_iterator it  =   m_theVector.begin(); 
  typename std::vector<T>::const_iterator itx = x.m_theVector.begin(); 

  for (; it != m_theVector.end() ; ++it, ++itx ) { 
    if ( (*it) > (*itx) ) return false;
  }
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::operator>= ( TVector<T> const& x ) const
{
#ifndef NOCHECKS
  CHECKOUT(Dim() != x.Dim(), "TVector<T>::operator>=", "Dimensions incompatible.")
#endif

  typename std::vector<T>::const_iterator it  =   m_theVector.begin(); 
  typename std::vector<T>::const_iterator itx = x.m_theVector.begin(); 

  for (; it != m_theVector.end() ; ++it, ++itx ) { 
    if ( (*it) < (*itx) ) return false;
  }
  return true;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
bool TVector<T>::IsNull() const
{

  for (typename std::vector<T>::const_iterator it = m_theVector.begin(); it !=  m_theVector.end(); ++it ) {
    if ( (*it) != T()  ) return false; 
  }

  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// Utilities ..

template<typename T>
TVector<T> TVector<T>::Abs() const
{
  TVector<T> z( Dim() );

  typename std::vector<T>::const_iterator it  =   m_theVector.begin(); 
  typename std::vector<T>::iterator itz = z.m_theVector.begin(); 

  for ( ; it !=  m_theVector.end(); ++it, ++itz ) {
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

  typename std::vector<T>::const_iterator it  =   m_theVector.begin(); 
  typename std::vector<T>::iterator itz = z.m_theVector.begin(); 

  for ( ; it !=  m_theVector.end(); ++it, ++itz ) {
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
void TVector<T>::Rotate ( TVector& v, double const& theta ) const
{
#ifndef NOCHECKS
  CHECKOUT(( Dim() != 3) || ( v.Dim() != 3 ),
           "Vector::Rotate",
           "Dimension must be 3." )
#endif

  TVector<T> e  = Unit();
 
  T c = cos( theta );
  T s = sin( theta );

  TVector<T> u =   ( c*v )
                 + ( s*( e^v) )
                 + ( ( ( 1.0 - c )*(e*v) )*e );
 
  typename std::vector<T>::iterator itu  = u.m_theVector.begin(); 
  typename std::vector<T>::iterator itv =  v.m_theVector.begin(); 

  for ( ; itu !=  u.m_theVector.end(); ++itu, ++itv ) {
    (*itv) = (*itu);
  }

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<typename T>
ostream& operator<<( std::ostream & os, TVector<T> const& v ) 
{
  OutputFormat* q;
  q = v.m_ofPtr;
  if( ( (OutputFormat*) 0 ) == q  ) {
    q = TVector<T>::m_defOFPtr;
  }

  typename std::vector<T>::const_iterator it = v.m_theVector.begin();

  if( ( (OutputFormat*) 0 ) == q  ) {
    os << "( ";
 
    for ( ; it !=  v.m_theVector.end()-1; ++it) {
      os << (*it) << ", ";
    }
    os << (*it)<< " )";
    return os;
  }

  else {
    os << "( ";
    for ( ; it !=  v.m_theVector.end()-1; ++it) {
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

 return m_theVector.begin();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_iterator TVector<T>::begin()  const
{
   return m_theVector.begin();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::iterator  TVector<T>::end()
{

    return m_theVector.end();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_iterator TVector<T>::end() const
{

    return m_theVector.end();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::reverse_iterator TVector<T>::rbegin()
{

   return m_theVector.rbegin();  

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_reverse_iterator TVector<T>::rbegin() const
{

   return m_theVector.rbegin(); 


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::reverse_iterator TVector<T>::rend() 
{

   return m_theVector.rend(); 


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename T>
typename TVector<T>::const_reverse_iterator TVector<T>::rend()  const
{
    return m_theVector.rend(); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#undef CHECKOUT

#endif // TVECTOR_TCC
