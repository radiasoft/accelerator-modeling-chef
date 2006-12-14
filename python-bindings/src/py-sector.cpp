/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-sector.cpp
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

#include <beamline/sector.h>

void wrap_sector () {
  

using namespace boost::python;


 class_<sector, bases<bmlnElmnt> >("sector") 
   .def( init<char const*>() )
   .def( init<char const*,     double const&>() )
   .def( init<             Mapping const&,  double const&>() )
   .def( init<char const*, Mapping const&,  double const&>() ) 
   .def("Type",         &sector::Type);
   //.def("getMap",    sector::getMap)
 
}

