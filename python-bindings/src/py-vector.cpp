#include <boost/python.hpp>
#include <VectorD.h>

using namespace boost::python;

void wrap_vector () {

class_<Vector>("Vector", init<int>() );

}
