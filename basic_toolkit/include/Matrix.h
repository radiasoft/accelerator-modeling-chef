#ifndef MATRIX_H
#define MATRIX_H

#include "ML.h"

class MatrixC;


//class ostream;


class MatrixD {
private:
  MLD* ml;

  // Friends
  friend class MatrixC;

  void copy_column(MatrixD& x,int from_col, int to_col);
  void switch_columns(int,int);
  void switch_rows(int,int);
  MatrixD scale();
  MatrixD lu_decompose(int*, int&) const;
  void lu_back_subst(int*,MatrixD&);
  void error(char * msgl, char * msg2 = " ")const ;

public:
  char  stacked;

  // Constructors and destructors_____________________________________
  MatrixD();
  MatrixD(int);
  MatrixD(int rows, int columns);
  MatrixD(int rows, int columns, double initval);
  MatrixD(int rows, int columns, double* initval);
  MatrixD(char* flag, int dimension); // create an identity matrix
				     // or a symplectic matrix
  MatrixD(const MatrixD& X);
  ~MatrixD();

  // Public member functions__________________________________________
  inline int rows() const { return ml->r;}
  inline int cols() const { return ml->c;}
  MatrixD transpose() const;
  MatrixD Square() const;
  double determinant();
  MatrixD inverse() const;
  MatrixC eigenValues();
  MatrixC eigenVectors();
  double trace(); // return the trace of a square matrix
  void SVD( MatrixD&, MatrixD&, MatrixD& ) const;
  

  // Operators________________________________________________________

  MatrixD& operator=(const MatrixD&);
  MatrixD& DeepCopy(const MatrixD&);
  double& operator()(int row, int column);
  double  getElement(int row, int column) const;
  double& operator()(int row);
  void operator+=( const double&);
  void operator-=( const double&);
friend char operator==( const MatrixD&, const MatrixD& );
friend char operator==( const MatrixD&, const double& );
friend char operator==( const double&, const MatrixD& );
friend ostream& operator<<(ostream&, const MatrixD&);
//friend istream& operator>>(istream&, MatrixD&);
friend MatrixD operator+ (const MatrixD &,const MatrixD &);
friend MatrixD operator+ (const MatrixD &,const double&); 
friend MatrixD operator+ (const double& ,const MatrixD &); 
friend MatrixD operator-(const MatrixD &); 
friend MatrixD operator-(const MatrixD &,const MatrixD&); 
friend MatrixD operator-(const MatrixD &,const double&); 
friend MatrixD operator-(const double&,const MatrixD &); 
friend MatrixD operator*(const MatrixD &,const MatrixD&); 
friend MatrixC operator*(const MatrixD &,const MatrixC&); 
friend MatrixC operator*(const MatrixC &,const MatrixD&); 
friend MatrixD operator*(const MatrixD &,const double);
friend MatrixD operator*(const double,const MatrixD &);
friend MatrixD operator/(const MatrixD &,const double);
friend MatrixD operator/(const double,MatrixD &);
friend MatrixD operator/(MatrixD& ,MatrixD &);
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

void operator-=( MatrixD&, const MatrixD& );
void operator*=( MatrixD&, const MatrixD& );
//void operator/=( MatrixD&, const MatrixD& );
//void operator/=( MatrixD&, const double );
char operator!=( const MatrixD&, const MatrixD& );
char operator!=( const MatrixD&, const double& );
char operator!=( const double&, const MatrixD& );

class MatrixC {
private:
  MLC* ml;
  friend class MatrixD;


  void copy_column(MatrixC& x,int from_col, int to_col);
  void switch_columns(int,int);
  void switch_rows(int,int);
  MatrixC scale();
  MatrixC lu_decompose(int*, int&);
  void lu_back_subst(int*,MatrixC&);
  void error(char * msgl, char * msg2 = " ")const ;

public:
  char  stacked;

  // Constructors and destructors_____________________________________
  MatrixC();
  MatrixC(int);
  MatrixC(int rows, int columns );
  MatrixC(int rows, int columns, const Complex& initval );
  MatrixC(int rows, int columns, Complex* initval);
  MatrixC(const MatrixD&);
  MatrixC(char* flag, int dimension); // create an identity matrix
				     // or a symplectic matrix
  MatrixC(const MatrixC& X);
  ~MatrixC();

