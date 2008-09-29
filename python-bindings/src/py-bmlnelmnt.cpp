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
#include <beamline/bmlnElmnt.h>


class PropFunc;

using namespace boost::python;

namespace {

void   (bmlnElmnt::*propagate_particle     )  (      Particle&   ) const = &bmlnElmnt::propagate;
void   (bmlnElmnt::*propagate_jetparticle  )  (   JetParticle&   ) const = &bmlnElmnt::propagate;
void   (bmlnElmnt::*propagate_bunch        )  ( ParticleBunch&   ) const = &bmlnElmnt::propagate;

void   (bmlnElmnt::* setStrength1)               (double const&  )        =  &bmlnElmnt::setStrength;
void   (bmlnElmnt::* enterLocalFrame_particle)   (     Particle& )  const =  &bmlnElmnt::enterLocalFrame; 
void   (bmlnElmnt::* enterLocalFrame_jetparticle)(  JetParticle& )  const =  &bmlnElmnt::enterLocalFrame; 
void   (bmlnElmnt::* leaveLocalFrame_particle)   (     Particle& )  const =  &bmlnElmnt::leaveLocalFrame; 
void   (bmlnElmnt::* leaveLocalFrame_jetparticle)(  JetParticle& )  const =  &bmlnElmnt::leaveLocalFrame; 
void   (bmlnElmnt::* localPropagate_particle)    (     Particle& )  const =  &bmlnElmnt::localPropagate; 
void   (bmlnElmnt::* localPropagate_jetparticle) (  JetParticle& )  const =  &bmlnElmnt::localPropagate; 
void   (bmlnElmnt::* setReferenceTime_double   ) ( double const& )        =  &bmlnElmnt::setReferenceTime;


tuple split( bmlnElmnt const& elm, double pct ) {
  return boost::python::tuple( elm.split(pct) );
}

} // namespace

void wrap_bmlnelmnt() {
  
  class_<bmlnElmnt, boost::shared_ptr<bmlnElmnt>, boost::noncopyable>("bmlnElmnt", no_init)
    .def("propagate",                     propagate_particle      )
    .def("propagate",                     propagate_jetparticle   )
    .def("propagate",                     propagate_bunch         )
    .def("getTag",                        &bmlnElmnt::getTag      )
    .def("setTag",                        &bmlnElmnt::setTag      )
     /****  virtual functions ***/ 
    .def("setLength",                     &bmlnElmnt::setLength   )
    .def("Strength",                      &bmlnElmnt::Strength    )
    .def("setStrength",                   &bmlnElmnt::setStrength )
    .def("Name",                          &bmlnElmnt::Name        )
    .def("rename",                        &bmlnElmnt::rename      )
    .def("getReferenceTime",              &bmlnElmnt::getReferenceTime)
    .def("setReferenceTime",              setReferenceTime_double )
    .def("Type",                          &bmlnElmnt::Type        )
    .def("OrbitLength",                   &bmlnElmnt::OrbitLength )
    .def("split",                         split       );
}  

