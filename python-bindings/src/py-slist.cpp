#include <boost/python.hpp>
#include <basic_toolkit/slist.h>

void wrap_slist () {
  using namespace boost::python;
  class_<slist>("slist")
    .def(init<>() );
}
