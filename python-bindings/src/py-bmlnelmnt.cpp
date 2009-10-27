/***************************************************************************                                                               
****************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-bmlnelmnt.cpp
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
****************************************************************************
****************************************************************************/
#include <boost/python.hpp>
#include <string>

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/TBunch.h>
#include <beamline/BmlnElmnt.h>


class PropFunc;

using namespace boost::python;

namespace {

void   (BmlnElmnt::*propagate_particle     )  (      Particle&   ) const = &BmlnElmnt::propagate;
void   (BmlnElmnt::*propagate_jetparticle  )  (   JetParticle&   ) const = &BmlnElmnt::propagate;
void   (BmlnElmnt::*propagate_bunch        )  ( ParticleBunch&   ) const = &BmlnElmnt::propagate;

void   (BmlnElmnt::* setStrength1)               (double const&  )        =  &BmlnElmnt::setStrength;
void   (BmlnElmnt::* enterLocalFrame_particle)   (     Particle& )  const =  &BmlnElmnt::enterLocalFrame; 
void   (BmlnElmnt::* enterLocalFrame_jetparticle)(  JetParticle& )  const =  &BmlnElmnt::enterLocalFrame; 
void   (BmlnElmnt::* leaveLocalFrame_particle)   (     Particle& )  const =  &BmlnElmnt::leaveLocalFrame; 
void   (BmlnElmnt::* leaveLocalFrame_jetparticle)(  JetParticle& )  const =  &BmlnElmnt::leaveLocalFrame; 
void   (BmlnElmnt::* localPropagate_particle)    (     Particle& )  const =  &BmlnElmnt::localPropagate; 
void   (BmlnElmnt::* localPropagate_jetparticle) (  JetParticle& )  const =  &BmlnElmnt::localPropagate; 
void   (BmlnElmnt::* setReferenceTime_double   ) ( double        )        =  &BmlnElmnt::setReferenceTime;


tuple split( BmlnElmnt const& elm, double pct ) {
  return boost::python::tuple( elm.split(pct) );
}

} // namespace

void wrap_BmlnElmnt() {
  
  class_<BmlnElmnt, boost::shared_ptr<BmlnElmnt>, boost::noncopyable>("BmlnElmnt", no_init)
    .def("propagate",                     propagate_particle      )
    .def("propagate",                     propagate_jetparticle   )
    .def("propagate",                     propagate_bunch         )
    .def("getTag",                        &BmlnElmnt::getTag      )
    .def("setTag",                        &BmlnElmnt::setTag      )
     /****  virtual functions ***/ 
    .def("setLength",                     &BmlnElmnt::setLength   )
    .def("Strength",                      &BmlnElmnt::Strength    )
    .def("setStrength",                   &BmlnElmnt::setStrength )
    .def("Name",                          &BmlnElmnt::Name        )
    .def("rename",                        &BmlnElmnt::rename      )
    .def("getReferenceTime",              &BmlnElmnt::getReferenceTime)
    .def("setReferenceTime",              setReferenceTime_double )
    .def("Type",                          &BmlnElmnt::Type        )
    .def("OrbitLength",                   &BmlnElmnt::OrbitLength )
    .def("split",                         split       );
}  

