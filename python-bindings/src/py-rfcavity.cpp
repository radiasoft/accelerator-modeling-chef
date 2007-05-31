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
  .def( init<double,double,double,double,double,double>() )    
  .def( init<char*, double,double,double,double,double,double>() )    
  .def("getPhi",             &rfcavity::getPhi,             return_value_policy<copy_const_reference>())
  .def("setPhi",             &rfcavity::setPhi)
  .def("getRadialFrequency", &rfcavity::getRadialFrequency, return_value_policy<copy_const_reference>())
  .def("getQ",               &rfcavity::getQ,               return_value_policy<copy_const_reference>())
  .def("getR",               &rfcavity::getR,               return_value_policy<copy_const_reference>())
  .def("Type",               &rfcavity::Type);


class_<thinrfcavity, bases<bmlnElmnt>, ThinRFCavityPtr >("thinrfcavity", init<char *>() )
  .def(init< double,   double, double, double, double> () )    
  .def(init< char*,    double,   double, double, double, double> () )    
  .def("getPhi",             &thinrfcavity::getPhi,             return_value_policy<copy_const_reference>())
  .def("setPhi",             &thinrfcavity::setPhi)
  .def("getRadialFrequency", &thinrfcavity::getRadialFrequency, return_value_policy<copy_const_reference>())
  .def("getQ",               &thinrfcavity::getQ,               return_value_policy<copy_const_reference>())
  .def("getR",               &thinrfcavity::getR,               return_value_policy<copy_const_reference>())
  .def("Type",               &thinrfcavity::Type);

}