  // Public member functions__________________________________________
  inline int rows() const { return ml->r;}
  inline int cols() const { return ml->c;}
  MatrixC transpose() const;
  MatrixC Square() const;
  Complex determinant();
  MatrixC inverse();
  MatrixC eigenValues();
  MatrixC eigenVectors();
  Complex trace(); // return the trace of a square matrix
  

  // Operators________________________________________________________

  MatrixC& operator=(const MatrixC&);
  MatrixC& DeepCopy(const MatrixC&);
  Complex& operator()(int row, int column);
  Complex& operator()(int row);
  void operator+=( const Complex&);
  void operator-=( const Complex&);
friend char operator==( const MatrixC&, const MatrixC& );
friend char operator==( const MatrixC&, const Complex& );
friend char operator==( const Complex&, const MatrixC& );
friend ostream& operator<<(ostream&, const MatrixC&);
//friend istream& operator>>(istream&, MatrixC&);
friend MatrixC operator+ (const MatrixC &,const MatrixC &);
friend MatrixC operator+ (const MatrixC &,const Complex&); 
friend MatrixC operator+ (const Complex& ,const MatrixC &); 
friend MatrixC operator-(const MatrixC &); 
friend MatrixC operator-(const MatrixC &,const MatrixC&); 
friend MatrixC operator-(const MatrixC &,const Complex&); 
friend MatrixC operator-(const Complex&,const MatrixC &); 
friend MatrixC operator*(const MatrixC &,const MatrixC&); 
friend MatrixC operator*(const MatrixC &,const MatrixD&); 
friend MatrixC operator*(const MatrixD &,const MatrixC&); 
friend MatrixC operator*(const MatrixC &,const Complex&);
friend MatrixC operator*(const Complex& ,const MatrixC &);
friend MatrixC operator/(const MatrixC &,const Complex&);
friend MatrixC operator/(const Complex&,MatrixC &);
friend MatrixC operator/(const MatrixC &,const double);
friend MatrixC operator/(const double,MatrixC &);
friend MatrixC operator/(MatrixC& ,MatrixC &);
friend MatrixC operator/(MatrixD& ,MatrixC &);
friend MatrixC operator/(MatrixC& ,MatrixD &);
#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

};

void operator-=( MatrixC&, const MatrixC& );
void operator*=( MatrixC&, const MatrixC& );
//void operator/=( MatrixC&, const MatrixC& );
//void operator/=( MatrixC&, const complex );
char operator!=( const MatrixC&, const MatrixC& );
char operator!=( const MatrixC&, const Complex& );
char operator!=( const Complex&, const MatrixC& );

class MatrixI {
private:
  MLI* ml;

  void error(char * msgl, char * msg2 = " ")const ;
public:
  char  stacked;

  // Constructors and destructors_____________________________________
  MatrixI();
  MatrixI(int);
  MatrixI(int rows, int columns );
  MatrixI(int rows, int columns, int initval );
  MatrixI(int rows, int columns, int* initval);
  MatrixI(char* flag, int dimension); // create an identity matrix
				     // or a symplectic matrix
  MatrixI(const MatrixI& X);
  ~MatrixI();

  // Public member functions__________________________________________
  inline int rows() const { return ml->r;}
  inline int cols() const { return ml->c;}
  MatrixI transpose() const;
  int trace();
  // Operators________________________________________________________

  MatrixI& operator=(const MatrixI&);
  MatrixI& DeepCopy(const MatrixI&);
  int& operator()(int row, int column);
  int& operator()(int row);
  void operator+=( const int&);
  void operator-=( const int&);
friend char operator==( const MatrixI&, const MatrixI& );
friend char operator==( const MatrixI&, const int& );
friend char operator==( const int&, const MatrixI& );
friend ostream& operator<<(ostream&, const MatrixI&);
//friend istream& operator>>(istream&, MatrixI&);
friend MatrixI operator+ (const MatrixI &,const MatrixI &);
friend MatrixI operator+ (const MatrixI &,const int&); 
friend MatrixI operator+ (const int& ,const MatrixI &); 
friend MatrixI operator-(const MatrixI &); 
friend MatrixI operator-(const MatrixI &,const MatrixI&); 
friend MatrixI operator-(const MatrixI &,const int&); 
friend MatrixI operator-(const int&,const MatrixI &); 
friend MatrixI operator*(const MatrixI &,const MatrixI&); 
friend MatrixI operator*(const MatrixI &,const int);
friend MatrixI operator*(const int,const MatrixI &);
friend MatrixI operator/(const MatrixI &,const int);

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};
 


#endif // MATRIX_H
