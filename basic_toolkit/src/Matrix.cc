/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.1
******                                    
******  File:      Matrix.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <MathConstants.h>


#include "Matrix.h"
//extern "C" {strstr(char*,char*);} // this may need to be commented out
				  // on some systems.

extern "C" { void rg_(int*, int*, double*, double*,double*, int*, double*,
		 int*, double*, int*); }

extern "C" { void cg_(int*, int*, double*, double*,double*,double*, int*, 
		      double*,double*,double*, double*, double*,int*); }


using std::abs;
using FNAL::Complex;

const double tiny = double(1e-20);
const double limit = double(1e-14);
const int PHASESPACE = 6;

#ifdef OBJECT_DEBUG
int MatrixD::objectCount = 0;
int MatrixC::objectCount = 0;
int MatrixI::objectCount = 0;
#endif

#define M_SMALL  1.0e-30
#define WID setw(8)
#define PREC setprecision(8)


MatrixD::MatrixD() : stacked(0) {
  ml = new MLD;
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixD::MatrixD(int rows) : stacked(0) {
  ml = new MLD(rows,1,0.0);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixD::MatrixD(int rows, int columns) : stacked(0) {
  ml = new MLD(rows,columns,0.0);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixD::MatrixD(int rows, int columns, double initval) : stacked(0) {
  ml = new MLD(rows,columns,initval);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixD::MatrixD(int rows, int columns, double* initval) : stacked(0) {
  ml = new MLD(rows,columns,initval);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixD::MatrixD(const char* flag, int dimension) : stacked(0) {

  int    i;
  double tmp_float = dimension;

#ifdef OBJECT_DEBUG
  objectCount++;
#endif

  ml = new MLD(dimension,dimension,0.0);
  
  if( ((double) (dimension/2) != tmp_float/2.0) && 
      (flag[0] == 'J')
    )
    error("dimension must be even","for J matrix");

  if (flag[0]  == 'I') {
    for (i = 0; i< dimension; i++) {
      ml->m[i][i] = 1.0;
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      ml->m[i-dimension/2][i] = 1;
      ml->m[i][i-dimension/2] = -1;
    }
  } else
    error("Unknown flag: ",flag);
}

/* ??? OLD FORM: REMOVE
MatrixD::MatrixD(const char* flag, int dimension) {
  int i;
  float tmp_float = dimension/2.0;
  ml = new MLD(dimension,dimension,0.0);
  
  if(((dimension/2) != tmp_float) && (flag[0] == 'J'))
    error("dimension must be even","for J and I matrix");
  if (flag[0]  == 'I') {
    for (i = 0; i< dimension; i++) {
      ml->m[i][i] = 1.0;
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      ml->m[i-dimension/2][i] = 1;
      ml->m[i][i-dimension/2] = -1;
    }
  } else
    error("Unknown flag: ",flag);
}
*/
MatrixD::MatrixD(const MatrixD& X) {
  stacked = X.stacked;
  ml = X.ml;
  (ml->rc)++;
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixD::~MatrixD() {
  if( --(ml->rc) == 0) delete ml;
#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

MatrixD& MatrixD::operator=(const MatrixD& x) {
  if(this != &x) {
    if( --(ml->rc) == 0) delete ml;
#ifndef FORBIDSTACKED
    if(x.stacked) {
      ml = x.ml;
      (ml->rc)++;
    } else {
#endif
      ml = new MLD;
      *ml = *(x.ml);
#ifndef FORBIDSTACKED
    }
#endif
  }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

MatrixD& MatrixD::DeepCopy(const MatrixD& x) {
 if(this != &x) {
   if( --(ml->rc) == 0) delete ml;
     ml = new MLD;
     *ml = *(x.ml);
 }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

void MatrixD::error( const char* msg1, const char* msg2 ) const {
  double temp = 0.0;
  cerr << "MatrixD error: " << msg1 << " " << msg2  << endl;
  double temp1 = 1.0/temp;
//  exit(1);
}

char operator==( const MatrixD& x, const MatrixD& y ) {
 return *(x.ml) == *(y.ml);
}

char operator==( const MatrixD& x, const double& y ) {
 return *(x.ml) == y;
}

char operator==( const double& y, const MatrixD& x ){
 return *(x.ml) == y;
}

char operator!=( const MatrixD& x, const MatrixD& y ) {
 return !( x == y );
}

char operator!=( const MatrixD& x, const double& y ) {
 return !( x == y );
}

char operator!=( const double& x, const MatrixD& y ) {
 return !( x == y );
}

void MatrixD::operator+=( const double& x ) {
 ml->operator+=( x );
}

void MatrixD::operator-=( const double& x ) {
 ml->operator-=( x );
}

MatrixD MatrixD::Square() const {
  int d;

  if( rows() == cols() ) return *this;

  if( rows() < cols() ) d = rows();
  else                  d = cols();

  int     i, j;
  MatrixD z( d, d, 0.0 );
  MLD*    zPtr = z.ml;

  for( i = 0; i < d; i++ )
    for( j = 0; j < d; j++ ) 
      zPtr->m[i][j] = ml->m[i][j];
      
  return z;
}

MatrixD MatrixD::transpose() const {
  MatrixD z(cols(),rows(),0.0);
  MLD* zPtr = z.ml;

  for (int row = 0; row < rows(); row++)  {
    for(int col = 0; col < cols(); col++)
      zPtr->m[col][row] = ml->m[row][col];
  }
  z.stacked = 1;
  return z;
}

double MatrixD::trace() {
  double temp = 0.0;

  if(rows()  != cols())
    error("MatrixD must be square to take a trace!\n");
  for (int i = 0; i < rows(); i++)  {
      temp += ml->m[i][i];
  }
  return temp;
}

double MatrixD::determinant() {
  if(rows()  != cols())
    error("MatrixD must be square for determinant()");
  int* indx = new int[cols()]; // create the "index vector
			       // see pp 38. in Numerical Recipes
  int d;
  // perform the decomposition once:

  MatrixD decomp = lu_decompose(indx,d);
  double determinant = d;
  for(int i=0; i < cols() ; i++)
    determinant *= decomp.ml->m[i][i];
  delete []indx;
  return determinant;
}

MatrixD MatrixD::inverse() const {
  if(rows()  != cols())
    error("MatrixD must be square for inverse()");
  MatrixD Y("I",rows()); // create an identity MatrixD
  int* indx = new int[cols()];  // create the "index vector"
  MatrixD B(cols());     // see Press & Flannery
  int d;
  // perform the decomposition once:
  MatrixD decomp = lu_decompose(indx,d);
  for(int col = 0; col < cols(); col++){
    B.copy_column(Y,col,0);
    decomp.lu_back_subst(indx,B);
    Y.copy_column(B,0,col);
  }
  delete []indx;
  return Y;
}

MatrixC MatrixD::eigenValues() {
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
  MatrixC tmp(rows(),cols(),complex_0);
  MatrixC tmp1(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      b[k] = ml->m[j][i];	// the rg_ routine uses the transpose
      k++;
    }
  }
  rg_(&nm,&n,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    cerr << "MatrixD: Error in eigenvalue routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return tmp;
  }
  MatrixD z1(nm,n,0.0);
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
      tmp.switch_columns(3,4);
      tmp1.switch_columns(3,4);
      tmp.switch_columns(1,2);
      tmp1.switch_columns(1,2);
      tmp.switch_columns(2,3);
      tmp1.switch_columns(2,3);
    }
    if(imag(tmp1(0,0)) == 0.0) {
      if(imag(tmp1(0,1)) == 0.0) {
	tmp.switch_columns(0,2);
	tmp1.switch_columns(0,2);
	tmp.switch_columns(1,4);
	tmp1.switch_columns(1,4);
	tmp.switch_columns(4,5);
	tmp1.switch_columns(4,5);
      } else if(imag(tmp1(0,3)) == 0.0) {
	tmp.switch_columns(2,3);
	tmp1.switch_columns(2,3);
	tmp.switch_columns(0,1);
	tmp1.switch_columns(0,1);
	tmp.switch_columns(2,4);
	tmp1.switch_columns(2,4);
	tmp.switch_columns(4,5);
	tmp1.switch_columns(4,5);
      } else if(imag(tmp1(0,5)) == 0.0) {
	tmp.switch_columns(2,3);
	tmp1.switch_columns(2,3);
	tmp.switch_columns(0,1);
	tmp1.switch_columns(0,1);
	tmp.switch_columns(1,2);
	tmp1.switch_columns(1,2);
      }
    } else if(imag(tmp1(0,2)) == 0.0) {
      if(imag(tmp1(0,5)) == 0.0) {
	tmp.switch_columns(1,3);
	tmp1.switch_columns(1,3);
      } else if(imag(tmp1(0,3)) == 0.0) {
	tmp.switch_columns(4,5);
	tmp1.switch_columns(4,5);
	tmp.switch_columns(1,5);
	tmp1.switch_columns(1,5);
	tmp.switch_columns(3,5);
	tmp1.switch_columns(3,5);
      }
    } else if(imag(tmp1(0,4)) == 0.0) {
      tmp.switch_columns(3,4);
      tmp1.switch_columns(3,4);
      tmp.switch_columns(1,2);
      tmp1.switch_columns(1,2);
      tmp.switch_columns(2,3);
      tmp1.switch_columns(2,3);
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
	tmp.switch_columns(0,i); 
	tmp1.switch_columns(0,i); 
	sortFlag = 1;
	if((oddEven*2) == nm) {
	  tmp.switch_columns(oddEven,oddEven+i);
	  tmp1.switch_columns(oddEven,oddEven+i);
	}
      }
    }
    counter++;
  }
  if(counter >= 10) 
    cerr << "MatrixD: Something is wrong with the eigenValue sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(tmp(1,1)) < abs(tmp(1,i))) {
      tmp.switch_columns(1,i);
      tmp1.switch_columns(1,i);
      sortFlag = 1;
      if((oddEven*2) == nm) {
	tmp.switch_columns(oddEven+1,oddEven+i);
	tmp1.switch_columns(oddEven+1,oddEven+i);
      }
    }
  }
  return tmp1;

}

MatrixC MatrixD::eigenVectors() {
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
  MatrixC tmp(rows(),cols(),complex_0);
  MatrixC tmp1(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      b[k] = ml->m[j][i];	// the rg_ routine uses the transpose
      k++;
    }
  }
  rg_(&nm,&n,b,wr,wi,&matz,c,iv1,fv1,&ierr);

  if(ierr != 0) {
    cerr << "MatrixD: Error in eigenvector routine. error = ";
    cerr << ierr<< endl;
    delete []wr;
    delete []wi;
    delete []iv1;
    delete []fv1;
    delete []b;
    delete []c;
    return tmp;
  }
  MatrixD z1(nm,n,0.0);
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
      tmp.switch_columns(3,4);
      tmp.switch_columns(1,2);
      tmp.switch_columns(2,3);
    }
    if(imag(tmp1(0,0)) == 0.0) {
      if(imag(tmp1(0,1)) == 0.0) {
	tmp.switch_columns(0,2);
	tmp.switch_columns(1,4);
	tmp.switch_columns(4,5);
      } else if(imag(tmp1(0,3)) == 0.0) {
	tmp.switch_columns(2,3);
	tmp.switch_columns(0,1);
	tmp.switch_columns(2,4);
	tmp.switch_columns(4,5);
      } else if(imag(tmp1(0,5)) == 0.0) {
	tmp.switch_columns(2,3);
	tmp.switch_columns(0,1);
	tmp.switch_columns(1,2);
      }
    } else if(imag(tmp1(0,2)) == 0.0) {
      if(imag(tmp1(0,5)) == 0.0) {
	tmp.switch_columns(1,3);
      } else if(imag(tmp1(0,3)) == 0.0) {
	tmp.switch_columns(4,5);
	tmp.switch_columns(1,5);
	tmp.switch_columns(3,5);
      }
    } else if(imag(tmp1(0,4)) == 0.0) {
      tmp.switch_columns(3,4);
      tmp.switch_columns(1,2);
      tmp.switch_columns(2,3);
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
	tmp.switch_columns(0,i); 
	sortFlag = 1;
	if((oddEven*2) == nm)
	  tmp.switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10) 
    cerr << "MatrixD: Something is wrong with the eigenVector sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(tmp(1,1)) < abs(tmp(1,i))) {
      tmp.switch_columns(1,i);
      if((oddEven*2) == nm)
	tmp.switch_columns(1+oddEven,oddEven+i);
    }
  }
  return tmp;

}
// operators

