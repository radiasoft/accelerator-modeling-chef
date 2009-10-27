/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-jetparticle.cpp
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

Vector  const& (   Particle::*State_ptr)()    const      = &Particle::state;
Mapping const& (JetParticle::*JetState_ptr)() const      = &JetParticle::state;

  Jet (JetParticle::*getX)()    const = &JetParticle::x; 
  Jet (JetParticle::*getY)()    const = &JetParticle::y; 
  Jet (JetParticle::*getCdt)()  const = &JetParticle::cdt; 

  Jet (JetParticle::*getNpx)()  const = &JetParticle::npx; 
  Jet (JetParticle::*getNpy)()  const = &JetParticle::npy; 
  Jet (JetParticle::*getNdp)()  const = &JetParticle::ndp; 

  void (JetParticle::*setX)(Jet const&)      = &JetParticle::x; 
  void (JetParticle::*setY)(Jet const&)      = &JetParticle::y; 
  void (JetParticle::*setCdt)(Jet const&)    = &JetParticle::cdt; 

  void (JetParticle::*setNpx)(Jet const&)    = &JetParticle::npx; 
  void (JetParticle::*setNpy)(Jet const&)    = &JetParticle::npy; 
  void (JetParticle::*setNdp)(Jet const&)    = &JetParticle::ndp; 


} // anonymous namespace

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_jetparticle () {


  class_<JetParticle, boost::noncopyable> JetParticle_("JetParticle", no_init);
  
  JetParticle_.def( init<const Particle&>());
  JetParticle_.def( "xIndex",   &JetParticle::xIndex);
  JetParticle_.staticmethod("xIndex");
  JetParticle_.def( "yIndex",   &JetParticle::yIndex);
  JetParticle_.staticmethod("yIndex");  
  JetParticle_.def( "cdtIndex", &JetParticle::cdtIndex);
  JetParticle_.staticmethod("cdtIndex");  
  JetParticle_.def( "npxIndex", &JetParticle::npxIndex);
  JetParticle_.staticmethod("npxIndex");  
  JetParticle_.def( "npyIndex", &JetParticle::npyIndex);
  JetParticle_.staticmethod("npyIndex");  
  JetParticle_.def( "ndpIndex", &JetParticle::ndpIndex);
  JetParticle_.staticmethod("ndpIndex");  

  JetParticle_.def("setREnergy", &JetParticle::setRefEnergy);
  JetParticle_.def("state",       JetState_ptr,  return_value_policy<copy_const_reference>() );

  JetParticle_.def("x",              getX);
  JetParticle_.def("y",              getY);
  JetParticle_.def("cdt",            getCdt);
  JetParticle_.def("npx",            getNpx);
  JetParticle_.def("npy",            getNpy);
  JetParticle_.def("ndp",            getNdp);

  JetParticle_.def("set_x",          setX);
  JetParticle_.def("set_y",          setY);
  JetParticle_.def("set_cdt",        setCdt);
  JetParticle_.def("set_npx",        setNpx);
  JetParticle_.def("set_npy",        setNpy);
  JetParticle_.def("set_ndp",        setNdp);

  JetParticle_.def("energy",             &JetParticle::energy);
  JetParticle_.def("kineticEnergy",      &JetParticle::kineticEnergy);
  JetParticle_.def("momentum",           &JetParticle::momentum);
  JetParticle_.def("normalizedMomentum", &JetParticle::normalizedMomentum);
  JetParticle_.def("mass",               &JetParticle::mass,              return_value_policy<copy_const_reference>() );
  JetParticle_.def("refBrho",            &JetParticle::refBrho,           return_value_policy<copy_const_reference>() );
  JetParticle_.def("refBeta",            &JetParticle::refBeta,           return_value_policy<copy_const_reference>() );
  JetParticle_.def("refGamma",           &JetParticle::refGamma,          return_value_policy<copy_const_reference>() );
  JetParticle_.def("gamma",              &JetParticle::gamma);
  JetParticle_.def("refMomentum",        &JetParticle::refMomentum,       return_value_policy<copy_const_reference>() );
  JetParticle_.def("pn",                 &JetParticle::pn);
  JetParticle_.def("refEnergy",          &JetParticle::refEnergy);
  JetParticle_.def("charge",             &JetParticle::charge,            return_value_policy<copy_const_reference>() );
  
           //JetVector VectorBeta() const;
           //JetVector VectorMomentum() const;
           //JetVector NormalizedVectorMomentum() const;
           //inline Jet Beta() const {
           //Jet BRho() const {
           //JetParticle& operator=(const JetParticle&);


  class_<JetProton, bases<JetParticle> >("JetProton")
   .def( init<double const&>())
   .def( init<Proton const&>());

  class_<JetAntiProton, bases<JetParticle> >("JetAntiProton") 
   .def( init<double const&>())
   .def( init<AntiProton const&>());

  class_<JetElectron, bases<JetParticle> >("JetElectron") 
   .def( init<double const&>())
   .def( init<Electron const&>());

  class_<JetPositron, bases<JetParticle> >("JetPositron") 
   .def( init<double const&>())
   .def( init<Positron const&>());

  class_<JetMuon, bases<JetParticle> >("JetMuon") 
   .def( init<double const&>())
   .def( init<Muon const&>());

  class_<JetAntiMuon, bases<JetParticle> >("JetAntiMuon") 
   .def( init<double const&>())
   .def( init<AntiMuon const&>());

}


