/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-vector.cpp
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
#include <basic_toolkit/VectorD.h>

using namespace boost::python;

namespace {

struct VectorWrapper: public Vector {

  PyObject* m_self;  

  VectorWrapper(PyObject* self, const Vector& vec) : m_self(self), Vector(vec)  {}

  VectorWrapper(PyObject* self, int dimension ): m_self(self), Vector(dimension) { }  

  VectorWrapper(PyObject* self,  boost::python::tuple components ) : m_self(self) { 
    int dim = extract<int>( components.attr("__len__")() );
    double* comps = new double[dim];
    for (int i=0; i<dim; ++i) {
      comps[i] = extract<double>( components[i]);
    } 
    Vector* v = this;
    new(v) Vector(dim, comps); 
    // NOTE: comps is owned and destroyed by Vector
  }
   
  operator Vector () const {
    int dim = Dim();
      double* v = new double[ dim ];
      for ( int i=0; i< dim; ++dim ) {
	v[i] = (*this)(i);
      }
      return *( new Vector(dim,v) ); 
  }            

  void Set( boost::python::tuple components ) {
    
    int dim = extract<int>( components.attr("__len__")() );

    double* comps = new double[dim];
    for (int i=0; i<dim; ++i) {
      comps[i] = extract<double>( components[i] );
    } 
    Vector::Set(comps);
    delete [] comps; 
  }
};

} // anonymous namespace


//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_vector () {

 class_<Vector,VectorWrapper> Vector_class_("Vector", init<int>() );

 Vector_class_.def( init<boost::python::tuple>() );
 Vector_class_.def("Dim",       &VectorWrapper::Dim);
 Vector_class_.def("Set",       &VectorWrapper::Set);     // void        Set              ( const double* );
 Vector_class_.def("IsNull",    &VectorWrapper::IsNull);  // char        IsNull           () const;
 Vector_class_.def("IsUnit",    &VectorWrapper::IsUnit);  // char        IsUnit           () const;
 Vector_class_.def("Abs",       &VectorWrapper::Abs);     // Vector      Abs              () const;
 Vector_class_.def("Norm",      &VectorWrapper::Norm);    // double      Norm             () const;
 Vector_class_.def("Unit",      &VectorWrapper::Unit);    // Vector      Unit             () const;           // returns unit vector
 Vector_class_.def("Rotate",    &VectorWrapper::Rotate);  // void        Rotate           ( Vector& v,       double  theta ) const;


//-----------------------------------
// *** addition ***
//-----------------------------------
Vector_class_.def(self + self);
//-----------------------------------
// *** subtraction ***
//-----------------------------------
Vector_class_.def(self - self);
//-----------------------------------
// *** multiplication ***
//-----------------------------------
Vector_class_.def(self * self);
Vector_class_.def(self * double() );
Vector_class_.def(double() * self );
//-----------------------------------
// *** division ****
//-----------------------------------
Vector_class_.def( self / double() );
//-----------------------------------
// *** string representation ****
//-----------------------------------
Vector_class_.def( self_ns::str(self) );         

}

