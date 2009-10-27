/*******************************************************************************
********************************************************************************
********************************************************************************
******  Python bindings for mxyzpltk/beamline libraries 
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
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <beamline/rbend.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>


using namespace boost::python;


namespace {

double (rbend::* setEntryAngle_double)          (     double   const&  ) =  &rbend::setEntryAngle; 
double (rbend::* setEntryAngle_particle)        (     Particle const&  ) =  &rbend::setEntryAngle; 
double (rbend::*  setExitAngle_double)          (     double   const&  ) =  &rbend::setExitAngle; 
double (rbend::*  setExitAngle_particle)        (     Particle const&  ) =  &rbend::setExitAngle; 
  //double (rbend::* setPoleFaceAngle_double)       (     double   const&  ) =  &rbend::setPoleFaceAngle;
  //double (rbend::* setPoleFaceAngle_particle)     (     Particle const&  ) =  &rbend::setPoleFaceAngle;
  //double (rbend::* setPoleFaceAngle_jetparticle)  (  JetParticle const&  ) =  &rbend::setPoleFaceAngle;

} // anonymous namespace

void wrap_rbend () {
  
using namespace boost::python;

class_<rbend, bases<BmlnElmnt>, RBendPtr >("rbend", init<char const*,    double const&,  double const&,  double const&>() )
  .def(init<char const*,    double const&,  double const&,  double const&,  double const&,  double const&>() )
  .def("getEntryAngle",     &rbend::getEntryAngle        )
  .def("getExitAngle",      &rbend::getExitAngle         )
  .def("setEntryAngle",     setEntryAngle_double         )
  .def("setEntryAngle",     setEntryAngle_particle       )
  .def("setExitAngle",      setExitAngle_double          )
  .def("setExitAngle",      setExitAngle_particle        )
  .def("getEntryFaceAngle", &rbend::getEntryFaceAngle    ) 
  .def("getExitFaceAngle",  &rbend::getExitFaceAngle     ) 
  //.def("setPoleFaceAngle",  setPoleFaceAngle_double      )
  //.def("setPoleFaceAngle",  setPoleFaceAngle_particle    )
  //.def("setPoleFaceAngle",  setPoleFaceAngle_jetparticle )
  .def("hasParallelFaces",  &rbend::hasParallelFaces     )
  .def("hasStandardFaces",  &rbend::hasStandardFaces     );


  //  void accept( BmlVisitor& v );
  //  void accept( ConstBmlVisitor& v ) const;

}
