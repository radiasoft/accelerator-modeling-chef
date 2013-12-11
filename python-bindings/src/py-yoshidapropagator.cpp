/*************************************************************************
**************************************************************************
**************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries
******
******
******  File:      py-yoshidapropagator.cpp
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
******  Jun 2013           michelotti@fnal.gov
******  - initial implementation. This is a prototype version, primarily 
******    intended for further testing.  Use cautiously otherwise.
******    Planned future versions will have a different interface
******    (esp. by using templates to expand the application beyond
******    quadrupole to other beamline elements).
******
**************************************************************************
*************************************************************************/


#include <boost/python.hpp>
#include <beamline/quadrupole.h>
#include <beamline/YoshidaPropagator.h>

using namespace boost::python;

void wrap_yoshidapropagator() 
{
  class_<BasePropagator<quadrupole>, boost::noncopyable >("BasePropagator_quadrupole", no_init);
  class_<YoshidaPropagator, bases<BasePropagator<quadrupole> >, YoshidaPropagatorPtr >("YoshidaPropagator", init<int>() );
}

