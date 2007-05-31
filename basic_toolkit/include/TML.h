/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Basic TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      TML.h
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
******  Sep-Nov 2005   Jean-Francois Ostiguy
******                 ostiguy@fnal.gov
******  
******   - refactored code to use single template parameter instead of two
******   - introduced implicit conversions 
******   - using boost::intrusive_pointer for reference counting
******   - eliminated access to internals of implementation class (TML) from class TMatrix.
******   - eliminated separate MatrixC class implementation (used to be derived from Matrix<T1,T2>)
******   - organized code to support both implicit and explicit template instantiations
******   - fixed incorrect complex version of eigenvalues/eigenvectors 
******   - separated eigenvalue/eigenvector reordering function 
******   - eliminated code that attempted to discriminate between objects allocated
******     on the stack and objects allocated from the free store.
******                                                                
**************************************************************************
*************************************************************************/
#ifndef TML_H
#define TML_H

#include <iostream>
#include <complex>
#include <cmath>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/ReferenceCounter.h>
#include <basic_toolkit/MLPtr.h>

template<typename T>
class TMatrix;

template<typename T>
class TVector;

template<typename T> MLPtr<T> Negate( MLPtr<T> const& );

template<typename T> MLPtr<T> add(MLPtr<T> const& x, MLPtr<T> const& y); 
template<typename T> MLPtr<T> add(MLPtr<T> const& x, const T& y);

template<typename T> MLPtr<T> subtract(MLPtr<T> const& x, MLPtr<T> const& y); 


template<typename T> MLPtr<T> multiply(  MLPtr<T>   const& x, const T& y); 
template<typename T> MLPtr<T> multiply(  MLPtr<T>   const& x, MLPtr<T> const& y);
template<typename T> MLPtr<T> multiply(  MLPtr<T>   const& x, TVector<T>  const& y);
template<typename T> MLPtr<T> multiply(  TVector<T> const& x, MLPtr<T>    const& y);

template<typename T> MLPtr<T> divide (MLPtr<T> const& x,   T        const& y); 
template<typename T> MLPtr<T> divide (T        const& y,   MLPtr<T> const& x  ); 
template<typename T> MLPtr<T> divide (MLPtr<T> const& y,   MLPtr<T> const& x);


MLPtr<double> real_part( MLPtr<std::complex<double> > const& x );
MLPtr<double> imag_part( MLPtr<std::complex<double> > const& x );

template<typename T> std::ostream& operator<< (std::ostream& os, TML<T> const& x);

template <typename T>
class TVector;

typedef TVector<double> Vector;

template<typename T> 
class DLLLOCAL TML : public ReferenceCounter<TML<T> > {

  template <typename U>
  friend class TML;

 private:

  T** mdata_;      // matrix data
  int nrows_;      // number of rows
  int ncols_;      // number of columns

  void       copy_column(MLPtr<T> const& mm, int from_col, int to_col); 
  void       switch_rows(int row1, int row2); 

  MLPtr<T>   scale() const; 
  MLPtr<T>   lu_decompose(int* indx, int& d ) const;
  void       lu_back_subst(int* indx, MLPtr<T>& b);  

  static double tiny_;     // pivot threshold

 public:

  // Constructors and destructors_____________________________________

  TML();

  TML(TML const&);

  template <typename U>
  TML(TML<U> const&);

  explicit TML(const char*, int);
  explicit TML(int rows, int columns,        T  initval = T());
  explicit TML(int rows, int columns,  const T* initval);

 ~TML();
 
  void dispose() {delete this;} // needed by ReferenceCounter
  MLPtr<T> clone() 
                           { return MLPtr<T>( new TML<T>(*this) ); } 

  // Public member functions__________________________________________

  void       switch_columns(int col1, int col2); 

  inline int rows() const { return nrows_;}
  inline int cols() const { return ncols_;}

  void                 clear(); 

  MLPtr<T> Square()    const; 
  MLPtr<T> transpose() const; 
  MLPtr<T> dagger()    const; 
  MLPtr<T> inverse()   const;
 
  MLPtr<std::complex<double> > eigenValues()     const; 
  MLPtr<std::complex<double> > eigenVectors()    const; 
  
