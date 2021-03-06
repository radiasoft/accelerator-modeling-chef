/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-bblens.cpp
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
*********************************************************************************
*********************************************************************************
********************************************************************************/

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/BBLens.h>

#include <boost/python.hpp>

//-------------------------------------------------------------------------------
// local code and definitions
//-------------------------------------------------------------------------------

namespace {

void local_NormalizedEField(double x, double y) {
     //  NormalizedEField(x,y)
}
  
void local_JetNormalizedEField(const Jet& x, const Jet& y) {
     //  NormalizedEField(x,y)
  
}



} // anonymous namespace

//-------------------------------------------------------------------------------
// wrapper definition
//-------------------------------------------------------------------------------

using namespace boost::python;


void wrap_bblens () {

 class_<BBLens, bases<BmlnElmnt>, BBLensPtr >("BBLens") 
   .def(init<std::string const&, double const&, double const&, double const&,  double const*>())
   //  .def("NormalizedEField",          &local_NormalizedEField)
   //.def("JetNormalizedEField",         &local_JetNormalizedEField)
   .def_readwrite("useRound",            &BBLens::useRound);

}


