/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TMatrix.h
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
******                                                                
******                                                                
**************************************************************************
*************************************************************************
#ifndef TMATRIX_H
#define TMATRIX_H

#include <exception>
#include <string>
#include <complex>
#include <complexAddon.h>

#include <TML.h>

template<typename T> class TMatrix; 


// standalone functions

TMatrix<double> real( const TMatrix<FNAL::Complex>& x );
TMatrix<double> imag( const TMatrix<FNAL::Complex>& x );

// Friend functions

template<typename T> std::ostream& operator<<(std::ostream&, const TMatrix<T>&);

template<typename T> TMatrix<T> operator+(const TMatrix<T>&, const TMatrix<T>&);
template<typename T> TMatrix<T> operator+(const TMatrix<T>&, const T&); 
template<typename T> TMatrix<T> operator+(const T&, const TMatrix<T>&); 

template<typename T> TMatrix<T> operator-(const TMatrix<T>&); 
template<typename T> TMatrix<T> operator-(const TMatrix<T>&, const TMatrix<T>&); 
template<typename T> TMatrix<T> operator-(const TMatrix<T>&, const T&); 
template<typename T> TMatrix<T> operator-(const T&, const TMatrix<T>&); 

template<typename T> TMatrix<T> operator*(const TMatrix<T>&, const TMatrix<T>&); 
template<typename T> TMatrix<T> operator*(const TMatrix<T>&, const T);
template<typename T> TMatrix<T> operator*(const T, const TMatrix<T>&);

template<typename T> TMatrix<T> operator/(const TMatrix<T>&, const T);
template<typename T> TMatrix<T> operator/(const T, TMatrix<T>&);
template<typename T> TMatrix<T> operator/(TMatrix<T>&, TMatrix<T>&);

template<typename T> bool operator==( const TMatrix<T>&, const TMatrix<T>& );
template<typename T> bool operator==( const TMatrix<T>&, const T& );
template<typename T> bool operator==( const T&, const TMatrix<T>& );
template<typename T> void operator-=( TMatrix<T>&, const TMatrix<T>& );
template<typename T> void operator*=( TMatrix<T>&, const TMatrix<T>& );
template<typename T> bool operator!=( const TMatrix<T>&, const TMatrix<T>& );
template<typename T> bool operator!=( const TMatrix<T>&, const T& );
template<typename T> bool operator!=( const T&, const TMatrix<T>& );


template<typename T>
class TMatrix
{
protected:
  TML<T>* _ml;

public:
  // Constructors and destructors_____________________________________
  TMatrix();
  TMatrix(int);
  TMatrix(int rows, int columns);
  TMatrix(int rows, int columns, T initval);
  TMatrix(int rows, int columns, T* initval);
  TMatrix(const char* flag, int dimension); // create an identity matrix
                                            // or a symplectic matrix
  TMatrix(const TMatrix<T>& X);
  ~TMatrix();

  // Temporary measure: _stacked will be eliminated.
  bool _stacked;

  // Public member functions__________________________________________
  inline int rows() const { return _ml->_r;}
  inline int cols() const { return _ml->_c;}

  TMatrix<T> transpose() const;
  TMatrix<T> Square() const;
  T determinant() const;
  TMatrix<T> inverse() const;
  TMatrix<FNAL::Complex> eigenValues();
  TMatrix<FNAL::Complex> eigenVectors();
  T trace(); // return the trace of a square matrix
  void SVD( TMatrix&, TMatrix&, TMatrix& ) const;
  
  bool isOrthogonal() const;

  // Functions used by the eigen routines.
  void       _copy_column(TMatrix<T>& x, int, int );
  void       _switch_columns( int, int );
  void       _switch_rows( int, int );
  TMatrix<T> _scale();
  TMatrix<T> _lu_decompose( int*, int& ) const;
  void       _lu_back_subst( int*, TMatrix<T>& );

  // Operators________________________________________________________
  TMatrix<T>& operator=(const TMatrix&);
  TMatrix<T>& DeepCopy(const TMatrix&);
  T& operator()(int row, int column);
  T  operator()(int row, int column) const;
  T  getElement(int row, int column) const;
  T& operator()(int row);
  T  operator()(int row) const;
  void operator+=( const T&);
  void operator-=( const T&);

  // Friends
  friend bool operator==<>( const TMatrix&, const TMatrix& );
  friend bool operator==<>( const TMatrix&, const T& );
  friend bool operator==<>( const T&, const TMatrix& );

  friend std::ostream& operator<<<>(std::ostream&, const TMatrix&);

