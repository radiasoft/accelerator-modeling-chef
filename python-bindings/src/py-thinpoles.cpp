/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-thinpoles.cpp
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
#include <beamline/thinpoles.h>

using namespace boost::python;

void wrap_thinpoles () {


class_<thin12pole, bases<bmlnElmnt> >("thin12pole", init<double const&>() )
  .def( init<char const*, double const&>() )
  .def("Type",      &thin12pole::Type);


class_<thin14pole, bases<bmlnElmnt> >("thin14pole", init<double const&>() )
  .def( init<char const*, double const&>() )
  .def("Type",      &thin14pole::Type);


class_<thin16pole, bases<bmlnElmnt> >("thin16pole", init<double const&>() )
  .def( init<char const*, double const&>() )
  .def("Type",      &thin16pole::Type);


class_<thin18pole, bases<bmlnElmnt> >("thin18pole", init<double const&>() )
  .def( init<char const*, double const&>() )
  .def("Type",      &thin18pole::Type);


class_<thinMultipole, bases<bmlnElmnt> >("thinMultipole", init<double const&>() )
  .def( init<char* const, double const&>() )
  .def("Type",      &thinMultipole::Type);
}


