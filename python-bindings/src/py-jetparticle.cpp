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

Vector  (   Particle::*State_ptr)()    const      = &Particle::State;
Mapping (JetParticle::*JetState_ptr)() const      = &JetParticle::State;

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
  JetParticle_.def("createStandardEnvironments", &JetParticle::createStandardEnvironments);
  JetParticle_.staticmethod("createStandardEnvironments");  
     //.def( "psd",  &JetParticle::psd)
     //.staticmethod("psd")  

  JetParticle_.def("SetReferenceEnergy", &JetParticle::SetReferenceEnergy);
  JetParticle_.def("State",    JetState_ptr, return_value_policy<return_by_value>() );

  JetParticle_.def("get_x",              &JetParticle::get_x);
  JetParticle_.def("get_y",              &JetParticle::get_y);
  JetParticle_.def("get_cdt",            &JetParticle::get_cdt);
  JetParticle_.def("get_npx",            &JetParticle::get_npx);
  JetParticle_.def("get_npy",            &JetParticle::get_npy);
  JetParticle_.def("get_ndp",            &JetParticle::get_ndp);

  JetParticle_.def("set_x",               &JetParticle::set_x);
  JetParticle_.def("set_y",               &JetParticle::set_y);
  JetParticle_.def("set_cdt",             &JetParticle::set_cdt);
  JetParticle_.def("set_npx",             &JetParticle::set_npx);
  JetParticle_.def("set_npy",             &JetParticle::set_npy);
  JetParticle_.def("set_ndp",             &JetParticle::set_ndp);

  JetParticle_.def("Energy",             &JetParticle::Energy);
  JetParticle_.def("Momentum",           &JetParticle::Momentum);
  JetParticle_.def("NormalizedMomentum", &JetParticle::NormalizedMomentum);
  JetParticle_.def("Mass",               &JetParticle::Mass);
  JetParticle_.def("ReferenceBRho",      &JetParticle::ReferenceBRho);
  JetParticle_.def("ReferenceBeta",      &JetParticle::ReferenceBeta);
  JetParticle_.def("ReferenceGamma",     &JetParticle::ReferenceGamma);
  JetParticle_.def("Gamma",              &JetParticle::Gamma);
  JetParticle_.def("ReferenceMomentum",  &JetParticle::ReferenceMomentum);
  JetParticle_.def("PNI2",               &JetParticle::PNI2);
  JetParticle_.def("ReferenceEnergy",    &JetParticle::ReferenceEnergy);
  JetParticle_.def("Charge",             &JetParticle::Charge);
  
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


