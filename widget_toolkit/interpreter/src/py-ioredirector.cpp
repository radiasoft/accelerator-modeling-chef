/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application 
******             Layered on top of of BEAMLINE.
******                                                                
******  File:      py-ioredirector.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab                                           
******             Batavia, IL  60510                                
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

#include <ioredirector.h>

#include "qtmacrosoff.h"
#include <boost/python.hpp>
#include "qtmacroson.h"

using namespace boost::python;

// defining the symbol below would prevent the initioredirector function to be exported
// from the module, on platforms that supports this feature.

// #define BOOST_PYTHON_STATIC_MODULE


BOOST_PYTHON_MODULE( ioredirector )
{

 class_<QObject, boost::noncopyable> ("QObject", no_init);
  
 class_<IORedirector, bases<QObject>, boost::noncopyable>("IORedirector",init<>())
   .def("read",  &IORedirector::read)
   .def("write", &IORedirector::write);
 

}
