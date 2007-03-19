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
#include <beamline/bmlnElmnt.h>


class PropFunc;

using namespace boost::python;

namespace {

void   (bmlnElmnt::*propagateParticle)     (Particle&      ) = &bmlnElmnt::propagate;
void   (bmlnElmnt::*propagateJetParticle)  (JetParticle&   ) = &bmlnElmnt::propagate;
void   (bmlnElmnt::*propagateParticleBunch)(ParticleBunch& ) = &bmlnElmnt::propagate;

void   (bmlnElmnt::*setStrength1)          (double const&  ) = &bmlnElmnt::setStrength;

double (bmlnElmnt::*setReferenceTime1)     (Particle const&) = &bmlnElmnt::setReferenceTime;
double (bmlnElmnt::*setReferenceTime2)     (double   const&) = &bmlnElmnt::setReferenceTime;

}

void wrap_bmlnelmnt() {
  
  class_<bmlnElmnt, boost::shared_ptr<bmlnElmnt>, boost::noncopyable>("bmlnElmnt", no_init)
    .def("propagateParticle",             propagateParticle)
    .def("propagateJetParticle",          propagateJetParticle )
    .def("propagateParticleBunch",        propagateParticleBunch )
    .def("getTag",                        &bmlnElmnt::getTag)
    .def("setTag",                        &bmlnElmnt::setTag)
    .def("setShunt",                      &bmlnElmnt::setShunt)
    .def("getShunt",                      &bmlnElmnt::getShunt)        
     /****  virtual functions ***/ 
    .def("setLength",                     &bmlnElmnt::setLength)
    .def("Strength",                      &bmlnElmnt::Strength)
    .def("setStrength",                   setStrength1)
    .def("setCurrent",                    &bmlnElmnt::setCurrent)
    .def("Name",                          &bmlnElmnt::Name)
    .def("rename",                        &bmlnElmnt::rename)
    .def("getReferenceTime",              &bmlnElmnt::getReferenceTime)
    .def("setReferenceTime",              setReferenceTime1)
    .def("setReferenceTime",              setReferenceTime2)
    .def("Current",                       &bmlnElmnt::Current)
    .def("Type",                          &bmlnElmnt::Type)
    .def("OrbitLength",                   &bmlnElmnt::OrbitLength)
    .def_readwrite("dataHook",            &bmlnElmnt::dataHook);
}  

