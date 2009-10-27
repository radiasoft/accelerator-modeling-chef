/********************************************************************************
*********************************************************************************
*********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-octupole.cpp
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
*********************************************************************************
*********************************************************************************
********************************************************************************/

#include <boost/python.hpp>
#include <beamline/octupole.h>

using namespace boost::python;

//--------------------------------------------------------------------------------
// wrapper code
//--------------------------------------------------------------------------------


void wrap_octupole () {
 
class_<octupole,     bases<BmlnElmnt>,     OctupolePtr >("octupole",     init<std::string const&, double const&, double const&>() );
 
class_<thinOctupole, bases<BmlnElmnt>, ThinOctupolePtr >("thinOctupole", init<std::string const&, double const&>() );
 
}


