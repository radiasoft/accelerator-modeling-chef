/***************************************************************************
****************************************************************************
****************************************************************************
******  
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-chefcurve.cpp
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
******             ostiguy@fnal.gov                         
******                                                                
******             Fermi National Laboratory
******                    Batavia, IL
******
**************************************************************************** 
**************************************************************************** 
****************************************************************************/

#include <iostream>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/tuple.hpp>
#include <qnamespace.h>

#if 1
#include <numarray/numarray.h>
#include <numarray/libnumarray.h>
#endif

#if 0
#include "/usr/lib/python2.5/site-packages/numpy/numarray/numpy/libnumarray.h"
#endif

#include <string>
#include <CurveData.h>
#include "py-exception.h"

using namespace boost::python;

namespace {

static CurveData* makeCurveData ( numeric::array& x, numeric::array& y, const char* name )
{

 unsigned int size = std::min( x.nelements(), y.nelements() );
  
 CurveData* cd = new CurveData( size, name );

 double* xa = reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( x.ptr())->data ); 
 double* ya = reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( y.ptr())->data ); 
  
 CurveData::iterator it = cd->begin();
 for (unsigned int i=0; i< cd->size(); ++i ) { *(it++) = CurveData::Point( *(xa++), *(ya++) ); }  

#if 0 
  std::cout << "x.typecode()  = " << x.typecode()  << std::endl;
  std::cout << "x.type        = " << extract<const char*>( ( x.type()).attr("name") ) << std::endl;
  std::cout << "x.nelements() = " << x.nelements() << std::endl;
#endif 

 if ( !  x.is_c_array() ) 
    throw PyBindingsException("Plot underlying data array must be a c-array.");  

  if ( std::string( extract<const char*>( ( x.type()).attr("name") ) ) != std::string( "Float64") ) 
    throw PyBindingsException("Plot data array must be of type Float64");  

 return cd;

}

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  wrap_CurveData() 
{

  class_ <CurveData>("CurveData", no_init )
  .def("__init__", make_constructor( makeCurveData ));

}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

