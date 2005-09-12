/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BASIC TOOLKIT:  Low level utility C++ classes.
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
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <MathConstants.h>
#include <PhysicsConstants.h>

#include <TMatrix.h>


#ifdef WIN32
#include <Distribution.h> // for drand48
#endif

extern "C" { void rg_(int*, int*, double*, double*,double*, int*, double*,
                 int*, double*, int*); }

extern "C" { void cg_(int*, int*, double*, double*,double*,double*, int*,
                      double*,double*,double*, double*, double*,int*); }


using FNAL::Complex;

static const int PHASESPACE = 6;
#define M_SMALL  1.0e-30

RandomOrthogonal::~RandomOrthogonal()
{
  for( int i = 0; i < _dim; i++ ) {
    delete [] _omitted[i];
    delete [] _lowerTheta[i];
    delete [] _upperTheta[i];
    delete [] _rangeTheta[i];
  }

  delete [] _omitted;
  delete [] _lowerTheta;
  delete [] _upperTheta;
  delete [] _rangeTheta;
}


void RandomOrthogonal::omitIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) {
    _omitted[i][j] = true;
    _omitted[j][i] = true;
  }
}


void RandomOrthogonal::omitIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) {
    this->omitIndex( n, i );
  }
}


void RandomOrthogonal::includeIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) {
    _omitted[i][j] = false;
    _omitted[j][i] = false;
  }
}


void RandomOrthogonal::includeIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) {
    this->includeIndex( n, i );
  }
}


void RandomOrthogonal::setNumberOfPasses( int n )
{
  if( 0 < n ) { _passes = n; }
}


void RandomOrthogonal::setRange( int i, int j, double lo, double hi )
{
  // Check and condition the arguments
  if( i < 0  ||  _dim <= i  ||
      j < 0  ||  _dim <= j  ||
      hi <= lo )
  { return; }

  if( std::abs(lo) < 1.0e-12            ) { lo = 0.0;     }
  if( std::abs(hi - M_TWOPI ) < 1.0e-12 ) { hi = M_TWOPI; }

  while( lo < 0.0      ) { lo += M_TWOPI; }
  while( lo > M_TWOPI  ) { lo -= M_TWOPI; }
  while( hi < 0.0      ) { hi += M_TWOPI; }
  while( hi > M_TWOPI  ) { hi -= M_TWOPI; }

  // Set the range
  _lowerTheta[i][j] = lo;
  _lowerTheta[j][i] = lo;
  _upperTheta[i][j] = hi;
  _upperTheta[j][i] = hi;
  _rangeTheta[i][j] = hi - lo;
  _rangeTheta[j][i] = hi - lo;
}


void RandomOrthogonal::setRange( int n, double lo, double hi )
{
  for( int i = 0; i < _dim; i++ ) {
    this->setRange( n, i, lo, hi );
  }
}


TMatrix<double> RandomOrthogonal::build()
{
  // This was programmed very inefficiently
  // but quickly. It will have to be redone
  // if used many times in an application.

  int i, j, p;
  double theta, cs, sn, tmp;
  TMatrix<double> R( "I", _dim );
  TMatrix<double> U( "I", _dim );
  TMatrix<double> W( "I", _dim );

  for( p = 0; p < _passes; p++ ) {
    for( i = 0; i < _dim-1; i++ ) {
      for( j = i+1; j < _dim; j++ ) {
        if( !(_omitted[i][j]) ) {
          theta = _lowerTheta[i][j] + drand48()*_rangeTheta[i][j];
          cs = cos( theta );
          sn = sin( theta );
          W(i,i) =  cs;  W(i,j) = sn;
          W(j,i) = -sn;  W(j,j) = cs;
          R = R*W;
          W = U;
        }
      }
    }
  }

  return R;
}


// Specializations for class TMatrix 
// =======================================================

