/********************************************************************************
*********************************************************************************
*********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-monitor.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******  Copyright (c) Fermi Research Alliance/ Fermilab    
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
*********************************************************************************
*********************************************************************************
*********************************************************************************/

#include <boost/python.hpp>
#include <beamline/Monitor.h>

using namespace boost::python;

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_monitor () {

class_<Monitor,  bases<bmlnElmnt>, MonitorPtr >("Monitor", init<>() )
.def( init<std::string const&>() )
.def( init<std::string const&, double>() )
.def("isEnabled",         &Monitor::isEnabled ) 
.def("enable",            &Monitor::enable )
.def("setDriftFraction",  &Monitor::setDriftFraction)
.def("getDriftFraction",  &Monitor::getDriftFraction, return_value_policy<copy_const_reference>() )
.def("hposition",         &Monitor::hposition )
.def("vposition",         &Monitor::vposition )
.def("setHposition",      &Monitor::setHposition )
.def("setVposition",      &Monitor::setVposition )
.def("npx",               &Monitor::npx )
.def("npy",               &Monitor::npy )
.def("setNpx",            &Monitor::setNpx )
.def("setNpy",            &Monitor::setNpy );

class_<HMonitor, bases<bmlnElmnt>, HMonitorPtr >("HMonitor", init<>() )
.def( init<std::string const&>() )
.def( init<std::string const&, double const&>() )
.def("isEnabled",         &HMonitor::isEnabled )
.def("enable",            &HMonitor::enable)
.def("setDriftFraction",  &HMonitor::setDriftFraction)
.def("getDriftFraction",  &HMonitor::getDriftFraction, return_value_policy<copy_const_reference>() )
.def("hposition",         &HMonitor::hposition)
.def("npx",               &HMonitor::npx    )
.def("setNpx",            &HMonitor::setNpx );


class_<VMonitor,  bases<bmlnElmnt>, VMonitorPtr >("VMonitor", init<>() )
.def( init<std::string const&>() )
.def( init<std::string const&, double const&>() )
.def("isEnabled",         &VMonitor::isEnabled )
.def("enable",            &VMonitor::enable)
.def("setDriftFraction",  &VMonitor::setDriftFraction)
.def("getDriftFraction",  &VMonitor::getDriftFraction, return_value_policy<copy_const_reference>() )
.def("vposition",         &VMonitor::vposition )
.def("npy",               &VMonitor::npy    )
.def("setNpy",            &VMonitor::setNpy );

}
