#include <boost/python.hpp>

using namespace boost::python;

extern void wrap_barnacle();
extern void wrap_vector();

BOOST_PYTHON_MODULE( basic_toolkit )
{

wrap_barnacle();
wrap_vector(); 

}

