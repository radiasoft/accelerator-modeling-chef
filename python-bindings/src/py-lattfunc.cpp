/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-lattfunc.cpp
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
*******************************************************************************
*******************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <beamline/bmlnElmnt.h>
#include <physics_toolkit/LattFuncSage.h>
#include <physics_toolkit/EdwardsTengSage.h>
#include <physics_toolkit/LBSage.h>
#include <physics_toolkit/CovarianceSage.h>
#include <physics_toolkit/DispersionSage.h>
#include <iostream>

#define BOOST_PYTHON_STATIC_MODULE

namespace {

void verbose_hv_output(std::ostream &os, double h, double v) 
{
   os << std::setiosflags(std::ios::fixed | std::ios::right);
   os << std::setprecision(8);
  
   os << "(hor = "  << std::setw(15) << h << "," << " ver = " << std::setw(15) << v << ")" << std::endl;
   os << resetiosflags(std::ios::fixed | std::ios::right);
}

std::ostream& operator<<(std::ostream &os,  const LattFuncSage::lattFunc::dispersion_type& disp)    
{
      verbose_hv_output(os, disp.hor, disp.ver);
      return os;
}

std::ostream& operator<<(std::ostream &os,  const LattFuncSage::lattFunc::dPrime_type&     dPrime)  
{
      verbose_hv_output(os, dPrime.hor, dPrime.ver);
      return os;
}

std::ostream& operator<<(std::ostream &os,  const LattFuncSage::lattFunc::beta_type&       beta)    {
      verbose_hv_output(os, beta.hor, beta.ver);
      return os;
}

std::ostream& operator<<(std::ostream &os,  const LattFuncSage::lattFunc::alpha_type&      alpha)   
{
      verbose_hv_output(os, alpha.hor, alpha.ver);
      return os;
}

std::ostream& operator<<(std::ostream &os,  const LattFuncSage::lattFunc::psi_type&        psi)     
{
      verbose_hv_output(os, psi.hor, psi.ver);
      return os;
}



}// anonymous namespace 


static std::ostream& operator<<(std::ostream &os,  LattFuncSage::lattFunc const& lf) 
{
  os << "arc length = " << lf.arcLength  << std::endl
     << "dispersion = " << lf.dispersion << std::endl  
     << "dPrime     = " << lf.dPrime     << std::endl
     << "beta       = " << lf.beta       << std::endl
     << "alpha      = " << lf.alpha      << std::endl
     << "psi        = " << lf.psi        << std::endl;

      return os;
}

//////////////////////////////////////////////////////////////////////////
//                                                                      // 
// NOTE: The class LattFuncSage::lattFunc is wrapped here and put into  //
//       the beamline module because is is used by the barnacle class,  //
//       which is part of basic_toolkit.                                //
//                                                                      //
//       Not doing this would force one to import the physics_toolkit   //
//       module before the beamline module                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <boost/any.hpp>

using namespace boost::python;

void wrap_lattfunc() {

class_<LattFuncSage::lattFunc >("lattFunc",init<>())
  .def( self_ns::str(self) )
  .def_readwrite("arcLength",  &LattFuncSage::lattFunc::arcLength)
  .def_readwrite("dispersion", &LattFuncSage::lattFunc::dispersion)
  .def_readwrite("dPrime",     &LattFuncSage::lattFunc::dPrime)
  .def_readwrite("beta",       &LattFuncSage::lattFunc::beta)
  .def_readwrite("alpha",      &LattFuncSage::lattFunc::alpha)
  .def_readwrite("psi",        &LattFuncSage::lattFunc::psi);

class_<LattFuncSage::lattFunc::dispersion_type>("dispersion")
  .def_readwrite("hor", &LattFuncSage::lattFunc::dispersion_type::hor)
  .def_readwrite("ver", &LattFuncSage::lattFunc::dispersion_type::ver);

class_<LattFuncSage::lattFunc::dPrime_type>("dPrime")
  .def_readwrite("hor", &LattFuncSage::lattFunc::dPrime_type::hor)
  .def_readwrite("ver", &LattFuncSage::lattFunc::dPrime_type::ver);

class_<LattFuncSage::lattFunc::beta_type>("beta")
  .def_readwrite("hor", &LattFuncSage::lattFunc::beta_type::hor )
  .def_readwrite("ver", &LattFuncSage::lattFunc::beta_type::ver );

class_<LattFuncSage::lattFunc::alpha_type>("alpha")
  .def_readwrite("hor", &LattFuncSage::lattFunc::alpha_type::hor)
  .def_readwrite("ver", &LattFuncSage::lattFunc::alpha_type::ver);

class_<LattFuncSage::lattFunc::psi_type>("psi")
  .def_readwrite("hor", &LattFuncSage::lattFunc::psi_type::hor)
  .def_readwrite("ver", &LattFuncSage::lattFunc::psi_type::ver);


class_<EdwardsTengSage::Info>("ETFunc",init<>())
  ;

class_<LBSage::Info>("LBFunc",init<>())
  ;

class_<CovarianceSage::Info >("CovFunc",init<>())
    .def_readwrite("arcLength",&CovarianceSage::Info::arcLength)
    .def_readwrite("covariance",&CovarianceSage::Info::covariance)
    .def_readwrite("beta",&CovarianceSage::Info::beta)
    .def_readwrite("alpha",&CovarianceSage::Info::alpha)
  ;

class_<CovarianceSage::Info::beta_type>("beta")
  .def_readwrite("hor", &CovarianceSage::Info::beta_type::hor )
  .def_readwrite("ver", &CovarianceSage::Info::beta_type::ver );

class_<CovarianceSage::Info::alpha_type>("alpha")
  .def_readwrite("hor", &CovarianceSage::Info::alpha_type::hor)
  .def_readwrite("ver", &CovarianceSage::Info::alpha_type::ver);

class_<DispersionSage::Info >("DispFunc",init<>())
  ;

}
