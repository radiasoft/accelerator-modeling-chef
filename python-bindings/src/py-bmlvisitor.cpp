#include <boost/python.hpp>
#include <beamline/BmlVisitor.h>

void wrap_bmlvisitor () {
  using namespace boost::python;
  class_<BmlVisitor, boost::noncopyable >("BmlVisitor",no_init);
}
