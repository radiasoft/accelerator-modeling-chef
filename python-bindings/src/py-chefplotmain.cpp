#include <boost/python.hpp>
#include <chefplotmain.h>

using namespace boost::python;
 
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

void wrap_chefplot_CHEFPlotMain( )
{ 

   void  (CHEFPlotMain::*resize_int_int)(int, int)    = &CHEFPlotMain::resize;
  
   class_ <CHEFPlotMain, boost::shared_ptr<CHEFPlotMainWrapper>, boost::noncopyable> ("CHEFPlotMain", init<>( ) )
     .def("addData",  &CHEFPlotMain::addData)
     .def("show"   ,  &CHEFPlotMain::show)
     .def("hide"   ,  &CHEFPlotMain::hide)
     .def("resize" ,  resize_int_int)
 ;

}

