#include <boost/python.hpp>

#include <JetVector.h>
#include <JetCVector.h>
#include <Mapping.h>
#include <MappingC.h>


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

