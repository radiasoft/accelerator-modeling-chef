#include <boost/python.hpp>
#include <beamline/RefRegVisitor.h>

void wrap_refregvisitor () {
  using namespace boost::python;
  class_<RefRegVisitor,bases<BmlVisitor> >("RefRegVisitor",init<const Particle&>() );
}