TMatrix<double> real( const TMatrix<FNAL::Complex>& x )
{
  int r = x.rows();
  int c = x.cols();
  TMatrix<double> ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = real((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}


TMatrix<double> imag( const TMatrix<FNAL::Complex>& x )
{
  int r = x.rows();
  int c = x.cols();
  TMatrix<double> ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = imag((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}


// The four eigen routines written here are anachronistic
// messes! They should be rewritten from scratch and
// fully templatized!

TMatrix<FNAL::Complex> TMatrix<double>::eigenValues() 
{
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  int* iv1 = new int[cols()];
  double* fv1 = new double[cols()];
  double* b = new double[rows()*cols()];
  double* c = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;
  int realCount = 0;
  TMatrix<FNAL::Complex> tmp(rows(),cols(),complex_0);
  TMatrix<FNAL::Complex> tmp1(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      b[k] = _ml->_m[j][i];       // the rg_ routine uses the transpose
      k++;
    }
  }
  rg_(&nm,&n,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    cerr << "TMatrix<double>: Error in eigenvalue routine. error = ";
    cerr << ierr << endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return tmp;
  }
  TMatrix<double> z1(nm,n,0.0);
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = c[k];
      k++;
    }
  }

  i = 0;
  k = 0;
  j = 0;
  while( i< nm) {
    if(wi[i] == 0){
      realCount++;
      for(j=0; j<n; j++) {
        tmp(j,k) = Complex(z1(i,j),0.0);
      }
      tmp1(0,k) = Complex(wr[i],0.0);
      k++;
      i++;
    } else {
      tmp1(0,k)   = Complex(wr[i],wi[i]);
      tmp1(0,k+1) = Complex(wr[i+1],wi[i+1]);
      for(j=0; j<n; j++) {
        tmp(j,k)   = Complex(z1(i,j),z1(i+1,j));
        tmp(j,k+1) = Complex(z1(i,j),-z1(i+1,j));
      }
      k += 2;
      i += 2;
    }
  }
  if((nm == PHASESPACE) && (n == PHASESPACE)) {
    if(realCount == 0) {
      tmp._switch_columns(3,4);
      tmp1._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp1._switch_columns(1,2);
      tmp._switch_columns(2,3);
      tmp1._switch_columns(2,3);
    }
    if(imag(tmp1(0,0)) == 0.0) {
      if(imag(tmp1(0,1)) == 0.0) {
        tmp._switch_columns(0,2);
        tmp1._switch_columns(0,2);
        tmp._switch_columns(1,4);
        tmp1._switch_columns(1,4);
        tmp._switch_columns(4,5);
        tmp1._switch_columns(4,5);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp1._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp1._switch_columns(0,1);
        tmp._switch_columns(2,4);
        tmp1._switch_columns(2,4);
        tmp._switch_columns(4,5);
        tmp1._switch_columns(4,5);
      } else if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp1._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp1._switch_columns(0,1);
        tmp._switch_columns(1,2);
        tmp1._switch_columns(1,2);
      }
    } else if(imag(tmp1(0,2)) == 0.0) {
      if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(1,3);
        tmp1._switch_columns(1,3);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(4,5);
        tmp1._switch_columns(4,5);
        tmp._switch_columns(1,5);
        tmp1._switch_columns(1,5);
        tmp._switch_columns(3,5);
        tmp1._switch_columns(3,5);
      }
    } else if(imag(tmp1(0,4)) == 0.0) {
      tmp._switch_columns(3,4);
      tmp1._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp1._switch_columns(1,2);
      tmp._switch_columns(2,3);
      tmp1._switch_columns(2,3);
    }
  }

  delete []wr;
  delete []wi;
  delete []iv1;
  delete []fv1;
  delete []b;
  delete []c;

  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; i++) {
      if(abs(tmp(0,0)) < abs(tmp(0,i))) {
        tmp._switch_columns(0,i);
        tmp1._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm) {
          tmp._switch_columns(oddEven,oddEven+i);
          tmp1._switch_columns(oddEven,oddEven+i);
        }
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<double>: Something is wrong with the eigenValue sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(tmp(1,1)) < abs(tmp(1,i))) {
      tmp._switch_columns(1,i);
      tmp1._switch_columns(1,i);
      sortFlag = 1;
      if((oddEven*2) == nm) {
        tmp._switch_columns(oddEven+1,oddEven+i);
        tmp1._switch_columns(oddEven+1,oddEven+i);
      }
    }
  }
  return tmp1;

}

