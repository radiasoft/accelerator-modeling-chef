/*************************************************************************** 
**************************************************************************** 
**************************************************************************** 
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-lattfuncsage.cpp
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
**************************************************************************** 
**************************************************************************** 
****************************************************************************/

#include <boost/python.hpp>

#include <beamline/bmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <sqlite/connection.hpp>
#include <iostream>


#if 0 

static void verbose_hv_output(std::ostream &os, double h, double v) {
   os << std::setiosflags(std::ios::fixed | std::ios::right);
   os << std::setprecision(8);
  
   os << "(hor = "  << std::setw(15) << h << "," << " ver = " << std::setw(15) << v << ")" << std::endl;
   os << resetiosflags(std::ios::fixed | std::ios::right);
   
}


static std::ostream& operator<<(std::ostream &os,  
                         const LattFuncSage::lattRing::tune_type& tune) {

  verbose_hv_output(os, tune.hor, tune.ver);
  return os;
      
}

static std::ostream& operator<<(std::ostream &os,  
                         const LattFuncSage::lattRing::chromaticity_type& chroma) {

      verbose_hv_output(os, chroma.hor, chroma.ver);
      return os;
}



static std::ostream& operator<<(std::ostream &os,  const LattFuncSage::lattRing& lr) {
  
  os << lr.tune         << std::endl
     << lr.chromaticity << std::endl;

      return os;
}
 

// int (LattFuncSage::* Twiss_Calc1)(JetParticle&)    = & LattFuncSage::Twiss_Calc;


void wrap_lattfuncsage() {

using namespace boost::python;


class_<LattFuncSage::lattRing>("lattRing")
      .def( self_ns::str(self) )
      .def_readwrite( "tune",          &LattFuncSage::lattRing::tune)
      .def_readwrite( "chromaticity",  &LattFuncSage::lattRing::chromaticity);
 
class_<LattFuncSage::lattRing::tune_type>("tune")
      .def_readwrite( "hor", &LattFuncSage::lattRing::tune_type::hor)
      .def_readwrite( "ver", &LattFuncSage::lattRing::tune_type::ver);

 
class_<LattFuncSage::lattRing::chromaticity_type>("chromaticity")
  .def_readwrite("hor", &LattFuncSage::lattRing::chromaticity_type::hor)
  .def_readwrite("ver", &LattFuncSage::lattRing::chromaticity_type::ver);



 class_<LattFuncSage,  boost::noncopyable>("LattFuncSage", init< BmlPtr, sqlite::connection& >())
   .def(init<beamline const&, sqlite::connection& >())
   .def("TuneCalc",   &LattFuncSage::TuneCalc)
   .def("eraseAll",   &LattFuncSage::eraseAll);
 
}

#endif

// placeholder 

void wrap_lattfuncsage() { }
