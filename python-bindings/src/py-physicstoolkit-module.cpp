/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-physicstoolkit-module.cpp
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

#include <beamline/beamline.h>

extern beamline* DriftsToSlots( beamline& original );

using namespace boost::python;

extern void wrap_lattfuncsage();
extern void wrap_beamlinecontext();

BOOST_PYTHON_MODULE( physics_toolkit )
{

  def("DriftsToSlots",DriftsToSlots, 
      return_value_policy<reference_existing_object>());
wrap_lattfuncsage();
wrap_beamlinecontext();

}

