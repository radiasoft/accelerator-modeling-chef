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
#include <EnvPtr.h>
#include <string>
#include <iostream>

static EnvPtr<std::complex<double> >::Type getLastEnvC_local( ) 
{
  return JetC::_lastEnv; 
}

static EnvPtr<double>::Type  getLastEnv_local( ) 
{
  return Jet::_lastEnv; 
}

static void setLastEnvC_local( EnvPtr<std::complex<double> >::Type env ) 
{
  JetC::_lastEnv = env; 
}

static void setLastEnv_local(  EnvPtr<double>::Type env  ) 
{
  Jet::_lastEnv = env; 
}

static TJet<double>  (*log_ptr  )(const TJet<double>& )          = &::log;
static TJet<double>  (*log10_ptr)(const TJet<double>& )          = &::log10;
static TJet<double>  (*sqrt_ptr )(const TJet<double>& )          = &::sqrt;
static TJet<double>  (*powd_ptr )(const TJet<double>&, const double& )  = &::pow;
static TJet<double>  (*pow_ptr  )(const TJet<double>&, int    )  = &::pow;
static TJet<double>  (*exp_ptr  )(const TJet<double>& )          = &::exp;
static TJet<double>  (*sin_ptr  )(const TJet<double>& )          = &::sin;
static TJet<double>  (*cos_ptr  )(const TJet<double>& )          = &::cos;
static TJet<double>  (*sinh_ptr )(const TJet<double>& )          = &::sinh;
static TJet<double>  (*cosh_ptr )(const TJet<double>& )          = &::cosh;
static TJet<double>  (*tanh_ptr )(const TJet<double>& )          = &::tanh;
static TJet<double>  (*asin_ptr )(const TJet<double>& )          = &::asin;
static TJet<double>  (*tan_ptr  )(const TJet<double>& )          = &::tan;
static TJet<double>  (*erf_ptr  )(const TJet<double>& )          = &::erf;
static TJet<double>  (*erfc_ptr )(const TJet<double>& )          = &::erfc;

static TJet<std::complex<double> >  (*clog_ptr  )( const TJet<std::complex<double> >& )         = &::log;
static TJet<std::complex<double> >  (*clog10_ptr)( const TJet<std::complex<double> >& )         = &::log10;
static TJet<std::complex<double> >  (*csqrt_ptr )( const TJet<std::complex<double> >& )         = &::sqrt;
//static TJet<std::complex<double> >  (*cpowd_ptr )( const TJet<std::complex<double> >&, const double& )  = &::pow; //not implemented
//static TJet<std::complex<double> >  (*cpow_ptr  )( const TJet<std::complex<double> >&, int           )  = &::pow; //not implemented
static TJet<std::complex<double> >  (*cexp_ptr  )( const TJet<std::complex<double> >& )         = &::exp;
static TJet<std::complex<double> >  (*csin_ptr  )( const TJet<std::complex<double> >& )         = &::sin;
static TJet<std::complex<double> >  (*ccos_ptr  )( const TJet<std::complex<double> >& )         = &::cos;
static TJet<std::complex<double> >  (*csinh_ptr )( const TJet<std::complex<double> >& )         = &::sinh;
static TJet<std::complex<double> >  (*ccosh_ptr )( const TJet<std::complex<double> >& )         = &::cosh;
static TJet<std::complex<double> >  (*ctanh_ptr )( const TJet<std::complex<double> >& )         = &::tanh;
static TJet<std::complex<double> >  (*casin_ptr )( const TJet<std::complex<double> >& )         = &::asin;
static TJet<std::complex<double> >  (*ctan_ptr  )( const TJet<std::complex<double> >& )         = &::tan;
static TJet<std::complex<double> >  (*cerf_ptr  )( const TJet<std::complex<double> >& )         = &::erf;
static TJet<std::complex<double> >  (*cerfc_ptr )( const TJet<std::complex<double> >& )         = &::erfc;


using namespace boost::python;

