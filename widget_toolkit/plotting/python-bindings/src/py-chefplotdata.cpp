/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-chefplotdata.cpp
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
#include <PlotData.h>
#include <beamline/beamline.h>

using namespace boost::python;


void wrap_chefplot_PlotData() {


  class_ <PlotData>("PlotData", init<>() )
    
    .def("addCurve",           &PlotData::addCurve  )
    .def("setBeamline",        &PlotData::setBeamline )
    .def("setXLabel",          &PlotData::setXLabel )
    .def("setYLabel",          &PlotData::setYLabel );

    //.def("getBeamline", &PlotData::getBeamline )
    //.def("setScaleMagLeft",    &PlotData::setScaleMagLeft  )
    //.def("setScaleMagRight",   &PlotData::setScaleMagRight )
    //.def("scaleMagLeft",       &PlotData::scaleMagLeft )
    //.def("scaleMagRight",      &PlotData::scaleMagRight );
    // CHEFCurve* operator[](int i) const;
    ;
}
