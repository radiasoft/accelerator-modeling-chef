/********************************************************************************
*********************************************************************************
*********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-monitor.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******  Copyright (c) Fermi Research Alliance/ Fermilab    
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
*********************************************************************************
*********************************************************************************
*********************************************************************************/

#include <boost/python.hpp>
#include <beamline/monitor.h>

using namespace boost::python;

//------------------------------------------------------------------------------
// local code and definition
//------------------------------------------------------------------------------

namespace {

struct monitorWrap: monitor {
  monitorWrap(PyObject* self)
     : self_(self) {}

  monitorWrap(PyObject* self, monitor const& elm)
     : monitor(elm),self_(self)  {}

  monitorWrap(PyObject* self, const char* name)
     : self_(self), monitor(name) {}

  monitorWrap(PyObject* self, const char* name, double length):
       self_(self),monitor(name,length) {}

    bool on();  
    bool off();
    const char* version();
   
    PyObject* self_;
     
};


bool monitorWrap::on(){ 
return call_method<bool>(self_, "on"); }

bool monitorWrap::off(){ 
return call_method<bool>(self_, "off"); }

const char* monitorWrap::version(){ 
return call_method<const char*>(self_, "version"); }

}

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_monitor () {

class_<monitor, bases<bmlnElmnt>, boost::shared_ptr<monitorWrap> >("monitor", init<>() )
  .def(init<const char*>() )
  .def(init<const char*, double>() )
  .def("on",               &monitorWrap::on)  
  .def("off",              &monitorWrap::off) 
  .def("version",          &monitorWrap::version)
  .def("setDriftFraction", &monitor::setDriftFraction);
 
 class_<hmonitor, bases<monitor>, HMonitorPtr >("hmonitor", init<>() )
    .def( init<const char*>() )
    .def( init<const char*, double const&>() );


class_<vmonitor, bases<monitor>, VMonitorPtr >("vmonitor", init<>() )
  .def(init<const char*>() )
  .def(init<const char*, double const&>() );
 

}
