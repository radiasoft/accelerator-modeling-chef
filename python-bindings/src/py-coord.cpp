#include <boost/python.hpp>

#include <Jet.h>
#include <JetC.h>

using namespace boost::python;


void wrap_mxyzptlk_coord() {

  class_<coord,  bases<Jet>  > ("coord", init<double>() );
  class_<coordC, bases<JetC> > ("coordC", init<std::complex<double> >() );

};
