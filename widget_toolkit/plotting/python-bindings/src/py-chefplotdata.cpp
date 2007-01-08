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
#include <CHEFPlotData.h>
#include <beamline/beamline.h>

using namespace boost::python;


void wrap_chefplot_CHEFPlotData() {


  class_ <CHEFPlotData, boost::shared_ptr<CHEFPlotData> >("CHEFPlotData", init<>() )
    
    .def("addCurve",    &CHEFPlotData::addCurve  )

    .def("setBeamline", &CHEFPlotData::setBeamline )

    //.def("getBeamline", &CHEFPlotData::getBeamline )
    
    .def("setXLabel",   &CHEFPlotData::setXLabel )
    .def("setYLabel",   &CHEFPlotData::setYLabel )

    .def("setScaleMagLeft",   &CHEFPlotData::setScaleMagLeft  )
    .def("setScaleMagRight",  &CHEFPlotData::setScaleMagRight )

    .def("scaleMagLeft",       &CHEFPlotData::scaleMagLeft )
    .def("scaleMagRight",      &CHEFPlotData::scaleMagRight );

    // CHEFCurve* operator[](int i) const;
    
}
