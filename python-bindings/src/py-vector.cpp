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
#include <boost/python/detail/api_placeholder.hpp> // this is needed with boost < 1.34.X

using namespace boost::python;

namespace {

  Vector* constructor_wrapper( boost::python::tuple const& components ) { 

    std::vector<double> comps( boost::python::len(components) );

    int i=0;
    for ( std::vector<double>::iterator it  = comps.begin(); 
	                                it != comps.end(); ++it, ++i ) {
       (*it) = extract<double>( components[i]);
    } 

    return new Vector(comps.begin(),comps.end() ); 
  }
   
  void Set( Vector& o, boost::python::tuple const&  components ) {
    
    int dim = boost::python::len(components); //  same effect as: extract<int>( components.attr("__len__")() );

    for (int i=0; i<dim; ++i) {
      o[i] = extract<double>( components[i] );
    } 
  }

} // anonymous namespace


//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_vector () {

 class_<Vector> Vector_class_("Vector", init<int>() );
 Vector_class_.def("__init__",  make_constructor( &::constructor_wrapper ) );
 Vector_class_.def("Dim",       &Vector::Dim);
 Vector_class_.def("Set",       &::Set);               
 Vector_class_.def("IsNull",    &Vector::IsNull);  // char        IsNull           () const;
 Vector_class_.def("IsUnit",    &Vector::IsUnit);  // char        IsUnit           () const;
 Vector_class_.def("Abs",       &Vector::Abs);     // Vector      Abs              () const;
 Vector_class_.def("Norm",      &Vector::Norm);    // double      Norm             () const;
 Vector_class_.def("Unit",      &Vector::Unit);    // Vector      Unit             () const;           // returns unit vector
 Vector_class_.def("Rotate",    &Vector::Rotate);  // void        Rotate           ( Vector& v,       double  theta ) const;


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