double& MatrixD::operator()(int i, int j) {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** double& MatrixD::operator()(int i, int j) " << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for "<< i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
 return ml->m[i][j];
}

double  MatrixD::operator()(int i, int j) const {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** double& MatrixD::operator()(int i, int j) " << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for "<< i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
 return ml->m[i][j];
}

double MatrixD::getElement(int i, int j) const {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** double& MatrixD::getElement(int i, int j) " << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for "<< i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
 return ml->m[i][j];
}

double& MatrixD::operator()(int i) {
  if( rows() == 1 ) return ml->m[0][i];
  if( cols() == 1 ) return ml->m[i][0];
  cerr << "\n*** ERROR *** " << endl;
  cerr << "*** ERROR *** double& MatrixD::operator()(int i) " << endl;
  cerr << "*** ERROR *** Can only be used with single row  " << endl;
  cerr << "*** ERROR *** or column matrices. " << endl;
  cerr << "*** ERROR *** " << endl;
  exit(1);
  return ml->m[0][0];
}

ostream& operator<<(ostream& os, const MatrixD& x)
{
  int i,j;
  
  for(i=0; i< x.rows(); i++) {
    os << "( ";
    for(j=0; j< x.cols(); j++) {
      os  << x.ml->m[i][j] << ", ";
    }
    os << " )\n";
  }
  return os << endl;
}


