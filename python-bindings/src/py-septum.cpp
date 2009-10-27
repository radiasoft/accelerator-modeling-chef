/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                   
******  File:      py-septum.cpp
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
****************************************************************************
****************************************************************************
****************************************************************************/

#include <boost/python.hpp>
#include <beamline/septum.h>

void wrap_septum () {
  

using namespace boost::python;


class_<thinSeptum, bases<BmlnElmnt> >("thinSeptum", init<char const*>() )
  .def(init<char const*, double const&, double const&, double const&>() )
  .def(init<             double const&, double const&, double const&>() )
  .def("setStrengths",  &thinSeptum::setStrengths)
  .def("setWire",       &thinSeptum::setWire)
  .def("Type",          &thinSeptum::Type);


}


