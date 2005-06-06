/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-mapping.cpp
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

#ifdef FNAL_FIRST_ORDER
#include <JetVector.h>
#include <JetCVector.h>
#include <Mapping.h>
#include <MappingC.h>
#else
#include <JetVector.h>
#include <Mapping.h>
#endif

using namespace boost::python;

void wrap_mxyzptlk_mapping() {

  class_<Mapping, bases<JetVector> >("Mapping", init<>() );

}

void wrap_mxyzptlk_mappingc() {

  class_<MappingC, bases<JetCVector> >("MappingC", init<>() );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if  0

class Mapping : public JetVector
{
 private:

  Mapping EpsInverse( Jet__environment* ) const;

 protected:
  // Friends
  // friend class  Jet;
  // friend struct JL;
  // friend class  CJetVector;

 public: 

  Mapping( const int&  /* dimension */   = Jet::_lastEnv->SpaceDim,
           const Jet*  /* components */  = 0, 
                 Jet__environment*       = Jet::_lastEnv 
     );
  Mapping( const Mapping& );
  Mapping( const JetVector& );
  Mapping( char*, 
           Jet__environment* = Jet::_lastEnv );    // Produces the identity.
  ~Mapping();

  Mapping& operator=( const Mapping& );
  Vector  operator()( const Vector& ) const;
  Mapping operator()( const Mapping& ) const;  // Mapping composition.
  Mapping operator* ( const Mapping& ) const;  // Mapping composition also; an alias.

  MatrixD Jacobian() const;
  Jet& operator()( const int& ) const; 

  Mapping Inverse() const;

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

void 
operator*=( Mapping& x, const Mapping& y );


inline 
Jet& 
Mapping::operator()( const int& i ) 
const
{
  return JetVector::operator()( i );   // Regrettably, this is necessary.
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

