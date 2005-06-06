/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-cf_sbend.h
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
#include <bmlnElmnt.h>
#include <CF_sbend.h>
#include <Particle.h>

using namespace boost::python;

double (CF_sbend::*setEntryAngleParticle_Ptr)   (const Particle&      ) = &CF_sbend::setEntryAngle;
double (CF_sbend::*setExitAngleParticle_Ptr)    (const Particle&      ) = &CF_sbend::setExitAngle;
double (CF_sbend::*setEntryAngleDouble_Ptr)     (double               ) = &CF_sbend::setEntryAngle;
double (CF_sbend::*setExitAngleDouble_Ptr)      (double               ) = &CF_sbend::setExitAngle;
double (CF_sbend::*AdjustPositionProton_Ptr)    (const Proton&        ) = &CF_sbend::AdjustPosition;
double (CF_sbend::*AdjustPositionJetProton_Ptr) (const JetProton&     ) = &CF_sbend::AdjustPosition;


void wrap_cf_sbend () {

  class_<CF_sbend, bases<bmlnElmnt> >("CF_sbend", init<double, double, double>() )

  .def(init<const char*, double, double, double>() )
  .def(init<double,      double, double, int>() )
  .def(init<const char*, double, double, double, int>() )
  .def(init<double,      double, double, double, double, int>() )
  .def(init<const char*, double, double, double, double, double, int>() )
  .def("eliminate",               &CF_sbend::eliminate)
  .def("setQuadrupole",           &CF_sbend::setQuadrupole)
  .def("setSextupole",            &CF_sbend::setSextupole)
  .def("setOctupole",             &CF_sbend::setOctupole) 
  .def("setDipoleField",          &CF_sbend::setDipoleField)
  .def("getQuadrupole",           &CF_sbend::getQuadrupole) 
  .def("getSextupole",            &CF_sbend::getSextupole)  
  .def("getOctupole",             &CF_sbend::getOctupole)
  .def("getDipoleField",          &CF_sbend::getDipoleField) 
  .def("getAngle",                &CF_sbend::getAngle)
  .def("Angle",                   &CF_sbend::Angle)
  .def("setEntryAngle",           setEntryAngleDouble_Ptr)
  .def("setExitAngle",            setExitAngleDouble_Ptr)
  .def("setEntryAngleParticle",   setEntryAngleParticle_Ptr)
  .def("setExitAngleParticle",    setExitAngleParticle_Ptr)
  .def("getEntryAngle",           &CF_sbend::getEntryAngle)
  .def("getExitAngle",            &CF_sbend::getExitAngle)
  .def("getEntryEdgeAngle",       &CF_sbend::getEntryEdgeAngle)
  .def("getExitEdgeAngle",        &CF_sbend::getExitEdgeAngle)
  .def("hasParallelFaces",        &CF_sbend::hasParallelFaces)
  .def("hasStandardFaces",        &CF_sbend::hasStandardFaces) 
  .def("AdjustPositionProton",    AdjustPositionProton_Ptr)
  .def("AdjustPositionJetProton", AdjustPositionJetProton_Ptr )
  .def("OrbitLength",             &CF_sbend::OrbitLength);

}

