/*************************************************************************
**************************************************************************
**************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries
******
******
******  File:      py-cf_sbend_madpropagator.cpp
******
******  Copyright (c) Fermi Research Alliance
******                Universities Research Association, Inc.
******                Fermilab
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contracts No. DE-AC02-76CH03000
******  and No. DE-AC02-07CH11359.
******
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******
******  Author:    Leo Michelotti
******             Email: michelotti@fnal.gov
******
******
******  ----------------
******  REVISION HISTORY
******  ----------------
******
******  Jan 2014           michelotti@fnal.gov
******  - initial implementation. This is a prototype version, primarily 
******    intended for further testing.  Use cautiously otherwise.
******    Planned future versions will have a different interface
******    (esp. by using templates to expand the application beyond
******    quadrupole to other beamline elements).
******
**************************************************************************
*************************************************************************/


#include <boost/python.hpp>
#include <beamline/CF_sbend.h>
#include <beamline/CF_sbend_MADPropagator.h>

using namespace boost::python;

void wrap_cf_sbend_madpropagator() 
{
  class_<BasePropagator<CF_sbend>, boost::noncopyable >("BasePropagator_CF_sbend", no_init);
  class_<CF_sbend_MADPropagator, bases<BasePropagator<CF_sbend> >, CF_sbend_MADPropagatorPtr >("CF_sbend_MADPropagator", init<int>() );
}