TMatrix<FNAL::Complex> TMatrix<double>::eigenVectors() 
{
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  int* iv1 = new int[cols()];
  double* fv1 = new double[cols()];
  double* b = new double[rows()*cols()];
  double* c = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;
  int realCount = 0;
  TMatrix<FNAL::Complex> tmp(rows(),cols(),complex_0);
  TMatrix<FNAL::Complex> tmp1(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      b[k] = _ml->_m[j][i];       // the rg_ routine uses the transpose
      k++;
    }
  }
  rg_(&nm,&n,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    cerr << "TMatrix<double>: Error in eigenvector routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return tmp;
  }
  TMatrix<double> z1(nm,n,0.0);
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = c[k];
      k++;
    }
  }

  i = 0;
  k = 0;
  j = 0;
  while( i< nm) {
    if(wi[i] == 0){
      realCount++;
      for(j=0; j<n; j++) {
        tmp(j,k) = Complex(z1(i,j),0.0);
      }
      tmp1(0,k) = Complex(wr[i],0.0);
      k++;
      i++;
    } else {
      tmp1(0,k)   = Complex(wr[i],wi[i]);
      tmp1(0,k+1) = Complex(wr[i+1],wi[i+1]);
      for(j=0; j<n; j++) {
        tmp(j,k)   = Complex(z1(i,j),z1(i+1,j));
        tmp(j,k+1) = Complex(z1(i,j),-z1(i+1,j));
      }
      k += 2;
      i += 2;
    }
  }
  if((nm == PHASESPACE) && (n == PHASESPACE)) {
    if(realCount == 0) {
      tmp._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp._switch_columns(2,3);
    }
    if(imag(tmp1(0,0)) == 0.0) {
      if(imag(tmp1(0,1)) == 0.0) {
        tmp._switch_columns(0,2);
        tmp._switch_columns(1,4);
        tmp._switch_columns(4,5);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp._switch_columns(2,4);
        tmp._switch_columns(4,5);
      } else if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(2,3);
        tmp._switch_columns(0,1);
        tmp._switch_columns(1,2);
      }
    } else if(imag(tmp1(0,2)) == 0.0) {
      if(imag(tmp1(0,5)) == 0.0) {
        tmp._switch_columns(1,3);
      } else if(imag(tmp1(0,3)) == 0.0) {
        tmp._switch_columns(4,5);
        tmp._switch_columns(1,5);
        tmp._switch_columns(3,5);
      }
    } else if(imag(tmp1(0,4)) == 0.0) {
      tmp._switch_columns(3,4);
      tmp._switch_columns(1,2);
      tmp._switch_columns(2,3);
    }
  }

  delete []wr;
  delete []wi;
  delete []iv1;
  delete []fv1;
  delete []b;
  delete []c;
  int sortFlag = 1;
  int counter = 0;
  while ((sortFlag == 1)&& (counter < 10)) {
    sortFlag = 0;
    for(i=1; i < oddEven; i++) {
      if(abs(tmp(0,0)) < abs(tmp(0,i))) {
        tmp._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
          tmp._switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<double>: Something is wrong with the eigenVector sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(tmp(1,1)) < abs(tmp(1,i))) {
      tmp._switch_columns(1,i);
      if((oddEven*2) == nm)
        tmp._switch_columns(1+oddEven,oddEven+i);
    }
  }
  return tmp;

}


