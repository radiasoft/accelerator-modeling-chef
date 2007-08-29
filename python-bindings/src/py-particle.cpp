/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-particle.cpp
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
******             ostiguy@fnal.gov                         
******                                                                
******    Fermi National Laboratory, Batavia, IL   60510
******
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/Mapping.h>

using namespace boost::python;

//------------------------------------------------------------------------------
// local code and definitions
//------------------------------------------------------------------------------

namespace {

Vector const& (Particle::*State_void)()       const      = &Particle::State;

} // anonymous namespace

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_particle () {

  class_<Particle, boost::noncopyable> Particle_("Particle", no_init ); 
  Particle_.def("psd",                  &Particle::psd)
  .staticmethod("psd")
  .def("SetReferenceEnergy",   &Particle::SetReferenceEnergy)
  .def("SetReferenceMomentum", &Particle::SetReferenceMomentum)
  .def("setStateToZero",       &Particle::setStateToZero)
  
  .def ("get_x",               &Particle::get_x)
  .def ("get_y",               &Particle::get_y)
  .def ("get_cdt",             &Particle::get_cdt)
  .def ("get_npx",             &Particle::get_npx)
  .def ("get_npy",             &Particle::get_npy)
  .def ("get_ndp",             &Particle::get_ndp)
  .def ("set_x",               &Particle::set_x)
  .def ("set_y",               &Particle::set_y )
  .def ("set_cdt",             &Particle::set_cdt)
  .def ("set_npx",             &Particle::set_npx)
  .def ("set_npy",             &Particle::set_npy)
  .def ("set_ndp",             &Particle::set_ndp)
  .def("State",                State_void, return_value_policy<copy_const_reference>() )
  .def("Energy",               &Particle::Energy)
  .def("Momentum",             &Particle::Momentum)
  .def("NormalizedMomentum",   &Particle::NormalizedMomentum)
  .def("Gamma",                &Particle::Gamma)
  .def("Beta",                 &Particle::Beta)
  .def("Mass",                 &Particle::Mass,             return_value_policy<copy_const_reference>() )
  .def("ReferenceBRho",        &Particle::ReferenceBRho,    return_value_policy<copy_const_reference>() )
  .def("ReferenceBeta",        &Particle::ReferenceBeta,    return_value_policy<copy_const_reference>() )
  .def("ReferenceMomentum",    &Particle::ReferenceMomentum,return_value_policy<copy_const_reference>() )

  .def("PNI2",                 &Particle::PNI2,             return_value_policy<copy_const_reference>() )

  .def("ReferenceEnergy",      &Particle::ReferenceEnergy,  return_value_policy<copy_const_reference>() )
  .def("Charge",               &Particle::Charge,           return_value_policy<copy_const_reference>() )
  .def("BRho",                 &Particle::BRho);


  class_<Proton, bases<Particle> >("Proton", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());

  class_<AntiProton, bases<Particle> >("AntiProton", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());


  class_<Electron, bases<Particle> >("Electron", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());

  class_<Positron, bases<Particle> >("Positron", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());

  class_<Muon, bases<Particle> >("Muon", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());

  class_<AntiMuon, bases<Particle> >("AntiMuon", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());

 scope scope_Particle = Particle_;

 enum_<Particle::PhaseSpaceIndex>("PhaseSpaceIndex")
    .value("xIndex",     Particle::xIndex   )
    .value("yIndex",     Particle::yIndex   )
    .value("cdtIndex",   Particle::cdtIndex )
    .value("npxIndex",   Particle::npxIndex )
    .value("npyIndex",   Particle::npyIndex )
    .value("ndpIndex",   Particle::ndpIndex )
  ;



}