MatrixD operator+(const MatrixD& x, const MatrixD& y) {
  MatrixD z(x.rows(),x.cols(),0.0);
  MLD* xPtr = x.ml;
  MLD* yPtr = y.ml;
  MLD* zPtr = z.ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols()))
    x.error("Must have equal dimensions for addition!");
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] + yPtr->m[i][j];
      if(fabs(zPtr->m[i][j]) < M_SMALL*fabs(yPtr->m[i][j]))
	zPtr->m[i][j] = 0.0;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixD operator+(const MatrixD& x, const double& y) {
 MatrixD z; 
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}

MatrixD operator+( const double& y, const MatrixD& x ) {
 MatrixD z;
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}

MatrixD operator-(const MatrixD& x) {
  MatrixD z(x.rows(),x.cols(),0.0);
  MLD* xPtr = x.ml;
  MLD* zPtr = z.ml;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = -1.0* xPtr->m[i][j];
    }
  }
  z.stacked = 1;
  return z;
}

MatrixD operator-(const MatrixD& x, const MatrixD& y) {
  MatrixD z(x.rows(),x.cols(),0.0);
  MLD* xPtr = x.ml;
  MLD* yPtr = y.ml;
  MLD* zPtr = z.ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols()))
    x.error("Must have equal dimensions for addition!");
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] - yPtr->m[i][j];
      if(fabs(zPtr->m[i][j]) < M_SMALL*fabs(yPtr->m[i][j]))
	zPtr->m[i][j] = 0.0;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixD operator-(const MatrixD& x, const double& y) {
 MatrixD z; 
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixD operator-( const double& y, const MatrixD& x ) {
 MatrixD z;
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixD operator*( const MatrixD& x, const double y) {
  MatrixD z(x.rows(),x.cols(),0.0);
  MLD* xPtr = x.ml;
  MLD* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixD operator*( const double y, const MatrixD& x) {
  MatrixD z(x.rows(),x.cols(),0.0);
  MLD* xPtr = x.ml;
  MLD* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixD operator*(const MatrixD& x, const MatrixD& y)  {
  MatrixD z(x.rows(),y.cols(),0.0);
  MLD* xPtr = x.ml;
  MLD* yPtr = y.ml;
  MLD* zPtr = z.ml;
  if( x.cols() != y.rows())
    x.error("# of rows of 2nd matrix must equal # cols of 1st for multiply");
  double tmp;
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      double sum = 0.0;
      for(int i = 0; i < x.cols(); i++) {
	tmp = xPtr->m[row][i] * yPtr->m[i][col];
	sum += tmp;
	if(fabs(sum) < M_SMALL*fabs(tmp))
	  sum = 0.0;
      }
      zPtr->m[row][col] = sum;
    }
  }
  z.stacked = 1;
  return z;
}
MatrixD operator/( const MatrixD& x, const double y) {
  MatrixD z(x.rows(),x.cols(),0.0);
  MLD* xPtr = x.ml;
  MLD* zPtr = z.ml;
  if(y == 0.0)
    cerr << "MatrixD:operator/ divide by zero" << endl;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] / y;
    }
  }
 z.stacked = 1;
 return z;
}
MatrixD operator/( const double y, MatrixD& x) {
  return (y*x.inverse());
}
MatrixD operator/( MatrixD& y, MatrixD& x) {
  return (y*x.inverse());
}
/********************************************************
The private support functions for determinant & inverse:
*********************************************************/

// copy the from_col of mm to the to_col of "this"

void MatrixD::copy_column(MatrixD& mm, int from_col, int to_col){
  if(rows()  != mm.rows())
    error("number of rows must be equal for copy_column()");
  for(int row=0; row < rows(); row++)
    ml->m[row][to_col] = mm.ml->m[row][from_col];
}

void MatrixD::switch_columns(int col1, int col2) {
  MatrixD temp(rows());
  
  int row; // O.K.
  
  for(row = 0; row < rows(); row++)
    // temporarily store col 1:
    temp.ml->m[row][0] = ml->m[row][col1];
  for(row = 0; row < rows(); row++)
    ml->m[row][col1] = ml->m[row][col2]; // move col2 to col1
  for(row = 0; row < rows(); row++)
    ml->m[row][col2] = temp.ml->m[row][0]; // move temp to col2
}

void MatrixD::switch_rows(int row1, int row2) {
  MatrixD temp(cols());
  int col; // O.K.
  
  for(col = 0; col < cols(); col++)
    // temporarily store row 1:
    temp.ml->m[col][0] = ml->m[row1][col];
  for(col = 0; col < cols(); col++)
    ml->m[row1][col] = ml->m[row2][col]; // move row2 to row1
  for(col = 0; col < cols(); col++)
    ml->m[row2][col] = temp.ml->m[col][0]; // move temp to row2
}

// scale a matrix (used in L-U decomposition)

MatrixD MatrixD::scale()  {
  double temp;
  if (rows() <= 0 || cols() <= 0)
    error(" bad MatrixD for scale()");
  if (rows() != cols())
    error(" MatrixD must be square for scale()");   
  MatrixD scale_vector(cols());
  
  for (int row = 0; row < rows(); row++){
    double maximum = 0.0;
    for(int col = 0; col < cols(); col++) {
      temp = ml->m[row][col];
      if (fabs(temp) > maximum)
	maximum = fabs(temp); 
    }
    if(maximum == 0.0) {
		cerr << "Matrix = \n" << *this << endl;
      error("singular MatrixD in scale()");
    }
    // save scaling
    scale_vector.ml->m[row][0] = 1.0/maximum; 
  }
  return scale_vector;
}

MatrixD MatrixD::lu_decompose(int* indx, int& d ) const 
{
/*
 Returns the L-U decomposition of a matrix. indx is an ouput
 vector which records the row permutation affected by the
 partial pivoting, d is output as +-1 depending on whether the
 number of row interchanges was even or odd, respectively.
 This routine is used in combination with lu_back_subst to
 solve linear equations or invert a matrix.
*/
  if(rows()  != cols())
    error("Matrix must be square to L-U decompose!\n");
  d = 1; // parity check
  int j,i,k,col_max; // counters
  double dum; // from the book - I don't know signj
  double sum;
  double maximum;
  MatrixD lu_decomp;
  double tmp;

  // make a direct copy of the original matrix:
  lu_decomp = *this;
  MatrixD scale_vector = lu_decomp.scale(); // scale the matrix
  
  // The loop over columns of Crout's method:
  for(j = 0; j < rows(); j++) {
    if (j > 0) {
      // eqn 2.3.12 except for j=i:
      for (i = 0; i <= j-1; i++) {
        sum = lu_decomp.ml->m[i][j];
        if(i > 0) {
	  for(k = 0; k <= i-1; k++) {
	    tmp = lu_decomp.ml->m[i][k]*lu_decomp.ml->m[k][j];
	    sum -= tmp;
	    if(fabs(sum) < M_SMALL*fabs(tmp))
	      sum = 0.0;
	  }
          lu_decomp.ml->m[i][j] = sum;
        }
      }
    }
    
// Initialize the search for the largest pivot element:
    maximum = 0.0;
    // i=j of eq 2.3.12 & i=j+I..N of 2.3.13:
    for(i=j; i <= cols()-1; i++) {
      sum = lu_decomp.ml->m[i][j];
      
      if(j > 0) {
	for(k=0; k <= j-1; k++) {
	  tmp =  lu_decomp.ml->m[i][k] * lu_decomp.ml->m[k][j];
	  sum -= tmp;
	  if(fabs(sum) < M_SMALL*fabs(tmp))
	    sum = 0.0;
	}
	lu_decomp.ml->m[i][j] = sum;
      }
      // figure of merit for pivot:
      dum = scale_vector.ml->m[i][0] * fabs(sum);      
      if (dum >= maximum){
	// is it better than the best so far ?
	col_max = i;
	maximum = dum;
      }
    }
    // Do we need to interchange rows?

    if(j != col_max)  {
      lu_decomp.switch_rows(col_max,j); // Yes, do so ...
      d *= -1;  // ... and change the parity of d
      // also interchange the scale factor:
      dum = scale_vector.ml->m[col_max][0];
      scale_vector.ml->m[col_max][0] = scale_vector.ml->m[j][0];
      scale_vector.ml->m[j][0] = dum;
    }

    indx[j] = col_max;
    // Now, finally, divide by the pivot element:
    if(j != rows() -1){
      // If the pivot element is .zero the matrix is
      // singular (at least to the precision of the
      // algorithm). For some applications on singular
      // matrices, it is desirable to substitute tiny for 0
      if(lu_decomp.ml->m[j][j] == 0.0)
	lu_decomp.ml->m[j][j] = tiny;
      dum = 1/lu_decomp.ml->m[j][j];
      for(i=j+1; i <= cols()-1; i++)
	lu_decomp.ml->m[i][j] *= dum;
    }

  }
  if(lu_decomp.ml->m[rows()-1][cols()-1] == 0)
    lu_decomp.ml->m[rows()-1][cols()-1] = tiny;

  return lu_decomp;
}


