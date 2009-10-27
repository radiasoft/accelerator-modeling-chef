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

  Vector const& (Particle::*State_ptr)()       const      = &Particle::state;

  double (Particle::*getX)()    const = &Particle::x; 
  double (Particle::*getY)()    const = &Particle::y; 
  double (Particle::*getCdt)()  const = &Particle::cdt; 

  double (Particle::*getNpx)()  const = &Particle::npx; 
  double (Particle::*getNpy)()  const = &Particle::npy; 
  double (Particle::*getNdp)()  const = &Particle::ndp; 

  void (Particle::*setX)(double)      = &Particle::x; 
  void (Particle::*setY)(double)      = &Particle::y; 
  void (Particle::*setCdt)(double)    = &Particle::cdt; 

  void (Particle::*setNpx)(double)    = &Particle::npx; 
  void (Particle::*setNpy)(double)    = &Particle::npy; 
  void (Particle::*setNdp)(double)    = &Particle::ndp; 



} // anonymous namespace

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_particle () {

  class_<Particle, boost::noncopyable> Particle_("Particle", no_init ); 
  Particle_.def("psd",                  &Particle::psd)
  .staticmethod("psd")
  .def("setRefEnergy",         &Particle::setRefEnergy)
  .def("setRefMomentum",       &Particle::setRefMomentum)
  .def("setStateToZero",       &Particle::setStateToZero)
  .def("x",                    getX)
  .def("y",                    getY)
  .def("cdt",                  getCdt)
  .def("npx",                  getNpx)
  .def("npy",                  getNpy)
  .def("ndp",                  getNdp)
  .def("set_x",                setX)
  .def("set_y",                setY )
  .def("set_cdt",              setCdt)
  .def("set_npx",              setNpx)
  .def("set_npy",              setNpy)
  .def("set_ndp",              setNdp)
  .def("state",                State_ptr, return_value_policy<copy_const_reference>() )
  .def("energy",               &Particle::energy)
  .def("kineticEnergy",        &Particle::kineticEnergy)
  .def("momentum",             &Particle::momentum)
  .def("normalizedMomentum",   &Particle::normalizedMomentum)
  .def("gamma",                &Particle::gamma)
  .def("beta",                 &Particle::beta)
  .def("mass",                 &Particle::mass,             return_value_policy<copy_const_reference>() )
  .def("refBrho",              &Particle::refBrho,          return_value_policy<copy_const_reference>() )
  .def("refBeta",              &Particle::refBeta,          return_value_policy<copy_const_reference>() )
  .def("refMomentum",          &Particle::refMomentum,      return_value_policy<copy_const_reference>() )
  .def("pn",                   &Particle::pn)
  .def("refEnergy",            &Particle::refEnergy )
  .def("charge",               &Particle::charge,           return_value_policy<copy_const_reference>() )
  .def("brho",                 &Particle::brho);


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

 enum_<Particle::index>("PhaseSpaceIndex")
    .value("xIndex",     Particle::i_x   )
    .value("yIndex",     Particle::i_y   )
    .value("cdtIndex",   Particle::i_cdt )
    .value("npxIndex",   Particle::i_npx )
    .value("npyIndex",   Particle::i_npy )
    .value("ndpIndex",   Particle::i_ndp )
  ;



}


