#include <boost/python.hpp>

#include <Jet.h>

#ifdef FNAL_FIRST_ORDER
#include <JetC.h>
#endif

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

//  static TJetEnvironment<T1,T2>* CreateEnvFrom( TJetEnvironment<T2,T1>* );
//  static TJetEnvironment<T1,T2>* CreateEnvFrom( const Vector&, int );

// Jet__environment*   ( Jet::*  )(Jet__environment*  )             =  &Jet::CreateEnvFrom;
JetC__environment*     ( * CreateEnvFromPtr)(Jet__environment*  )   = &JetC::CreateEnvFrom;



void wrap_mxyzptlk_jetc() {


 class_<Jet__environment>  Jet__environmentClass_("Jet__environment", no_init);
 class_<JetC__environment> JetC__environmentClass_("JetC__environment", no_init);


 class_<JetC> JetC_Class_("JetC", init<>()); 

  JetC_Class_.def(self_ns::str(self));
  JetC_Class_.def("BeginEnvironment", &JetC::BeginEnvironment);
  JetC_Class_.staticmethod("BeginEnvironment");
  JetC_Class_.def( "EndEnvironment",  &JetC::EndEnvironment,  return_value_policy<reference_existing_object>());
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
  Jet_Class_.def( "EndEnvironment",   &Jet::EndEnvironment,  return_value_policy<reference_existing_object>());
  Jet_Class_.staticmethod("EndEnvironment");
  Jet_Class_.def("getLastEnv",        &::getLastEnv_local,  return_value_policy<reference_existing_object>()); 
  Jet_Class_.staticmethod("getLastEnv");
  Jet_Class_.def("setLastEnv",        &::setLastEnv_local );
  Jet_Class_.staticmethod("setLastEnv");

  //Jet_Class_.def("CreateEnvFrom",    &Jet::CreateEnvFrom,  return_value_policy<reference_existing_object>());
  //Jet_Class_.staticmethod("CreateEnvFrom");

}

