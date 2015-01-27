/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******                                    
******  File:      py-nonlinearlens.cpp
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
******  Author:    Chong Shik Park
******             cspark@fnal.gov                                     
******             Fermi National Laboratory, Batavia, IL   60510                                
******
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <beamline/nonLinearLens.h>

using namespace boost::python;

void wrap_nonlinearlens () {

  class_<nonLinearLens, bases<bmlnElmnt>, NonLinearLensPtr >("nonLinearLens", init<char*,  double const&, double const& >() )
    .def(init<char const*, double const&, double const&, double const&, double const& >() )
    .def("set_knll",   &nonLinearLens::set_knll)
    .def("set_cnll",   &nonLinearLens::set_cnll)
    .def("set_bcoeff", &nonLinearLens::set_bcoeff)
    .def("set_dcoeff", &nonLinearLens::set_dcoeff);
}