void MatrixD::lu_back_subst(int* indx, MatrixD& b)  {

// Solves the set of N linear equations A*X = B. Here "this"
// is the LU-decomposition of the matrix A, determined by the
// routine lu_decompose(). Indx is input as the permutation
// vector returned  by lu_decompose(). B is input as the
// right-hand side vector B, and returns with the solution
// vector X. This routine takes into  account the possibility
// that B will begin with many zero elements, so it is efficient
// for use in matrix inversion. See pp 36-37 in
// Press & Flannery.
  
  if(rows()  != cols())
    error ("non-square lu_decomp matrix in lu_back_subst()");
  if(rows()  != b.rows()) 
    error("wrong size B vector passed to lu_back_subst()");
//  if(rows()  != indx.rows())
//    error("wrong size indx vector passed to lu_back_subst()");
  int i,j,ip;
  int ii = -1;
  double sum;
  double tmp;

  for(i=0;i < rows(); i++){
    ip = indx[i];
    sum = b.ml->m[ip][0];
    b.ml->m[ip][0] = b.ml->m[i][0];
    if (ii >= 0) {
      for(j = ii; j <= i-1; j++) {
	tmp = ml->m[i][j] * b.ml->m[j][0];
	sum -= tmp;
	if(fabs(sum) < M_SMALL*fabs(tmp))
	  sum = 0.0;
      }
    } else if(sum != 0)
      ii = i;
    b.ml->m[i][0] = sum;
  }
  for(i = rows() -1; i >= 0; i--){
    sum = b.ml->m[i][0];
    if (i < cols() -1) {
      for (j = i + 1; j <= rows()-1; j++) {
	tmp = ml->m[i][j] * b.ml->m[j][0];
	sum -= tmp;
	if(fabs(sum) < M_SMALL*fabs(tmp))
	  sum = 0.0;
      }
    }
    // store a component of the soln vector X:
    b.ml->m[i][0] = sum/ml->m[i][i];
  }
}


// Implementation of class MatrixD::RandomOrthogonal

MatrixD::RandomOrthogonal::RandomOrthogonal( int n )
: _dim(n), _passes(1)
{
  int i, j;

  _omitted = new bool*[ _dim ];
  for( i = 0; i < _dim; i++ ) {
    _omitted[i] = new bool[ _dim ];
    for( j = 0; j < _dim; j++ ) { _omitted[i][j] = false; }
    _omitted[i][i] = true;
  }

  _lowerTheta = new double*[ _dim ];
  for( i = 0; i < _dim; i++ ) { 
    _lowerTheta[i] = new double[ _dim ];
    for( j = 0; j < _dim; j++ ) { _lowerTheta[i][j] = 0.0; }
  }

  _upperTheta = new double*[ _dim ];
  for( i = 0; i < _dim; i++ ) { 
    _upperTheta[i] = new double[ _dim ];
    for( j = 0; j < _dim; j++ ) { _upperTheta[i][j] = M_TWOPI; }
    _upperTheta[i][i] = 0.0;
  }

  _rangeTheta = new double*[ _dim ];
  for( i = 0; i < _dim; i++ ) { 
    _rangeTheta[i] = new double[ _dim ];
    for( j = 0; j < _dim; j++ ) { _rangeTheta[i][j] = M_TWOPI; }
    _rangeTheta[i][i] = 0.0;
  }
}


MatrixD::RandomOrthogonal::~RandomOrthogonal()
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


void MatrixD::RandomOrthogonal::omitIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) { 
    _omitted[i][j] = true;
    _omitted[j][i] = true;
  }
}


void MatrixD::RandomOrthogonal::omitIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) { 
    this->omitIndex( n, i ); 
  }
}


void MatrixD::RandomOrthogonal::includeIndex( int i, int j )
{
  if( 0 <= i  &&  i < _dim  &&  0 <= j  &&  j < _dim ) { 
    _omitted[i][j] = false;
    _omitted[j][i] = false;
  }
}


void MatrixD::RandomOrthogonal::includeIndex( int n )
{
  for( int i = 0; i < _dim; i++ ) { 
    this->includeIndex( n, i ); 
  }
}


void MatrixD::RandomOrthogonal::setNumberOfPasses( int n )
{
  if( 0 < n ) { _passes = n; }
}


void MatrixD::RandomOrthogonal::setRange( int i, int j, double lo, double hi )
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


void MatrixD::RandomOrthogonal::setRange( int n, double lo, double hi )
{
  for( int i = 0; i < _dim; i++ ) { 
    this->setRange( n, i, lo, hi ); 
  }
}


