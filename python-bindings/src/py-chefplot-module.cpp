#include <boost/python.hpp>

#define BOOST_PYTHON_STATIC_MODULE

using namespace boost::python;
using namespace std;

extern  void wrap_chefplot_CHEFPlotMain( );
extern  void wrap_chefplot_CHEFCurve();
extern  void wrap_chefplot_CHEFPlotData();

BOOST_PYTHON_MODULE(chefplot)
{

  wrap_chefplot_CHEFPlotMain( );
  wrap_chefplot_CHEFCurve();
  wrap_chefplot_CHEFPlotData();

}

