/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
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
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <string>
#include <iostream>
#include <mxyzptlk/Jet.h>
#include <mxyzptlk/EnvPtr.h>

namespace {

TJet<double>  (*log_ptr  )(const TJet<double>& )          = &::log;
TJet<double>  (*log10_ptr)(const TJet<double>& )          = &::log10;
TJet<double>  (*sqrt_ptr )(const TJet<double>& )          = &::sqrt;
TJet<double>  (*powd_ptr )(const TJet<double>&, const double& )  = &::pow;
TJet<double>  (*pow_ptr  )(const TJet<double>&, int    )  = &::pow;
TJet<double>  (*exp_ptr  )(const TJet<double>& )          = &::exp;
TJet<double>  (*sin_ptr  )(const TJet<double>& )          = &::sin;
TJet<double>  (*cos_ptr  )(const TJet<double>& )          = &::cos;
TJet<double>  (*sinh_ptr )(const TJet<double>& )          = &::sinh;
TJet<double>  (*cosh_ptr )(const TJet<double>& )          = &::cosh;
TJet<double>  (*tanh_ptr )(const TJet<double>& )          = &::tanh;
TJet<double>  (*asin_ptr )(const TJet<double>& )          = &::asin;
TJet<double>  (*tan_ptr  )(const TJet<double>& )          = &::tan;
TJet<double>  (*atan_ptr )(const TJet<double>& )          = &::atan;
TJet<double>  (*erf_ptr  )(const TJet<double>& )          = &::erf;  
//TJet<double>  (*erfc_ptr )(const TJet<double>& )          = &::erfc;  Not implemented 

 TJet<std::complex<double> >  (*clog_ptr  )( const TJet<std::complex<double> >& )         = &::log;
 TJet<std::complex<double> >  (*clog10_ptr)( const TJet<std::complex<double> >& )         = &::log10;
 TJet<std::complex<double> >  (*csqrt_ptr )( const TJet<std::complex<double> >& )         = &::sqrt;
// TJet<std::complex<double> >  (*cpowd_ptr )( const TJet<std::complex<double> >&, const double& )  = &::pow; //not implemented
// TJet<std::complex<double> >  (*cpow_ptr  )( const TJet<std::complex<double> >&, int           )  = &::pow; //not implemented
 TJet<std::complex<double> >  (*cexp_ptr  )( const TJet<std::complex<double> >& )         = &::exp;
 TJet<std::complex<double> >  (*csin_ptr  )( const TJet<std::complex<double> >& )         = &::sin;
 TJet<std::complex<double> >  (*ccos_ptr  )( const TJet<std::complex<double> >& )         = &::cos;
 TJet<std::complex<double> >  (*csinh_ptr )( const TJet<std::complex<double> >& )         = &::sinh;
 TJet<std::complex<double> >  (*ccosh_ptr )( const TJet<std::complex<double> >& )         = &::cosh;
 TJet<std::complex<double> >  (*ctanh_ptr )( const TJet<std::complex<double> >& )         = &::tanh;
 TJet<std::complex<double> >  (*casin_ptr )( const TJet<std::complex<double> >& )         = &::asin;
 TJet<std::complex<double> >  (*ctan_ptr  )( const TJet<std::complex<double> >& )         = &::tan;
 TJet<std::complex<double> >  (*catan_ptr )( const TJet<std::complex<double> >& )         = &::atan;
 TJet<std::complex<double> >  (*cerf_ptr  )( const TJet<std::complex<double> >& )         = &::erf;
//TJet<std::complex<double> >  (*cerfc_ptr )( const TJet<std::complex<double> >& )         = &::erfc; Not implemented

} // anonymous namespace

using namespace boost::python;

void wrap_mxyzptlk_jet() {

  class_<Jet> Jet_class_ ("Jet", init<>());
  Jet_class_.def( init<double>());
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
  def("atan",  atan_ptr  );
  def("erf",   erf_ptr   );
  // def("erfc",  erfc_ptr  );

}

void wrap_mxyzptlk_jetc() {

  class_<JetC> JetC_class_ ("JetC", init<>());
  JetC_class_.def( init<std::complex<double> >());
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
  def("log",   clog_ptr   );
  def("log10", clog10_ptr );
  def("sqrt",  csqrt_ptr  );
  def("exp",   csqrt_ptr  );
  def("sin",   csin_ptr   );
  def("cos",   ccos_ptr   );
  def("sinh",  csinh_ptr  );
  def("cosh",  ccosh_ptr  );
  def("tanh",  ctanh_ptr  );
  def("asin",  casin_ptr  );
  def("atan",  catan_ptr  );
  def("erf",   cerf_ptr   );
  //def("erfc",  cerfc_ptr  );


}

