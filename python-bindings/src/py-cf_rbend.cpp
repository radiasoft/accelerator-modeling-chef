/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-cf_rbend.h
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
#include <beamline/CF_rbend.h>
#include <beamline/Particle.h>

using namespace boost::python;

static double (CF_rbend::*setEntryAngleDouble_Ptr)     (double               ) = &CF_rbend::setEntryAngle;
static double (CF_rbend::*setEntryAngleParticle_Ptr)   (const Particle&      ) = &CF_rbend::setEntryAngle;

static double (CF_rbend::*setExitAngleDouble_Ptr)      (double               ) = &CF_rbend::setExitAngle;
static double (CF_rbend::*setExitAngleParticle_Ptr)    (const Particle&      ) = &CF_rbend::setExitAngle;

// The following functions exist in the header file CF_rbend.h but currently have no implementation.

//static double (CF_rbend::*AdjustPositionParticle_Ptr)    (const Particle&        ) = &CF_rbend::AdjustPosition;
//static double (CF_rbend::*AdjustPositionJetParticle_Ptr) (const JetParticle&     ) = &CF_rbend::AdjustPosition;

void wrap_cf_rbend () {

  class_<CF_rbend, bases<bmlnElmnt> >("CF_rbend", init<double, double, double>() )

  .def(init<const char*, double, double, double>() )
  .def(init<double,      double, double, int>() )
  .def(init<const char*, double, double, double, int>() )
  .def(init<double,      double, double, double, double, int>() )
  .def(init<const char*, double, double, double, double, double, int>() )
  .def("eliminate",               &CF_rbend::eliminate)
  .def("setQuadrupole",           &CF_rbend::setQuadrupole)
  .def("setSextupole",            &CF_rbend::setSextupole)
  .def("setOctupole",             &CF_rbend::setOctupole) 
  .def("setDipoleField",          &CF_rbend::setDipoleField)
  .def("getQuadrupole",           &CF_rbend::getQuadrupole) 
  .def("getSextupole",            &CF_rbend::getSextupole)  
  .def("getOctupole",             &CF_rbend::getOctupole)
  .def("getDipoleField",          &CF_rbend::getDipoleField) 
  .def("setEntryAngle",           setEntryAngleDouble_Ptr)
  .def("setExitAngle",            setExitAngleDouble_Ptr)
  .def("setEntryAngleParticle",   setEntryAngleParticle_Ptr)
  .def("setExitAngleParticle",    setExitAngleParticle_Ptr)
  .def("getEntryAngle",           &CF_rbend::getEntryAngle)
  .def("getExitAngle",            &CF_rbend::getExitAngle)
  .def("getEntryEdgeAngle",       &CF_rbend::getEntryEdgeAngle)
  .def("getExitEdgeAngle",        &CF_rbend::getExitEdgeAngle)
  .def("hasParallelFaces",        &CF_rbend::hasParallelFaces)
  .def("hasStandardFaces",        &CF_rbend::hasStandardFaces) 
    //  .def("AdjustPositionParticle",    AdjustPositionParticle_Ptr)
    //.def("AdjustPositionJetParticle", AdjustPositionJetParticle_Ptr )
  .def("OrbitLength",             &CF_rbend::OrbitLength);

}

