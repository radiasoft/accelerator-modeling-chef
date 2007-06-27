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
******  Revision History 
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
#ifndef TMATRIX_H
#define TMATRIX_H

#include <exception>
#include <string>
#include <complex>
#include <basic_toolkit/TML.h>

template <typename T> class TVector; 
template <typename T> class TMatrix; 

// Friend functions

TMatrix<double> real( TMatrix<std::complex<double> > const& x );
TMatrix<double> imag( TMatrix<std::complex<double> > const& x );

template<typename T> std::ostream& operator<<(std::ostream&, TMatrix<T> const&);
template<typename T> TMatrix<T> operator+(TMatrix<T> const&, TMatrix<T> const&);
template<typename T> TMatrix<T> operator+(TMatrix<T> const&, T          const&); 
template<typename T> TMatrix<T> operator+(T          const&, TMatrix<T> const&); 

template<typename T> TMatrix<T> operator-(TMatrix<T> const&); 
template<typename T> TMatrix<T> operator-(TMatrix<T> const&, TMatrix<T> const&); 
template<typename T> TMatrix<T> operator-(TMatrix<T> const&, T          const&); 
template<typename T> TMatrix<T> operator-(T          const&, TMatrix<T> const&); 

template<typename T> TMatrix<T>  operator*(TMatrix<T> const&, TMatrix<T> const&); 
template<typename T> TMatrix<T>  operator*(TMatrix<T> const&, T          const&);
template<typename T> TMatrix<T>  operator*(T          const&, TMatrix<T> const&);

TMatrix<std::complex<double> >  operator*( TMatrix<std::complex<double> > const& x, TMatrix<double>                const& y);
TMatrix<std::complex<double> >  operator*( TMatrix<double>                const& y, TMatrix<std::complex<double> > const& x);

template<typename T> TMatrix<T> operator*(TMatrix<T>  const&,  TVector<T> const&); // right multiply
template<typename T> TMatrix<T> operator*(TVector<T>  const&,  TMatrix<T> const&);  // left multiply


template<typename T> TMatrix<T> operator/(TMatrix<T> const&,     T          const&);
template<typename T> TMatrix<T> operator/(T          const&,     TMatrix<T> const&);
template<typename T> TMatrix<T> operator/(TMatrix<T> const&,     TMatrix<T> const&);

template<typename T> bool operator==( TMatrix<T> const&, TMatrix<T> const& );
template<typename T> bool operator==( TMatrix<T> const&, T          const& );
template<typename T> bool operator==( T          const&, TMatrix<T> const& );
template<typename T> bool operator!=( TMatrix<T> const&, TMatrix<T> const& );
template<typename T> bool operator!=( TMatrix<T> const&, T          const& );
template<typename T> bool operator!=( T          const&, TMatrix<T> const& );

template<typename T>

class DLLEXPORT TMatrix {

  template<typename U>
  friend class TMatrix;

protected:

  MLPtr<T> ml_;

  // Functions used by the eigenroutines.

 private:

  void       copy_column(TMatrix<T>& x, int, int );
  void       switch_rows( int, int );
  TMatrix    scale() const;
  TMatrix    lu_decompose( int*, int& ) const;
  void       lu_back_subst( int*, TMatrix<T>& );

public:

  // Constructors and destructors_____________________________________

  TMatrix();
  TMatrix(int);
  TMatrix(int rows, int columns);
  TMatrix(int rows, int columns, T  initval);
  TMatrix(int rows, int columns, T* initval);
  TMatrix(const char* flag, int dimension); // create an identity matrix
                                           // or a symplectic matrix

  template<typename U>
  TMatrix(TMatrix<U> const& );

  TMatrix(TMatrix const& );

  TMatrix(MLPtr<T> const& ml);

  TMatrix<T>& DeepCopy(TMatrix<T> const& x); // forces a deep copy  

 ~TMatrix();

  // Public member functions__________________________________________


  void      switch_columns( int, int ); // used by SurveyMatcher

  inline int rows() const { return ml_->rows();}
  inline int cols() const { return ml_->cols();}

  TMatrix                        transpose()    const; 
  TMatrix                        dagger()       const;
  TMatrix                        Square()       const;
  T                              determinant()  const;
  TMatrix                        inverse()      const;
  TMatrix<std::complex<double> > eigenValues()  const;
  TMatrix<std::complex<double> > eigenVectors() const;
  T                              trace()        const ; 

