/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-rbend.h
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

#include <beamline/rbend.h>
#include <beamline/Particle.h>

void wrap_rbend () {
  
using namespace boost::python;


  double   (rbend::* setEntryAngle_overload_1)( const Particle&)    = &rbend::setEntryAngle;
  // double   (rbend::* setEntryAngle_overload_2)( const JetParticle&) = &rbend::setEntryAngle;
  double   (rbend::* setEntryAngle_overload_3)( double)             = &rbend::setEntryAngle;

  double   (rbend::* setPoleFaceAngle_overload_1)( const Particle&)     = &rbend::setPoleFaceAngle;
  double   (rbend::* setPoleFaceAngle_overload_2)( const JetParticle&)  = &rbend::setPoleFaceAngle;


class_<rbend, bases<bmlnElmnt> >("rbend", init<double,double>() )
  .def( init<double, double, double>() )
  .def( init<char*, double, double>() )
  .def( init<char*, double, double, double>() )
  .def( "getReferenceTime",        &rbend::getReferenceTime)
  .def( "PoleFaceAngle",           &rbend::PoleFaceAngle)
  .def( "getPoleFaceAngle" ,       &rbend::getPoleFaceAngle)   
  .def( "getTanPoleFaceAngle",     &rbend::getTanPoleFaceAngle)
  .def( "getEntryAngle",           &rbend::getEntryAngle)
  .def( "setPoleFaceAngle",        setPoleFaceAngle_overload_1)
  .def( "setPoleFaceAngle",        setPoleFaceAngle_overload_2)
  .def( "setEntryAngle",           setEntryAngle_overload_1)
  //.def( "setEntryAngle",           setEntryAngle_overload_2)
  .def( "setEntryAngle",           setEntryAngle_overload_3)
  .def( "Type",                    &rbend::Type);

}


