/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-chefplot-module.cpp
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

#define BOOST_PYTHON_STATIC_MODULE

using namespace boost::python;
using namespace std;

extern  void wrap_chefplot_CHEFPlotMain( );
extern  void wrap_CurveData();
extern  void wrap_chefplot_PlotData();

BOOST_PYTHON_MODULE(chefplot)
{

  wrap_chefplot_CHEFPlotMain( );
  wrap_CurveData();
  wrap_chefplot_PlotData();

}

