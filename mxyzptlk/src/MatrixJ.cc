/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******  Version:   4.1                    
******                                    
******  File:      MatrixJ.cc
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


#include "MatrixJ.h"

#include <iostream>
#include <iomanip>

//#define M_SMALL  1.0e-30

// ================================================================
//      Global variables
//

#ifdef OBJECT_DEBUG
int MatrixJ::objectCount = 0;
#endif

using namespace std;

// ================================================================
//      Constructors and the destructor ...
//


MatrixJ::MatrixJ( Jet__environment* pje ) : stacked(0) {
  myEnv = pje;
  ml = new MLJ;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixJ::MatrixJ( int rows, Jet__environment* pje ) : stacked(0) {
  myEnv = pje;
  Jet initval;
  initval.setEnvTo( pje );
  initval = 0.0;
  ml = new MLJ(rows,1,initval);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


MatrixJ::MatrixJ( int rows, int columns,  Jet__environment* pje ) : stacked(0) {
  myEnv = pje;
  Jet initval;
  initval.setEnvTo( pje );
  initval = 0.0;
  ml = new MLJ(rows,columns,initval);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


MatrixJ::MatrixJ( int rows, int columns, const Jet& initval,  Jet__environment* pje ) : stacked(0) {
  myEnv = pje;
  ml = new MLJ(rows,columns,initval);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}


MatrixJ::MatrixJ( int rows, int columns, Jet* initval, Jet__environment* pje ) : stacked(0) {
  myEnv = pje;
  ml = new MLJ(rows,columns,initval);

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixJ::MatrixJ( char * flag, int dimension,  Jet__environment* pje ) {
  myEnv = pje;
  int i;
  Jet initval;
  initval.setEnvTo( pje );
  initval = 0.0;

#ifdef OBJECT_DEBUG
  objectCount++;
#endif

  float tmp_float = dimension/2.0;
  ml = new MLJ(dimension,dimension,initval);

  if(((dimension/2) != tmp_float) && (flag[0] == 'J'))
    error("dimension must be even","for J and I matrix");
  if (flag[0]  == 'I') {
    for (i = 0; i< dimension; i++) {
      ml->m[i][i] = 1.0;
    }
  } else if (flag[0] == 'J') {
    for (i = dimension/2; i< dimension; i++) {
      ml->m[i-dimension/2][i] = 1.0;
      ml->m[i][i-dimension/2] = -1.0;
    }
  } else
    error("Unknown flag: ",flag);
}

MatrixJ::MatrixJ( const MatrixJ& X ) {
  myEnv = X.myEnv;
  stacked = X.stacked;
  ml = X.ml;
  (ml->rc)++;

#ifdef OBJECT_DEBUG
 objectCount++;
#endif
}

MatrixJ::~MatrixJ() {
  if( --(ml->rc) == 0) delete ml;

#ifdef OBJECT_DEBUG
 objectCount--;
#endif
}

MatrixJ MatrixJ::transpose( ) const {
  MatrixJ z(cols(),rows(), myEnv);
  MLJ* zPtr = z.ml;
  for (int row = 0; row < rows(); row++)  {
    for(int col = 0; col < cols(); col++)
      zPtr->m[col][row] = ml->m[row][col];
  }
  z.stacked = 1;
  return z;
}

MatrixJ MatrixJ::Square( ) const {
  int d;
  MatrixJ w(cols(),rows(), myEnv);
  w = *this;
  if( rows() == cols() ) return w;

  if( rows() < cols() ) d = rows();
  else                  d = cols();

  int     i, j;
  MatrixJ z( d, d, myEnv);
  MLJ*    zPtr = z.ml;

  for( i = 0; i < d; i++ )
    for( j = 0; j < d; j++ ) 
      zPtr->m[i][j] = ml->m[i][j];
      
  return z;
}

Jet MatrixJ::determinant( ) {
  if(rows()  != cols())
    error("MatrixJ must be square for determinant()");
  int* indx = new int[cols()]; // create the "index vector
			       // see pp 38. in Numerical Recipes
  int d;
  // perform the decomposition once:

  MatrixJ decomp(myEnv);
  decomp = lu_decompose(indx,d,myEnv);
  Jet determinant;
  determinant.setEnvTo( myEnv );
  determinant = d;
  for(int i=0; i < cols() ; i++)
    determinant *= decomp.ml->m[i][i];
  delete []indx;
  return determinant;
}

MatrixJ MatrixJ::inverse( ) {
  if(rows()  != cols())
    error("MatrixJ must be square for inverse()");
  MatrixJ Y("I",rows(),myEnv); // create an identity MatrixJ
  int* indx = new int[cols()];  // create the "index vector"
  MatrixJ B(cols(),myEnv);     // see Press & Flannery
  int d;
  // perform the decomposition once:
  MatrixJ decomp(myEnv);
  decomp = lu_decompose(indx,d,myEnv);
  for(int col = 0; col < cols(); col++){
    B.copy_column(Y,col,0);
    decomp.lu_back_subst(indx,B);
    Y.copy_column(B,0,col);
  }
  delete []indx;
  return Y;
}

Jet MatrixJ::trace( ) {
  Jet temp;
  temp.setEnvTo( myEnv );
  temp = 0.0;
  
  if(rows()  != cols())
    error("MatrixJ must be square to take a trace!\n");
  for (int i = 0; i < rows(); i++)  {
    temp += ml->m[i][i];
  }
  return temp;
}

int MatrixJ::CheckEnv( ) {
  int SameEnv = 1;
  for (int i = 0; i < rows(); i++)
    for (int j = 0; j < cols(); j++)
      if ( myEnv != ml->m[i][j].Env() )
	SameEnv = 0;
  return SameEnv;
}

MatrixJ& MatrixJ::operator=(const MatrixJ& x) {
  if(this != &x) {
    if( --(ml->rc) == 0) delete ml;
#ifndef FORBIDSTACKED
    if(x.stacked) {
      ml = x.ml;
      (ml->rc)++;
    } else {
#endif
      ml = new MLJ;
      *ml = *(x.ml);
#ifndef FORBIDSTACKED
    }
#endif
  }
 stacked = 0;   // ??? This line is probably unnecessary.
 myEnv = x.myEnv;
 return *this; 
}


MatrixJ& MatrixJ::DeepCopy( const MatrixJ& x ) {
 if(this != &x) {
   if( --(ml->rc) == 0) delete ml;
     ml = new MLJ;
     *ml = *(x.ml);
 }
 stacked = 0;   // ??? This line is probably unnecessary.
 return *this; 
}


Jet& MatrixJ::operator()( int i, int j ) {
  if((i >= rows()) || (j >= cols())) {
    cerr << "\n*** ERROR *** " << endl;
    cerr << "*** ERROR *** int& MatrixJ::operator()(int i, int j) " << endl;
    cerr << "*** ERROR *** limits are " << rows() << " " << cols() << endl;
    cerr << "*** ERROR *** You asked for " << i << " " << j << endl;
    cerr << "*** ERROR *** matrix limits exceeded " << endl;
    exit(1);
  }
 return ml->m[i][j];
}

Jet& MatrixJ::operator()( int i ) {
  if( rows() == 1 ) return ml->m[0][i];
  if( cols() == 1 ) return ml->m[i][0];
  cerr << "\n*** ERROR *** " << endl;
  cerr << "*** ERROR *** int& MatrixJ::operator()(int i) " << endl;
  cerr << "*** ERROR *** Can only be used with single row  " << endl;
  cerr << "*** ERROR *** or column matrices. " << endl;
  cerr << "*** ERROR *** " << endl;
  exit(1);
  return ml->m[0][0];
}


void MatrixJ::operator+=( const Jet& x ) {
 ml->operator+=( x );
}

void MatrixJ::operator-=( const Jet& x ) {
 ml->operator-=( x );
}


char operator==( const MatrixJ& x, const MatrixJ& y ) {
 return *(x.ml) == *(y.ml);
}

char operator==( const MatrixJ& x, const Jet& y ) {
 return *(x.ml) == y;
}

char operator==( const Jet& y, const MatrixJ& x ){
 return *(x.ml) == y;
}



ostream& operator<<( ostream& os, const MatrixJ& x)
{
  int i,j;
  
  for(i=0; i< x.rows(); i++) {
    for(j=0; j< x.cols(); j++) {
      os << "( " << i << ", " << j << " )" << endl;
      x.ml->m[i][j].printCoeffs();
    }
  }
  return os << endl;
}

MatrixJ operator+( const MatrixJ& x, const MatrixJ& y ) {
  MatrixJ z(x.rows(),x.cols(), x.myEnv);
  MLJ* xPtr = x.ml;
  MLJ* yPtr = y.ml;
  MLJ* zPtr = z.ml;
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

MatrixJ operator+( const MatrixJ& x, const Jet& y ) {
 MatrixJ z(x.myEnv); 
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}


MatrixJ operator+( const Jet& y, const MatrixJ& x ) {
 MatrixJ z(x.myEnv);
 z.DeepCopy( x );
 z += y;
 z.stacked = 1;
 return z;
}

MatrixJ operator-( const MatrixJ& x ) {
  MatrixJ z(x.rows(),x.cols(),x.myEnv);
  MLJ* xPtr = x.ml;
  MLJ* zPtr = z.ml;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = -1* xPtr->m[i][j];
    }
  }
  z.stacked = 1;
  return z;
}

MatrixJ operator-( const MatrixJ& x, const MatrixJ& y ) {
  MatrixJ z(x.rows(),x.cols(),x.myEnv);
  MLJ* xPtr = x.ml;
  MLJ* yPtr = y.ml;
  MLJ* zPtr = z.ml;
  if(( x.rows() != y.rows()) || (x.cols() != y.cols()))
    x.error("Must have equal dimensions for addition!");
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] - yPtr->m[i][j];
      //if(fabs(zPtr->m[i][j]) < M_SMALL*fabs(yPtr->m[i][j]))
      //zPtr->m[i][j] = 0;
    }
  }
  z.stacked = 1;
  return z;
}

