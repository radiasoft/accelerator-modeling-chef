/*******************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries
******
******
******  File:      py-slot.cpp
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
******  ----------------
******  REVISION HISTORY
******  ----------------
******  April, 2010            egstern@fnal.gov   (Eric Stern)
******  - added wrappers for methods introduced into class Slot
******    in January, 2010.
******
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <beamline/Slot.h>

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

using namespace boost::python;

  double getMinSplitLength(const Slot& dummy)
    {
      return (Slot::minSplitLength);
    }

  double setMinSplitLength(const Slot& dummy, double mslen)
    {
      Slot::minSplitLength = mslen;
      return (mslen);
    }

  double getMinSplitLengthDefault(const Slot& dummy)
    {
      return (Slot::minSplitLengthDefault);
    }

void wrap_slot () {
  
class_<Slot, bases<BmlnElmnt>, SlotPtr >("Slot")
  .def(init<const char*>())
  .def("makeUpstreamHorizontal",   &Slot::makeUpstreamHorizontal)
  .def("makeDownstreamHorizontal", &Slot::makeDownstreamHorizontal)
  .def("makeUpstreamVertical",     &Slot::makeUpstreamVertical)
  .def("makeDownstreamVertical",   &Slot::makeDownstreamVertical)
  .def( "Type", &Slot::Type)
  .def("getMinSplitLength", getMinSplitLength)
  .def("setMinSplitLength", setMinSplitLength)
  .def("getMinSplitLengthDefault", getMinSplitLengthDefault)
  ;
 
}
