#include <boost/python.hpp>

#ifdef FNAL_FIRST_ORDER
#include <Jet.h>
#include <JetC.h>
#else
#include <Jet.h>
#endif

using namespace boost::python;


void wrap_mxyzptlk_coord() {

  class_<coord,  bases<Jet>  > ("coord", init<double>() );
  class_<coordC, bases<JetC> > ("coordC", init<std::complex<double> >() );

};
