/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
******
******  File:      TML.cc
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
******  Revision (Sep 2005 - Dec 2005):
******
******             Jean-Francois Ostiguy
******             ostiguy@fnal.gov                                   
******             
******   - refactored code to use single template parameter instead of two
******   - introduced implicit conversions 
******   - using boost::intrusive_pointer for reference counting
******   - eliminated access to private implementation  ( class (TML) )
******     from class TMatrix.
******   - eliminated separate MatrixC class implementation 
******     (used to inherit from Matrix<T1,T2>)
******   - organized code to support both implicit and explicit 
******     template instantiations
******   - fixed incorrect complex version of eigenvalues/eigenvectors 
******   - separated eigenvalue/eigenvector reordering function 
****** 
**************************************************************************
*************************************************************************/
#include <complex>
#include <basic_toolkit/TVector.h>
#include <basic_toolkit/TML.h>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/utils.h>
#include <algorithm>

using FNAL::pcout;
using FNAL::pcerr;

static const int PHASESPACE = 6;
#define M_SMALL  1.0e-30

static std::complex<double> complex_0(0.0, 0.0);
 

// ===================================================================
// Specializations for class TML
// ==================================================================

template<>
double TML<double>::abs( double const& arg)
{
  return std::abs(arg);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TML<std::complex<double> >::abs( std::complex<double> const& arg)
{
  return std::abs(arg);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TML<double>::norm( double const& arg)
{
  return std::abs(arg);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
double TML<std::complex<double> >::norm( std::complex<double> const& arg)
{
  return std::abs(arg);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
template<>
TML<std::complex<double> >::TML( TML<double> const& ml)
: nrows_(ml.nrows_), ncols_(ml.ncols_)  { 
  
  mdata_ = new std::complex<double>* [nrows_];

  int sz = nrows_*ncols_;

  std::complex<double>* dataPtr = data_ = new std::complex<double> [ sz ];

  for( int i=0; i<nrows_; ++i) { 
     mdata_[i] = dataPtr;
     dataPtr += ncols_;
  }

  // note: we do not use memcopy() because of the type conversion.

  for( int i=0; i< nrows_; ++i ) {
    for( int j=0; j< ncols_; ++j ) {
      mdata_[i][j] = std::complex<double>( ml.mdata_[i][j], 0.0 );
    }
  }
  
  return;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
MLPtr<std::complex<double> > 
TML<std::complex<double> >::dagger() const {

  MLPtr<std::complex<double> >  z( new  TML<std::complex<double> >(*this) );

  for(int row=0;  row<nrows_; ++row) {
    for(int col=0; col < ncols_; ++col) {
      z->mdata_[col][row] = conj(mdata_[row][col]);
    }
  }
  return z;
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


MLPtr<double> real_part( MLPtr<std::complex<double> > const& x )
{
  int sz =  x->nrows_ * x->ncols_; 

  MLPtr<double> ret( new TML<double>(x->nrows_, x->ncols_, 0.0) );
  
  std::complex<double>* p =   x->mdata_[0];
  double*               q = ret->mdata_[0];

  for(  int i=0; i< sz; ++i ) {
   
      (*q) = std::real (*p);
      ++q; ++p;
  }
  return ret;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MLPtr<double> imag_part( MLPtr<std::complex<double> > const& z )
{
  int sz =  z->nrows_ * z->ncols_; 

  MLPtr<double> x( new TML<double>(z->nrows_, z->ncols_, 0.0) );
  
  std::complex<double>* p =   z->mdata_[0];
  double*               q =   x->mdata_[0];

  for(  int i=0; i< sz; ++i ) {
   
      (*q) = std::imag(*p);
      ++q; ++p;
  }

  return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
MLPtr<std::complex<double> > TML<double>::eigenVectors()  const
{

  double wr[ncols_];
  double wi[ncols_];

  int     iv1[ncols_];
  double  fv1[ncols_];
  double    b[nrows_*ncols_];
  double    c[nrows_*ncols_];

  MLPtr<std::complex<double> >   eigenvectors ( new TML<std::complex<double> >(nrows_,ncols_,complex_0) );
  TVector<std::complex<double> > eigenvalues( ncols_ );

  int nrows  = nrows_;
  int ncols  = ncols_;
  int ierr   = 0;
  int matz   = 1;

  int k = 0;
  for( int i=0; i< nrows_; ++i) {
    for( int j=0; j< ncols_; ++j) {
      b[k] = mdata_[j][i];       // the rg_ routine uses the transpose
      ++k;
    }
  }
  
  rg_(&nrows,&ncols,b,wr,wi,&matz,c,iv1,fv1,&ierr);


  if(ierr != 0) {
    (*pcerr) << "TML<double>: Error in eigenvector routine. error = ";
    (*pcerr) << ierr << std::endl;
  }
  
  // ----------------------------------------------------------------------------------------------------
  // 
  // Upon returning from rg_:
  //  
  // wr[i] and  wi[i]  contain the real and imaginary parts,
  // respectively, of the eigenvalues.  Complex conjugate
  // pairs of eigenvalues appear consecutively with the
  // eigenvalue having the _positive_ imaginary part first.
  //
  // c  contains the real and imaginary parts of the eigenvectors
  // if matz is not zero.  if the j-th eigenvalue is real, the
  // j-th column of  c contains its eigenvector. If the j-th
  // eigenvalue is complex with positive imaginary part, the
  // j-th and (j+1)-th columns of  c  contain the real and
  // imaginary parts of its eigenvector.  The conjugate of this
  // vector is the eigenvector for the conjugate eigenvalue.
  //
  // The code below extracts the eigenvectors from the array c 
  // and put them in uncompressed format. The order of 
  // eigenvalue/eigenvectors is **unchanged**. 
   
  // ------------------------------------------------------------------------------------------------------

  int i = 0;
  int j = 0;
  k = 0;
  int realCount = 0;
  while( i< nrows) {
    if(wi[i] == 0.0){  // real eigenvalue, get corresponding eigenvector directly
      for(j=0; j<ncols; ++j) {
        eigenvectors->mdata_[j][k] = std::complex<double>( c[ j+i*ncols], 0.0);// **** note conversion from fortran to c storage order ***  
      }
      eigenvalues[k] = std::complex<double>(wr[i],0.0);
      ++k; 
      ++i;
    } else { // *** complex eigenvalues *** , need to form eigenvector using combinations of c[i][j] and c[i+i][j]

      eigenvalues[k]   = std::complex<double>(wr[i],  wi[i]);
      eigenvalues[k+1] = std::complex<double>(wr[i+1],wi[i+1]);
      for(j = 0; j<ncols; j++) {
        eigenvectors->mdata_[j][k]   = std::complex<double>(c[j+i*ncols],  c[ j+(i+1)*ncols ]);
        eigenvectors->mdata_[j][k+1] = std::complex<double>(c[j+i*ncols], -c[ j+(i+1)*ncols ]);
      }
      k += 2;
      i += 2;
    }
  }

  return eigenvectors;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TVector<std::complex<double> > TML<double>::eigenValues() const
{
  int nrows = nrows_;
  int ncols = ncols_;

  double wr[ncols_];
  double wi[ncols_];

  int     iv1[ncols_];
  double  fv1[ncols_];
  double    b[nrows_*ncols_];
  double    c[nrows_*ncols_];


  MLPtr<std::complex<double> >    eigenvectors(  new TML<std::complex<double> >(nrows_, ncols_, complex_0));
  TVector<std::complex<double> >  eigenvalues( ncols_ );

  int k = 0;
  for(int i=0;  i< nrows_; ++i) {
    for(int j=0; j<ncols_; ++j) {
      b[k] = mdata_[j][i];  // the rg_ routine uses the transpose  
      k++;
    }
  }

  //-----------------------------------------------------------------
  //  matz  is an integer variable set equal to zero if
  //  only eigenvalues are desired.  otherwise it is set to
  //  any non-zero integer for both eigenvalues and eigenvectors.
  // 
  //  Unfortunately, the eigenvectors are needed to distinguish 
  //  between the "horizontal" and "vertical" eigenmodes ;-( 
  //-------------------------------------------------------------------

  int matz = 0;

  int ierr;
  rg_(&nrows,&ncols,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    (*pcerr) << "TMatrix<double>: Error in eigenvalue routine. rg (EISPACK) returns error = ";
    (*pcerr) << ierr << std::endl;
    return eigenvalues;
  }

  //------------------------------------------------------------------------------------------------ 
  // EISPACK rg returns conjugate eigenvalue pairs ordered so that the eigenvalue with positive 
  // appear _first_. 
  //-------------------------------------------------------------------------------------------------

  for ( int i=0; i<ncols; ++i ) { eigenvalues[i] = std::complex<double>(wr[i], wi[i]);  }

  return eigenvalues; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
TVector<std::complex<double> > TML<std::complex<double> >::eigenValues() const { 

  int nm   = nrows_;
  int n    = ncols_;

  double  wr[ncols_];
  double  wi[ncols_];
  double fv1[ncols_];
  double fv2[ncols_];
  double fv3[ncols_];
  double  br[nrows_*ncols_];
  double  bi[nrows_*ncols_];
  double  cr[nrows_*ncols_];
  double  ci[nrows_*ncols_];


  int oddEven = nm/2;
  TVector<std::complex<double> > eigenvalues( ncols_);

  int k = 0;
  for(int i=0; i < nrows_; ++i) {
    for(int j = 0; j< ncols_; ++j) {

      br[k] = std::real(mdata_[j][i]); // the cg_ routine uses the transpose
      bi[k] = std::imag(mdata_[j][i]); // the cg_ routine uses the transpose
      ++k;
    }
  }
  MLPtr<std::complex<double> > eigenvectors( new TML<std::complex<double> >(nm,n,complex_0) );

  //-----------------------------------------------------------------
  //  matz  is an integer variable set equal to zero if
  //  only eigenvalues are desired.  otherwise it is set to
  //  any non-zero integer for both eigenvalues and eigenvectors.
  // 
  //-------------------------------------------------------------------

  int matz = 0;      

  int ierr = 0;
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);

  if(ierr != 0) {
    (*pcerr) << "TMatrix<std::complex<double> >: Error in eigenvalue routine. error = ";
    (*pcerr) << ierr<< std::endl;

  }

  //---------------------------------------------------------------------------------------------
  // Note: EISPACK cg is for a general complex matrix. In contrast with rg, the corresponding 
  // routine for a real matrix, the eigenvalues do not appear in complex conjugate peair and
  // therefore are not put in any particular order.
  //---------------------------------------------------------------------------------------------

  for(int i=0; i<ncols_; ++i)
    eigenvalues[i] = std::complex<double>( wr[i],wi[i] );
 
  return eigenvalues;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
MLPtr<std::complex<double> > TML<std::complex<double> >::eigenVectors() const {

  int nm   = nrows_;
  int n    = ncols_;

  double   wr[ncols_];
  double   wi[ncols_];
  double  fv1[ncols_];
  double  fv2[ncols_];
  double  fv3[ncols_];
  double   br[nrows_*ncols_];
  double   bi[nrows_*ncols_];
  double   cr[nrows_*ncols_];
  double   ci[nrows_*ncols_];

  int ierr = 0;

  int k = 0;
  for(int i = 0; i < nrows_; ++i) {
    for(int j = 0; j< ncols_; ++j) {

      br[k] = std::real(mdata_[j][i]); // the cg_ routine uses the transpose
      bi[k] = std::imag(mdata_[j][i]); // the cg_ routine uses the transpose
      ++k;
    }
  }

  //-----------------------------------------------------------------
  //  matz  is an integer variable set equal to zero if
  //  only eigenvalues are desired.  otherwise it is set to
  //  any non-zero integer for both eigenvalues and eigenvectors.
  // 
  //-------------------------------------------------------------------
  int matz = 1;

  MLPtr<std::complex<double> > eigenvectors( new TML<std::complex<double> >(nm,n,complex_0));

  cg_(&nm, &n, br, bi, wr, wi, &matz, cr, ci, fv1, fv2, fv3, &ierr);

  if(ierr != 0) {
    (*pcerr) << "TML<std::complex<double> >: Error in eigenvector routine. error = ";
    (*pcerr) << ierr<< std::endl;
    return eigenvectors;
  }

  k = 0;
  for(int i=0;  i< nrows_; ++i) {
    for(int j = 0; j< ncols_; ++j) {
      eigenvectors->mdata_[j][i] = std::complex<double>(cr[k],ci[k]); // NOTE transposition !
      ++k; 
   }
  }

  return eigenvectors;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

