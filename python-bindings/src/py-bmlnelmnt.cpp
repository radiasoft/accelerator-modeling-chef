/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-bmlnelmnt.cpp
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
#include <string>
#include <Particle.h>
#include <ParticleBunch.h>
#include <bmlnElmnt.h>
#include <boost/python.hpp>

class PropFunc;

using namespace boost::python;


struct bmlnElmntWrap : bmlnElmnt {
    bmlnElmntWrap(PyObject* self, const char* name = "NONAME", PropFunc* pf=0):
       _self(self),bmlnElmnt(name,pf) {}
    
    bmlnElmntWrap(PyObject* self,   double  length,   double  strength, PropFunc* pf=0):
        _self(self),bmlnElmnt(length,strength,pf) {}
 
    bmlnElmntWrap(PyObject* self, const char* name,   double  length,   PropFunc* pf=0):
        _self(self),bmlnElmnt(name, length,pf) {}

    bmlnElmntWrap(PyObject* self, const char* name,   double  length,   double strength,  PropFunc* pf=0):
        _self(self),bmlnElmnt(name, length, strength, pf) {}

    void   setLength(double);
    void   setStrength(double);
    void   setStrength(double,int);
    void   setCurrent(double);
    void   Rename(const char* name);
    double getReferenceTime() const;
    double setReferenceTime( double );
    double Current();
    const char*  Type() const;
    double OrbitLength( const Particle& );

          PyObject* _self;
    };


void bmlnElmntWrap::setLength(double){ 
call_method<void>(_self, "setLength"); }

void bmlnElmntWrap::setStrength(double){
call_method<void>(_self, "setStrength"); }

void bmlnElmntWrap::setStrength(double,int){
call_method<void>(_self, "setStrength"); }

void bmlnElmntWrap::setCurrent(double){ 
call_method<void>(_self, "setCurrent"); }

void bmlnElmntWrap::Rename(const char* name){ 
call_method<void>(_self, "Rename"); }

double  bmlnElmntWrap::getReferenceTime() const {
return call_method<double>(_self, "getReferenceTime"); }

double  bmlnElmntWrap::setReferenceTime( double ){
return call_method<double>(_self, "setReferenceTime"); }

double  bmlnElmntWrap::Current(){ 
return call_method<double>(_self, "Current"); }

const char*   bmlnElmntWrap::Type() const{
return call_method<const char*>(_self, "Type"); }

double  bmlnElmntWrap::OrbitLength( const Particle& ){ 
return call_method<double>(_self, "OrbitLength"); }



short          (bmlnElmnt::*readTag1)(char*, short, short) const   = &bmlnElmnt::readTag;
short          (bmlnElmnt::*readTag2)(char*) const                 = &bmlnElmnt::readTag;
char           (bmlnElmnt::*readTag3)(short) const                 = &bmlnElmnt::readTag;
std::string    (bmlnElmnt::*readTag4)(short,short) const           = &bmlnElmnt::readTag;
std::string    (bmlnElmnt::*readTag5)() const                      = &bmlnElmnt::readTag;

   // short (bmlnElmnt::*writeTag1)     (char)                                    = &bmlnElmnt::writeTag;
short (bmlnElmnt::*writeTag2)     (char,short)                              = &bmlnElmnt::writeTag;
short (bmlnElmnt::*writeTag3)     (const char*,short)                       = &bmlnElmnt::writeTag;
short (bmlnElmnt::*writeTag4)     (const char*)                             = &bmlnElmnt::writeTag;
short (bmlnElmnt::*writeTag5)     (const std::string&,short)                = &bmlnElmnt::writeTag;
short (bmlnElmnt::*writeTag6)     (const std::string&)                      = &bmlnElmnt::writeTag;


void (bmlnElmnt::*propagateParticle)     (Particle&      ) = &bmlnElmnt::propagate;
void (bmlnElmnt::*propagateJetParticle)  (JetParticle&   ) = &bmlnElmnt::propagate;
void (bmlnElmnt::*propagateParticleBunch)(ParticleBunch& ) = &bmlnElmnt::propagate;

void (bmlnElmnt::*setStrength1)(double      ) = &bmlnElmnt::setStrength;
void (bmlnElmnt::*setStrength2)(double, int ) = &bmlnElmnt::setStrength;

double (bmlnElmnt::*setReferenceTime1)(const Particle&  ) = &bmlnElmnt::setReferenceTime;
double (bmlnElmnt::*setReferenceTime2)(double)            = &bmlnElmnt::setReferenceTime;

void (beamline::*beamline_propagateParticle)     (Particle&      ) = &beamline::propagate;
void (beamline::*beamline_propagateJetParticle)  (JetParticle&   ) = &beamline::propagate;
void (beamline::*beamline_propagateParticleBunch)(ParticleBunch& ) = &beamline::propagate;


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(beamline_overloads1,     countHowMany,       0, 0 );
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(beamline_overloads2,     countHowManyDeeply, 0, 0 );


