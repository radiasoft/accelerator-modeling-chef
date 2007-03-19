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
  .def("getPhi",       &rfcavity::getPhi)
  .def("setPhi",       &rfcavity::setPhi)
  .def("getFrequency", &rfcavity::getFrequency )
  .def("getQ",         &rfcavity::getQ)
  .def("getR",         &rfcavity::getR)
  .def("Type",         &rfcavity::Type);


class_<thinrfcavity, bases<bmlnElmnt>, ThinRFCavityPtr >("thinrfcavity", init<char *>() )
  .def(init< double,   double, double, double, double> () )    
  .def(init< char*,  double,   double, double, double, double> () )    
  .def("getPhi",       &thinrfcavity::getPhi)
  .def("setPhi",       &thinrfcavity::setPhi)
  .def("getFrequency", &thinrfcavity::getFrequency )
  .def("getQ",         &thinrfcavity::getQ)
  .def("getR",         &thinrfcavity::getR)
  .def("Type",         &thinrfcavity::Type);

}