MatrixD MatrixD::RandomOrthogonal::build()
{
  // This was programmed very inefficiently
  // but quickly. It will have to be redone
  // if used many times in an application.

  int i, j, p;
  double theta, cs, sn, tmp;
  MatrixD R( "I", _dim );
  MatrixD U( "I", _dim );
  MatrixD W( "I", _dim );

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



// Implementation of class MatrixC


MatrixC::MatrixC() : stacked(0) {
  ml = new MLC;
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::MatrixC(int rows) : stacked(0) {
  ml = new MLC(rows,1,complex_0);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::MatrixC(const MatrixD& x) : stacked(0) {
  ml = new MLC(*x.ml);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::MatrixC(int rows, int columns) : stacked(0) {
  ml = new MLC(rows,columns,complex_0);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::MatrixC(int rows, int columns, const Complex& initval) : stacked(0) {
  ml = new MLC(rows,columns,initval);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::MatrixC(int rows, int columns, Complex* initval) : stacked(0) {
  ml = new MLC(rows,columns,initval);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::MatrixC(const char* flag, int dimension) {
  int i;
  float tmp_float = dimension/2.0;
  ml = new MLC(dimension,dimension,complex_0);
  
  if(((dimension/2) != tmp_float) && (flag[0] == 'J'))
    error("dimension must be even","for J and I matrix");
  if (flag[0]  == 'I') {
    for (i = 0; i< dimension; i++) {
      ml->m[i][i] = complex_1;
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      ml->m[i-dimension/2][i] = complex_1;
      ml->m[i][i-dimension/2] = -complex_1;
    }
  } else
    error("Unknown flag: ",flag);
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::MatrixC(const MatrixC& X) {
  stacked = X.stacked;
  ml = X.ml;
  (ml->rc)++;
#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixC::~MatrixC() {
  if( --(ml->rc) == 0) delete ml;
#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

MatrixC& MatrixC::operator=(const MatrixC& x) {
  if(this != &x) {
    if( --(ml->rc) == 0) delete ml;
#ifndef FORBIDSTACKED
    if(x.stacked) {
      ml = x.ml;
      (ml->rc)++;
    } else {
#endif
      ml = new MLC;
      *ml = *(x.ml);
#ifndef FORBIDSTACKED
    }
#endif
  }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

MatrixC& MatrixC::DeepCopy(const MatrixC& x) {
 if(this != &x) {
   if( --(ml->rc) == 0) delete ml;
     ml = new MLC;
     *ml = *(x.ml);
 }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

void MatrixC::error( const char* msg1, const char* msg2 ) const {
  cerr << "MatrixC error: " << msg1 << " " << msg2  << endl;
  exit(1);
}

char operator==( const MatrixC& x, const MatrixC& y ) {
 return *(x.ml) == *(y.ml);
}

char operator==( const MatrixC& x, const Complex& y ) {
 return *(x.ml) == y;
}

char operator==( const Complex& y, const MatrixC& x ){
 return *(x.ml) == y;
}

char operator!=( const MatrixC& x, const MatrixC& y ) {
 return !( x == y );
}

char operator!=( const MatrixC& x, const Complex& y ) {
 return !( x == y );
}

char operator!=( const Complex& x, const MatrixC& y ) {
 return !( x == y );
}

void MatrixC::operator+=( const Complex& x ) {
 ml->operator+=( x );
}

void MatrixC::operator-=( const Complex& x ) {
 ml->operator-=( x );
}

MatrixC MatrixC::Square() const {
  int d;

  if( rows() == cols() ) return *this;

  if( rows() < cols() ) d = rows();
  else                  d = cols();

  int     i, j;
  MatrixC z( d, d, complex_0 );
  MLC*    zPtr = z.ml;

  for( i = 0; i < d; i++ )
    for( j = 0; j < d; j++ ) 
      zPtr->m[i][j] = ml->m[i][j];
      
  return z;
}

MatrixC MatrixC::transpose() const {
  MatrixC z(cols(),rows(),complex_0);
  MLC* zPtr = z.ml;

  for (int row = 0; row < rows(); row++)  {
    for(int col = 0; col < cols(); col++)
      zPtr->m[col][row] = ml->m[row][col];
  }
  z.stacked = 1;
  return z;
}

MatrixC MatrixC::dagger() const {
  MatrixC z(cols(),rows(),complex_0);
  MLC* zPtr = z.ml;

  for (int row = 0; row < rows(); row++)  {
    for(int col = 0; col < cols(); col++)
      zPtr->m[col][row] = conj(ml->m[row][col]);
  }
  z.stacked = 1;
  return z;
}

Complex MatrixC::trace() {
  Complex temp = complex_0;

  if(rows()  != cols())
    error("MatrixC must be square to take a trace!\n");
  for (int i = 0; i < rows(); i++)  {
      temp += ml->m[i][i];
  }
  return temp;
}

Complex MatrixC::determinant() {
  if(rows()  != cols())
    error("MatrixC must be square for determinant()");
  int* indx = new int[cols()]; // create the "index vector
			       // see pp 38. in Numerical Recipes
  int d;
  // perform the decomposition once:

  MatrixC decomp = lu_decompose(indx,d);
  Complex determinant = d;
  for(int i=0; i < cols() ; i++)
    determinant *= decomp.ml->m[i][i];
  delete []indx;
  return determinant;
}

MatrixC MatrixC::inverse() {
  if(rows()  != cols())
    error("MatrixC must be square for inverse()");
  MatrixC Y("I",rows()); // create an identity MatrixC
  int* indx = new int[cols()];  // create the "index vector"
  MatrixC B(cols());     // see Press & Flannery
  int d;
  // perform the decomposition once:
  MatrixC decomp = lu_decompose(indx,d);
  for(int col = 0; col < cols(); col++){
    B.copy_column(Y,col,0);
    decomp.lu_back_subst(indx,B);
    Y.copy_column(B,0,col);
  }
  delete []indx;
  return Y;
}



MatrixC MatrixC::eigenValues() {
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
  MatrixC tmp(1,cols(),complex_0);

  k = 0;
  for(i = 0; i < rows(); i++) {
    for(j = 0; j< cols(); j++) {
      br[k] = real(ml->m[j][i]); // the cg_ routine uses the transpose
      bi[k] = imag(ml->m[j][i]); // the cg_ routine uses the transpose
      k++;
    }
  }
  MatrixC z1(nm,n,complex_0);
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);
  if(ierr != 0) {
    cerr << "MatrixC: Error in eigenvalue routine. error = ";
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
    tmp.ml->m[0][i] = Complex(wr[i],wi[i]);
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
	z1.switch_columns(0,i); 
	tmp.switch_columns(0,i); 
	sortFlag = 1;
	if((oddEven*2) == nm)
	  z1.switch_columns(oddEven,oddEven+i);
	  tmp.switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10) 
    cerr << "MatrixC: Something is wrong with the eigenValue sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(z1(1,1)) < abs(z1(1,i))) {
      z1.switch_columns(1,i);
      tmp.switch_columns(1,i);
      if((oddEven*2) == nm)
	z1.switch_columns(1+oddEven,oddEven+i);
	tmp.switch_columns(1+oddEven,oddEven+i);
    }
  }
  return tmp;
}
MatrixC MatrixC::eigenVectors() {
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
      br[k] = real(ml->m[j][i]); // the cg_ routine uses the transpose
      bi[k] = imag(ml->m[j][i]); // the cg_ routine uses the transpose
      k++;
    }
  }
  MatrixC z1(nm,n,complex_0);
  cg_(&nm,&n,br,bi,wr,wi,&matz,cr,ci,fv1,fv2,fv3,&ierr);
  if(ierr != 0) {
    cerr << "MatrixC: Error in eigenvector routine. error = ";
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
	z1.switch_columns(0,i); 
	sortFlag = 1;
	if((oddEven*2) == nm)
	  z1.switch_columns(oddEven,oddEven+i);
      }
    }
    counter++;
  }
  if(counter >= 10) 
    cerr << "MatrixC: Something is wrong with the eigenVector sort" << endl;
  for(i=2; i < oddEven; i++) {
    if(abs(z1(1,1)) < abs(z1(1,i))) {
      z1.switch_columns(1,i);
      if((oddEven*2) == nm)
	z1.switch_columns(1+oddEven,oddEven+i);
    }
  }
  return z1;
}

// operators

Complex& MatrixC::operator()(int i, int j) {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** Complex& MatrixC::operator()(int i, int j)" << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for "<< i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
  return ml->m[i][j];
}

Complex MatrixC::operator()(int i, int j) const {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** Complex& MatrixC::operator()(int i, int j)" << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for "<< i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
  return ml->m[i][j];
}

Complex& MatrixC::operator()(int i) {
  if( rows() == 1 ) return ml->m[0][i];
  if( cols() == 1 ) return ml->m[i][0];
  cerr << "\n*** ERROR *** " << endl;
  cerr << "*** ERROR *** Complex& MatrixC::operator()(int i) " << endl;
  cerr << "*** ERROR *** Can only be used with single row  " << endl;
  cerr << "*** ERROR *** or column matrices. " << endl;
  cerr << "*** ERROR *** " << endl;
  exit(1);
  return ml->m[0][0];
}

ostream& operator<<(ostream& os, const MatrixC& x)
{
  int i,j;
  
  for(i=0; i< x.rows(); i++) {
    os << "( ";
    for(j=0; j< x.cols(); j++) {
      os  << x.ml->m[i][j] << ", ";
    }
    os << " )\n";
  }
  return os << endl;
}


MatrixC operator+(const MatrixC& x, const MatrixC& y) {
  MatrixC z(x.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* yPtr = y.ml;
  MLC* zPtr = z.ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols()))
    x.error("Must have equal dimensions for addition!");
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] + yPtr->m[i][j];
      if(abs(zPtr->m[i][j]) < M_SMALL*abs(yPtr->m[i][j]))
	zPtr->m[i][j] = complex_0;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixC operator+(const MatrixC& x, const Complex& y) {
 MatrixC z; 
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}

MatrixC operator+( const Complex& y, const MatrixC& x ) {
 MatrixC z;
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}

MatrixC operator-(const MatrixC& x) {
  MatrixC z(x.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* zPtr = z.ml;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = -complex_1* xPtr->m[i][j];
    }
  }
  z.stacked = 1;
  return z;
}

MatrixC operator-(const MatrixC& x, const MatrixC& y) {
  MatrixC z(x.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* yPtr = y.ml;
  MLC* zPtr = z.ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols()))
    x.error("Must have equal dimensions for addition!");
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] - yPtr->m[i][j];
      if(abs(zPtr->m[i][j]) < M_SMALL*abs(yPtr->m[i][j]))
	zPtr->m[i][j] = complex_0;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixC operator-(const MatrixC& x, const Complex& y) {
 MatrixC z; 
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixC operator-( const Complex& y, const MatrixC& x ) {
 MatrixC z;
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixC operator*( const MatrixC& x, const Complex& y ) {
  MatrixC z(x.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixC operator*( const Complex& y, const MatrixC& x) {
  MatrixC z(x.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixC operator*(const MatrixC& x, const MatrixC& y)  {
  MatrixC z(x.rows(),y.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* yPtr = y.ml;
  MLC* zPtr = z.ml;
  if( x.cols() != y.rows())
    x.error("# of rows of 2nd matrix must equal # cols of 1st for multiply");
  Complex tmp;
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      Complex sum = complex_0;
      for(int i = 0; i < x.cols(); i++) {
	tmp = xPtr->m[row][i] * yPtr->m[i][col];
	sum += tmp;
	if(abs(sum) < M_SMALL*abs(tmp))
	  sum = complex_0;
      }
      zPtr->m[row][col] = sum;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixC operator*(const MatrixC& x, const MatrixD& y)  {
  MatrixC z(x.rows(),y.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLD* yPtr = y.ml;
  MLC* zPtr = z.ml;
  if( x.cols() != y.rows())
    x.error("# of rows of 2nd matrix must equal # cols of 1st for multiply");
  Complex tmp;
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      Complex sum = complex_0;
      for(int i = 0; i < x.cols(); i++) {
	tmp = xPtr->m[row][i] * Complex(yPtr->m[i][col],0.0);
	sum += tmp;
	if(abs(sum) < M_SMALL*abs(tmp))
	  sum = complex_0;
      }
      zPtr->m[row][col] = sum;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixC operator*(const MatrixD& y, const MatrixC& x)  {
  MatrixC z(y.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLD* yPtr = y.ml;
  MLC* zPtr = z.ml;
  if( y.cols() != x.rows())
    x.error("# of rows of 2nd matrix must equal # cols of 1st for multiply");
  Complex tmp;
  for(int row = 0; row < y.rows(); row++) {
    for(int col = 0; col < x.cols(); col++){
      Complex sum = complex_0;
      for(int i = 0; i < y.cols(); i++) {
	tmp = Complex(yPtr->m[row][i],0.0) * xPtr->m[i][col];
	sum += tmp;
	if(abs(sum) < M_SMALL*abs(tmp))
	  sum = complex_0;
      }
      zPtr->m[row][col] = sum;
    }
  }
  z.stacked = 1;
  return z;
}
MatrixC operator/( const MatrixC& x, const Complex& y) {
  MatrixC z(x.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* zPtr = z.ml;

  if(y == complex_0)
    cerr << "MatrixC:operator/ divide by zero" << endl;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] / y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixC operator/( const Complex& y, MatrixC& x) {
  return (y*x.inverse());
}

MatrixC operator/( MatrixC& y, MatrixC& x) {
  return (y*x.inverse());
}

MatrixC operator/( MatrixC& y, MatrixD& x) {
  return (y*x.inverse());
}

MatrixC operator/( MatrixD& y, MatrixC& x) {
  return (y*x.inverse());
}

MatrixC operator/( const MatrixC& x, const double y) {
  MatrixC z(x.rows(),x.cols(),complex_0);
  MLC* xPtr = x.ml;
  MLC* zPtr = z.ml;
  Complex temp = Complex(y,0.0);
  if(y == 0.0)
    cerr << "MatrixC:operator/ divide by zero" << endl;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] / temp;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixC operator/( const double y, MatrixC& x) {
  return (Complex(y,0.0)*x.inverse());
}


/********************************************************
The private support functions for determinant & inverse:
*********************************************************/

// copy the from_col of mm to the to_col of "this"

void MatrixC::copy_column(MatrixC& mm, int from_col, int to_col){
  if(rows()  != mm.rows())
    error("number of rows must be equal for copy_column()");
  for(int row=0; row < rows(); row++)
    ml->m[row][to_col] = mm.ml->m[row][from_col];
}

void MatrixC::switch_columns(int col1, int col2) {
  MatrixC temp(rows());
  int row; // O.K.
  for(row = 0; row < rows(); row++)
    // temporarily store col 1:
    temp.ml->m[row][0] = ml->m[row][col1];
  for(row = 0; row < rows(); row++)
    ml->m[row][col1] = ml->m[row][col2]; // move col2 to col1
  for(row = 0; row < rows(); row++)
    ml->m[row][col2] = temp.ml->m[row][0]; // move temp to col2
}

void MatrixC::switch_rows(int row1, int row2) {
  MatrixC temp(cols());
  
  int col; // O.K.
  
  for( col = 0; col < cols(); col++)
    // temporarily store row 1:
    temp.ml->m[col][0] = ml->m[row1][col];
  for(col = 0; col < cols(); col++)
    ml->m[row1][col] = ml->m[row2][col]; // move row2 to row1
  for(col = 0; col < cols(); col++)
    ml->m[row2][col] = temp.ml->m[col][0]; // move temp to row2
}

// scale a matrix (used in L-U decomposition)

MatrixC MatrixC::scale()  {
  Complex temp;
  if (rows() <= 0 || cols() <= 0)
    error(" bad MatrixC for scale()");
  if (rows() != cols())
    error(" MatrixC must be square for scale()");   
  MatrixC scale_vector(cols());
  
  for (int row = 0; row < rows(); row++){
    double maximum = 0.0;
    for(int col = 0; col < cols(); col++) {
      temp = ml->m[row][col];
      if (abs(temp) > maximum)
	maximum = abs(temp); 
    }
    if(maximum == 0.0)
      error("singular MatrixC in scale()");
    // save scaling
    scale_vector.ml->m[row][0] = Complex(1.0/maximum,0.0); 
  }
  return scale_vector;
}

MatrixC MatrixC::lu_decompose(int* indx, int& d ) {
/*
 Returns the L-U decomposition of a matrix. indx is an ouput
 vector which records the row permutation affected by the
 partial pivoting, d is output as +-1 depending on whether the
 number of row interchanges was even or odd, respectively.
 This routine is used in combination with lu_back_subst to
 solve linear equations or invert a matrix.
*/
  if(rows()  != cols())
    error("Matrix must be square to L-U decompose!\n");
  d = 1; // parity check
  int j,i,k,col_max; // counters
  Complex dum; // from the book - I don't know signj
  Complex sum;
  Complex maximum;
  MatrixC lu_decomp;
  Complex tmp;

  // make a direct copy of the original matrix:
  lu_decomp = *this;
  MatrixC scale_vector = lu_decomp.scale(); // scale the matrix
  
  // The loop over columns of Crout's method:
  for(j = 0; j < rows(); j++) {
    if (j > 0) {
      // eqn 2.3.12 except for j=i:
      for (i = 0; i <= j-1; i++) {
        sum = lu_decomp.ml->m[i][j];
        if(i > 0) {
	  for(k = 0; k <= i-1; k++) {
	    tmp = lu_decomp.ml->m[i][k]*lu_decomp.ml->m[k][j];
	    sum -= tmp;
	    if(abs(sum) < M_SMALL*abs(tmp))
	      sum = complex_0;
	  }
          lu_decomp.ml->m[i][j] = sum;
        }
      }
    }
    
// Initialize the search for the largest pivot element:
    maximum = complex_0;
    // i=j of eq 2.3.12 & i=j+I..N of 2.3.13:
    for(i=j; i <= cols()-1; i++) {
      sum = lu_decomp.ml->m[i][j];
      
      if(j > 0) {
	for(k=0; k <= j-1; k++) {
	  tmp =  lu_decomp.ml->m[i][k] * lu_decomp.ml->m[k][j];
	  sum -= tmp;
	  if(abs(sum) < M_SMALL*abs(tmp))
	    sum = complex_0;
	}
	lu_decomp.ml->m[i][j] = sum;
      }
      // figure of merit for pivot:
      dum = scale_vector.ml->m[i][0] * sum;
      if (abs(dum) >= abs(maximum)){
	// is it better than the best so far ?
	col_max = i;
	maximum = dum;
      }
    }
    // Do we need to interchange rows?

    if(j != col_max)  {
      lu_decomp.switch_rows(col_max,j); // Yes, do so ...
      d *= -1;  // ... and change the parity of d
      // also interchange the scale factor:
      dum = scale_vector.ml->m[col_max][0];
      scale_vector.ml->m[col_max][0] = scale_vector.ml->m[j][0];
      scale_vector.ml->m[j][0] = dum;
    }

    indx[j] = col_max;
    // Now, finally, divide by the pivot element:
    if(j != rows() -1){
      // If the pivot element is .zero the matrix is
      // singular (at least to the precision of the
      // algorithm). For some applications on singular
      // matrices, it is desirable to substitute tiny for 0
      if(lu_decomp.ml->m[j][j] == complex_0)
	lu_decomp.ml->m[j][j] = tiny;
      dum = complex_1/lu_decomp.ml->m[j][j];
      for(i=j+1; i <= cols()-1; i++)
	lu_decomp.ml->m[i][j] *= dum;
    }

  }
  if(lu_decomp.ml->m[rows()-1][cols()-1] == complex_0)
    lu_decomp.ml->m[rows()-1][cols()-1] = Complex(tiny,0.0);

  return lu_decomp;
}


void MatrixC::lu_back_subst(int* indx, MatrixC& b)  {

// Solves the set of N linear equations A*X = B. Here "this"
// is the LU-decomposition of the matrix A, determined by the
// routine lu_decompose(). Indx is input as the permutation
// vector returned  by lu_decompose(). B is input as the
// right-hand side vector B, and returns with the solution
// vector X. This routine takes into  account the possibility
// that B will begin with many zero elements, so it is efficient
// for use in matrix inversion. See pp 36-37 in
// Press & Flannery.
  
  if(rows()  != cols())
    error ("non-square lu_decomp matrix in lu_back_subst()");
  if(rows()  != b.rows()) 
    error("wrong size B vector passed to lu_back_subst()");
//  if(rows()  != indx.rows())
//    error("wrong size indx vector passed to lu_back_subst()");
  int i,j,ip;
  int ii = -1;
  Complex sum;
  Complex tmp;

  for(i=0;i < rows(); i++){
    ip = indx[i];
    sum = b.ml->m[ip][0];
    b.ml->m[ip][0] = b.ml->m[i][0];
    if (ii >= 0) {
      for(j = ii; j <= i-1; j++) {
	tmp = ml->m[i][j] * b.ml->m[j][0];
	sum -= tmp;
	if(abs(sum) < M_SMALL*abs(tmp))
	  sum = complex_0;
      }
    } else if(sum != complex_0)
      ii = i;
    b.ml->m[i][0] = sum;
  }
  for(i = rows() -1; i >= 0; i--){
    sum = b.ml->m[i][0];
    if (i < cols() -1) {
      for (j = i + 1; j <= rows()-1; j++) {
	tmp = ml->m[i][j] * b.ml->m[j][0];
	sum -= tmp;
	if(abs(sum) < M_SMALL*abs(tmp))
	  sum = complex_0;
      }
    }
    // store a component of the soln vector X:
    b.ml->m[i][0] = sum/ml->m[i][i];
  }
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




MatrixI::MatrixI() : stacked(0) {
  ml = new MLI;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixI::MatrixI(int rows) : stacked(0) {
  ml = new MLI(rows,1,0);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixI::MatrixI(int rows, int columns) : stacked(0) {
  ml = new MLI(rows,columns,0);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixI::MatrixI(int rows, int columns, int initval) : stacked(0) {
  ml = new MLI(rows,columns,initval);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixI::MatrixI(int rows, int columns, int* initval) : stacked(0) {
  ml = new MLI(rows,columns,initval);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixI::MatrixI(const char* flag, int dimension) {
  int i;
  float tmp_float = dimension/2.0;
  ml = new MLI(dimension,dimension,0);
  
  if(((dimension/2) != tmp_float) && (flag[0] == 'J'))
    error("dimension must be even","for J and I matrix");
  if (flag[0]  == 'I') {
    for (i = 0; i< dimension; i++) {
      ml->m[i][i] = 1;
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      ml->m[i-dimension/2][i] = 1;
      ml->m[i][i-dimension/2] = -1;
    }
  } else
    error("Unknown flag: ",flag);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixI::MatrixI(const MatrixI& X) {
  stacked = X.stacked;
  ml = X.ml;
  (ml->rc)++;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixI::~MatrixI() {
  if( --(ml->rc) == 0) delete ml;

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

MatrixI MatrixI::transpose() const {
  MatrixI z(cols(),rows(),0);
  MLI* zPtr = z.ml;

  for (int row = 0; row < rows(); row++)  {
    for(int col = 0; col < cols(); col++)
      zPtr->m[col][row] = ml->m[row][col];
  }
  z.stacked = 1;
  return z;
}
int MatrixI::trace() {
  int temp = 0;

  if(rows()  != cols())
    error("MatrixI must be square to take a trace!\n");
  for (int i = 0; i < rows(); i++)  {
      temp += ml->m[i][i];
  }
  return temp;
}

MatrixI& MatrixI::operator=(const MatrixI& x) {
  if(this != &x) {
    if( --(ml->rc) == 0) delete ml;
#ifndef FORBIDSTACKED
    if(x.stacked) {
      ml = x.ml;
      (ml->rc)++;
    } else {
#endif
      ml = new MLI;
      *ml = *(x.ml);
#ifndef FORBIDSTACKED
    }
#endif
  }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

MatrixI& MatrixI::DeepCopy(const MatrixI& x) {
 if(this != &x) {
   if( --(ml->rc) == 0) delete ml;
     ml = new MLI;
     *ml = *(x.ml);
 }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}

void MatrixI::error( const char* msg1, const char* msg2 ) const {
  cerr << "MatrixI error: " << msg1 << " " << msg2  << endl;
  exit(1);
}

int& MatrixI::operator()(int i, int j) {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** int& MatrixI::operator()(int i, int j) " << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for " << i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
 return ml->m[i][j];
}

int  MatrixI::operator()(int i, int j) const {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** int& MatrixI::operator()(int i, int j) " << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for " << i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
 return ml->m[i][j];
}

int& MatrixI::operator()(int i) {
  if( rows() == 1 ) return ml->m[0][i];
  if( cols() == 1 ) return ml->m[i][0];
  cerr << "\n*** ERROR *** " << endl;
  cerr << "*** ERROR *** int& MatrixI::operator()(int i) " << endl;
  cerr << "*** ERROR *** Can only be used with single row  " << endl;
  cerr << "*** ERROR *** or column matrices. " << endl;
  cerr << "*** ERROR *** " << endl;
  exit(1);
  return ml->m[0][0];
}

void MatrixI::operator+=( const int& x ) {
 ml->operator+=( x );
}

void MatrixI::operator-=( const int& x ) {
 ml->operator-=( x );
}

char operator==( const MatrixI& x, const MatrixI& y ) {
 return *(x.ml) == *(y.ml);
}

char operator==( const MatrixI& x, const int& y ) {
 return *(x.ml) == y;
}

char operator==( const int& y, const MatrixI& x ){
 return *(x.ml) == y;
}

ostream& operator<<(ostream& os, const MatrixI& x)
{
  int i,j;
  
  for(i=0; i< x.rows(); i++) {
    os << "( ";
    for(j=0; j< x.cols(); j++) {
      os << x.ml->m[i][j] << ", ";
    }
    os << " )\n";
  }
  return os << endl;
}


MatrixI operator+(const MatrixI& x, const MatrixI& y) {
  MatrixI z(x.rows(),x.cols(),0);
  MLI* xPtr = x.ml;
  MLI* yPtr = y.ml;
  MLI* zPtr = z.ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols()))
    x.error("Must have equal dimensions for addition!");
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] + yPtr->m[i][j];
    }
  }
  z.stacked = 1;
  return z;
}

MatrixI operator+(const MatrixI& x, const int& y) {
 MatrixI z; 
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}

MatrixI operator+( const int& y, const MatrixI& x ) {
 MatrixI z;
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}

MatrixI operator-(const MatrixI& x) {
  MatrixI z(x.rows(),x.cols(),0);
  MLI* xPtr = x.ml;
  MLI* zPtr = z.ml;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = -1* xPtr->m[i][j];
    }
  }
  z.stacked = 1;
  return z;
}

MatrixI operator-(const MatrixI& x, const MatrixI& y) {
  MatrixI z(x.rows(),x.cols(),0);
  MLI* xPtr = x.ml;
  MLI* yPtr = y.ml;
  MLI* zPtr = z.ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols()))
    x.error("Must have equal dimensions for addition!");
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] - yPtr->m[i][j];
      if(abs(zPtr->m[i][j]) < M_SMALL*abs(yPtr->m[i][j]))
	zPtr->m[i][j] = 0;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixI operator-(const MatrixI& x, const int& y) {
 MatrixI z; 
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixI operator-( const int& y, const MatrixI& x ) {
 MatrixI z;
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixI operator*( const MatrixI& x, const int y) {
  MatrixI z(x.rows(),x.cols(),0);
  MLI* xPtr = x.ml;
  MLI* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixI operator*( const int y, const MatrixI& x) {
  MatrixI z(x.rows(),x.cols(),0);
  MLI* xPtr = x.ml;
  MLI* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixI operator*(const MatrixI& x, const MatrixI& y)  {
  MatrixI z(x.rows(),y.cols(),0);
  MLI* xPtr = x.ml;
  MLI* yPtr = y.ml;
  MLI* zPtr = z.ml;
  if( x.cols() != y.rows())
    x.error("# of rows of 2nd matrix must equal # cols of 1st for multiply");
  int tmp;
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      int sum = 0;
      for(int i = 0; i < x.cols(); i++) {
	tmp = xPtr->m[row][i] * yPtr->m[i][col];
	sum += tmp;
      }
      zPtr->m[row][col] = sum;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixI operator/( const MatrixI& x, const int y) {
  MatrixI z(x.rows(),x.cols(),0);
  MLI* xPtr = x.ml;
  MLI* zPtr = z.ml;

  if(y == 0)
    cerr << "MatrixI:operator/ divide by zero" << endl;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] / y;
    }
  }
 z.stacked = 1;
 return z;
}
