/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                   
******  File:      py-jetvector.cpp
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

#include <JetVector.h>

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