MatrixJ operator-( const MatrixJ& x, const Jet& y ) {
 MatrixJ z(x.myEnv); 
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixJ operator-( const Jet& y, const MatrixJ& x ) {
 MatrixJ z(x.myEnv);
 z.DeepCopy( x );
 z -= y;
 z.stacked = 1;
 return z;
}

MatrixJ operator*( const MatrixJ& x, const Jet& y ) {
  MatrixJ z(x.rows(),x.cols(),x.myEnv);
  MLJ* xPtr = x.ml;
  MLJ* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixJ operator*( const Jet& y, const MatrixJ& x ) {
  MatrixJ z(x.rows(),x.cols(),x.myEnv);
  MLJ* xPtr = x.ml;
  MLJ* zPtr = z.ml;

  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] * y;
    }
  }
 z.stacked = 1;
 return z;
}

MatrixJ operator*( const MatrixJ& x, const MatrixJ& y )  {
  MatrixJ z(x.rows(),y.cols(),x.myEnv);
  MLJ* xPtr = x.ml;
  MLJ* yPtr = y.ml;
  MLJ* zPtr = z.ml;

  if( x.cols() != y.rows())
    x.error("# of rows of 2nd matrix must equal # cols of 1st for multiply");
  Jet tmp;
  tmp.setEnvTo( x.myEnv );     //to prevent accidental use of last env
  for(int row = 0; row < x.rows(); row++) {
    for(int col = 0; col < y.cols(); col++){
      Jet sum;
      sum.setEnvTo( x.myEnv ); //to prevent accidental use of last env 
      sum = 0.0;
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

MatrixJ operator/( const MatrixJ& x, const Jet& y ) {
  MatrixJ z(x.rows(),x.cols(),x.myEnv);
  MLJ* xPtr = x.ml;
  MLJ* zPtr = z.ml;

  if(y == 0)
    cerr << "MatrixJ:operator/ divide by zero" << endl;
  for(int i = 0; i< x.rows() ; i++) {
    for(int j = 0; j < x.cols(); j++) {
      zPtr->m[i][j] = xPtr->m[i][j] / y;
    }
  }
 z.stacked = 1;
 return z;
}

/********************************************************
The private support functions for determinant & inverse:
*********************************************************/

void MatrixJ::copy_column( MatrixJ& mm, int from_col, int to_col ){
  if(rows()  != mm.rows())
    error("number of rows must be equal for copy_column()");
  for(int row=0; row < rows(); row++)
    ml->m[row][to_col] = mm.ml->m[row][from_col];
}

void MatrixJ::switch_rows( int row1, int row2 ) {
  MatrixJ temp(cols(),myEnv);
  
  int col; // O.K.
  
  for( col = 0; col < cols(); col++)
    // temporarily store row 1:
    temp.ml->m[col][0] = ml->m[row1][col];
  for(col = 0; col < cols(); col++)
    ml->m[row1][col] = ml->m[row2][col]; // move row2 to row1
  for(col = 0; col < cols(); col++)
    ml->m[row2][col] = temp.ml->m[col][0]; // move temp to row2
}

MatrixJ MatrixJ::scale( )  {
  Jet temp;
  temp.setEnvTo( myEnv );
  if (rows() <= 0 || cols() <= 0)
    error(" bad MatrixJ for scale()");
  if (rows() != cols())
    error(" MatrixJ must be square for scale()");   
  MatrixJ scale_vector(cols(),myEnv);
  
  for (int row = 0; row < rows(); row++){
    Jet maximum;
    maximum.setEnvTo( myEnv );
    maximum = 0.0;
    for(int col = 0; col < cols(); col++) {
      temp = ml->m[row][col];
      if (abs(temp.standardPart()) > abs(maximum.standardPart()))
	maximum = temp; 
    }
    if(maximum == 0.0) {
      cerr << "*** WARNING *** Attempt to divide by zero."
	   << "Singular MatrixJ in scale()" << endl;
      return *this;
    }
    // save scaling
    scale_vector.ml->m[row][0] = 1.0/maximum; 
  }
  return scale_vector;
}

MatrixJ MatrixJ::lu_decompose( int* indx, int& d, Jet__environment* pje ) {
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
  Jet dum; // from the book - I don't know signj
  Jet sum;
  Jet maximum;
  MatrixJ lu_decomp(pje);
  Jet tmp;
  dum.setEnvTo( pje );
  sum.setEnvTo( pje );
  maximum.setEnvTo( pje );
  tmp.setEnvTo( pje );


  // make a direct copy of the original matrix:
  lu_decomp = *this;
  MatrixJ scale_vector(pje);
  scale_vector = lu_decomp.scale(); // scale the matrix
  
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
	    //if(abs(sum) < M_SMALL*abs(tmp))
	      //sum = complex_0;
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
	  //if(abs(sum) < M_SMALL*abs(tmp))
	    //sum = 0.0;
	}
	lu_decomp.ml->m[i][j] = sum;
      }
      // figure of merit for pivot:
      dum = scale_vector.ml->m[i][0] * sum;
      if (abs(dum.standardPart()) >= abs(maximum.standardPart())){
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
      if(lu_decomp.ml->m[j][j] == 0.0) {
	cerr << "*** ERROR *** Attempt to divide by zero" << endl;
	error("singular MatrixJ in lu_decomp()");
      }
      dum = 1.0/lu_decomp.ml->m[j][j];
      for(i=j+1; i <= cols()-1; i++)
	lu_decomp.ml->m[i][j] *= dum;
    }

  }
  return lu_decomp;
}

