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

// wrappers for overloaded definitions
void (rfcavity::*rfcavity_setHarmonicNumber_int)(int n) = &rfcavity::setHarmonicNumber;
void (rfcavity::*rfcavity_setHarmonicNumber_double)(double const &x) = &rfcavity::setHarmonicNumber;

void (thinrfcavity::*thinrfcavity_setHarmonicNumber_int)(int n) = &thinrfcavity::setHarmonicNumber;
void (thinrfcavity::*thinrfcavity_setHarmonicNumber_double)(double const &) = &thinrfcavity::setHarmonicNumber;
void wrap_rfcavity () {
  
class_<rfcavity, bases<bmlnElmnt>, RFCavityPtr >("rfcavity")
  .def( init<const char*>() )
  .def( init<char*, double,double,double,double,double,double>() )    
  .def("getHarmonicNumber", &rfcavity::getHarmonicNumber, return_value_policy<copy_const_reference>())
  .def("getPhi",             &rfcavity::getPhi,             return_value_policy<copy_const_reference>())
  .def("setHarmonicNumber", rfcavity_setHarmonicNumber_int)
  .def("setHarmonicNumber", rfcavity_setHarmonicNumber_double)
  .def("setPhi",             &rfcavity::setPhi)
  .def("setRadialFrequency",             &rfcavity::setRadialFrequency)
    .def("setDisplacedFrequency", &rfcavity::setDisplacedFrequency)
  .def("setFrequency",             &rfcavity::setFrequency)
  .def("getRadialFrequency", &rfcavity::getRadialFrequency, return_value_policy<copy_const_reference>())
  .def("getQ",               &rfcavity::getQ,               return_value_policy<copy_const_reference>())
  .def("getR",               &rfcavity::getR,               return_value_policy<copy_const_reference>())
  .def("addHarmonic", &rfcavity::adddHarmonic)
  .def("turnUpdate", &rfcavity::turnUpdate)
  .def("setStrength",             &rfcavity::setStrength)
  .def("Type",               &rfcavity::Type);


class_<thinrfcavity, bases<bmlnElmnt>, ThinRFCavityPtr >("thinrfcavity", init<char *>() )
  .def(init< char*,    double,   double, double, double, double> () )    
  .def("getHarmonicNumber", &thinrfcavity::getHarmonicNumber, return_value_policy<copy_const_reference>())
  .def("getPhi",             &thinrfcavity::getPhi,             return_value_policy<copy_const_reference>())
  .def("setHarmonicNumber", thinrfcavity_setHarmonicNumber_int)
  .def("setHarmonicNumber", thinrfcavity_setHarmonicNumber_double)
  .def("setPhi",             &thinrfcavity::setPhi)
  .def("setRadialFrequency",             &thinrfcavity::setRadialFrequency)
    .def("setDisplacedFrequency", &thinrfcavity::setDisplacedFrequency)
  .def("setFrequency",             &thinrfcavity::setFrequency)
  .def("getRadialFrequency", &thinrfcavity::getRadialFrequency, return_value_policy<copy_const_reference>())
  .def("getQ",               &thinrfcavity::getQ,               return_value_policy<copy_const_reference>())
  .def("getR",               &thinrfcavity::getR,               return_value_policy<copy_const_reference>())
  .def("addHarmonic", &thinrfcavity::addHarmonic)
  .def("turnUpdate", &thinrfcavity::turnUpdate)
  .def("Type",               &thinrfcavity::Type);

}


