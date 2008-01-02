/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-sbend.cpp
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
*******************************************************************************/

#include <boost/python.hpp>
#include <beamline/sbend.h>
#include <beamline/Particle.h>
#include <iostream>

namespace {

double (sbend::* setEntryAngle_double)       (     double   const&  ) =  &sbend::setEntryAngle; 
double (sbend::* setEntryAngle_particle)     (     Particle const&  ) =  &sbend::setEntryAngle; 
double (sbend::*  setExitAngle_double)       (     double   const&  ) =  &sbend::setExitAngle; 
double (sbend::*  setExitAngle_particle)     (     Particle const&  ) =  &sbend::setExitAngle; 

} // anonymous namespace

void wrap_sbend () {
  
using namespace boost::python;

class_<sbend, bases<bmlnElmnt>, SBendPtr >("sbend", init<char const*,    double const&,  double const&,  double const&>() )
  .def(init<char const*,    double const&,  double const&,  double const&,  double const&,  double const&>() )
  .def("setBendAngle",      &sbend::setBendAngle            )
  .def("getBendAngle",      &sbend::getBendAngle,    return_value_policy<copy_const_reference>() )
  .def("Type",              &sbend::Type                )
  .def("getEntryAngle",     &sbend::getEntryAngle       )
  .def("getExitAngle",      &sbend::getExitAngle        )
  .def("setEntryAngle",     setEntryAngle_double        )
  .def("setEntryAngle",     setEntryAngle_particle      )
  .def("setExitAngle",      setExitAngle_double         )
  .def("setExitAngle",      setExitAngle_particle       )
  .def("getEntryFaceAngle", &sbend::getEntryFaceAngle   ) 
  .def("getExitFaceAngle",  &sbend::getExitFaceAngle    ) 
  .def("hasParallelFaces",  &sbend::hasParallelFaces    )
  .def("hasStandardFaces",  &sbend::hasStandardFaces    );

  //  void accept( BmlVisitor& v );
  //  void accept( ConstBmlVisitor& v ) const;


}

