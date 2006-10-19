/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-mapping.cpp
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
****************************************************************************/
#include <boost/python.hpp>
#include <mxyzptlk/JetVector.h>
#include <mxyzptlk/Mapping.h>
#include <mxyzptlk/EnvPtr.h>

using namespace boost::python;

//------------------------------------------------------------------------------
// Local auxiliary functions and wrapper classe(s)
//------------------------------------------------------------------------------


template<typename T>
class MappingWrapper: public TMapping<T> {
 
 private:
  
   PyObject* m_self;

 public:

  MappingWrapper( PyObject* self): m_self(self), TMapping<T>() {}

  MappingWrapper( PyObject* self, TMapping<T> const& map): m_self(self), TMapping<T>(map) {}

  MappingWrapper( PyObject* self, boost::python::tuple components): m_self(self) {
 
    int dim = extract<int>( components.attr("__len__")() );
    TJet<T>* comps = new TJet<T> [dim];
    for (int i=0; i<dim; ++i) {
      comps[i] = extract<TJet<T> >(components[i]); 
    }
    TMapping<T>* mapping = this; 
    new(mapping) TMapping<T>(dim, comps); // placement new   
    delete[] comps;
  }

  MappingWrapper(  PyObject* self, boost::python::tuple components, EnvPtr<T> const& env ): m_self(self) {

      int dim = extract<int>( components.attr("__len__")() );
      TJet<T>* comps = new TJet<T> [dim];
      for (int i=0; i<dim; ++i) {
        comps[i] = extract<TJet<T> >(components[i]); 
      }
      TMapping<T>* mapping = this; 
      new(mapping) TMapping<T>(dim, comps); // placement new   
      delete[] comps;
  }

  tuple toPython() {
    boost::python::list terms[6];
    for (int i = 0; i<6; ++i) {
      Jet::iterator jet_it(this->operator()(i));
      while(++jet_it) {
        tuple term = make_tuple(jet_it->coefficient(),
                                make_tuple(jet_it->exponents()(0),
                                           jet_it->exponents()(1),
                                           jet_it->exponents()(2),
                                           jet_it->exponents()(3),
                                           jet_it->exponents()(4),
                                           jet_it->exponents()(5)));
        terms[i].append(term);
      }
    }
    return make_tuple(terms[0],terms[1],terms[2],terms[3],terms[4],terms[5]);
  }

};




//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


//------------------------------------------------------------------------------
//  Python wrapper classes
//------------------------------------------------------------------------------



void wrap_mxyzptlk_mapping() {


  class_<Mapping, bases<JetVector>, MappingWrapper<double> > Mapping_class_("Mapping", init<>() );


  Mapping_class_.def( init<boost::python::tuple>() );
  Mapping_class_.def( init<boost::python::tuple, EnvPtr<double> >() );
  Mapping_class_.def("inverse", &MappingWrapper<double>::inverse  );
  Mapping_class_.def("jacobian",&MappingWrapper<double>::jacobian );
  Mapping_class_.def("toPython",&MappingWrapper<double>::toPython );

  //-----------------------------------
  // *** composition ***
  //-----------------------------------

  Mapping_class_.def(self * self);
 
}

void wrap_mxyzptlk_mappingc() {

  class_<MappingC, bases<JetCVector>,  MappingWrapper<std::complex<double> > >MappingC_class_("MappingC", init<>() );

  MappingC_class_.def( init<boost::python::tuple>() );
  MappingC_class_.def( init<boost::python::tuple, EnvPtr<std::complex<double> > >() );

  MappingC_class_.def("inverse",  &MappingWrapper<std::complex<double> >::inverse  );
  MappingC_class_.def("jacobian", &MappingWrapper<std::complex<double> >::jacobian );


  //-----------------------------------
  // *** composition ***
  //-----------------------------------
  MappingC_class_.def(self * self);
 
}
