#include <boost/python.hpp>

using namespace boost::python;

extern void wrap_lattfuncsage();
extern void wrap_beamlinecontext();

BOOST_PYTHON_MODULE( physics_toolkit )
{

wrap_lattfuncsage();
wrap_beamlinecontext();
 

}

