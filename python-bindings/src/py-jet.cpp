#include <boost/python.hpp>

// #include <Jet>
#include <Jet.h>
#include <JetC.h>

#include <string>
#include <iostream>

using namespace boost::python;


static Jet__environment*  EndEnvironment_local() 
{
  Jet::EndEnvironment( 0 );
}


static JetC__environment* EndEnvironmentC_local() 
{
  JetC::EndEnvironment( 0 );
}

static void  setLastEnv_local (  Jet__environment* env)  { Jet::_lastEnv = env; }
static void  setLastEnvC_local( JetC__environment* env)  { JetC::_lastEnv = env; }

static Jet__environment*  getLastEnv_local()   {  return Jet::_lastEnv; }
static JetC__environment* getLastEnvC_local()  { return JetC::_lastEnv; }

void wrap_mxyzptlk_jetc() {

 class_<JetC> JetC_Class_("JetC", init<>()); 

  JetC_Class_.def(self_ns::str(self));
  JetC_Class_.def("BeginEnvironment", &JetC::BeginEnvironment);
  JetC_Class_.staticmethod("BeginEnvironment");
  JetC_Class_.def( "EndEnvironment",  &::EndEnvironmentC_local,  return_value_policy<return_opaque_pointer>());
  JetC_Class_.staticmethod("EndEnvironment");
  JetC_Class_.def("getLastEnv",       &::getLastEnvC_local, return_value_policy<return_opaque_pointer>() ) ;
  JetC_Class_.staticmethod("getLastEnv");
  JetC_Class_.def("setLastEnv",       &::setLastEnvC_local);
  JetC_Class_.staticmethod("setLastEnv");
  JetC_Class_.def("CreateEnvFrom",    &JetC::CreateEnvFrom,  return_value_policy<return_opaque_pointer>());
  JetC_Class_.staticmethod("CreateEnvFrom");


}

void wrap_mxyzptlk_jet() {

  class_<Jet> Jet_Class_ ("Jet", init<>());
  Jet_Class_.def(self_ns::str(self));
  Jet_Class_.def( "BeginEnvironment", &Jet::BeginEnvironment);
  Jet_Class_.staticmethod("BeginEnvironment");
  Jet_Class_.def( "EndEnvironment",   &::EndEnvironment_local,  return_value_policy<return_opaque_pointer>());
  Jet_Class_.staticmethod("EndEnvironment");
  Jet_Class_.def("getLastEnv",        &::getLastEnv_local,  return_value_policy<return_opaque_pointer>()); 
  Jet_Class_.staticmethod("getLastEnv");
  Jet_Class_.def("setLastEnv",        &::setLastEnv_local );
  Jet_Class_.staticmethod("setLastEnv");


}