  friend TMatrix operator+<>(const TMatrix&, const TMatrix&);
  friend TMatrix operator+<>(const TMatrix&, const T&); 
  friend TMatrix operator+<>(const T&, const TMatrix&); 
  friend TMatrix operator-<>(const TMatrix&); 
  friend TMatrix operator-<>(const TMatrix&, const TMatrix&); 
  friend TMatrix operator-<>(const TMatrix&, const T&); 
  friend TMatrix operator-<>(const T&, const TMatrix&); 
  friend TMatrix operator*<>(const TMatrix&, const TMatrix&); 

  friend TMatrix<FNAL::Complex> operator*(const TMatrix<double>&, const TMatrix<FNAL::Complex>&); 
  friend TMatrix<FNAL::Complex> operator*(const TMatrix<FNAL::Complex>&, const TMatrix<double>&); 

  friend TMatrix operator*<>(const TMatrix&, const T);
  friend TMatrix operator*<>(const T, const TMatrix&);
  friend TMatrix operator/<>(const TMatrix&, const T);
  friend TMatrix operator/<>(const T, TMatrix&);
  friend TMatrix operator/<>(TMatrix&, TMatrix&);

  // Exception subclasses
  struct IndexRange : public std::exception
  {
    // User has tried to access a matrix element with
    // out-of-range indices: M(i,j) with 
    // i or j < 0
    // or i > rows - 1
    // or j > cols - 1
    IndexRange( int, int, int, int, const char* );
    ~IndexRange() throw() {}
    const char* what() const throw();
    int i,  j;   // Row and column indices called
    int im, jm;  // Maximum allowed values
  };

  struct NotVector : public std::exception
  {
    // Attempt to access a matrix element as a vector element,
    // with one index, when the matrix is not a vector.
    NotVector( int, int, int, const char* );
    ~NotVector() throw() {}
    const char* what() const throw();
    int i, r, c;  // index, rows, columns
  };

  struct Incompatible : public std::exception
  {
    // Attempting operations on matrices who dimensions
    // are not compatible.
    Incompatible( int, int, int, int, const char* );
    ~Incompatible() throw() {}
    const char* what() const throw();
    int ra, ca, rb, cb;
  };

  struct NotSquare : public std::exception
  {
    // Non-square matrix handed over for square
    // matrix operations
    NotSquare( int, int, const char* );
    ~NotSquare() throw() {}
    const char* what() const throw();
    int r, c;
  };

  struct GenericException : public std::exception
  {
    // Miscellaneous other errors
    GenericException( int, int, const char*, const char* = "" );
    // 1st argument: number of rows
    // 2nd         : number of columns
    // 3rd         : identifies function containing throw
    // 4th         : identifies type of error
    ~GenericException() throw() {}
    const char* what() const throw();
    std::string errorString;
    int r, c;
  };
};


// TMatrix Specializations
 
template<> TMatrix<FNAL::Complex> TMatrix<double>::eigenValues();
template<> TMatrix<FNAL::Complex> TMatrix<double>::eigenVectors(); 

template<> TMatrix<FNAL::Complex> TMatrix<FNAL::Complex>::eigenValues();
template<> TMatrix<FNAL::Complex> TMatrix<FNAL::Complex>::eigenVectors();



// Special class RandomOrthogonal

class RandomOrthogonal
{
  public: 
    RandomOrthogonal( int /* dimension */ );
    ~RandomOrthogonal();

    void omitIndex( int );  
    void omitIndex( int, int );  
    void includeIndex( int );
    void includeIndex( int, int );
    // By default, all are included
    void setNumberOfPasses( int );
    // By default, = 1.      

    void setRange( int, double /* radians */, double );
    void setRange( int, int, double /* radians */, double );
    // Default range = [ 0, 2 pi ) for all indices

    TMatrix<double> build();
    
  private:
    int      _dim;
    int      _passes;
    bool**   _omitted;        // owned
    double** _lowerTheta;     // owned
    double** _upperTheta;     // owned
    double** _rangeTheta;     // owned
};


// Derived class MatrixC
// Done to replicate member functions dagger() and MatrixC(const TMatrix<double>&)

struct MatrixC : public TMatrix<FNAL::Complex>
{
  MatrixC();
  MatrixC(const MatrixC& );
  MatrixC(const TMatrix<FNAL::Complex>& );
  MatrixC(const TMatrix<double>&);
  MatrixC(int);
  MatrixC(int rows, int columns );
  MatrixC(int rows, int columns, const FNAL::Complex& initval );
  MatrixC(int rows, int columns, FNAL::Complex* initval);
  MatrixC(const char* flag, int dimension); // create an identity matrix
				     // or a symplectic matrix
  ~MatrixC();

  MatrixC dagger() const;
};


typedef TMatrix<double>         Matrix;
typedef TMatrix<double>         MatrixD;
typedef TMatrix<int>            MatrixI;

MatrixD real( const MatrixC& x );
MatrixD imag( const MatrixC& x );

#endif // TMATRIX_H


