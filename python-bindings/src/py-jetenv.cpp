/***************************************************************************                                                               
****************************************************************************
****************************************************************************
****************************************************************************
******  
******  Python bindings for mxyzpltk/beamline libraries 
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
******             ostiguy@fnal.gov                         
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******
****************************************************************************
****************************************************************************
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

//-----------------------------------------------------------------------------------------------
// locally defined classes and functions 
//-----------------------------------------------------------------------------------------------

namespace { 


EnvPtr<double> makeJetEnvironment_3(int maxweight, int nvar, int spacedim ) 
{
 
       return Jet__environment::makeJetEnvironment(maxweight, nvar, spacedim);
}

EnvPtr<double> makeJetEnvironment_4(int maxweight, int nvar, int spacedim, numeric::array& refpt ) 
{
 
  if ( !   refpt.is_c_array() ) 
    throw PyBindingsException("makeJetEnvironment: reference points underlying data array must be a c-array.");  

  if ( std::string( extract<const char*>( ( refpt.type()).attr("name") ) ) != std::string( "Float64") ) 
    throw PyBindingsException("makeJetEnvironment:: reference pt data array must be of type Float64");  
      
  double*  p = reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( refpt.ptr())->data );
  return Jet__environment::makeJetEnvironment(maxweight, nvar, spacedim,  Vector(p, p+nvar) );
}


EnvPtr<std::complex<double> > makeJetCEnvironment_3(int maxweight, int nvar, int spacedim ) 
{
 
       return JetC__environment::makeJetEnvironment(maxweight, nvar, spacedim);
}


EnvPtr<std::complex<double> > toCmplxEnvironment( EnvPtr<double> const& env) {
       
  return TJetEnvironment<std::complex<double> >::makeJetEnvironment(env); // implicit conversion 
}

} // anonymous namespace   

//-----------------------------------------------------------------------------------------------
// end of locally defined classes and functions 
//-----------------------------------------------------------------------------------------------


void wrap_mxyzptlk_jetenv() {

  def("BeginEnvironment",  &Jet__environment::BeginEnvironment);
  def("EndEnvironment",    &Jet__environment::EndEnvironment);
  def("makeJetEnvironment",&makeJetEnvironment_3 );      
  def("makeJetEnvironment",&makeJetEnvironment_4 );      
  def("createStandardEnvironments", &createStandardEnvironments);

  class_<EnvPtr<double> >  Jet__environmentClass_("Jet__environment", no_init); 
  Jet__environmentClass_.def("popEnv",        &Jet__environment::popEnv  ); 
  Jet__environmentClass_.staticmethod("popEnv");
  Jet__environmentClass_.def("topEnv",        &Jet__environment::topEnv  ); 
  Jet__environmentClass_.staticmethod("topEnv");
  Jet__environmentClass_.def("pushEnv",       &Jet__environment::pushEnv );
  Jet__environmentClass_.staticmethod("pushEnv");
  Jet__environmentClass_.def( self_ns::str(self));
  def("makeJetEnvironment", &makeJetEnvironment_3);      

}

void wrap_mxyzptlk_jetcenv() {

 def("BeginEnvironmentC",  &JetC__environment::BeginEnvironment);
 def("EndEnvironmentC",    &JetC__environment::EndEnvironment  );
 
 class_<EnvPtr<std::complex<double> > >  JetC__environmentClass_("JetC__environment", no_init);
 JetC__environmentClass_.def("popEnv",        &JetC__environment::popEnv ); 
 JetC__environmentClass_.staticmethod("popEnv");
 JetC__environmentClass_.def("topEnv",        &JetC__environment::topEnv ); 
 JetC__environmentClass_.staticmethod("topEnv");
 JetC__environmentClass_.def("pushEnv",       &JetC__environment::pushEnv );
 JetC__environmentClass_.staticmethod("pushEnv");
 JetC__environmentClass_.def( self_ns::str(self));

 def("makeJetCEnvironment", &makeJetCEnvironment_3 );      

 def("toCmplxEnvironment",&toCmplxEnvironment);      

 // def("toRealEnvironment", &toRealEnvironment);      


}



