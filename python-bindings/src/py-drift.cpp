#include <boost/python.hpp>
#include <bmlnElmnt.h>
#include <drift.h>

using namespace boost::python;

void wrap_drift () {

class_<drift, bases<bmlnElmnt> >("drift", init<>())
  .def(init<double>() )
  .def(init<char*>() )
  .def(init<char*, double>() );
 
}


#if 0
class drift : public bmlnElmnt
{
public:
  drift();
  drift( double );             // length of drift in meters
  drift( char* );              // name
  drift( char*,                // name
         double );             // length of drift in meters
  drift( bmlnElmntData& );
  drift( const drift& );
  ~drift();

  void accept( BmlVisitor& v ) { v.visitDrift( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitDrift( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "drift") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new drift( *this ); }
} ;

#endif 

