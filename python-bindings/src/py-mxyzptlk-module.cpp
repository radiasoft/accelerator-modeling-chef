#include <boost/python.hpp>

using namespace boost::python;


extern void wrap_mxyzptlk_jet();
extern void wrap_mxyzptlk_jetc();
extern void wrap_mxyzptlk_jetvector();
extern void wrap_mxyzptlk_jetvectorc();
extern void wrap_mxyzptlk_mapping();
extern void wrap_mxyzptlk_mappingc();
extern void wrap_mxyzptlk_coord();

BOOST_PYTHON_MODULE(mxyzptlk)
{

  wrap_mxyzptlk_jet();
  wrap_mxyzptlk_jetc();
  wrap_mxyzptlk_jetvector();
  wrap_mxyzptlk_jetvectorc();
  wrap_mxyzptlk_mapping();
  wrap_mxyzptlk_mappingc();
  wrap_mxyzptlk_coord();

}    


