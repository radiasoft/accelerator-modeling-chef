/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-chefplotmain.cpp
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
#include <CHEFPlotMain.h>
#include <PlotData.h>

using namespace boost::python;
 
#if 0 
==========================================================
class CHEFPlotMainWrapper: public CHEFPlotMain {

public:

   CHEFPlotMainWrapper(PyObject * self): _self(self) {}

   virtual ~CHEFPlotMainWrapper( )
   {  
   

#if 0
   boost::python::handle<> hdl (borrowed( _self) );
   object o(hdl);
   boost::python::extract<boost::shared_ptr<CHEFPlotMainWrapper> > x (o);    

   if ( x.check() ) {
       std::auto_ptr<CHEFPlotMainWrapper> ap = x();
       ap.release();
       }
#endif

   }

   private:
  
   PyObject* _self;


};
=====================================================================================
#endif

namespace {

void  (CHEFPlotMain::*resize_int_int)(int, int)    = &CHEFPlotMain::resize;

}


void wrap_chefplot_CHEFPlotMain( )
{ 

  
   class_ <CHEFPlotMain, boost::shared_ptr<CHEFPlotMain>, boost::noncopyable> ("CHEFPlotMain", init<>( ) )
     .def("addData",  &CHEFPlotMain::addData)
     .def("show"   ,  &CHEFPlotMain::show)
     .def("hide"   ,  &CHEFPlotMain::hide)
     .def("resize" ,  resize_int_int)
 ;

}

