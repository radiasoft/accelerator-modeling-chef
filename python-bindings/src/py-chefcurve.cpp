#include <iostream>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/tuple.hpp>
#include <qnamespace.h>
#include <qwt/qwt_curve.h>
#include <numarray/numarray.h>
#include <numarray/libnumarray.h>
#include <string>
#include "chefplotdata.h"
#include "py-exception.h"

using namespace boost::python;


class CHEFCurveWrap: public CHEFCurve 
{

public:

  CHEFCurveWrap(PyObject* self,  numeric::array& x, numeric::array& y, const char* name);
  void setData( numeric::array& x, numeric::array& y);

private:

    PyObject* _self; 
};
   

CHEFCurveWrap::CHEFCurveWrap(PyObject* self,  numeric::array& x, numeric::array& y, const char* name): 
_self(self), CHEFCurve( CurveData ( reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( x.ptr())->data ), 
                                    reinterpret_cast<double*>( reinterpret_cast<PyArrayObject*>( y.ptr())->data ), 
                                    std::min( x.nelements(), y.nelements() )),  
                         name)
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


void 
CHEFCurveWrap::setData( numeric::array& x,   numeric::array& y)
{

  if ( !  x.is_c_array() ) 
    throw PyBindingsException("Plot underlying data array must be a c-array.");  

  if ( std::string( extract<const char*>( ( x.type()).attr("name") ) ) != std::string( "Float64") ) 
    throw PyBindingsException("Plot data array must be of type Float64.");  


  PyArrayObject* x_paobj = reinterpret_cast<PyArrayObject*>( x.ptr() );
  PyArrayObject* y_paobj = reinterpret_cast<PyArrayObject*>( y.ptr() );

  double *xdata = reinterpret_cast<double*>( x_paobj ->data );
  double *ydata = reinterpret_cast<double*>( y_paobj ->data );

  CHEFCurve::setData( CurveData( xdata, ydata, std::min( x.nelements(), y.nelements() ) ) );  

}

void  wrap_chefplot_CHEFCurve() {

 
  class_ <CHEFCurve, boost::shared_ptr<CHEFCurveWrap>,  boost::noncopyable>("CHEFCurve", init<numeric::array&, numeric::array&, const char*>() )
  .def("setData",              &CHEFCurveWrap::setData )
    //  .def("widgetDeletesCurve",   &CHEFCurveWrap::widgetDeletesCurve)
     ;
         
  //class_ <CHEFCurve, CHEFCurveWrap, boost::noncopyable>("CHEFCurve", no_init );
    //.def("scalemag", &CHEFCurve::scalemag )
    // .def("setData",  &QwtCurve::setData );


}


