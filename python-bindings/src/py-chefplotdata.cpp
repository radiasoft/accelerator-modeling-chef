#include <boost/python.hpp>
#include <chefplotdata.h>
#include <beamline.h>

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
