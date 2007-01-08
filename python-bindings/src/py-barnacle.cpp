/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-barnacle.cpp
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
#include <string>
#include <basic_toolkit/Barnacle.h>
#include <physics_toolkit/LattFuncSage.h>

using namespace boost::python;

struct BarnacleListIteratorWrapper: public BarnacleList::iterator {

   BarnacleListIteratorWrapper( PyObject* self, BarnacleList::iterator const& it):  BarnacleList::iterator(it), self_(self) {} 

   BarnacleListIteratorWrapper( PyObject* self): self_(self) {} 

   std::string&         get_id()     { return (*this)->id;   }    
   boost::any&        get_info()     { return (*this)->info; }    

   PyObject* self_;
 
};

static LattFuncSage::lattFunc& lattFunc_local(PyObject* o) {
 
  return boost::any_cast<LattFuncSage::lattFunc&> ( extract<boost::any&>(o) ); 

}


void wrap_barnacle () {

class_<boost::any>("Any")
 .def( "lattFunc",  &lattFunc_local, return_value_policy<reference_existing_object>() );

class_<Barnacle>("Barnacle", init<std::string, boost::any >() )
  .def_readwrite("id",     &Barnacle::id)
  .def_readwrite("info",   &Barnacle::info);
 

class_<BarnacleList>("BarnacleList")
 .def("append",      &BarnacleList::append)
 .def("insert",      &BarnacleList::insert) 
 .def("eraseFirst",  &BarnacleList::eraseFirst) 
 .def("eraseAll",    &BarnacleList::eraseAll)
 .def("find",        &BarnacleList::find )
 .def("remove",      &BarnacleList::remove)
 .def("begin",       &BarnacleList::begin)
 .def("end",         &BarnacleList::end)
 .def("__iter__",    range( &BarnacleList::begin, &BarnacleList::end ) );


 class_<BarnacleList::iterator, BarnacleListIteratorWrapper >("BarnacleListIterator")
   .def("id",   &BarnacleListIteratorWrapper::get_id,   return_value_policy<reference_existing_object>()  )  
   .def("info", &BarnacleListIteratorWrapper::get_info, return_value_policy<reference_existing_object>()  ); 
}

