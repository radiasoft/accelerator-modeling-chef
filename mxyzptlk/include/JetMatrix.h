#ifndef JETMATRIX_H
#define JETMATRIX_H

#include "JetML.h"

class JetMatrix {
private:
  JetML* ml;
  friend class MatrixD;

  void copy_column(JetMatrix& x,int from_col, int to_col);
  void switch_columns(int,int);
  void switch_rows(int,int);
  JetMatrix scale();
  JetMatrix lu_decompose(int*, int&);
  void lu_back_subst(int*,JetMatrix&);
  void error(char * msgl, char * msg2 = " ")const ;

public:
  char  stacked;

  // Constructors and destructors_____________________________________
  JetMatrix();
  JetMatrix(int);
  JetMatrix(int rows, int columns );
  JetMatrix(int rows, int columns, const Complex& initval );
  JetMatrix(int rows, int columns, Complex* initval);
  JetMatrix(const MatrixD&);
  JetMatrix(char* flag, int dimension); // create an identity matrix
				     // or a symplectic matrix
  JetMatrix(const JetMatrix& X);
  ~JetMatrix();

  // Public member functions__________________________________________
  inline int rows() const { return ml->r;}
  inline int cols() const { return ml->c;}
  JetMatrix transpose() const;
  JetMatrix Square() const;
  Complex determinant();
  JetMatrix inverse();
  JetMatrix eigenValues();
  JetMatrix eigenVectors();
  Complex trace(); // return the trace of a square matrix
  

  // Operators________________________________________________________

  JetMatrix& operator=(const JetMatrix&);
  JetMatrix& DeepCopy(const JetMatrix&);
  Complex& operator()(int row, int column);
  Complex& operator()(int row);
  void operator+=( const Complex&);
  void operator-=( const Complex&);
friend char operator==( const JetMatrix&, const JetMatrix& );
friend char operator==( const JetMatrix&, const Complex& );
friend char operator==( const Complex&, const JetMatrix& );
friend ostream& operator<<(ostream&, const JetMatrix&);
//friend istream& operator>>(istream&, JetMatrix&);
friend JetMatrix operator+ (const JetMatrix &,const JetMatrix &);
friend JetMatrix operator+ (const JetMatrix &,const Complex&); 
friend JetMatrix operator+ (const Complex& ,const JetMatrix &); 
friend JetMatrix operator-(const JetMatrix &); 
friend JetMatrix operator-(const JetMatrix &,const JetMatrix&); 
friend JetMatrix operator-(const JetMatrix &,const Complex&); 
friend JetMatrix operator-(const Complex&,const JetMatrix &); 
friend JetMatrix operator*(const JetMatrix &,const JetMatrix&); 
friend JetMatrix operator*(const JetMatrix &,const MatrixD&); 
friend JetMatrix operator*(const MatrixD &,const JetMatrix&); 
friend JetMatrix operator*(const JetMatrix &,const Complex&);
friend JetMatrix operator*(const Complex& ,const JetMatrix &);
friend JetMatrix operator/(const JetMatrix &,const Complex&);
friend JetMatrix operator/(const Complex&,JetMatrix &);
friend JetMatrix operator/(const JetMatrix &,const double);
friend JetMatrix operator/(const double,JetMatrix &);
friend JetMatrix operator/(JetMatrix& ,JetMatrix &);
friend JetMatrix operator/(MatrixD& ,JetMatrix &);
friend JetMatrix operator/(JetMatrix& ,MatrixD &);
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

};

void operator-=( JetMatrix&, const JetMatrix& );
void operator*=( JetMatrix&, const JetMatrix& );
//void operator/=( JetMatrix&, const JetMatrix& );
//void operator/=( JetMatrix&, const complex );
char operator!=( const JetMatrix&, const JetMatrix& );
char operator!=( const JetMatrix&, const Complex& );
char operator!=( const Complex&, const JetMatrix& );

#endif // JETMATRIX_H
