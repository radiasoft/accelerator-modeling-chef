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
******   - eliminated access to internals of implementation class (TML) from class TMatrix.
******   - eliminated separate MatrixC class implementation (used to be derived from Matrix<T1,T2>)
******   - organized code to support both implicit and explicit template instantiations
******   - fixed incorrect complex version of eigenvalues/eigenvectors 
******   - separated eigenvalue/eigenvector reordering function 
****** 
**************************************************************************
*************************************************************************/
#include <complex>
#include <basic_toolkit/TML.h>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <basic_toolkit/utils.h>

using FNAL::pcout;
using FNAL::pcerr;

static const int PHASESPACE = 6;
#define M_SMALL  1.0e-30

static std::complex<double> complex_0(0.0, 0.0);
 

// ===================================================================
// Specializations for class TML
// ==================================================================

template<>
template<>
TML<std::complex<double> >::TML( TML<double> const& ml): _nrows(ml._nrows), _ncols(ml._ncols)  { 
  
  _mdata = new std::complex<double>* [_nrows];

  int sz = _nrows*_ncols;

  std::complex<double>* dataPtr = new std::complex<double> [ sz ];

  for( int i=0; i<_nrows; ++i) { 
     _mdata[i] = dataPtr;
     dataPtr += _ncols;
  }

  // note: we do not use memcopy() because of the type conversion.

  for( int i=0; i< _nrows; ++i ) {
    for( int j=0; j< _ncols; ++j ) {
      _mdata[i][j] = std::complex<double>( ml._mdata[i][j], 0.0 );
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

  for(int row=0;  row<_nrows; ++row) {
    for(int col=0; col < _ncols; ++col) {
      z->_mdata[col][row] = conj(_mdata[row][col]);
    }
  }
  return z;
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


MLPtr<double> real_part( MLPtr<std::complex<double> > const& x )
{
  int sz =  x->_nrows * x->_ncols; 

  MLPtr<double> ret( new TML<double>(x->_nrows, x->_ncols, 0.0) );
  
  std::complex<double>* p =   x->_mdata[0];
  double*               q = ret->_mdata[0];

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
  int sz =  z->_nrows * z->_ncols; 

  MLPtr<double> x( new TML<double>(z->_nrows, z->_ncols, 0.0) );
  
  std::complex<double>* p =   z->_mdata[0];
  double*               q =   x->_mdata[0];

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

  double* wr = new double[_ncols];
  double* wi = new double[_ncols];

  int* iv1        = new int[_ncols];
  double* fv1     = new double[_ncols];
  double* b       = new double[_nrows*_ncols];
  double* c       = new double[_nrows*_ncols];

  int i,j;

  MLPtr<std::complex<double> > eigenvectors ( new TML<std::complex<double> >(_nrows,_ncols,complex_0) );
  MLPtr<std::complex<double> > eigenvalues  ( new TML<std::complex<double> >(1,_ncols,complex_0)      );

  int nrows  = _nrows;
  int ncols  = _ncols;
  int ierr   = 0;
  int matz   = 1;

  int k = 0;
  for( i=0; i< _nrows; ++i) {
    for( j=0; j< _ncols; ++j) {
      b[k] = _mdata[j][i];       // the rg_ routine uses the transpose
      ++k;
    }
  }
  //std::cout << "Before call to rg_ ... " <<std::endl;
  //for (int i=0; i< _nrows*_ncols; ++i) std::cout << b[i] <<", ";  
  //std::cout << std::endl;
  
  rg_(&nrows,&ncols,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  //std::cout << "After call to rg_ ... " <<std::endl;
  //for (int i=0; i< _nrows*_ncols; ++i) std::cout << c[i] <<", ";  
  //std::cout << std::endl;

  if(ierr != 0) {
    (*pcerr) << "TML<double>: Error in eigenvector routine. error = ";
    (*pcerr) << ierr << std::endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return eigenvectors;
  }
  

  // ----------------------------------------------------------------------------------------------------
  // The following code extracts the eigenvectors from the array c which contains
  // contains the *unsorted* eigenvectors, in compressed format (assuming matz is not zero)
  // the j-th column of  c  contains its eigenvector.  if the j-th eigenvalue is complex with positive 
  // imaginary part, the j-th and (j+1)-th columns of c contain the real and
  // imaginary parts of its eigenvector.  The conjugate of this vector is the eigenvector for the 
  // conjugate eigenvalue.
  // 
  // wr[i] and wi[i] are the real an imaginary parts of eigenvalue i.
  // ------------------------------------------------------------------------------------------------------
  i = 0;
  k = 0;
  j = 0;
  int realCount = 0;
  while( i< nrows) {
    if(wi[i] == 0.0){  // real eigenvalue, get corresponding eigenventor directly
      ++realCount;
      for(j=0; j<ncols; ++j) {
        eigenvectors->_mdata[j][k] = std::complex<double>( c[ j+i*ncols], 0.0);// **** note conversion from fortran to c storage order ***  
      }
      eigenvalues->_mdata[0][k] = std::complex<double>(wr[i],0.0);
      ++k; 
      ++i;
    } else { // *** complex eigenvalues *** , need to form eigenvector using combinations of c[i][j] and c[i+i][j]

      eigenvalues->_mdata[0][k]   = std::complex<double>(wr[i],  wi[i]);
      eigenvalues->_mdata[0][k+1] = std::complex<double>(wr[i+1],wi[i+1]);
      for(j = 0; j<ncols; j++) {
        eigenvectors->_mdata[j][k]   = std::complex<double>(c[j+i*ncols],  c[ j+(i+1)*ncols ]);
        eigenvectors->_mdata[j][k+1] = std::complex<double>(c[j+i*ncols], -c[ j+(i+1)*ncols ]);
      }
      k += 2;
      i += 2;
    }
  }

  delete []wr;
  delete []wi;
  delete []iv1;
  delete []fv1;
  delete []b;
  delete []c;

  TML<std::complex<double> >::orderCoordinates(eigenvalues, eigenvectors);

  return eigenvectors;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
MLPtr<std::complex<double> > TML<double>::eigenValues() const
{
  int nrows = _nrows;
  int ncols = _ncols;

  int matz = 1;

  double* wr = new double[_ncols];
  double* wi = new double[_ncols];

  int*    iv1   = new int[_ncols];
  double* fv1   = new double[_ncols];
  double* b     = new double[_nrows*_ncols];
  double* c     = new double[_nrows*_ncols];

  int oddEven   = nrows/2;
  int realCount = 0;

  MLPtr<std::complex<double> > eigenvectors(  new TML<std::complex<double> >(_nrows, _ncols, complex_0));
  MLPtr<std::complex<double> > eigenvalues( new TML<std::complex<double> >(1, _ncols, complex_0));

  int k = 0;
  for(int i=0;  i< _nrows; ++i) {
    for(int j=0; j<_ncols; ++j) {
      b[k] = _mdata[j][i];  // the rg_ routine uses the transpose  
      k++;
    }
  }

  int ierr;
  rg_(&nrows,&ncols,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    (*pcerr) << "TMatrix<double>: Error in eigenvalue routine. error = ";
    (*pcerr) << ierr << std::endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return eigenvectors;
  }

  int i = 0;
  int j = 0;
  k = 0; 
  while( i< nrows) {
    if(wi[i] == 0){
      ++realCount;
      for(j=0; j<ncols; ++j) {
        eigenvectors->_mdata[j][k] = std::complex<double>( c[j+i*ncols], 0.0);
      }
      eigenvalues->_mdata[0][k] = std::complex<double>(wr[i],0.0);
      ++k;
      ++i;
    } else {
      eigenvalues->_mdata[0][k]   =  std::complex<double>(wr[i],  wi[i]);
      eigenvalues->_mdata[0][k+1] =  std::complex<double>(wr[i+1],wi[i+1]);

      for(j=0; j<ncols; j++) {
        eigenvectors->_mdata[j][k]   =  std::complex<double>(c[ j + i*ncols],  c[j + (i+1)*ncols]);  
        eigenvectors->_mdata[j][k+1] =  std::complex<double>(c[ j + i*ncols], -c[j + (i+1)*ncols]);
      }
      k += 2;
      i += 2;
    }
  }


  delete []wr;
  delete []wi;
  delete []iv1;
  delete []fv1;
  delete []b;
  delete []c;

 

  TML<std::complex<double> >::orderCoordinates(eigenvalues, eigenvectors);

  //std::cout << "Retuning from TML::eigenValues()" << std::endl;
  //std::cout << "eigenvalues = "  << *eigenvalues  << std::endl;
  //std::cout << "eigenvectors = " << *eigenvectors << std::endl;

  return eigenvalues;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
MLPtr<std::complex<double> > TML<std::complex<double> >::eigenValues() const { 

  int nm   = _nrows;
  int n    = _ncols;
  int matz = 1;       // eigenvectors needed ?

  double* wr   = new double[_ncols];
  double* wi   = new double[_ncols];
  double* fv1  = new double[_ncols];
  double* fv2  = new double[_ncols];
  double* fv3  = new double[_ncols];
  double* br   = new double[_nrows*_ncols];
  double* bi   = new double[_nrows*_ncols];
  double* cr   = new double[_nrows*_ncols];
  double* ci   = new double[_nrows*_ncols];

  int ierr,i,j,k;
  int oddEven = nm/2;
  MLPtr<std::complex<double> > eigenvalues( new TML<std::complex<double> >( 1, _ncols, complex_0));

  k = 0;
  for(i = 0; i < _nrows; ++i) {
    for(j = 0; j< _ncols; ++j) {

      br[k] = std::real(_mdata[j][i]); // the cg_ routine uses the transpose
      bi[k] = std::imag(_mdata[j][i]); // the cg_ routine uses the transpose
      ++k;
    }
  }
  MLPtr<std::complex<double> > eigenvectors( new TML<std::complex<double> >(nm,n,complex_0) );
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);

  if(ierr != 0) {
    (*pcerr) << "TMatrix<std::complex<double> >: Error in eigenvalue routine. error = ";
    (*pcerr) << ierr<< std::endl;
    delete []wr;
    delete []wi;
    delete []fv1;
    delete []fv2;
    delete []fv3;
    delete []br;
    delete []bi;
    delete []cr;
    delete []ci;
    return eigenvalues;
  }
  for(i = 0; i< _ncols; ++i)
    eigenvalues->_mdata[0][i] = std::complex<double>( wr[i],wi[i] );
 
 k = 0;
 for( i=0; i < _nrows; i++) {
    for( j=0; j< _ncols; j++) {
      eigenvectors->_mdata[j][i] = std::complex<double>( cr[k++],ci[k++] );  // NOTE transposition
    }
  }

  delete []wr;
  delete []wi;
  delete []fv1;
  delete []fv2;
  delete []fv3;
  delete []br;
  delete []bi;
  delete []cr;
  delete []ci;

  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; i++) {
      if(abs( eigenvectors->_mdata[0][0]) < abs(eigenvectors->_mdata[0][i])) {
         eigenvectors->_switch_columns(0,i);
        eigenvalues->_switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
           eigenvectors->_switch_columns(oddEven,oddEven+i);
          eigenvalues->_switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10)
    (*pcerr) << "TMatrix<std::complex<double> >: Something is wrong with the eigenValue sort" << std::endl;

  for(i=2; i < oddEven; i++) {
    if(abs(eigenvectors->_mdata[1][1]) < abs(eigenvectors->_mdata[1][i])) {
       eigenvectors->_switch_columns(1,i);
      eigenvalues->_switch_columns(1,i);
      if((oddEven*2) == nm)
	 eigenvectors->_switch_columns(1+oddEven,oddEven+i);   
        eigenvalues->_switch_columns(1+oddEven,oddEven+i); 
    }
  }
  return eigenvalues;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
MLPtr<std::complex<double> > TML<std::complex<double> >::eigenVectors() const {

  int nm   = _nrows;
  int n    = _ncols;
  int matz = 1;

  double* wr    = new double[_ncols];
  double* wi    = new double[_ncols];
  double* fv1   = new double[_ncols];
  double* fv2   = new double[_ncols];
  double* fv3   = new double[_ncols];
  double* br    = new double[_nrows*_ncols];
  double* bi    = new double[_nrows*_ncols];
  double* cr    = new double[_nrows*_ncols];
  double* ci    = new double[_nrows*_ncols];

  int ierr,i,j,k;
  int oddEven = nm/2;

  k = 0;
  for(i = 0; i < _nrows; ++i) {
    for(j = 0; j< _ncols; ++j) {

      br[k] = std::real(_mdata[j][i]); // the cg_ routine uses the transpose
      bi[k] = std::imag(_mdata[j][i]); // the cg_ routine uses the transpose
      ++k;
    }
  }

  MLPtr<std::complex<double> > eigenvectors( new TML<std::complex<double> >(nm,n,complex_0));
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);

  if(ierr != 0) {
    (*pcerr) << "TML<std::complex<double> >: Error in eigenvector routine. error = ";
    (*pcerr) << ierr<< std::endl;
    delete []wr;
    delete []wi;
    delete []fv1;
    delete []fv2;
    delete []fv3;
    delete []br;
    delete []bi;
    delete []cr;
    delete []ci;
    return eigenvectors;
  }

  k = 0;
  for(i=0;  i< _nrows; ++i) {
    for(j = 0; j< _ncols; ++j) {
      eigenvectors->_mdata[j][i] = std::complex<double>(cr[k],ci[k]); // NOTE transposition !
      ++k; 
   }
  }

  delete []wr;
  delete []wi;
  delete []fv1;
  delete []fv2;
  delete []fv3;
  delete []br;
  delete []bi;
  delete []cr;
  delete []ci;

  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; ++i) {
      if(abs(eigenvectors->_mdata[0][0]) < abs(eigenvectors->_mdata[0][i])) {
        eigenvectors->_switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
          eigenvectors->_switch_columns(oddEven,oddEven+i);
      }
    }
    ++counter;
  }
  if(counter >= 10)
    (*pcerr) << "TMatrix<std::complex<double> >: Something is wrong with the eigenVector sort" << std::endl;
  for(i=2; i < oddEven; i++) {
    if(abs(eigenvectors->_mdata[1][1]) < abs(eigenvectors->_mdata[1][i])) {
      eigenvectors->_switch_columns(1,i);
      if((oddEven*2) == nm)
        eigenvectors->_switch_columns(1+oddEven,oddEven+i);
    }
  }
  return eigenvectors;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template<>
void 
TML<std::complex<double> >::orderCoordinates(MLPtr<std::complex<double> >& eigenvalues,   MLPtr<std::complex<double> >& eigenvectors) {

//
// eigenvectors is now a 1 x n matrix (row) containing the eigenvalues 
// eigenvalues  is now a nrows x ncols matrix containg the corresponding eigenvectors (in uncompressed format) 
// 
// Internally, the beamline class uses the following ordering for phase space variables
// 
// index 0: x  
// index 1: y  
// index 2: ct    ( =z only when beta=1)  
// index 3: xp    (px/p)  
// index 4: yp    (py/p)  
// index 5: dp/p    
// 
// The matrix of eigenvectors returned from the eigensolver is *not* not ordered
// in a way consistent with the convention for phase space coordinates.    
// 
// Assuming A is real symplectic matrix, the eigenvalues and eigenvectors must obey the following constraints  
// 
// for each complex eigenvalue lambda, there is a corresponding eigenvalue lambda*  
// for each complex eigenvalue lambda, there is a corresponding eigenvalue 1/lambda;
// 
// det A = 1 (assuming suitable normalization)
//  
// consequences: pure real eigenvalues always appear in pairs: lambda and 1/lambda  
//               pure imag eigenvalues always appear in quads: jlambda, -j/lambda, -jlambda, j/lambda (1/2 int resonance ?)
//               complex conjugate pairs have corresponding complex conjugate eigenvectors.   
//
// The code below reorders columns of the eigenvector and eigenvalue matrices of a *symplectic matrix* 
// Assuming that  all the eigenvalues are complex (the most likely case) or that only one pair of eigenvalues is real.  
// The latter case corresponds to a coasting beam, i.e. no RF.    
// 
// In the general, stable case where all eigenvalues are complex, the column ordering 
// produced by the eigensolver would be as follows
// 
// 0:lambda_1 1:lambda_1* 2:lambda_2 3:lambda_2* 4:lambda_3 5:lambda_3*
//
// where lambda* denotes the conjugate of lambda.
// 
// Internally, the beamline libraries assume the following ordering for the coordinates:
//  
// 0:x 1:y 2:ct 3:xp 4:yp 5:dpp
// 
// Now, the time evolution of xp, yp and dpp is in quadrature with x ,y, and ct. This means that  
// lambda and lambda* must correspond to a couple of conjugate (in the Hamiltonian sense) variables
// (x,xp). Similarly, lambda and 1/lambda  correspond to Hamiltonian conjugates.
//
// The goal is to get an ordering of the form   
// 
// 0:lambda_1, 1:lambda_2, 2:lambda_3, 3:lambda_1*, 4:lambda_2*, 5:lambda_3* )
//
// 
//------------------------------------------------------------------------------------------------------

if(    (eigenvectors->_nrows == PHASESPACE) 
    && (eigenvectors->_ncols == PHASESPACE)) {
  
   int realCount = 0;
   for (int i=0; i<eigenvalues->_ncols; ++i){
      if ( std::imag(eigenvalues->_mdata[0][i]) == 0.0 ) ++realCount;
   } 
 

//-------------------------------------------------------------
  if(realCount == 0) {                                            // all eigenvalues are complex
//-------------------------------------------------------------

      eigenvectors->_switch_columns(3,4);
      eigenvectors->_switch_columns(1,2);
      eigenvectors->_switch_columns(2,3);
      eigenvalues->_switch_columns(3,4);
      eigenvalues->_switch_columns(1,2);
      eigenvalues->_switch_columns(2,3);
}
//-------------------------------------------------------------
  if( std::imag(eigenvalues->_mdata[0][0]) == 0.0) {          //   0:real  1:?     2:?        3:?        4:?        5:? 
//-------------------------------------------------------------
 
    if( std::imag(eigenvalues->_mdata[0][1]) == 0.0)         {  //  0:real, 1:real, 2:complex, 3:complex, 4: complex, 5: complex          
        eigenvectors->_switch_columns(0,2); 
        eigenvectors->_switch_columns(1,4); 
        eigenvectors->_switch_columns(4,5);
        eigenvalues->_switch_columns(0,2); 
        eigenvalues->_switch_columns(1,4); 
        eigenvalues->_switch_columns(4,5);

     } else if( std::imag(eigenvalues->_mdata[0][3]) == 0.0) {   //  0:real, 1:complex, 2:complex, 3:real, 4: complex, 5: complex 
        eigenvectors->_switch_columns(2,3);                       
        eigenvectors->_switch_columns(0,1);
        eigenvectors->_switch_columns(2,4);
        eigenvectors->_switch_columns(4,5);
        eigenvalues->_switch_columns(2,3);                       
        eigenvalues->_switch_columns(0,1);
        eigenvalues->_switch_columns(2,4);
        eigenvalues->_switch_columns(4,5);

     } else if( std::imag(eigenvalues->_mdata[0][5]) == 0.0) {   //   0:real, 1:complex, 2:complex, 3:complex, 4:complex, 5:real   
        eigenvectors->_switch_columns(2,3);
        eigenvectors->_switch_columns(0,1);
        eigenvectors->_switch_columns(1,2);
        eigenvalues->_switch_columns(2,3);
        eigenvalues->_switch_columns(0,1);
        eigenvalues->_switch_columns(1,2);
     }
//-------------------------------------------------------------
  }   else if( std::imag(eigenvalues->_mdata[0][2]) == 0.0) {    //   0:complex, 1:complex, 2:real, 3:?,  4: ?,       5: ?    
//-------------------------------------------------------------

     if( std::imag(eigenvalues->_mdata[0][5]) == 0.0) {         //   0:complex, 1:complex, 2:real, 3:complex, 4: complex, 5: real    
        eigenvectors->_switch_columns(1,3);
        eigenvalues->_switch_columns(1,3);
     } else if( std::imag(eigenvalues->_mdata[0][3]) == 0.0) {  //   0:complex, 1:complex, 2:real, 3:real,    4: complex, 5: complex    
        eigenvectors->_switch_columns(4,5);
        eigenvectors->_switch_columns(1,5);
        eigenvectors->_switch_columns(3,5);
        eigenvalues->_switch_columns(4,5);
        eigenvalues->_switch_columns(1,5);
        eigenvalues->_switch_columns(3,5);
     }

//-------------------------------------------------------------
  } else if( std::imag(eigenvalues->_mdata[0][4]) == 0.0) {    //   0:complex, 1:complex, 2:complex, 3:complex, 4: real, 5: real
//-------------------------------------------------------------
    
      eigenvectors->_switch_columns(3,4);
      eigenvectors->_switch_columns(1,2);
      eigenvectors->_switch_columns(2,3);
      eigenvalues->_switch_columns(3,4);
      eigenvalues->_switch_columns(1,2);
      eigenvalues->_switch_columns(2,3);
  }

} // If PHASESPASE

//--------------------------------------------------------------------------------------------------------
// At this point, although the vectors are ordered in conjugate pairs according to the prescription above,
// there there is no guarantee that index 0 corresponds to the "horizontal" mode etc...  
// The code below sorts the vectors such that mode 0 is "horizontal", mode 1 is "vertical" ... etc 
// Note that in presence of significant coupling, this simple minded approach will fail to distinguish
// the modes. 
//--------------------------------------------------------------------------------------------------------

int sortFlag = 1;
int counter  = 0;
int oddEven  = eigenvectors->_nrows/2;
int nrows    = eigenvectors->_nrows;

while ((sortFlag == 1)&& (counter < 10)) {
  sortFlag = 0;
  for(int i=1; i < oddEven; ++i) {
    if(abs(eigenvectors->_mdata[0][0]) < abs(eigenvectors->_mdata[0][i])) { // put the eigenvector with the largest "x" component first
      eigenvectors->_switch_columns(0,i);
      eigenvalues->_switch_columns(0,i);
      sortFlag = 1;
      if((oddEven*2) == nrows) { // true = the no of columns is even        // switch the order of corresponding conjugate coordinate
        eigenvectors->_switch_columns(oddEven,oddEven+i);
        eigenvalues->_switch_columns(oddEven,oddEven+i);
      }
    }
  }
  ++counter;
}

  if(counter >= 10) {
    (*pcerr) << "TML<double> : Something is wrong with the eigenVector sort. counter = " << counter << std::endl;
  }

  for(int i=2; i < oddEven; ++i) {
    if(abs(eigenvectors->_mdata[1][1]) < abs(eigenvectors->_mdata[1][i])) {
      eigenvectors->_switch_columns(1,i);
      eigenvalues->_switch_columns(1,i);

      if((oddEven*2) == nrows) {
        eigenvectors->_switch_columns(1+oddEven,oddEven+i);
        eigenvalues->_switch_columns(1+oddEven, oddEven+i);
      }
    }

  }
}


