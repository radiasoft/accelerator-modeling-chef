/***************************************************************************                                                               
****************************************************************************                                                               
****************************************************************************                                                               
******
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-beamlinecontext.cpp
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

#include <iostream>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>
#include <beamline/LatticeFunctions.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <basic_toolkit/Matrix.h>
#include <boost/shared_ptr.hpp>

#include <physics_toolkit/BeamlineContext.h>


//----------------------------------------------------------------
// local class and functions
//----------------------------------------------------------------


//----------------------------------------------------------------
// end local class and functions
//----------------------------------------------------------------

void wrap_beamlinecontext() {

using namespace boost::python;

 class_<BeamlineContext, bases<beamline>, boost::noncopyable> ( "BeamlineContext", init<Particle const&, beamline const& >() );

}

