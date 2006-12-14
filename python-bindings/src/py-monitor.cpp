/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-monitor.cpp
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
#include <beamline/monitor.h>

using namespace boost::python;

 struct monitorWrap: monitor {
  monitorWrap(PyObject* self):
       _self(self) {}

  monitorWrap(PyObject* self, const char* name):
       _self(self),monitor(name) {}

  monitorWrap(PyObject* self, const char* name, double length):
       _self(self),monitor(name,length) {}

    bool on();  
    bool off();
    const char* version();
   
    PyObject* _self;
     
 };

bool monitorWrap::on(){ 
return call_method<bool>(_self, "on"); }

bool monitorWrap::off(){ 
return call_method<bool>(_self, "off"); }

const char* monitorWrap::version(){ 
return call_method<const char*>(_self, "version"); }

void wrap_monitor () {


class_<monitor, bases<bmlnElmnt>, monitorWrap, boost::noncopyable >("monitor", init<>() )
  .def(init<const char*>() )
  .def(init<const char*, double>() )
  .def("on",               &monitorWrap::on)  
  .def("off",              &monitorWrap::off) 
  .def("version",          &monitorWrap::version)
  .def("setDriftFraction", &monitor::setDriftFraction);
 

          // operator[]( int );  // Readout of data

class_<hmonitor, bases<monitor> >("hmonitor", init<>() )
  .def( init<const char*>() )
  .def( init<const char*, double const&>() );


class_<vmonitor, bases<monitor> >("vmonitor", init<>() )
  .def(init<const char*>() )
  .def(init<const char*, double const&>() );

 
}

