#include <Aperture.h>
#include <boost/python.hpp>

void wrap_aperture () {
  
using namespace boost::python;

   /** this does nothing ***/
   /** Aperture is currently a pure virtual class ***/

#if 0

class_<Aperture>("Aperture")
  .def( "Type", &Aperture::Type );
 
#endif
}



