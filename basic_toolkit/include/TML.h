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
#include <math.h>
#include <ReferenceCounter.h>
#include <MLPtr.h>

template<typename T> typename MLPtr<T>::Type Negate( typename MLPtr<T>::Type const& );

template<typename T> typename MLPtr<T>::Type add(typename MLPtr<T>::Type const& x, typename MLPtr<T>::Type const& y); 
template<typename T> typename MLPtr<T>::Type add(typename MLPtr<T>::Type const& x, const T& y);

template<typename T> typename MLPtr<T>::Type subtract(typename MLPtr<T>::Type const& x, typename MLPtr<T>::Type const& y); 


template<typename T> typename MLPtr<T>::Type multiply( typename MLPtr<T>::Type const& x, const T& y); 
template<typename T> typename MLPtr<T>::Type multiply(typename MLPtr<T>::Type const& x, typename MLPtr<T>::Type const& y);

//MLPtr<std::complex<double> >::Type  multiply( MLPtr<std::complex<double> >::Type const& x,  MLPtr<double>::Type const&  y);


template<typename T> typename MLPtr<T>::Type divide (typename MLPtr<T>::Type const& x, T const& y); 
template<typename T> typename MLPtr<T>::Type divide (T const&y,                        typename MLPtr<T>::Type const& x  ); 
template<typename T> typename MLPtr<T>::Type divide (typename MLPtr<T>::Type const& y, typename MLPtr<T>::Type const& x);


MLPtr<double>::Type real_part( MLPtr<std::complex<double> >::Type const& x );
MLPtr<double>::Type imag_part( MLPtr<std::complex<double> >::Type const& x );

template<typename T> std::ostream& operator<< (std::ostream& os, const TML<T>& x);


template<typename T> 
class TML : public ReferenceCounter {

  friend class TML<double>;
  friend class TML<std::complex<double> >;


 private:

  T** _mdata;      // matrix data
  int _nrows;      // number of rows
  int _ncols;      // number of columns

  void                    _clear();
  void                    _copy_column(typename MLPtr<T>::Type const& mm, int from_col, int to_col); 
  void                    _switch_rows(int row1, int row2); 
  typename MLPtr<T>::Type _scale() const; 
  typename MLPtr<T>::Type _lu_decompose(int* indx, int& d ) const;
  void                    _lu_back_subst(int* indx, typename MLPtr<T>::Type& b);  

 public:

  // Constructors and destructors_____________________________________

  TML();
  TML(const TML<T>&);
  explicit TML(const char*, int);
  explicit TML(int rows, int columns,        T  initval);
  explicit TML(int rows, int columns,  const T* initval);
 ~TML();
 
  operator TML<std::complex<double> >* (); 

  virtual void dispose() {delete this;} // needed by ReferenceCounter
  inline static typename MLPtr<T>::Type clone(typename MLPtr<T>::Type const& ml) 
                           { return typename MLPtr<T>::Type( new TML<T>(*ml) ); } 

  // Public member functions__________________________________________

  void                    _switch_columns(int col1, int col2); 

  inline int rows() const { return _nrows;}
  inline int cols() const { return _ncols;}

  typename MLPtr<T>::Type Square()    const; 
  typename MLPtr<T>::Type transpose() const; 
  typename MLPtr<T>::Type dagger()    const; 
  typename MLPtr<T>::Type inverse()   const;
 
  MLPtr<std::complex<double> >::Type eigenValues()     const; 
  MLPtr<std::complex<double> >::Type eigenVectors()    const; 
  
  static void orderCoordinates(MLPtr<std::complex<double> >::Type& eigenvalues,   MLPtr<std::complex<double> >::Type& eigenvectors); 
 
  void SVD (typename MLPtr<T>::Type& U, typename MLPtr<T>::Type& W, typename MLPtr<T>::Type& V ) const;   

  T      determinant() const; 
  T      trace() const; 
  bool   isOrthogonal() const;

  T& operator()(int i, int j);  
  T  operator()(int i, int j) const; 
  T& operator()(int i); 
  T  operator()(int i) const; 

  friend typename MLPtr<T>::Type add<T>(typename MLPtr<T>::Type const& x, typename MLPtr<T>::Type const& y); 
  friend typename MLPtr<T>::Type add<T>(typename MLPtr<T>::Type const& x, const T& y);
 
  friend typename MLPtr<T>::Type subtract<T>(typename MLPtr<T>::Type const& x, typename MLPtr<T>::Type const& y); 

  friend typename MLPtr<T>::Type multiply<T>( typename MLPtr<T>::Type const& x, const T& y); 
  friend typename MLPtr<T>::Type multiply<T>( typename MLPtr<T>::Type const& x, typename MLPtr<T>::Type const& y);
  
  //friend MLPtr<std::complex<double> >::Type  
  //                    multiply( MLPtr<std::complex<double> >::Type const& x,  MLPtr<double>::Type const&  y);

  
  friend typename MLPtr<T>::Type divide<T>( typename MLPtr<T>::Type const& x, T const& y); 
  friend typename MLPtr<T>::Type divide<T>( T const&y,                        typename MLPtr<T>::Type const& x ); 
  friend typename MLPtr<T>::Type divide<T>( typename MLPtr<T>::Type const& y, typename MLPtr<T>::Type const& x ); 

  friend typename MLPtr<T>::Type Negate<T>( typename MLPtr<T>::Type const& x ); 

  friend MLPtr<double>::Type real_part( MLPtr<std::complex<double> >::Type const& x );
  friend MLPtr<double>::Type imag_part( MLPtr<std::complex<double> >::Type const& x );

  friend std::ostream& operator<< <T>(std::ostream& os, const TML<T>& x);

  TML<T>& operator=(const TML<T>&);
  TML<T>& operator+=(const T&);
  TML<T>& operator-=(const T&);

  bool operator==(const TML<T>&);
  bool operator==(const T&);


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
MLPtr<double>::Type                 TML<double>::dagger()                                const;     
template<> 
MLPtr<std::complex<double> >::Type  TML<std::complex<double> >::dagger()                 const; 

template<> MLPtr<std::complex<double> >::Type TML<double>::eigenValues()                 const;  
template<> MLPtr<std::complex<double> >::Type TML<double>::eigenVectors()                const; 

template<> MLPtr<std::complex<double> >::Type TML<std::complex<double> >::eigenValues()  const;
template<> MLPtr<std::complex<double> >::Type TML<std::complex<double> >::eigenVectors() const;

template<>
void TML<double>::orderCoordinates(MLPtr<std::complex<double> >::Type& eigenvalues,   MLPtr<std::complex<double> >::Type& eigenvectors);  // undefined !
template<>
void TML<std::complex<double> >::orderCoordinates(MLPtr<std::complex<double> >::Type& eigenvalues,   MLPtr<std::complex<double> >::Type& eigenvectors); 

template<> void TML<double>::SVD (MLPtr<double>::Type& U, MLPtr<double>::Type& W, MLPtr<double>::Type& V ) const;   

template<>
TML<double>::operator   TML<std::complex<double> >* ();

template<>
TML<std::complex<double> >::operator   TML<std::complex<double> >* ();  // not implemented !


#endif //TML_H
