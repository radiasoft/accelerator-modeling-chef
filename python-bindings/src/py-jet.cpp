/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-jet.cpp
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

#include <Jet.h>


#include <string>
#include <iostream>

using namespace boost::python;

static JetC__environment*  getLastEnvC_local( ) 
{
  return JetC::_lastEnv; 
}

static Jet__environment*  getLastEnv_local( ) 
{
  return Jet::_lastEnv; 
}

static void setLastEnvC_local( JetC__environment* env ) 
{
  JetC::_lastEnv = env; 
}

static void setLastEnv_local(  Jet__environment* env  ) 
{
  Jet::_lastEnv = env; 
}

static void EndEnvironment_local( ) 
{
  // double* scale = new double[ Jet::_workEnv->numVar] 

  //  For the moment, the signature of this function ignores the scale array. 
  //  TO DO: an additional function that accepts a python array type as a argument
  //  passing a null ptr implies default scaling 

  Jet::EndEnvironment(0);  
}

static void EndEnvironmentC_local( ) 
{
  // double* scale = new double[JetC::_workEnv->numVar] 

  //  for the moment, the signature of this function ignores the scale array. 
  //  TO DO: an additional function that accepts a python array type as a argument
  //  passing a null ptr implies default scaling 

  JetC::EndEnvironment(0); 
}


// Jet__environment*   ( Jet::*  )(Jet__environment*  )                   =  &Jet__environment::CreateEnvFrom;
JetC__environment*     ( * CreateEnvFromPtr)(const Jet__environment*  )   = &JetC__environment::CreateEnvFrom;



void wrap_mxyzptlk_jetc() {


 class_<Jet__environment,  boost::noncopyable>  Jet__environmentClass_("Jet__environment", no_init);
 class_<JetC__environment, boost::noncopyable> JetC__environmentClass_("JetC__environment", no_init);


 class_<JetC> JetC_Class_("JetC", init<>()); 

  JetC_Class_.def(self_ns::str(self));
  JetC_Class_.def("BeginEnvironment", &JetC::BeginEnvironment);
  JetC_Class_.staticmethod("BeginEnvironment");
  JetC_Class_.def( "EndEnvironment",  EndEnvironmentC_local,  return_value_policy<reference_existing_object>());
  JetC_Class_.staticmethod("EndEnvironment");
  JetC_Class_.def("getLastEnv",       &getLastEnvC_local, return_value_policy<reference_existing_object>() ) ;
  JetC_Class_.staticmethod("getLastEnv");
  JetC_Class_.def("setLastEnv",       &setLastEnvC_local);
  JetC_Class_.staticmethod("setLastEnv");
  JetC_Class_.def("CreateEnvFrom",    CreateEnvFromPtr,  return_value_policy<reference_existing_object>());
  JetC_Class_.staticmethod("CreateEnvFrom");

}

void wrap_mxyzptlk_jet() {

  class_<Jet> Jet_Class_ ("Jet", init<>());
  Jet_Class_.def(self_ns::str(self));
  Jet_Class_.def( "BeginEnvironment", &Jet::BeginEnvironment);
  Jet_Class_.staticmethod("BeginEnvironment");
  Jet_Class_.def( "EndEnvironment",   EndEnvironment_local,  return_value_policy<reference_existing_object>());
  Jet_Class_.staticmethod("EndEnvironment");
  Jet_Class_.def("getLastEnv",        &::getLastEnv_local,  return_value_policy<reference_existing_object>()); 
  Jet_Class_.staticmethod("getLastEnv");
  Jet_Class_.def("setLastEnv",        &::setLastEnv_local );
  Jet_Class_.staticmethod("setLastEnv");

  //Jet_Class_.def("CreateEnvFrom",    &Jet::CreateEnvFrom,  return_value_policy<reference_existing_object>());
  //Jet_Class_.staticmethod("CreateEnvFrom");

}