TMatrix<FNAL::Complex> TMatrix<FNAL::Complex>::eigenValues() {
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  double* fv1 = new double[cols()];
  double* fv2 = new double[cols()];
  double* fv3 = new double[cols()];
  double* br = new double[rows()*cols()];
  double* bi = new double[rows()*cols()];
  double* cr = new double[rows()*cols()];
  double* ci = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;
  TMatrix<FNAL::Complex> tmp(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      br[k] = real(_ml->_m[j][i]); // the cg_ routine uses the transpose
      bi[k] = imag(_ml->_m[j][i]); // the cg_ routine uses the transpose
      k++;
    }
  }
  TMatrix<FNAL::Complex> z1(nm,n,complex_0);
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);
  if(ierr != 0) {
    cerr << "TMatrix<FNAL::Complex>: Error in eigenvalue routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []fv1;
    delete []fv2;
    delete []fv3;
    delete []br;
    delete []bi;
    delete []cr;
    delete []ci;
    return tmp;
  }
  for(i = 0; i< cols(); i++)
    tmp._ml->_m[0][i] = Complex(wr[i],wi[i]);
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = Complex(cr[k],ci[k]);
      k++;
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
      if(abs(z1(0,0)) < abs(z1(0,i))) {
        z1._switch_columns(0,i);
        tmp._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
          z1._switch_columns(oddEven,oddEven+i);
          tmp._switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<FNAL::Complex>: Something is wrong with the eigenValue sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(z1(1,1)) < abs(z1(1,i))) {
      z1._switch_columns(1,i);
      tmp._switch_columns(1,i);
      if((oddEven*2) == nm)
        z1._switch_columns(1+oddEven,oddEven+i);
        tmp._switch_columns(1+oddEven,oddEven+i);
    }
  }
  return tmp;
}


TMatrix<FNAL::Complex> TMatrix<FNAL::Complex>::eigenVectors() {
  int nm = rows();
  int n  = cols();
  int matz = 1;
  double* wr = new double[cols()];
  double* wi = new double[cols()];
  double* fv1 = new double[cols()];
  double* fv2 = new double[cols()];
  double* fv3 = new double[cols()];
  double* br = new double[rows()*cols()];
  double* bi = new double[rows()*cols()];
  double* cr = new double[rows()*cols()];
  double* ci = new double[rows()*cols()];
  int ierr,i,j,k;
  int oddEven = nm/2;

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      br[k] = real(_ml->_m[j][i]); // the cg_ routine uses the transpose
      bi[k] = imag(_ml->_m[j][i]); // the cg_ routine uses the transpose
      k++;
    }
  }
  TMatrix<FNAL::Complex> z1(nm,n,complex_0);
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);
  if(ierr != 0) {
    cerr << "TMatrix<FNAL::Complex>: Error in eigenvector routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []fv1;
    delete []fv2;
    delete []fv3;
    delete []br;
    delete []bi;
    delete []cr;
    delete []ci;
    return z1;
  }
  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      z1(i,j) = Complex(cr[k],ci[k]);
      k++;
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
      if(abs(z1(0,0)) < abs(z1(0,i))) {
        z1._switch_columns(0,i);
        sortFlag = 1;
        if((oddEven*2) == nm)
          z1._switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10)
    cerr << "TMatrix<FNAL::Complex>: Something is wrong with the eigenVector sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(z1(1,1)) < abs(z1(1,i))) {
      z1._switch_columns(1,i);
      if((oddEven*2) == nm)
        z1._switch_columns(1+oddEven,oddEven+i);
    }
  }
  return z1;
}


// Implementation of derived class MatrixC
// Done to replicate member function dagger()

MatrixC::MatrixC()
: TMatrix<Complex>()
{
}

MatrixC::MatrixC( const MatrixC& x )
: TMatrix<Complex>( (const TMatrix<Complex>&) x )
{
}

MatrixC::MatrixC( const TMatrix<Complex>& x )
: TMatrix<Complex>( x )
{
}

MatrixC::MatrixC( int i )
: TMatrix<Complex>(i)
{
}

MatrixC::MatrixC( int rows, int columns )
: TMatrix<Complex>( rows, columns )
{
}

MatrixC::MatrixC( int rows, int columns, const FNAL::Complex& initval )
: TMatrix<Complex>( rows, columns, initval )
{
}

