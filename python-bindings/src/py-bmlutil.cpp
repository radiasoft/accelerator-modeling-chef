/***************************************************************************
****************************************************************************
****************************************************************************
******
******  Boost.python Python bindings for mxyzpltk/beamline libraries
******
******
******  File:      py-bmlutil.cpp
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
******  Author:    James Amundson
******
******             Fermi National Laboratory, Batavia, IL   60510
******             amundson@fnal.gov
******
****************************************************************************
****************************************************************************
****************************************************************************/
#include <boost/python.hpp>

#include <iostream>
#include <physics_toolkit/BmlUtil.h>
#include <beamline/Particle.h>

static int makeCovariance_default( CovarianceSage::Info& info,  
                             Particle const& particle)
{
    return BmlUtil::makeCovariance(info, particle);
}

void wrap_bmlutil()
{

    using namespace boost::python;

    class_<BmlUtil> BmlUtil_( "BmlUtil", init<>() );
    BmlUtil_.def( "normalize", &BmlUtil::normalize);
    BmlUtil_.def( "makeCovariance", &makeCovariance_default);
    BmlUtil_.staticmethod( "makeCovariance");
    BmlUtil_.def( "cloneLineAndInsert", &BmlUtil::cloneLineAndInsert);
    BmlUtil_.def( "writeAsTransport", &BmlUtil::writeAsTransport);
    BmlUtil_.def( "isSpace", &BmlUtil::isSpace);
    //~ .def( "isKnown", &BmlUtil::isKnown)

}