void wrap_mxyzptlk_jet() {

  class_<Jet> Jet_class_ ("Jet", init<>());
  Jet_class_.def("getLastEnv",        &::getLastEnv_local ); 
  Jet_class_.staticmethod("getLastEnv");
  Jet_class_.def("setLastEnv",        &::setLastEnv_local );
  Jet_class_.staticmethod("setLastEnv");
  //-----------------------------------
  // *** addition ***
  //-----------------------------------
  Jet_class_.def(self +  self);
  Jet_class_.def(self += self);
  Jet_class_.def(self +  double() );
  Jet_class_.def(self += double() );
  Jet_class_.def(double() + self );
  //-----------------------------------
  // *** subtraction ***
  //-----------------------------------
  Jet_class_.def(self - self);
  Jet_class_.def(self - double());
  Jet_class_.def(double() - self);
  //-----------------------------------
  // *** multiplication ***
  //-----------------------------------
  Jet_class_.def(self * self);
  Jet_class_.def(self * double());
  Jet_class_.def(double() * self);
  //-----------------------------------
  // *** division ****
  //-----------------------------------
  Jet_class_.def(self / self);
  Jet_class_.def(self / double());
  Jet_class_.def(double() / self);
    //-----------------------------------
  // *** boolean ****
  //-----------------------------------
  Jet_class_.def(self == self);   	
  Jet_class_.def(self != self); 	
  //Jet_class_.def(self < self); 	
  //Jet_class_.def(self > self); 	
  //Jet_class_.def(self <= self); 	
  //Jet_class_.def(self >= self); 	
  //----------------------------
  // *** unary ****
  //----------------------------
  Jet_class_.def( -self );  	
  //Jet_class_.def( +self ); 	
  //Jet_class_.del( ~self );  // inverse	
  //Jet_class_.def( !self );  // non-zero	

  //----------------------------
  // *** special methods ****
  //----------------------------
  Jet_class_.def( self_ns::str(self));
  Jet_class_.def( pow( self, int()) ); 
  Jet_class_.def( pow( self, double()) );             
  
  //-----------------------------------
  // *** functions ****
  //-----------------------------------
  def("log",   log_ptr   );
  def("log10", log10_ptr );
  def("sqrt",  sqrt_ptr  );
  def("exp",   sqrt_ptr  );
  def("sin",   sin_ptr   );
  def("cos",   cos_ptr   );
  def("sinh",  sinh_ptr  );
  def("cosh",  cosh_ptr  );
  def("tanh",  tanh_ptr  );
  def("asin",  asin_ptr  );
  def("atan",  asin_ptr  );
  def("erf",   asin_ptr  );
  def("erfc",  asin_ptr  );

}

void wrap_mxyzptlk_jetc() {

  class_<JetC> JetC_class_ ("JetC", init<>());
  JetC_class_.def("getLastEnv",        &::getLastEnvC_local ); 
  JetC_class_.staticmethod("getLastEnv");
  JetC_class_.def("setLastEnv",        &::setLastEnvC_local );
  JetC_class_.staticmethod("setLastEnv");
  //-----------------------------------
  // *** addition ***
  //-----------------------------------
  JetC_class_.def(self +  self);
  JetC_class_.def(self += self);
  JetC_class_.def(self +  std::complex<double>() );
  JetC_class_.def(self += std::complex<double>() );
  JetC_class_.def(std::complex<double>() + self );
  //-----------------------------------
  // *** subtraction ***
  //-----------------------------------
  JetC_class_.def(self - self);
  JetC_class_.def(self - std::complex<double>());
  JetC_class_.def(std::complex<double>() - self);
  //-----------------------------------
  // *** multiplication ***
  //-----------------------------------
  JetC_class_.def(self * self);
  JetC_class_.def(self * std::complex<double>());
  JetC_class_.def(std::complex<double>() * self);
  //-----------------------------------
  // *** division ****
  //-----------------------------------
  JetC_class_.def(self / self);
  JetC_class_.def(self / std::complex<double>());
  JetC_class_.def(std::complex<double>() / self);
  //-----------------------------------
  // *** boolean ****
  //-----------------------------------
  JetC_class_.def(self == self);   	
  JetC_class_.def(self != self); 	
  //JetC_class_.def(self < self); 	
  //JetC_class_.def(self > self); 	
  //JetC_class_.def(self <= self); 	
  //JetC_class_.def(self >= self); 	
  //----------------------------
  // *** unary ****
  //----------------------------
  JetC_class_.def( -self );  	
  //JetC_class_.def( +self ); 	
  //JetC_class_.del( ~self );  // inverse	
  //JetC_class_.def( !self );  // non-zero	

  //----------------------------
  // *** special methods ****
  //----------------------------
  JetC_class_.def( pow( self, int()    )); 
  //JetC_class_.def( pow( self, double() ));              
  //JetC_class_.def(pow( self, std::complex<double>() ));              
  JetC_class_.def(self_ns::str(self)  );             

  //-----------------------------------
  // *** functions ****
  //-----------------------------------
  def("log",   log_ptr   );
  def("log10", log10_ptr );
  def("sqrt",  sqrt_ptr  );
  def("exp",   sqrt_ptr  );
  def("sin",   sin_ptr   );
  def("cos",   cos_ptr   );
  def("sinh",  sinh_ptr  );
  def("cosh",  cosh_ptr  );
  def("tanh",  tanh_ptr  );
  def("asin",  asin_ptr  );
  def("atan",  asin_ptr  );
  def("erf",   asin_ptr  );
  def("erfc",  asin_ptr  );


}

