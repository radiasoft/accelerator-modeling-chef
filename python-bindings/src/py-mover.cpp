#include <boost/python.hpp>

#include <mover.h>


void wrap_mover () {
  

using namespace boost::python;


class_<mover>("mover")
  .def(init<const char*>())
  .def( "switchOn",   &mover::switchOn)
  .def( "switchOff",  &mover::switchOff);
 
}

#if 0

class mover : public circuit {
protected:
  alignmentData align;
public:
  mover();
  ~mover();
  mover( const char* );
  mover( bmlnElmnt* );
  mover( const char*, bmlnElmnt* );
  void append( bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );

};

#endif // MOVER_H