void wrap_bmlnelmnt() {
  
  class_<bmlnElmnt, bmlnElmntWrap, boost::noncopyable>("bmlnElmnt", no_init)
    .def("propagateParticle",             propagateParticle)
    .def("propagateJetParticle",          propagateJetParticle )
    .def("propagateParticleBunch",        propagateParticleBunch )
       //    .def("writeTag",                 writeTag1)
    .def("writeTag",                 writeTag2)
    .def("writeTag",                 writeTag3)
    .def("writeTag",                 writeTag4)
    .def("writeTag",                 writeTag5)
    .def("writeTag",                 writeTag6)
    .def("readTag1",                 readTag1)
    .def("readTag2",                 readTag2)
    .def("readTag3",                 readTag3)
    .def("readTag4",                 readTag4)
    .def("readTag5",                 readTag5)
    .def("getTagSize",               &bmlnElmnt::getTagSize)
    .def("setShunt",                 &bmlnElmnt::setShunt)
    .def("getShunt",                 &bmlnElmnt::getShunt)
      /****  virtual functions ***/ 
    .def("setLength",                &bmlnElmnt::setLength)
    .def("setStrength",              setStrength1)
    .def("setCurrent",               &bmlnElmnt::setCurrent)
    .def("Rename",                   &bmlnElmnt::Rename)
    .def("getReferenceTime",         &bmlnElmnt::getReferenceTime)
    .def("setReferenceTime",         setReferenceTime1)
    .def("setReferenceTime",         setReferenceTime2)
    .def("Current",                  &bmlnElmnt::Current)
    .def("Type",                     &bmlnElmnt::Type)
    .def("OrbitLength",              &bmlnElmnt::OrbitLength)
    .def_readwrite("dataHook",       &bmlnElmnt::dataHook);
  


     //lattFunc (beamline::*whatIsLattice1)(int   )            = &beamline::whatIsLattice;
     //lattFunc (beamline::*whatIsLattice2)(char* )            = &beamline::whatIsLattice;


int (beamline::*twiss1)( JetParticle&, double, short )            = &beamline::twiss;
int (beamline::*twiss2)( char,JetParticle& )                      = &beamline::twiss;
int (beamline::*twiss3)( lattFunc&, JetParticle&, short)          = &beamline::twiss;

void (beamline::*insert1) ( bmlnElmnt&  )          = &beamline::insert;
void (beamline::*append1) (  bmlnElmnt& )          = &beamline::append;


class_<beamline>("beamline", init<>() )
  .def( init<const char*>() )
  .def( init<const beamline& >())
  .def("propagateParticle",      beamline_propagateParticle)
  .def("propagateJetParticle",   beamline_propagateJetParticle)
  .def("propagateParticleBunch", beamline_propagateParticleBunch)
  .def("zap",    &beamline::zap) 
  .def("clear",  &beamline::clear)
  .def("insert", insert1)
  .def("append", append1 )
     //beamline& operator^( bmlnElmnt& );  // Alters the beamline
     //beamline& operator^( beamline& );
   //beamline& operator+( bmlnElmnt& );  // Does not alter the beamline
   //.def(self + self)
   //.def(self - self)     
     //beamline& operator+( beamline& );
     //beamline& operator-( beamline& );
     //friend beamline& operator-( beamline& );
  //.def("resetGeometry",  &beamline::resetGeometry)
  // PROPAGATE PARTICLES
  .def("setEnergy",      &beamline::setEnergy)
  .def("unTwiss",        &beamline::unTwiss)
  .def("eraseBarnacles", &beamline::eraseBarnacles)
  .def("twiss", twiss1) 
  .def("twiss", twiss2)
  .def("twiss", twiss3)
  // QUERIES
  //.def("whatisLattice",            whatisLattice1) 
  //.def("whatisLattice",            whatisLattice2) 
  .def("countHowMany",             &beamline::countHowMany,       beamline_overloads1())
  .def("countHowManyDeeply",       &beamline::countHowManyDeeply, beamline_overloads2())
  .def("depth",                    &beamline::depth)
     //.def("contains",                 &beamline::contains)
     //.def("firstElement",             &beamline::firstElement)
     //.def("lastElement",              &beamline::lastElement)
  .def("twissIsDone",              &beamline::twissIsDone)
  .def("setTwissIsDone",           &beamline::setTwissIsDone)
  .def("unsetTwissIsDone",         &beamline::unsetTwissIsDone)
  .def("Energy",                   &beamline::Energy)
  .def("OrbitLength",              &beamline::OrbitLength);
 
   //  .def("InsertElementAt",          &beamline::InsertElementAt);
 
  //.def("whatIsRing",               &beamline::whatIsRing)
  // CLONING AND STORING
  //beamline& operator=( const beamline& );
  //.def("flatten",                  &beamline::flatten); returns a ptr, need to specify a policy 
 
     //void   writeLattFunc( );
     //void   writeLattFunc( FILE* );
     //void   writeLattFunc( char* );

     //extern beamline& operator*( int, beamline& );
     //extern beamline& operator*( int, bmlnElmnt& );
     //extern beamline& operator*( beamline&, int );
     //extern beamline& operator*( bmlnElmnt&, int );
     //extern beamline& operator^( bmlnElmnt&, bmlnElmnt& );
     //extern beamline& operator-( beamline& );

#if 0
    class_<lattFunc>("lattFunc", init<>() )
      .def( self_ns::str(self) )
      .def_readwrite("arcLength",    &lattFunc::arcLength)
      .def_readwrite("dispersion",   &lattFunc::dispersion)
      .def_readwrite("dPrime",       &lattFunc::dPrime)
      .def_readwrite("beta",         &lattFunc::beta)
      .def_readwrite("alpha" ,       &lattFunc::alpha)
      .def_readwrite("psi" ,         &lattFunc::psi);
    
#endif

}