  static void orderCoordinates(MLPtr<std::complex<double> >& eigenvalues,   MLPtr<std::complex<double> >& eigenvectors); 
 
  void                              SVD ( MLPtr<T>& U, Vector& W, MLPtr<T>& V ) const;   
  static TVector<T>       backSubstitute( MLPtr<T> const& U, Vector const& W, MLPtr<T> const & V, 
                                          TVector<T> const& rhs, double threshold);  

  T      determinant() const; 
  T      trace() const; 
  bool   isOrthogonal() const;

  T& operator()(int i, int j);  
  T  operator()(int i, int j) const; 
  T& operator()(int i); 
  T  operator()(int i) const; 

  friend MLPtr<T> add<T>(MLPtr<T> const& x, MLPtr<T> const& y); 
  friend MLPtr<T> add<T>(MLPtr<T> const& x, const T& y);
 
  friend MLPtr<T> subtract<T>(MLPtr<T> const& x, MLPtr<T> const& y); 

  friend MLPtr<T> multiply<T>( MLPtr<T> const& x, const T& y); 
  friend MLPtr<T> multiply<T>( MLPtr<T> const& x, MLPtr<T> const& y);

  friend MLPtr<T> multiply<T>( MLPtr<T>   const& x, TVector<T>   const& y);
  friend MLPtr<T> multiply<T>( TVector<T> const& x, MLPtr<T>     const& y);
  
  
  friend MLPtr<T> divide<T>( MLPtr<T>  const&  x, T        const& y); 
  friend MLPtr<T> divide<T>( T         const&  y, MLPtr<T> const& x ); 
  friend MLPtr<T> divide<T>( MLPtr<T>  const&  y, MLPtr<T> const& x ); 

  friend MLPtr<T> Negate<T>( MLPtr<T> const& x ); 

  friend MLPtr<double> real_part( MLPtr<std::complex<double> > const& x );
  friend MLPtr<double> imag_part( MLPtr<std::complex<double> > const& x );

  friend std::ostream& operator<< <T>(std::ostream& os, const TML<T>& x);

  TML<T>& operator=(TML<T> const&);
  TML<T>& operator+=(T const&);
  TML<T>& operator-=(T const&);

  bool operator==(TML<T> const&);
  bool operator==(T      const&);


  // Exception subclasses____________________________________________________

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
    // Attempting operations on matrices whose dimensions
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

  struct GenericMatrixException : public std::exception
  {
    // Miscellaneous other errors
    GenericMatrixException( int, int, const char*, const char* = "" );
    // 1st argument: number of rows
    // 2nd         : number of columns
    // 3rd         : identifies function containing throw
    // 4th         : identifies type of error
    ~GenericMatrixException() throw() {}
    const char* what() const throw();
    std::string errorString;
    int r, c;
  };

};

// TML Specializations





template<>
template<>
TML<std::complex<double> >::TML(const TML<double>&);

template<> MLPtr<double>                 TML<double>::dagger()                      const;     
template<> MLPtr<std::complex<double> >  TML<std::complex<double> >::dagger()       const; 

template<> MLPtr<std::complex<double> >  TML<double>::eigenValues()                 const;  
template<> MLPtr<std::complex<double> >  TML<double>::eigenVectors()                const; 

template<> MLPtr<std::complex<double> >  TML<std::complex<double> >::eigenValues()  const;
template<> MLPtr<std::complex<double> >  TML<std::complex<double> >::eigenVectors() const;

template<>
void TML<double>::orderCoordinates(MLPtr<std::complex<double> >& eigenvalues,   MLPtr<std::complex<double> >& eigenvectors);  // undefined !
template<>
void TML<std::complex<double> >::orderCoordinates(MLPtr<std::complex<double> >& eigenvalues,   MLPtr<std::complex<double> >& eigenvectors); 

template<> void TML<double>::SVD (MLPtr<double>& U, Vector& W, MLPtr<double>& V ) const;   

template<> TVector<double>  TML<double>::backSubstitute( MLPtr<double> const& U, TVector<double> const& W, MLPtr<double> const& V, 
                                                         TVector<double> const& rhs, double threshold);  


#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/TML.tcc>
#endif

#endif //TML_H
