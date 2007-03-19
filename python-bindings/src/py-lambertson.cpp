/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-lambertson.cpp
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
#include <beamline/lambertson.h>
#include <beamline/beamline.h>

using namespace boost::python;


//--------------------------------------------------------------------------------
// wrapper code
//--------------------------------------------------------------------------------

void wrap_lambertson () {
  
class_<thinLamb,bases<bmlnElmnt>, ThinLambPtr >("thinLamb", init<>() )
  .def(init<char const*>() )
  .def(init<char const*, double const&, BmlPtr&, double*>() )
  .def(init<double const&, BmlPtr&, double*>() )
  .def("setSeptum",   &thinLamb::setSeptum ) 
  .def("setBeamline", &thinLamb::setBeamline ) 
  .def("setRefState", &thinLamb::setRefState )
  .def("getRefState", &thinLamb::getRefState );
}

