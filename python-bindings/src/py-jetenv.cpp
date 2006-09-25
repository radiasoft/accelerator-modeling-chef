/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-jetenv.cpp
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

#include <mxyzptlk/Jet.h>
#include <mxyzptlk/EnvPtr.h>

#include <string>
#include <iostream>
#include <numarray/numarray.h>
#include <numarray/libnumarray.h>
#include <python-bindings/py-exception.h>

using namespace boost::python;


static void EndEnvironment_local( ) 
{
  // double* scale = new double[ Jet::_workEnv->numVar] 

  //  For the moment, the signature of this function ignores the scale array. 
  //  TO DO: an additional function that accepts a python array type as a argument
  //  passing a null ptr implies default scaling 

  Jet__environment::EndEnvironment();  
}

static void EndEnvironmentC_local( ) 
{
  // double* scale = new double[JetC::_workEnv->numVar] 

  //  for the moment, the signature of this function ignores the scale array. 
  //  TO DO: an additional function that accepts a python array type as a argument
  //  passing a null ptr implies default scaling 

  JetC__environment::EndEnvironment(); 
}

static EnvPtr<double> makeJetEnvironment_3_local(int maxweight, int nvar, int spacedim ) {
 
       return Jet__environment::makeJetEnvironment(maxweight, nvar, spacedim);
}

static EnvPtr<double> makeJetEnvironment_4_local(int maxweight, int nvar, int spacedim, numeric::array& refpt ) {
 
  if ( !   refpt.is_c_array() ) 
    throw PyBindingsException("makeJetEnvironment: reference points underlying data array must be a c-array.");  

  if ( std::string( extract<const char*>( ( refpt.type()).attr("name") ) ) != std::string( "Float64") ) 
    throw PyBindingsException("makeJetEnvironment:: reference pt data array must be of type Float64");  
      
       return Jet__environment::makeJetEnvironment(maxweight, nvar, spacedim, reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( refpt.ptr())->data ) );
}

static EnvPtr<std::complex<double> > makeJetCEnvironment_3_local(int maxweight, int nvar, int spacedim ) {
 
       return JetC__environment::makeJetEnvironment(maxweight, nvar, spacedim);
}


static EnvPtr<std::complex<double> > getLastEnvC_local( ) 
{
  return JetC__environment::getLastEnv(); 
}

static EnvPtr<double>  getLastEnv_local( ) 
{
  return Jet__environment::getLastEnv(); 
}

static void setLastEnvC_local( EnvPtr<std::complex<double> > const& env ) 
{
  JetC__environment::setLastEnv(env); 
}

static void setLastEnv_local(  EnvPtr<double> const& env  ) 
{
  Jet__environment::setLastEnv(env); 
}


static EnvPtr<std::complex<double> > toCmplxEnvironment( EnvPtr<double> const& env) {
       
  return TJetEnvironment<std::complex<double> >::makeJetEnvironment(env); // implicit conversion 
}

  

void wrap_mxyzptlk_jetenv() {

  def("BeginEnvironment",  &Jet__environment::BeginEnvironment);
  def("EndEnvironment",    &EndEnvironment_local);
  def("makeJetEnvironment",&makeJetEnvironment_3_local);      
  def("makeJetEnvironment",&makeJetEnvironment_4_local);      

  class_<EnvPtr<double> >  Jet__environmentClass_("Jet__environment", no_init); 
  Jet__environmentClass_.def("getLastEnv",        &getLastEnv_local ); 
  Jet__environmentClass_.staticmethod("getLastEnv");
  Jet__environmentClass_.def("setLastEnv",        &setLastEnv_local );
  Jet__environmentClass_.staticmethod("setLastEnv");
  Jet__environmentClass_.def( self_ns::str(self));
  def("makeJetEnvironment", &makeJetEnvironment_3_local);      

}

void wrap_mxyzptlk_jetcenv() {

 def("BeginEnvironmentC",  &JetC__environment::BeginEnvironment);
 def("EndEnvironmentC",    &EndEnvironmentC_local);
 
 class_<EnvPtr<std::complex<double> > >  JetC__environmentClass_("JetC__environment", no_init);
 JetC__environmentClass_.def("getLastEnv",        &getLastEnvC_local ); 
 JetC__environmentClass_.staticmethod("getLastEnv");
 JetC__environmentClass_.def("setLastEnv",        &setLastEnvC_local );
 JetC__environmentClass_.staticmethod("setLastEnv");
 JetC__environmentClass_.def( self_ns::str(self));

 def("makeJetCEnvironment", &makeJetCEnvironment_3_local);      

 def("toCmplxEnvironment",&toCmplxEnvironment);      

 // def("toRealEnvironment", &toRealEnvironment);      


}



