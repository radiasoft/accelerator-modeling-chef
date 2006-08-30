/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-beamline-module.cpp
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
#include <physics_toolkit/LattFuncSage.h>
#include <iostream>

#define BOOST_PYTHON_STATIC_MODULE
#include <boost/python.hpp>
using namespace boost::python;

extern void wrap_aperture();
extern void wrap_bblens();
extern void wrap_beamline();
extern void wrap_bmlnelmnt();
extern void wrap_cf_rbend();
extern void wrap_cf_sbend();
extern void wrap_circuit();
extern void wrap_decapole();
extern void wrap_drift();
extern void wrap_fcircuit();
extern void wrap_icircuit();
extern void wrap_ioredirector();
extern void wrap_kick();
extern void wrap_lambertson();
extern void wrap_marker();
extern void wrap_monitor();
extern void wrap_mover();
extern void wrap_octupole();
extern void wrap_particle();
extern void wrap_quadrupole();
extern void wrap_rbend();
extern void wrap_rfcavity();
extern void wrap_sbend();
extern void wrap_sector();
extern void wrap_septum();
extern void wrap_sextupole();
extern void wrap_slot();
extern void wrap_srot();
extern void wrap_thinpoles();
extern void wrap_beamlineiterator();
extern void wrap_lattfunc();

BOOST_PYTHON_MODULE( beamline )
{

wrap_aperture();
wrap_bblens();
wrap_bmlnelmnt();
wrap_cf_rbend();
wrap_cf_sbend();
   //wrap_circuit();
wrap_decapole();
wrap_drift();
   //wrap_fcircuit();
   //wrap_icircuit();
wrap_kick();
wrap_lambertson();
wrap_marker();
wrap_monitor();
wrap_mover();
wrap_octupole();
wrap_quadrupole();
wrap_rbend();
wrap_rfcavity();
wrap_sbend();
wrap_sector();
wrap_septum();
wrap_sextupole();
wrap_slot();
   //wrap_srot();
wrap_thinpoles();

wrap_particle();
wrap_beamlineiterator();
wrap_lattfunc();

}

