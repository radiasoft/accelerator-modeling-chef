#include "ioredirector.h"

#include "qtmacrosoff.h"
#include <boost/python.hpp>
#include "qtmacroson.h"

using namespace boost::python;

#define BOOST_PYTHON_STATIC_MODULE


BOOST_PYTHON_MODULE( ioredirector )
{

 class_<QObject, boost::noncopyable> ("QObject", no_init);
  
 class_<IORedirector, bases<QObject>, boost::noncopyable>("IORedirector",init<>())
   .def("read",  &IORedirector::read)
   .def("write", &IORedirector::write);
 

}
