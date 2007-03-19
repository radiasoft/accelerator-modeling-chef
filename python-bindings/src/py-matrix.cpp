/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-matrix.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <boost/shared_array.hpp>
#include <basic_toolkit/Matrix.h>
#include <basic_toolkit/VectorD.h>
#include <basic_toolkit/TMatrix.h>

using namespace boost::python;


namespace {

template <typename T>
class MatrixWrapper: public TMatrix<T> {

private:
  PyObject* m_self;  

public:

  MatrixWrapper(PyObject* self) : m_self(self), TMatrix<T>()  {}

  MatrixWrapper(PyObject* self, const char* flag, int dimension ): m_self(self), TMatrix<T>(flag, dimension) {  }// create an identity matrix or a symplectic matrix

  MatrixWrapper(PyObject* self, int rowcol) : m_self(self), TMatrix<T>(rowcol)  {}

  MatrixWrapper(PyObject* self, int rows, int cols ): m_self(self), TMatrix<T>(rows, cols) { }  

  MatrixWrapper(PyObject* self, int rows, int cols, T initval ): m_self(self), TMatrix<T>(rows, cols, initval) { }  

  //MatrixWrapper(PyObject* self, numarray::matrix  ): m_self(self) {
  // Matrix(row, cols, initval) { }  
  //
  //}

  MatrixWrapper(PyObject* self, TMatrix<T> const& matrix): m_self(self), TMatrix<T>(matrix) { }  

  T    get(int i, int j)           { return (*this)(i,j) ; }
  void set(int i, int j, T value ) { (*this)(i,j) = value; }

};

} // anonymous namespace


void wrap_matrix () {


 class_<TMatrix<double>, MatrixWrapper<double> > Matrix_class_("Matrix", init<>() );

 Matrix_class_.def( init<const char*, int>() );
 Matrix_class_.def( init<int>() );
 Matrix_class_.def( init<int,int>() );
 Matrix_class_.def( init<int,int,double>() );
 // Matrix_class_.def( init<int,int, numarray::matrix>() );

 Matrix_class_.def("rows",           &MatrixWrapper<double>::rows         );
 Matrix_class_.def("cols",           &MatrixWrapper<double>::cols         );
 Matrix_class_.def("transpose",      &MatrixWrapper<double>::transpose    ); 
 Matrix_class_.def("dagger",         &MatrixWrapper<double>::dagger       ); 
 Matrix_class_.def("Square",         &MatrixWrapper<double>::Square       ); 
 Matrix_class_.def("determinant",    &MatrixWrapper<double>::determinant  ); 
 Matrix_class_.def("inverse",        &MatrixWrapper<double>::inverse      ); 
 Matrix_class_.def("eigenValues",    &MatrixWrapper<double>::eigenValues  ); 
 Matrix_class_.def("eigenVectors",   &MatrixWrapper<double>::eigenVectors ); 
 Matrix_class_.def("trace",          &MatrixWrapper<double>::trace        ); 
 Matrix_class_.def("SVD",            &MatrixWrapper<double>::SVD          ); 
 Matrix_class_.def("isOrthogonal",   &MatrixWrapper<double>::isOrthogonal );
 Matrix_class_.def("get",            &MatrixWrapper<double>::get);
 Matrix_class_.def("set",            &MatrixWrapper<double>::set);

//-----------------------------------
// *** addition ***
//-----------------------------------
Matrix_class_.def(self + self);
Matrix_class_.def(self + double());
Matrix_class_.def(double() + self);
//Matrix_class_.def(self += self );
Matrix_class_.def(self += double( ));
//-----------------------------------
// *** subtraction ***
//-----------------------------------
Matrix_class_.def(self - self);
Matrix_class_.def(self - double());
Matrix_class_.def(double() - self);
//Matrix_class_.def(self -= self );
Matrix_class_.def(self -= double( ));
//-----------------------------------
// *** multiplication ***
//-----------------------------------
Matrix_class_.def(self * self       );
Matrix_class_.def(self * double()   );
Matrix_class_.def(double() * self   );
//Matrix_class_.def( self *= double() );
//Matrix_class_.def( self *= self     );
//-----------------------------------
// *** division ****
//-----------------------------------
Matrix_class_.def( self / self     );
Matrix_class_.def( self / double() );
Matrix_class_.def( double() /self  );
//----------------------------
// *** unary ****
//----------------------------
Matrix_class_.def( -self );  	
//-----------------------------------
// *** boolean ****
//-----------------------------------
 Matrix_class_.def( self     == self );
 Matrix_class_.def( self     != self );
 //Matrix_class_.def( double() == self );
 //Matrix_class_.def( self     == double() );
 //Matrix_class_.def( double() != self );
 //Matrix_class_.def( self     != double() );
//-----------------------------------
// *** string representation ****
//-----------------------------------
Matrix_class_.def( self_ns::str(self) );         

}


