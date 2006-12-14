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
#include <boost/python.hpp>
#include <string>

#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>
#include <beamline/beamline.h>
#include <beamline/InsertionList.h>


class PropFunc;

using namespace boost::python;


#define USE_INSERTELEMENTSFROMLIST_WORKARAOUND yes

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
    double Strength();
    void   setStrength(double);
    void   setStrength(double,int);
    void   setCurrent(double);
    void   rename(const char* name);
    double getReferenceTime() const;
    double setReferenceTime( double );
    double Current();
    const char*  Type() const;
    double OrbitLength( const Particle& );

          PyObject* _self;
    };


void bmlnElmntWrap::setLength(double){ 
call_method<void>(_self, "setLength"); }

double bmlnElmntWrap::Strength(){ 
call_method<void>(_self, "Strength"); }

void bmlnElmntWrap::setStrength(double){
call_method<void>(_self, "setStrength"); }

void bmlnElmntWrap::setStrength(double,int){
call_method<void>(_self, "setStrength"); }

void bmlnElmntWrap::setCurrent(double){ 
call_method<void>(_self, "setCurrent"); }

void bmlnElmntWrap::rename(const char* name){ 
call_method<void>(_self, "rename"); }

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



void (bmlnElmnt::*propagateParticle)     (Particle&      ) = &bmlnElmnt::propagate;
void (bmlnElmnt::*propagateJetParticle)  (JetParticle&   ) = &bmlnElmnt::propagate;
void (bmlnElmnt::*propagateParticleBunch)(ParticleBunch& ) = &bmlnElmnt::propagate;

void (bmlnElmnt::*setStrength1)(double const&      ) = &bmlnElmnt::setStrength;

double (bmlnElmnt::*setReferenceTime1)(const Particle&  ) = &bmlnElmnt::setReferenceTime;
double (bmlnElmnt::*setReferenceTime2)(double const&)            = &bmlnElmnt::setReferenceTime;

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
    .def("getTag",                   &bmlnElmnt::getTag)
    .def("setTag",                   &bmlnElmnt::setTag)
    .def("setShunt",                 &bmlnElmnt::setShunt)
    .def("getShunt",                 &bmlnElmnt::getShunt)
      /****  virtual functions ***/ 
    .def("setLength",                &bmlnElmnt::setLength)
    .def("Strength",                 &bmlnElmnt::Strength)
    .def("setStrength",              setStrength1)
    .def("setCurrent",               &bmlnElmnt::setCurrent)
    .def("Name",                     &bmlnElmnt::Name)
    .def("rename",                   &bmlnElmnt::rename)
    .def("getReferenceTime",         &bmlnElmnt::getReferenceTime)
    .def("setReferenceTime",         setReferenceTime1)
    .def("setReferenceTime",         setReferenceTime2)
    .def("Current",                  &bmlnElmnt::Current)
    .def("Type",                     &bmlnElmnt::Type)
    .def("OrbitLength",              &bmlnElmnt::OrbitLength)
    .def_readwrite("dataHook",       &bmlnElmnt::dataHook);
}  

//-------------------------------------------------------------------------------------------------------------
// class beamline 
//-------------------------------------------------------------------------------------------------------------

struct beamlineWrap: beamline {

    beamlineWrap(PyObject* self, const char* name = "NONAME"):
       _self(self), beamline(name) {}
    
    beamlineWrap(PyObject* self,   beamline const& bl):
        _self(self),beamline( bl) {}
 
    double InsertElementsFromListWrap( double s_0, InsertionList& il, slist& sl);  // a version of InsertElementsFromList
                                                                                // that is callable from Python.
 
    PyObject* _self;
};


double  
beamlineWrap::InsertElementsFromListWrap( double s_0, InsertionList& il, slist& sl ) 
{

 double tmp_s0 = s_0;  

 beamline::InsertElementsFromList(tmp_s0, il, sl); 

 return tmp_s0;

}


int (beamline::*twiss1)( JetParticle&, double const&, int  )     = &beamline::twiss;
int (beamline::*twiss2)( char,JetParticle& )                     = &beamline::twiss;
int (beamline::*twiss3)( lattFunc&, JetParticle&, int)           = &beamline::twiss;

void (beamline::*insert1) (  bmlnElmnt*  )          = &beamline::insert;
void (beamline::*append1) (  bmlnElmnt* )           = &beamline::append;
void (beamline::*accept1) (  BmlVisitor& )          = &beamline::accept;

void wrap_beamline() {

 class_<beamline, bases<bmlnElmnt>, beamlineWrap> beamlineWrap_("beamline", init<>() );

 beamlineWrap_.def( init<const char*>() );
 beamlineWrap_.def( init<const beamline& >() );
 beamlineWrap_.def("propagateParticle",      beamline_propagateParticle);
 beamlineWrap_.def("propagateJetParticle",   beamline_propagateJetParticle);
 beamlineWrap_.def("propagateParticleBunch", beamline_propagateParticleBunch);
 beamlineWrap_.def("zap",    &beamline::zap) ;
 beamlineWrap_.def("clear",  &beamline::clear);
 beamlineWrap_.def("insert", insert1);
 beamlineWrap_.def("append", append1 );


   //.def("InsertElementAt",          &beamline::InsertElementAt);

  // PROPAGATE PARTICLES

 beamlineWrap_.def("setEnergy",      &beamline::setEnergy);
 beamlineWrap_.def("unTwiss",        &beamline::unTwiss);
 beamlineWrap_.def("eraseBarnacles", &beamline::eraseBarnacles);
 beamlineWrap_.def("twiss", twiss1);
 beamlineWrap_.def("twiss", twiss2);
 beamlineWrap_.def("twiss", twiss3);

 // Editing
#ifdef USE_INSERTELEMENTSFROMLIST_WORKARAOUND
 // n.b. as soon as this workaround is no longer needed, the function
 // InsertElementsFromList1 should be deleted from
 // beamline/include/bmlnElmnt.h
 beamlineWrap_.def("InsertElementsFromList", &beamline::InsertElementsFromList1);
#else  
  beamlineWrap_.def("InsertElementsFromList", &beamlineWrap::InsertElementsFromListWrap);
#endif

  // QUERIES

  //beamlineWrap_.def("whatisLattice",            whatisLattice1); 
  //beamlineWrap_.def("whatisLattice",            whatisLattice2);
  beamlineWrap_.def("countHowMany",               &beamline::countHowMany,       beamline_overloads1());
  beamlineWrap_.def("countHowManyDeeply",         &beamline::countHowManyDeeply, beamline_overloads2());
  beamlineWrap_.def("depth",                      &beamline::depth);
//beamlineWrap_.def("contains",                   &beamline::contains);
//beamlineWrap_.def("firstElement",               &beamline::firstElement);
//beamlineWrap_.def("lastElement",                &beamline::lastElement);
  beamlineWrap_.def("twissIsDone",                &beamline::twissIsDone);
  beamlineWrap_.def("setTwissIsDone",             &beamline::setTwissIsDone);
  beamlineWrap_.def("unsetTwissIsDone",           &beamline::unsetTwissIsDone);
  beamlineWrap_.def("Energy",                     &beamline::Energy);
  beamlineWrap_.def("OrbitLength",                &beamline::OrbitLength);
  beamlineWrap_.def("accept",                     accept1);
  beamlineWrap_.def("flatten",&beamline::flatten,
       return_value_policy<reference_existing_object>());
 
  //beamlineWrap_.def("whatIsRing",               &beamline::whatIsRing)

}

