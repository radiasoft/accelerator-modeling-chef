/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-normalformsage.cpp
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

#include <beamline/bmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/normalFormSage.h>
#include <iostream>


void wrap_normalformsage() {

using namespace boost::python;

 class_<normalFormSage>("normalFormSage", 
			init<Mapping &, const JetParticle&, int>())
   .def(init<Mapping &, const Particle&>())
  .def( "cnvDataToNormalForm",&normalFormSage::cnvDataToNormalForm)
  .def( "cnvDataFromNormalForm", &normalFormSage::cnvDataFromNormalForm)
   .def( "getNormalEigenvectors", &normalFormSage::getNormalEigenvectors, return_value_policy<reference_existing_object>())
    ;
}
