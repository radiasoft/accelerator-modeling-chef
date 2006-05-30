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
#include <TML.h>

template<typename T> class TMatrix; 


// Friend functions

TMatrix<double> real( const TMatrix<std::complex<double> >& x );
TMatrix<double> imag( const TMatrix<std::complex<double> >& x );

template<typename T> std::ostream& operator<<(std::ostream&, const TMatrix<T>&);

template<typename T> TMatrix<T> operator+(const TMatrix<T>&, const TMatrix<T>&);
template<typename T> TMatrix<T> operator+(const TMatrix<T>&, const T&); 
template<typename T> TMatrix<T> operator+(const T&,          const TMatrix<T>&); 

template<typename T> TMatrix<T> operator-(const TMatrix<T>&); 
template<typename T> TMatrix<T> operator-(const TMatrix<T>&, const TMatrix<T>&); 
template<typename T> TMatrix<T> operator-(const TMatrix<T>&, const T&); 
template<typename T> TMatrix<T> operator-(const T&,          const TMatrix<T>&); 

template<typename T> TMatrix<T> operator*(const TMatrix<T>&, const TMatrix<T>&); 
template<typename T> TMatrix<T> operator*(const TMatrix<T>&, const T&);
template<typename T> TMatrix<T> operator*(const T&,          const TMatrix<T>&);

TMatrix<std::complex<double> > operator*(const TMatrix<std::complex<double> >& x, const TMatrix<double>& y);
TMatrix<std::complex<double> > operator*(const TMatrix<double>& y,                const TMatrix<std::complex<double> >& x);

template<typename T> TMatrix<T> operator/(const TMatrix<T>&,     const T&);
template<typename T> TMatrix<T> operator/(const T&,              TMatrix<T> const&);
template<typename T> TMatrix<T> operator/(TMatrix<T> const&,     TMatrix<T> const&);

template<typename T> bool operator==( const TMatrix<T>&, const TMatrix<T>& );
template<typename T> bool operator==( const TMatrix<T>&, const T& );
template<typename T> bool operator==( const T&,          const TMatrix<T>& );
template<typename T> void operator-=( TMatrix<T>&,       const TMatrix<T>& );
template<typename T> void operator*=( TMatrix<T>&,       const TMatrix<T>& );
template<typename T> bool operator!=( const TMatrix<T>&, const TMatrix<T>& );
template<typename T> bool operator!=( const TMatrix<T>&, const T& );
template<typename T> bool operator!=( const T&,          const TMatrix<T>& );

template<typename T>

class TMatrix {

  template<typename U>
  friend class TMatrix;

protected:

  MLPtr<T> _ml;

  // Functions used by the eigenroutines.

 private:

  void       _copy_column(TMatrix<T>& x, int, int );
  void       _switch_rows( int, int );
  TMatrix    _scale();
  TMatrix    _lu_decompose( int*, int& ) const;
  void       _lu_back_subst( int*, TMatrix<T>& );

public:

  // Constructors and destructors_____________________________________

  TMatrix();
  TMatrix(int);
  TMatrix(int rows, int columns);
  TMatrix(int rows, int columns, T initval);
  TMatrix(int rows, int columns, T* initval);
  TMatrix(const char* flag, int dimension); // create an identity matrix
                                            // or a symplectic matrix

  template<typename U>
  TMatrix(TMatrix<U> const& );

  TMatrix(TMatrix const& );

  TMatrix(MLPtr<T> const& ml);

 ~TMatrix();

  // Public member functions__________________________________________


  void      switch_columns( int, int ); // used by SurveyMatcher

  inline int rows() const { return _ml->rows();}
  inline int cols() const { return _ml->cols();}

  TMatrix                        transpose()    const; 
  TMatrix                        dagger()       const;
  TMatrix                        Square()       const;
  T                              determinant()  const;
  TMatrix                        inverse()      const;
  TMatrix<std::complex<double> > eigenValues()  const;
  TMatrix<std::complex<double> > eigenVectors() const;
  T                              trace()        const ; 
  void                           SVD( TMatrix&, TMatrix&, TMatrix& );
  bool                           isOrthogonal() const;

  // Operators________________________________________________________
  
  
  TMatrix& DeepCopy(const TMatrix&);

  TMatrix& operator=(const TMatrix&);

  T& operator()(int row, int column);
  T  operator()(int row, int column) const;
  T  getElement(int row, int column) const;
  T& operator()(int row);
  T  operator()(int row) const;
  TMatrix<T>& operator+=( const T&);
  TMatrix<T>& operator-=( const T&);

  // Friends

  friend TMatrix<double> real( const TMatrix<std::complex<double> >& x );
  friend TMatrix<double> imag( const TMatrix<std::complex<double> >& x );

  friend bool operator==<>( const TMatrix&, const TMatrix& );
  friend bool operator==<>( const TMatrix&, const T& );
  friend bool operator==<>( const T&,       const TMatrix& );

  friend std::ostream& operator<< <T>(std::ostream&, const TMatrix<T>&);

  friend TMatrix operator+<>(const TMatrix&, const TMatrix&);
  friend TMatrix operator+<>(const TMatrix&, const T&); 
  friend TMatrix operator+<>(const T&,       const TMatrix&); 
  friend TMatrix operator-<>(const TMatrix&); 
  friend TMatrix operator-<>(const TMatrix&, const TMatrix&); 
  friend TMatrix operator-<>(const TMatrix&, const T&); 
  friend TMatrix operator-<>(const T&,       const TMatrix&); 
  friend TMatrix operator*<>(const TMatrix&, const TMatrix&); 

  friend TMatrix<std::complex<double> > operator*(const TMatrix<double>&,                const TMatrix<std::complex<double> >&); 
  friend TMatrix<std::complex<double> > operator*(const TMatrix<std::complex<double> >&, const TMatrix<double>&); 

  friend TMatrix operator*<>(const TMatrix&,   const T&);
  friend TMatrix operator*<>(const T&,         const TMatrix&);
  friend TMatrix operator/<>(const TMatrix&,   const T&);
  friend TMatrix operator/<>(const T&,         TMatrix const&);
  friend TMatrix operator/<>(TMatrix const&,   TMatrix const&);

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

template<> void  TMatrix<double>::SVD( TMatrix<double>&, TMatrix<double>&, TMatrix<double>& );


// --------------------------------------------------------------------------------------------
// Special class RandomOrthogonal
// --------------------------------------------------------------------------------------------


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



#ifdef BASICTOOLKIT_IMPLICIT_TEMPLATES
#include <TMatrix.tcc>
#endif


#endif // TMATRIX_H


