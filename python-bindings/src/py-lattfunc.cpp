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
#include <beamline/BmlnElmnt.h>
#include <beamline/LatticeFunctions.h>
#include <iostream>
#include <iomanip>
#include <boost/any.hpp>

using namespace boost::python;

namespace {

void verbose_hv_output(std::ostream &os, double h, double v) 
{
   os << std::setiosflags(std::ios::fixed | std::ios::right);
   os << std::setprecision(8);
  
   os << "(hor = "  << std::setw(15) << h << "," << " ver = " << std::setw(15) << v << ")" << std::endl;
   os << resetiosflags(std::ios::fixed | std::ios::right);
}

#if 0
std::ostream& operator<<(std::ostream &os,  CSLattFuncs::dispersion_t const& disp)    
{
      verbose_hv_output(os, disp.hor, disp.ver);
      return os;
}

std::ostream& operator<<(std::ostream &os,  CSLattFuncs::dPrime_t const&   dPrime)  
{
      verbose_hv_output(os, dPrime.hor, dPrime.ver);
      return os;
}

#endif

std::ostream& operator<<(std::ostream &os,  CSLattFuncs::beta_t const&  beta)    
{
      verbose_hv_output(os, beta.hor, beta.ver);
      return os;
}

std::ostream& operator<<(std::ostream &os,  CSLattFuncs::alpha_t const&      alpha)   
{
      verbose_hv_output(os, alpha.hor, alpha.ver);
      return os;
}

std::ostream& operator<<(std::ostream &os,  CSLattFuncs::psi_t   const&        psi)     
{
      verbose_hv_output(os, psi.hor, psi.ver);
      return os;
}



}// anonymous namespace 


static std::ostream& operator<<(std::ostream &os,  CSLattFuncs const& lf) 
{
  os << "arc length = " << lf.arcLength  << std::endl
     << "beta       = " << lf.beta       << std::endl
     << "alpha      = " << lf.alpha      << std::endl
     << "psi        = " << lf.psi        << std::endl;

      return os;
}


void wrap_lattfunc() {

class_<CSLattFuncs>("LattFunc",init<>())
  .def( self_ns::str(self) )
  .def_readwrite("arcLength",  &CSLattFuncs::arcLength)
  .def_readwrite("beta",       &CSLattFuncs::beta)
  .def_readwrite("alpha",      &CSLattFuncs::alpha)
  .def_readwrite("psi",        &CSLattFuncs::psi);

class_<dispersion_t>("dispersion");

class_<CSLattFuncs::beta_t>("beta")
  .def_readwrite("hor", &CSLattFuncs::beta_t::hor )
  .def_readwrite("ver", &CSLattFuncs::beta_t::ver );

class_<CSLattFuncs::alpha_t>("alpha")
  .def_readwrite("hor", &CSLattFuncs::alpha_t::hor)
  .def_readwrite("ver", &CSLattFuncs::alpha_t::ver);

class_<CSLattFuncs::psi_t>("psi")
  .def_readwrite("hor", &CSLattFuncs::psi_t::hor)
  .def_readwrite("ver", &CSLattFuncs::psi_t::ver);


}
