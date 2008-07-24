/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
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
******
*****************************************************************************
*****************************************************************************
*****************************************************************************/

#include <beamline/kick.h>
#include <boost/python.hpp>

using namespace boost::python;

//------------------------------------------------------------------------------
// wrappper code
//------------------------------------------------------------------------------

void wrap_kick () {


class_<hkick, bases<bmlnElmnt>, HKickPtr >("hkick")
  .def(init<std::string const&>())
  .def(init<std::string const&,double const&>())
  .def(init<std::string const&,double const&, double const&>());


class_<vkick, bases<bmlnElmnt>, VKickPtr >("vkick")
  .def(init<std::string const&>())
  .def(init<std::string const&,double const&>())
  .def(init<std::string const&,double const&, double const&>());


class_<kick,bases<bmlnElmnt>, KickPtr >("kick")
  .def(init<std::string const&>())
  .def(init<std::string const&,double const&, double const&, double const&>())
  .def(init<std::string const&,double const&, double const&, double const&>());

}