  static void                    GaussJordan( TMatrix& A, TVector<T> & rhs);
  static void                    GaussJordan( TMatrix& A, TMatrix<T> & rhs);

  void                           SVD( TMatrix& U, Vector& W, TMatrix& V);

  static TVector<T>              backSubstitute(TMatrix const& U, TVector<T> const& W, 
                                                TMatrix const& V, TVector<T> const& rhs, double threshold=-1.0); // threshold < 0.0 ==> threshold = svmax*macheps
  bool                           isOrthogonal() const;

  // Operators________________________________________________________
  
  
  TMatrix& operator=(TMatrix    const&);
  TMatrix& operator=(TVector<T> const&);

  T& operator()(int row, int column);
  T  operator()(int row, int column) const;

  T  getElement(int row, int column) const;

  T& operator()(int row);
  T  operator()(int row) const;

  TMatrix<T>& operator+=( T const&);
  TMatrix<T>& operator-=( T const&);

  // Friends

  friend TMatrix<double> real( TMatrix<std::complex<double> > const& x );
  friend TMatrix<double> imag( TMatrix<std::complex<double> > const& x );

  friend bool operator==<>( TMatrix const&, TMatrix const& );
  friend bool operator==<>( TMatrix const&, T       const& );
  friend bool operator==<>( T       const&, TMatrix const& );

  friend TMatrix operator+<>(TMatrix const&, TMatrix  const&);
  friend TMatrix operator+<>(TMatrix const&, T        const&); 
  friend TMatrix operator+<>(T       const&, TMatrix  const&); 
  friend TMatrix operator-<>(TMatrix const&); 
  friend TMatrix operator-<>(TMatrix const&, TMatrix  const&); 
  friend TMatrix operator-<>(TMatrix const&, T        const&); 
  friend TMatrix operator-<>(T       const&, TMatrix  const&); 
  friend TMatrix operator*<>(TMatrix const&, TMatrix  const&); 

  friend TMatrix<std::complex<double> > operator*(TMatrix<double>                const&,  TMatrix<std::complex<double> > const&); 
  friend TMatrix<std::complex<double> > operator*(TMatrix<std::complex<double> > const&,  TMatrix<double>                const&); 

  friend TMatrix operator*<>(TMatrix     const&,   T          const&);
  friend TMatrix operator*<>(T           const&,   TMatrix    const&);
  friend TMatrix operator*<>(TMatrix     const&,   TVector<T> const&);
  friend TMatrix operator*<>(TVector<T>  const&,   TMatrix    const&);

  friend TMatrix operator/<>(TMatrix const&,   T       const&);
  friend TMatrix operator/<>(T       const&,   TMatrix const&);
  friend TMatrix operator/<>(TMatrix const&,   TMatrix const&);

  friend std::ostream& operator<< <T>(std::ostream&, TMatrix<T> const&);

};


// TMatrix Specializations


template<> 
template<> 
TMatrix<std::complex<double> >::TMatrix(TMatrix<double> const& );  

template<> 
TMatrix<double>                 TMatrix<double>::dagger() const; 

template<> 
TMatrix<std::complex<double> >  TMatrix<std::complex<double> >::dagger() const; 

 
template<> TMatrix<std::complex<double> > TMatrix<double>::eigenValues()    const;
template<> TMatrix<std::complex<double> > TMatrix<double>::eigenVectors()   const; 

template<> TMatrix<std::complex<double> > TMatrix<std::complex<double> >::eigenValues()  const;
template<> TMatrix<std::complex<double> > TMatrix<std::complex<double> >::eigenVectors() const;

template<> void  TMatrix<double>::SVD( TMatrix<double>&, Vector&, TMatrix<double>& );

template<> 
TVector<double> TMatrix<double>::backSubstitute(TMatrix<double> const& U, TVector<double> const& W, 
                                                TMatrix<double> const& V, TVector<double> const& rhs, double threshold);


#ifndef BASICTOOLKIT_EXPLICIT_TEMPLATES
#include <basic_toolkit/TMatrix.tcc>
#endif


#endif // TMATRIX_H


