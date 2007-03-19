/***************************************************************************                                                               
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
****************************************************************************/
#include <boost/python.hpp>

#include <beamline/bmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <physics_toolkit/LattFuncSage.h>
#include <iostream>

#define BOOST_PYTHON_STATIC_MODULE


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
 

int (LattFuncSage::* Twiss_Calc1)(JetParticle&)    = & LattFuncSage::Twiss_Calc;


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LattFuncSage_Disp_Calc_overloads,    Disp_Calc,    1, 1) 
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LattFuncSage_Fast_CS_Calc_overloads, Fast_CS_Calc, 1, 1) 
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LattFuncSage_Slow_CS_Calc_overloads, Slow_CS_Calc, 1, 1) 

     // BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LattFuncSage_ET_Disp_Calc_overloads,ET_Disp_Calc, 1, 1) 

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



 class_<LattFuncSage,  boost::noncopyable>("LattFuncSage", init< BmlPtr >())
   .def(init<beamline const&>())
   .def("TuneCalc",        &LattFuncSage::TuneCalc)
   .def("Disp_Calc",       &LattFuncSage::Disp_Calc,   LattFuncSage_Disp_Calc_overloads() ) // ( JetParticle*, Sage::CRITFUNC = 0 )
      //.def("Disp_Calc_old"  ( JetParticle*, Sage::CRITFUNC = 0 )
      //.def("NewDisp_Calc"   ( JetParticle*, bool onClosedOrbit = false )
   .def("Fast_CS_Calc",   &LattFuncSage::Fast_CS_Calc, LattFuncSage_Fast_CS_Calc_overloads() )//  ( /* const */ JetParticle*, Sage::CRITFUNC = 0 )
   .def("Slow_CS_Calc",   &LattFuncSage::Slow_CS_Calc, LattFuncSage_Slow_CS_Calc_overloads())  //  ( /* const */ JetParticle*, Sage::CRITFUNC = 0 )
      //.def("NewSlow_CS_Calc"( /* const */ JetParticle*, Sage::CRITFUNC = 0 )
      ///.def("ET_Disp_Calc",  &LattFuncSage::ET_Disp_Calc, LattFuncSage_ET_Disp_Calc_overloads() )        // (             JetParticle*, Sage::CRITFUNC = 0 )
      ///.def("CS_Disp_Calc",  &LattFuncSage::CS_Disp_Calc)  //  (             JetParticle*, Sage::CRITFUNC = 0 )
      //.def("FAD_Disp_Calc"  ( /* const */ JetParticle*, Sage::CRITFUNC = 0 )
      //.def("Twiss_Calc",  &LattFuncSage::Twiss_Calc)
      //.def("Twiss_Calc",  &LattFuncSage::Twiss_Calc)
     .def("eraseAll",  &LattFuncSage::eraseAll)
     .def("set_dpp",   &LattFuncSage::set_dpp)
     .def("get_dpp",   &LattFuncSage::get_dpp);
 
}


