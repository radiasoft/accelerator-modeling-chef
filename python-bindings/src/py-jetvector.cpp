#include <boost/python.hpp>

#include <JetVector.h>
#include <JetCVector.h>


using namespace boost::python;


void wrap_mxyzptlk_jetvector() {

  class_<JetVector> JetVector_class_ ("JetVector", init<>() );
  JetVector_class_.def( "Dim", &JetVector::Dim);
  JetVector_class_.def( "Env", &JetVector::Env, return_value_policy<return_opaque_pointer>() );
  JetVector_class_.def( "AccuWgt", &JetVector::AccuWgt);
  JetVector_class_.def( "Weight", &JetVector::Weight);

}

void wrap_mxyzptlk_jetvectorc() {

  class_<JetCVector>("JetCVector", init<>() );

}

#if 0
 // Queries ...
  int  Dim() const;
  Jet__environment* Env() const;
  int AccuWgt() const;
  int Weight()  const;
  void standardPart( double* ) const;
  void getReference( double* r ) const;
#endif
