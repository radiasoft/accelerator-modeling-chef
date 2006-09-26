#include <boost/python.hpp>

#include <beamline/beamline.h>
#include <beamline/InsertionList.h>

void wrap_insertionlistelement () {
  using namespace boost::python;
  class_<InsertionListElement>("InsertionListElement")
    .def(init<double,bmlnElmnt*>() );
}

void (InsertionList::*Append_ileref)(const InsertionListElement&) = &InsertionList::Append;

void wrap_insertionlist () {
  using namespace boost::python;
  class_<InsertionList>("InsertionList")
    .def(init<double>() )
    .def("Append", Append_ileref);
}
