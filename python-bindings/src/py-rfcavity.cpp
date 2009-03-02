/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-rfcavity.cpp
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
#include <beamline/rfcavity.h>

using namespace boost::python;

void wrap_rfcavity () {
  
class_<rfcavity, bases<bmlnElmnt>, RFCavityPtr >("rfcavity")
  .def( init<const char*>() )
  .def( init<char*, double,double,double,double,double,double>() )    
  .def("frequency",          &rfcavity::frequency,       return_value_policy<copy_const_reference>())
  .def("harmon",             &rfcavity::harmon,          return_value_policy<copy_const_reference>())
  .def("phi",                &rfcavity::phi,             return_value_policy<copy_const_reference>())
  .def("Q",                  &rfcavity::Q,               return_value_policy<copy_const_reference>())
  .def("R",                  &rfcavity::R,               return_value_policy<copy_const_reference>())
  .def("setHarmon",          &rfcavity::setHarmon)
  .def("setFrequency",       &rfcavity::setFrequency) 
  .def("setPhi",             &rfcavity::setPhi)
  .def("Q",                  &rfcavity::setQ)
  .def("R",                  &rfcavity::setR);


class_<thinrfcavity, bases<bmlnElmnt>, ThinRFCavityPtr >("thinrfcavity", init<char *>() )
  .def(init< char*,    double,   double, double, double, double> () )    
  .def("frequency",          &thinrfcavity::frequency,       return_value_policy<copy_const_reference>())
  .def("harmon",             &thinrfcavity::harmon,          return_value_policy<copy_const_reference>())
  .def("phi",                &thinrfcavity::phi,             return_value_policy<copy_const_reference>())
  .def("Q",                  &thinrfcavity::Q,               return_value_policy<copy_const_reference>())
  .def("R",                  &thinrfcavity::R,               return_value_policy<copy_const_reference>())
  .def("setHarmon",          &thinrfcavity::setHarmon)
  .def("setFrequency",       &thinrfcavity::setFrequency) 
  .def("setPhi",             &thinrfcavity::setPhi)
  .def("Q",                  &thinrfcavity::setQ)
  .def("R",                  &thinrfcavity::setR);
}


