/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
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
****************************************************************************/
#include <boost/python.hpp>

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <basic_toolkit/VectorD.h>
#include <mxyzptlk/Mapping.h>

using namespace boost::python;

Vector  (Particle::*State_void)() const            = &Particle::State;
Mapping (JetParticle::*JetState_void)() const      = &JetParticle::State;

void wrap_particle () {

  class_<Particle>("Particle", no_init)
  .def("xIndex",      &Particle::xIndex)
  .staticmethod("xIndex")
  .def("yIndex",      &Particle::yIndex)
  .staticmethod("yIndex")
  .def("cdtIndex",    &Particle::cdtIndex)
  .staticmethod("cdtIndex")
  .def("npxIndex",    &Particle::npxIndex)
  .staticmethod("npxIndex")
  .def("npyIndex",    &Particle::npyIndex)
  .staticmethod("npyIndex")
  .def("ndpIndex",    &Particle::ndpIndex)
  .staticmethod("ndpIndex")
  .def("psd",                  &Particle::psd)
  .staticmethod("psd")
  .def("SetReferenceEnergy",   &Particle::SetReferenceEnergy)
  .def("SetReferenceMomentum", &Particle::SetReferenceMomentum)
  .def("setStateToZero",       &Particle::setStateToZero)
  
  .def ("get_x",    &Particle::get_x)
  .def ("get_y",    &Particle::get_y)
  .def ("get_cdt",  &Particle::get_cdt)
  .def ("get_npx",  &Particle::get_npx)
  .def ("get_npy",  &Particle::get_npy)
  .def ("get_ndp",  &Particle::get_ndp)
  .def ("set_x",    &Particle::set_x)
  .def ("set_y",    &Particle::set_y )
  .def ("set_cdt",  &Particle::set_cdt)
  .def ("set_npx",  &Particle::set_npx)
  .def ("set_npy",  &Particle::set_npy)
  .def ("set_ndp",  &Particle::set_ndp)
  .def("State",     State_void, return_value_policy<return_by_value>() )
  .def("Energy",              &Particle::Energy)
  .def("Momentum",            &Particle::Momentum)
  .def("NormalizedMomentum",  &Particle::NormalizedMomentum)
  .def("Gamma",               &Particle::Gamma)
  .def("Beta",                &Particle::Beta)
  .def("ReferenceBRho",       &Particle::ReferenceBRho)
  .def("ReferenceBeta",       &Particle::ReferenceBeta)
  .def("ReferenceMomentum",   &Particle::ReferenceMomentum)

  .def("PNI2",                &Particle::PNI2)

  .def("ReferenceEnergy",     &Particle::ReferenceEnergy)
  .def("Charge",              &Particle::Charge)
  .def("BRho",                &Particle::BRho);



  class_<Proton, bases<Particle> >("Proton", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());


  class_<Positron, bases<Particle> >("Positron", init<>() )
   .def( init<double const&>())
   .def( init<double const&, Vector const&>());


  def("createStandardEnvironments", 
      &JetParticle::createStandardEnvironments);

   class_<JetParticle>("JetParticle", no_init)
       //.def( init<double>())
       //.def( init<double,double>())
       //.def( init<double,double, double*>())
       //.def( init<const Particle&>())
   .def( "xIndex",   &JetParticle::xIndex)
   .staticmethod("xIndex")  
   .def( "yIndex",   &JetParticle::yIndex)
   .staticmethod("yIndex")  
   .def( "cdtIndex", &JetParticle::cdtIndex)
   .staticmethod("cdtIndex")  
   .def( "npxIndex", &JetParticle::npxIndex)
   .staticmethod("npxIndex")  
   .def( "npyIndex", &JetParticle::npyIndex)
   .staticmethod("npyIndex")  
   .def( "ndpIndex", &JetParticle::ndpIndex)
   .staticmethod("ndpIndex")  
     //.def( "psd",  &JetParticle::psd)
     //.staticmethod("psd")  
   .def("SetReferenceEnergy", &JetParticle::SetReferenceEnergy)
       //void setState( double* );
       //void setState( const Vector& );
       //void setState( const Mapping& );
       //void getState( Mapping& );
       //void getState( Jet* );
    .def("State",    JetState_void, return_value_policy<return_by_value>() )

    .def("get_x",    &JetParticle::get_x)
    .def("get_y",    &JetParticle::get_y)
    .def("get_cdt",  &JetParticle::get_cdt)
    .def("get_npx",  &JetParticle::get_npx)
    .def("get_npy",  &JetParticle::get_npy)
    .def("get_ndp",  &JetParticle::get_ndp)

    .def("set_x",   &JetParticle::set_x)
    .def("set_y",   &JetParticle::set_y)
    .def("set_cdt", &JetParticle::set_cdt)
    .def("set_npx", &JetParticle::set_npx)
    .def("set_npy", &JetParticle::set_npy)
    .def("set_ndp", &JetParticle::set_ndp)
   //MatrixD SymplecticTest();  // Tests the state for the
                             // symplectic condition, 1 = - MJM^t; 
                             // returns unit matrix if all is well.
                             // Note: this assumes a 6x6 state: 
                             // ( x, y, cdt; px/p, py/p, dp/p )

    .def("Energy",              &JetParticle::Energy)
    .def("Momentum",            &JetParticle::Momentum)
    .def("NormalizedMomentum",  &JetParticle::NormalizedMomentum)
    .def("Mass",                &JetParticle::Mass)
    .def("ReferenceBRho",       &JetParticle::ReferenceBRho)
    .def("ReferenceBeta",       &JetParticle::ReferenceBeta)
    .def("ReferenceGamma",      &JetParticle::ReferenceGamma)
    .def("Gamma",               &JetParticle::Gamma)
    .def("ReferenceMomentum",   &JetParticle::ReferenceMomentum)
    .def("PNI2",                &JetParticle::PNI2)
    .def("ReferenceEnergy",     &JetParticle::ReferenceEnergy)
    .def("Charge",              &JetParticle::Charge);
  
           // JetVector VectorBeta() const;
           //JetVector VectorMomentum() const;
           //JetVector NormalizedVectorMomentum() const;
           //inline Jet Beta() const {
           //Jet BRho() const {
           //JetParticle& operator=(const JetParticle&);


  class_<JetProton, bases<JetParticle> >("JetProton") 
   .def( init<double const&>())
   .def( init<const Proton&>());

  class_<JetPositron, bases<JetParticle> >("JetPositron") 
   .def( init<double const&>())
   .def( init<const Positron&>());

}