void MatrixJ::lu_back_subst( int* indx, MatrixJ& b )  {

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
  Jet sum;
  Jet tmp;
  sum.setEnvTo( myEnv );
  tmp.setEnvTo( myEnv );

  for(i=0;i < rows(); i++){
    ip = indx[i];
    sum = b.ml->m[ip][0];
    b.ml->m[ip][0] = b.ml->m[i][0];
    if (ii >= 0) {
      for(j = ii; j <= i-1; j++) {
	tmp = ml->m[i][j] * b.ml->m[j][0];
	sum -= tmp;
	//if(abs(sum) < M_SMALL*abs(tmp))
	//sum = 0.0;
      }
    } else if(sum != 0.0)
      ii = i;
    b.ml->m[i][0] = sum;
  }
  for(i = rows() -1; i >= 0; i--){
    sum = b.ml->m[i][0];
    if (i < cols() -1) {
      for (j = i + 1; j <= rows()-1; j++) {
	tmp = ml->m[i][j] * b.ml->m[j][0];
	sum -= tmp;
	//if(abs(sum) < M_SMALL*abs(tmp))
	//sum = 0.0;
      }
    }
    // store a component of the soln vector X:
    b.ml->m[i][0] = sum/ml->m[i][i];
  }
}

void MatrixJ::error( char * msg1, char * msg2 ) const {
  cerr << "MatrixJ error: " << msg1 << " " << msg2  << endl;
  exit(1);
}




