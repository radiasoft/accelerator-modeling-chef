#include <boost/python.hpp>
#include "CHEFGUI.h"
#include "chefplotmain.h"
#include <qnamespace.h>

typedef uint WFlags;

#define BOOST_PYTHON_STATIC_MODULE

using namespace boost::python;
using namespace std;


class_<CHEFGUI, CHEFGUI*,  boost::noncopyable>*  PythonTypeCHEFGUIPtr = 0;

extern void wrap_chef_CHEFGUI();
extern void wrap_chef_BeamlineBrowser();

BOOST_PYTHON_MODULE( chef )
{

  wrap_chef_CHEFGUI();
  wrap_chef_BeamlineBrowser( );
}



void wrap_chef_BeamlineBrowser( )
{ 
  

}


void wrap_chef_CHEFGUI( )
{ 
  

 static class_<CHEFGUI, CHEFGUI*,  boost::noncopyable> PythonTypeCHEFGUI("CHEFGUI", no_init);
 
 PythonTypeCHEFGUIPtr = &PythonTypeCHEFGUI;


}


