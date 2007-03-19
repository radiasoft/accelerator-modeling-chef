/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
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
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
****************************************************************************/
#include <iostream>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/tuple.hpp>
#include <qnamespace.h>
#include <numarray/numarray.h>
#include <numarray/libnumarray.h>
#include <string>
#include <CurveData.h>
#include "py-exception.h"

using namespace boost::python;

namespace {

class CurveDataWrap: public CurveData 
{

public:

   CurveDataWrap(PyObject* self,  numeric::array& x, numeric::array& y, const char* name);
   CurveDataWrap(PyObject* self,  CurveData const&);

   void setData( numeric::array& x, numeric::array& y, std::string label);


private:

    PyObject* self_; 
};
   
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 CurveDataWrap::CurveDataWrap(PyObject* self,  CurveData const& data)
  : CurveData(data), self_(self) 
 {}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CurveDataWrap::CurveDataWrap(PyObject* self,  numeric::array& x, numeric::array& y, const char* name)
   :   CurveData ( reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( x.ptr())->data ), 
                   reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( y.ptr())->data ), 
                   std::min( x.nelements(), y.nelements() ), name ), self_(self)
{

#if 0 
  std::cout << "x.typecode()  = " << x.typecode()  << std::endl;
  std::cout << "x.type        = " << extract<const char*>( ( x.type()).attr("name") ) << std::endl;
  std::cout << "x.nelements() = " << x.nelements() << std::endl;
#endif 

 if ( !  x.is_c_array() ) 
    throw PyBindingsException("Plot underlying data array must be a c-array.");  

  if ( std::string( extract<const char*>( ( x.type()).attr("name") ) ) != std::string( "Float64") ) 
    throw PyBindingsException("Plot data array must be of type Float64");  

 }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  CurveDataWrap::setData( numeric::array& x,   numeric::array& y,  std::string label)
{

  if ( !  x.is_c_array() ) 
    throw PyBindingsException("Plot underlying data array must be a c-array.");  

  if ( std::string( extract<const char*>( ( x.type()).attr("name") ) ) != std::string( "Float64") ) 
    throw PyBindingsException("Plot data array must be of type Float64.");  


  PyArrayObject* x_paobj = reinterpret_cast<PyArrayObject*>( x.ptr() );
  PyArrayObject* y_paobj = reinterpret_cast<PyArrayObject*>( y.ptr() );

  double *xdata = reinterpret_cast<double*>( x_paobj ->data );
  double *ydata = reinterpret_cast<double*>( y_paobj ->data );

  CurveData( xdata, ydata, std::min( x.nelements(), y.nelements() ) ,  label);  

}

} // anonymous namespace

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void  wrap_CurveData() 
{

  class_ <CurveData, CurveDataWrap >("CurveData", init<numeric::array&, numeric::array&, const char*>() );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

