/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-chef-module.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
****************************************************************************/
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


