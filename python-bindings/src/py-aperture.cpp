/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-aperture.cpp
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
******  ----------------
******  REVISION HISTORY
******  ----------------
******  April, 2010            michelotti@fnal.gov
******  - rewritten to conform to updated beamline/include/Aperture.h.
******  - removed "#ifdef 0" block that voided compilation of file.
******
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>

#include <beamline/Particle.h>
#include <beamline/Aperture.h>

using namespace boost::python;

//----------------------------------------
// wrapper code
//----------------------------------------


void wrap_aperture () 
{
class_<ThinStarAperture>("ThinStarAperture", init<char const*>() )
  .def(init<char const*, std::vector<std::pair<double, double> > >())
  .def("Type", &ThinStarAperture::Type)
  .def("contains", &ThinStarAperture::contains);

class_<ThinRectangularAperture>("ThinRectangularAperture", init<char const*>() )
  .def(init<char const*, double const&, double const&, double const&, double const&, double const&>())
  .def("Type", &ThinRectangularAperture::Type)
  .def("contains", &ThinRectangularAperture::contains);

class_<ThinEllipticalAperture>("ThinEllipticalAperture", init<char const*>() )
  .def(init<char const*, double const&, double const&, double const&, double const&, double const&>())
  .def("Type", &ThinEllipticalAperture::Type)
  .def("contains", &ThinEllipticalAperture::contains);
}