MatrixC::MatrixC( int rows, int columns, FNAL::Complex* initval )
: TMatrix<Complex>( rows, columns, initval )
{
}

MatrixC::MatrixC( const char* flag, int dimension )
: TMatrix<Complex>( flag, dimension )
{
}

MatrixC::MatrixC( const TMatrix<double>& x )
: TMatrix<Complex>( x.rows(), x.cols() )
{
  int i, j;
  for( i=0; i< _ml->_r; i++ ) {
    for( j=0; j< _ml->_c; j++ ) {
      _ml->_m[i][j] = Complex( x(i,j), 0.0 );
    }
  }
}

MatrixC::~MatrixC()
{
}


MatrixC MatrixC::dagger() const
{
  MatrixC z( cols(), rows(), complex_0 );
  TML<Complex>* zPtr = z._ml;

  for(int row = 0; row < rows(); row++) {
    for(int col = 0; col < cols(); col++) {
      zPtr->_m[col][row] = conj(_ml->_m[row][col]);
    }
  }
  z._stacked = 1;
  return z;
}

MatrixD real( const MatrixC& x )
{
  int r = x.rows();
  int c = x.cols();
  MatrixD ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = real((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}


MatrixD imag( const MatrixC& x )
{
  int r = x.rows();
  int c = x.cols();
  MatrixD ret( r, c );
  for( int i = 0; i < r; i++ ) {
    for( int j = 0; j < c; j++ ) {
      ret(i,j) = imag((const FNAL::Complex&) (x(i,j)));
    }
  }
  return ret;
}

TMatrix<FNAL::Complex > operator*(const TMatrix<FNAL::Complex >& x, const TMatrix<double>& y)
{
  TMatrix<FNAL::Complex > z(x.rows(),y.cols(),complex_0);
  TML<FNAL::Complex >* xPtr = x._ml;
  TML<double>* yPtr = y._ml;
  TML<FNAL::Complex >* zPtr = z._ml;
  if( x.cols() != y.rows()) {
    throw( TMatrix<double>::Incompatible( x.rows(), x.cols(), y.rows(), y.cols(),
           "TMatrix<FNAL::Complex > operator*(const TMatrix<FNAL::Complex >& x, const TMatrix<double>& y)" ) );
  }
  FNAL::Complex tmp;
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      FNAL::Complex sum = complex_0;
      for(int i = 0; i < x.cols(); i++) {
        tmp = xPtr->_m[row][i] * FNAL::Complex(yPtr->_m[i][col],0.0);
        sum += tmp;
        if(abs(sum) < M_SMALL*abs(tmp))
          sum = complex_0;
      }
      zPtr->_m[row][col] = sum;
    }
  }
  z._stacked = true;
  return z;
}


TMatrix<FNAL::Complex> operator*(const TMatrix<double>& y, const TMatrix<FNAL::Complex>& x)
{
  TMatrix<FNAL::Complex> z(y.rows(),x.cols(),complex_0);
  TML<FNAL::Complex>* xPtr = x._ml;
  TML<double>* yPtr = y._ml;
  TML<FNAL::Complex>* zPtr = z._ml;
  if( y.cols() != x.rows()) {
    throw( TMatrix<double>::Incompatible( x.rows(), x.cols(), y.rows(), y.cols(),
           "TMatrix<FNAL::Complex> operator*(const TMatrix<double>& x, const TMatrix<FNAL::Complex>& y)" ) );
  }
  FNAL::Complex tmp;
  for(int row = 0; row < y.rows(); row++) {
    for(int col = 0; col < x.cols(); col++){
      FNAL::Complex sum = complex_0;
      for(int i = 0; i < y.cols(); i++) {
        tmp = FNAL::Complex(yPtr->_m[row][i],0.0) * xPtr->_m[i][col];
        sum += tmp;
        if(abs(sum) < M_SMALL*abs(tmp))
          sum = complex_0;
      }
      zPtr->_m[row][col] = sum;
    }
  }
  z._stacked = true;
  return z;
}
