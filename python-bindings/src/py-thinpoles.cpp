/*******************************************************************************
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-thinpoles.cpp
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
#include <beamline/thinpoles.h>
#include <beamline/thinMultipole.h>

using namespace boost::python;

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_thinpoles () {

  class_<thin2pole,  bases<bmlnElmnt>, Thin2polePtr >("thin2pole",   init<std::string const&, double const&>() );

  class_<thin12pole, bases<bmlnElmnt>, Thin12polePtr >("thin12pole", init<std::string const&, double const&>() );

  class_<thin14pole, bases<bmlnElmnt>, Thin14polePtr >("thin14pole", init<std::string const&, double const&>() );

  class_<thin16pole, bases<bmlnElmnt>,Thin16polePtr  >("thin16pole", init<std::string const&, double const&>() );

  class_<thin18pole, bases<bmlnElmnt>, Thin18polePtr >("thin18pole", init<std::string const&, double const&>() );

  class_<thinMultipole, bases<bmlnElmnt>, ThinMultipolePtr >("thinMultipole", init<std::string const& > () );
}


