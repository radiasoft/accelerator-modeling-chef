/********************************************************************************
*********************************************************************************
*********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-sextupole.cpp
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
********************************************************************************
********************************************************************************
********************************************************************************/

#include <boost/python.hpp>
#include <beamline/sextupole.h>


//--------------------------------------------------------------------------------
// wrapper code
//--------------------------------------------------------------------------------

void wrap_sextupole () {
  

using namespace boost::python;


class_<sextupole, bases<bmlnElmnt>, SextupolePtr >("sextupole", init<double,double const&>() )
  .def(init<char*, double const&, double const&>() )
  .def("setCurrent",         &sextupole::setCurrent)
  .def("Type",               &sextupole::Type);

class_<thinSextupole, bases<bmlnElmnt>, ThinSextupolePtr >("thinSextupole", init<double const&>() )
  .def( init<char const*, double const&>() )
  .def("Type",  &thinSextupole::Type);
 
}