void wrap_matrixc () {
 
class_<MatrixC, MatrixWrapper<std::complex<double> > > Matrix_class_("MatrixC", init<>() );

 Matrix_class_.def( init<const char*, int>() );
 Matrix_class_.def( init<int>() );
 Matrix_class_.def( init<int,int>() );
 Matrix_class_.def( init<int,int,std::complex<double> >() );
 // Matrix_class_.def( init<int,int, numarray::matrix>() );

 Matrix_class_.def("rows",           &MatrixWrapper<std::complex<double> >::rows         );
 Matrix_class_.def("cols",           &MatrixWrapper<std::complex<double> >::cols         );
 Matrix_class_.def("transpose",      &MatrixWrapper<std::complex<double> >::transpose    ); 
 Matrix_class_.def("dagger",         &MatrixWrapper<std::complex<double> >::dagger       ); 
 Matrix_class_.def("Square",         &MatrixWrapper<std::complex<double> >::Square       ); 
 Matrix_class_.def("determinant",    &MatrixWrapper<std::complex<double> >::determinant  ); 
 Matrix_class_.def("inverse",        &MatrixWrapper<std::complex<double> >::inverse      ); 
 Matrix_class_.def("eigenValues",    &MatrixWrapper<std::complex<double> >::eigenValues  ); 
 Matrix_class_.def("eigenVectors",   &MatrixWrapper<std::complex<double> >::eigenVectors ); 
 Matrix_class_.def("trace",          &MatrixWrapper<std::complex<double> >::trace        ); 
 //Matrix_class_.def("SVD",            &MatrixWrapper<std::complex<double> >::SVD          ); 
 Matrix_class_.def("isOrthogonal",   &MatrixWrapper<std::complex<double> >::isOrthogonal );
 Matrix_class_.def("get",            &MatrixWrapper<std::complex<double> >::get);
 Matrix_class_.def("set",            &MatrixWrapper<std::complex<double> >::set);

//-----------------------------------
// *** addition ***
//-----------------------------------
Matrix_class_.def(self + self);
Matrix_class_.def(self + std::complex<double> ());
Matrix_class_.def(std::complex<double> () + self);
//Matrix_class_.def(self += self );
Matrix_class_.def(self += std::complex<double> ( ));
//-----------------------------------
// *** subtraction ***
//-----------------------------------
Matrix_class_.def(self - self);
Matrix_class_.def(self - std::complex<double> ());
Matrix_class_.def(std::complex<double> () - self);
//Matrix_class_.def(self -= self );
Matrix_class_.def(self -= std::complex<double> ( ));
//-----------------------------------
// *** multiplication ***
//-----------------------------------
Matrix_class_.def(self * self       );
Matrix_class_.def(self * std::complex<double> ()   );
Matrix_class_.def(std::complex<double> () * self   );

//Matrix_class_.def( Matrix * self   ); //  mixed mode
//Matrix_class_.def( self   * Matrix );  // mixed mode

//Matrix_class_.def( self *= std::complex<double> () );
//Matrix_class_.def( self *= self     );
//-----------------------------------
// *** division ****
//-----------------------------------
Matrix_class_.def( self / self     );
Matrix_class_.def( self / std::complex<double> () );
Matrix_class_.def( std::complex<double> () /self  );
//----------------------------
// *** unary ****
//----------------------------
Matrix_class_.def( -self );  	
//-----------------------------------
// *** boolean ****
//-----------------------------------
 Matrix_class_.def( self     == self );
 Matrix_class_.def( self     != self );
 //Matrix_class_.def( std::complex<double> () == self );
 //Matrix_class_.def( self     == std::complex<double> () );
 //Matrix_class_.def( std::complex<double> () != self );
 //Matrix_class_.def( self     != std::complex<double> () );
//-----------------------------------
// *** string representation ****
//-----------------------------------
Matrix_class_.def( self_ns::str(self) );         


}

