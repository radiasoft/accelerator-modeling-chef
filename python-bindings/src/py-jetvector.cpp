#include <boost/python.hpp>

#ifdef FNAL_FIRST_ORDER
#include <JetVector.h>
#include <JetCVector.h>
#else
#include <JetVector.h>
#endif

//  TJetVector<T1,T2>::TJetVector<T1,T2>( int n,  const TJet<T1,T2>* x, const TJetEnvironment<T1,T2>* pje )


using namespace boost::python;


void wrap_mxyzptlk_jetvector() {

  class_<JetVector> JetVector_class_ ("JetVector", init<>() );
  JetVector_class_.def( "Dim",     &JetVector::Dim);
  JetVector_class_.def( "Env",     &JetVector::Env, return_value_policy<reference_existing_object>() );
  JetVector_class_.def( "AccuWgt", &JetVector::AccuWgt);
  JetVector_class_.def( "Weight",  &JetVector::Weight);

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
