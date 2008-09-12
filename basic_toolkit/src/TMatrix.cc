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
******  Revision History 
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov                                   
******             
******   Sep 2005 ostiguy@fnal.gov 
******   - reorganized code to support explicit template instantiations
******   - eliminated separate MatrixC class implementation
******
******   May 2008 ostiguy@fnal.gov 
******
******   - normalization routine relocated from physics_toolkit/BmlUtil. 
******   - normalization no longer hardwired for 6d phase
******     space. In addition, either beam physics or perturbation 
******     theory variable orderings are supported.
******   - refactored eigenvector column ordering code. 
******  
**************************************************************************
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
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/utils.h>
#include <sstream>
#include <list>
#include <vector>
#include <functional>

#ifdef WIN32
#include <Distribution.h> // for drand48
#endif

TMatrix<std::complex<double> > 
toNormalForm( TMatrix<std::complex<double> >const& E, TMatrix<double>::ordering_t ordering )
{
  //-----------------------------------------------------------------
  // 
  // Put the eigenvectors of (the linear part) of a transfer map 
  // into "normal form".  
  //
  // On input:  the columns of E contain eigenvectors of
  //            a one-turn (linear) map. 
  // On output: each column has been normalized and ordered
  //            for physical interpretation.
  // ------------------------------------------------------------------
  // 
  // Note: while the original linear transfer accelerator map 
  //       M is symplectic, that is, it satisfies  
  //
  //                 M^T J M = J
  // where J is the symplectic matrix; J*J = -I and J^-1 = -J                                 
  // 
  // the (linear) transformation to eigencoordinates E 
  // is almost, but not quite, symplectic. In that case,  
  // one has
  // 
  //          E^T J E   = i J
  //          
  //          E^T J E J = -i I    
  //   
  // with E is the matrix whose columns are 
  // the (normalized) eigenvectors of M. 
  // 
  // 
  //-----------------------------------------------------------------

  std::complex<double>  const mi(0.,-1.);    // -i

  int const psd = E.rows();

  TMatrix<double> K; 

  int i_q[psd/2], i_p[psd/2];

  switch (ordering) {
    case TMatrix<double>::s_ordering: 
      K = TMatrix<double>::Smatrix(psd);  // beam physics ordering 
      for (int i=0; i<psd ; i+=2 ) { i_q[i] = i; i_p[i] = i+1;   }
      break;  
    case TMatrix<double>::j_ordering: 
      K = TMatrix<double>::Jmatrix(psd);  // Hamiltonian perturbation theory ordering
      for (int i=0; i<psd ; ++i ) { i_q[i] = i; i_p[i] = i+psd/2; }
      break;
    default:
        throw( GenericException( __FILE__, __LINE__,
                                "MatrixC normalizeNormalForm( MatrixC&,  Matrix::ordering_t ordering )",
                                "Error: unknown ordering" ) );
  }     

  //----------------------------------------------------- 
  // Compute normalization matrix. Nx _must_ be diagonal  
  //-----------------------------------------------------

  TMatrix<std::complex<double> >  Nx = ( E.transpose() * K * E * K ) * mi; //  E^T J E J * i = I

  // --------------------------------------------------------
  // Test for non-zero diagonal elements in Nx and zero out 
  // very small ones that may arise from finite machine 
  // precision. The presence of non-diagonal elements indicates 
  // that the colums of E are not the eigenvectors of a real 
  // symplectic matrix.
  // --------------------------------------------------------

   for( int i=0; i< psd; ++i) {

     Nx[i][i] = 1.0/sqrt( Nx[i][i] );


     for( int j=0; j< psd; ++j) {
  
       if (i == j ) continue;

       if( std::abs( Nx[i][j] ) > 1.0e-6 ) {
              std::ostringstream uic;
              uic << "Invalid eigenvector matrix.\n"
                  << "Nondiagonal element in EJE^TJ." 
                  << "Element " << i << ", " << j << " = "
                  <<  std::abs( Nx[i][j]) << std::ends;
 
               throw( GenericException( __FILE__, __LINE__,
                        "MatrixC toNormalForm( MatrixC const&, )",
                         uic.str().c_str() ) );
       }; 
       Nx[i][j] = 0.0;
     } // for j
  } // for i


  TMatrix<std::complex<double> > EN = E*Nx;  

  // ---------------------------------------------------------------------------------
  // Note that the eigenvector solver returns eigenvectors that
  // are not necessarily ordered according to the "phase space" order.  
  // We order the eigenvectors  to ensure that the column order of the new 
  // (transformed) momenta coordinates (a*) is consistent with the 
  // original row order. This is necessary to ensure that the sign of the 
  // phase advance phi is consistent. 
  //  
  // The correct ordering can be determined by examining the sign of the 
  // imaginary component at (col) index i_p of eigenvector i_q. 
  // If it is not negative, we swap the eigenvectors (column) at
  // conjugate positions.   
  //------------------------------------------------------------------------------------ 

  for (int icol=0; icol<psd/2; ++icol) {
 
     if( imag( EN[i_p[icol]][i_q[icol]]/EN[i_q[icol]][i_q[icol]] ) > 0.0 ) {
        for( int k=0; k<psd; ++k) { std::swap( EN[k][i_q[icol]], EN[k][i_p[icol]]); }
     }
   }
 
  return EN;  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


TMatrix<std::complex<double> > matrix_conj( TMatrix<std::complex<double> > const& A )
{
  TMatrix<std::complex<double> > C( A.rows(), A.cols() );

  for (int i=0; i< A.rows(); ++i) {   
    for (int j=0; j< A.cols(); ++j) {    
      C[i][j] = std::conj( A[i][j] );
    }
  }
  return C;
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<std::complex<double> >      symplecticSort( TMatrix<std::complex<double> > const& E )
{
  // not implemented yet 
 
  TMatrix<std::complex<double> > EN = E; 
  return EN;
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<std::complex<double> >   normalize_columns( TMatrix<std::complex<double> > const& E )
{  
  int const rows = E.rows();
  int const cols = E.cols();

  double norm[cols];

  for (int j=0; j<cols; ++j ) {

      norm[j] = 0.0;

      for (int i=0; i<rows; ++i ) {
        norm[j] += ( E[i][j] * conj(E[i][j]) ).real();
      }
      norm[j] =  1.0 / sqrt( norm[j] ); 
  }

  TMatrix<std::complex<double> > EN(rows,cols);

   for (int i=0; i<cols; ++i ) {
     for (int j=0; j<cols; ++j ) {
       EN[i][j] = E[i][j]*norm[j];
     }
   }

   return EN;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
TMatrix<std::complex<double> >::TMatrix( TMatrix<double> const& m)
{ 
  ml_ = m.ml_; // implicit conversion
  return; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<double> TMatrix<double>::dagger() const {

//  for a real matrix, the Hermitian conjugate and the transpose
//  are the same
    
  TMatrix<double> ret;
  ret.ml_ = ml_->transpose();
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TMatrix<std::complex<double> > TMatrix<std::complex<double> >::dagger() const
{
  TMatrix<std::complex<double> > ret;
  ret.ml_= ml_->dagger();
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<double> real( TMatrix<std::complex<double> > const& x )
{
  TMatrix<double> ret; 
  ret.ml_= real_part(x.ml_); 
  return ret; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<double> imag( TMatrix<std::complex<double> > const& x ) 
{
  TMatrix<double> ret; 
  ret.ml_= imag_part(x.ml_); 
  return ret; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void TMatrix<double>::SVD( TMatrix<double>& U, Vector& W, TMatrix<double>& V) const
{
  ml_->SVD( U.ml_, W, V.ml_);

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<std::complex<double>  > operator*( TMatrix<std::complex<double>  > const& x, TMatrix<double> const& y)
{
 
  TMatrix<std::complex<double> > z = y; //implicit conversion 
  TMatrix<std::complex<double> > ret;

  ret.ml_ = multiply<std::complex<double> >(x.ml_, z.ml_); 

  return ret; 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

TMatrix<std::complex<double> > operator*(TMatrix<double> const& x, TMatrix<std::complex<double> > const& y)
{

  TMatrix<std::complex<double> > z = x; //implicit conversion 

  TMatrix<std::complex<double> > ret;
  ret.ml_ = multiply<std::complex<double> >(z.ml_, y.ml_); 
  return ret; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

/*** static function ***/

template<>
Vector 
TMatrix<double>::backSubstitute(TMatrix<double> const& U, Vector const& W, TMatrix<double> const& V, Vector const& rhs, double threshold) { 

  return TML<double>::backSubstitute(U.ml_, W, V.ml_, rhs, threshold);
 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

namespace {

  class NegImag: public std::unary_function< int const&, bool > {

  public:

    NegImag( TVector<std::complex<double> > const& e ): e_(e) {}
   
    bool operator()( int const& arg ) const { 
      return ( e_[arg].imag() < 0.0 );
    }

   private:
   
    TVector<std::complex<double> > const& e_;
 
  };
 
  //------------------------------------------------------------------------------------------------------
 
  class Reciprocal: public std::binary_function< int, int, bool > {
  
  public:

    Reciprocal(  TVector<std::complex<double> > const& e ) : e_(e) {} 

    bool operator()( int lhs,  int rhs) const {

      std::complex<double> product = e_[rhs] *e_[lhs];
  
      return  (  ( std::abs(product.real() - 1.0) < 1.0e-9 ) &&  
                 ( std::abs(product.imag()      ) < 1.0e-9 ) ) ; 
 
     }

    private:

    TVector<std::complex<double> > const& e_;

  };

}

//----------------------------------------------------------------------------------------------------

void orderColumns( TVector<std::complex<double> >& eigenvalues,   
                   TMatrix<std::complex<double> >& eigenvectors ) 
{

// ------------------------------------------------------------------------------
// eigenvalues  : vector containing the eigenvalues 
// eigenvectors : matrix containing the corresponding eigenvectors
// 
// The libraries assumes Hamiltonian perturbation theory ordering for
// phase space coordinates.
// 
// index 0: x  
// index 1: y  
// index 2: cdt    ( =z only when beta=1)  
// index 3: npx    (px/p_0)  
// index 4: npy    (py/p_0)  
// index 5: ndp    (dp/p_0)    
// 
// The columns of the matrix of eigenvectors returned from the eigensolver is not ordered
// in a way consistent with the convention for phase space coordinates.    
// 
// Assuming the original matrix A was a _real_ symplectic matrix, its eigenvalues 
// and eigenvectors must obey the following constraints:
//
// (1) They are either real or complex (conjugate) pairs; pure imag eigenvalues 
//     _do not_ occur 
// (2) for each  eigenvalue lambda, there is a corresponding eigenvalue 1/lambda;
// (3) for each  complex eigenvalue lambda, there is a corresponding eigenvalue lambda*
//     and lambda* = 1/lambda      
// 
// furthermore: 
//    
//  det A = lambda_0 * lamba_1 * ... * lambda_2n = 1 
//  
// This function reorders columns of the eigenvector matrix and the 
// corresponding entries in the eigenvalue vector. 
// 
// In the general, assuming a stable cases where all eigenvalues are complex, 
// the column ordering produced by the eigensolver would be as follows
// 
// 0:lambda_1 1:lambda_1* 2:lambda_2 3:lambda_2* 4:lambda_3 5:lambda_3*
//
// where lambda* denotes the conjugate of lambda. 
//
// Note that imag(lambda_1) < 0.0,  imag(lambda_2) < 0.0,  imag(lambda_3) < 0.0
// 
// Internally, the libraries assume the following ordering for the coordinates:
//  
// 0:x 1:y 2:ct 3:xp 4:yp 5:dpp
// 
// The time evolution of npx, npy and ndp is in quadrature with x ,y, and cdt. 
// In the uncoupled case, 
//
// x   = a_x  =  e[0] + e*[0] /2
// npx = a_x* =  e[0] - e*[0] /2i  
// 
// 
// The target ordering is
//
// a_x, a_y, a_ct  a_x* a_y* a_ct* 
//
//------------------------------------------------------------------------------------------------------
  
  std::list<int>    col_indices;
  std::vector<int>  sorted_indices(6);

  for ( int i=0; i<6; ++i ) { col_indices.push_back(i); }

  //---------------------------------
  // Put complex eigenvalues first.    
  //---------------------------------

  std::list<int>::iterator col1, col2;

  int i=-1;

   // first, we order the complex eigenvalues. *negative imaginary value first* ...
  
  while ( !col_indices.empty() ) {

   col1  = find_if( col_indices.begin(), col_indices.end(),  (NegImag(eigenvalues)) ); 

   if ( col1 ==  col_indices.end() ) break;
   sorted_indices[++i] = *col1; col_indices.erase( col1 );   

   col2  = std::find_if( col_indices.begin(), col_indices.end(),  std::bind2nd( (Reciprocal(eigenvalues)), sorted_indices[i] ) ); 
   sorted_indices[i+3] = *col2; col_indices.erase( col2);
 
  }
 
  // and then, the real eigenvalues ...

  while ( !col_indices.empty() ) {

    col1 = col_indices.begin();
    sorted_indices[++i] = *col1; col_indices.erase( col1 );   

    col2  = find_if( col_indices.begin(), col_indices.end(),  std::bind2nd( (Reciprocal(eigenvalues)), sorted_indices[i] ) ); 
    sorted_indices[i+3] = *col2; col_indices.erase( col2);

  } 
  
  //------------------------------------------------------------------------
  // attempt to distinguish between hor and ver coordinates. Order: hor, ver  
  //------------------------------------------------------------------------

  if ( std::abs( eigenvectors[0][ sorted_indices[0] ] ) < std::abs( eigenvectors[0][ sorted_indices[1] ] ) )
  {
    std::swap(  sorted_indices[0],    sorted_indices[1  ] );
    std::swap(  sorted_indices[0+3],  sorted_indices[1+3] );
  }


  TMatrix<std::complex<double> > ordered_eigenvectors( 6, 6 );   
  TVector<std::complex<double> > ordered_eigenvalues(6);

  i=0;
  for ( std::vector<int>::iterator it  = sorted_indices.begin(); 
                                   it != sorted_indices.end(); ++it, ++i ) {  

    ordered_eigenvalues[i] = eigenvalues[ *it ];

    for (int j=0; j<6; ++j ) {
      ordered_eigenvectors[j][i] = eigenvectors[j][*it];
    }

  }

  eigenvalues   = ordered_eigenvalues;      
  eigenvectors  = ordered_eigenvectors;

  return;

}
