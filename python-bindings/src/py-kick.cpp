/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-kick.cpp
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
#include <beamline/kick.h>
#include <boost/python.hpp>

using namespace boost::python;

void wrap_kick () {

class_<hkick, bases<bmlnElmnt> >("hkick")
  .def(init<double const&>())
  .def(init<char const*>())
  .def(init<char const*,double const&>())
  .def(init<char const*,double const&, double const&>())
  .def( "Type", &hkick::Type);


class_<vkick, bases<bmlnElmnt> >("vkick")
  .def(init<double const&>())
  .def(init<char const*>())
  .def(init<char const*,double const&>())
  .def(init<char const*,double const&, double const&>())
  .def( "Type", &vkick::Type);


class_<kick,bases<bmlnElmnt> >("kick")
  .def(init<double const&,double const&>())
  .def(init<char const*>())
  .def(init<char const*,double const&, double const&, double const&>())
  .def(init<char const*,double const&, double const&, double const&>())
  .def( "Type", &kick::Type);
}


