/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-bmlfactory-module.cpp
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

#include <bmlfactory/MAD8Factory.h>
#include <beamline/beamline.h>
#include <string>


using namespace boost::python;

// #define BOOST_PYTHON_STATIC_MODULE


BmlPtr ( MAD8Factory::* create_beamline_1 )(std::string        )        = &MAD8Factory::create_beamline;
BmlPtr ( MAD8Factory::* create_beamline_2 )(std::string, double)        = &MAD8Factory::create_beamline;

BOOST_PYTHON_MODULE(bmlfactory)
{

class_<MAD8Factory>("bmlfactory", init<std::string, const char*>() )
  .def( init<std::string, double, const char*>() ) 
  .def( init<std::string>() ) 
  .def( init<std::string, double>() ) 
  .def("create_beamline",  create_beamline_1 )
  .def("create_beamline",  create_beamline_2 );

  //  .def("create_beamline",  create_beamline_1, return_value_policy<manage_new_object>() )
  //.def("create_beamline",  create_beamline_2, return_value_policy<manage_new_object>() );
}


