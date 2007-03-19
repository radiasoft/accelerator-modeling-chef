/*******************************************************************************  
********************************************************************************
********************************************************************************
******
******  Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-beamline.cpp
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
********************************************************************************
********************************************************************************
*******************************************************************************/

#include <boost/python.hpp>
#include <string>

#include <boost/iterator/indirect_iterator.hpp>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/BmlVisitor.h>
#include <beamline/beamline.h>

class PropFunc;

using namespace boost::python;

namespace {

typedef boost::indirect_iterator<beamline::iterator>                     indirect_iterator;
typedef boost::indirect_iterator<beamline::pre_order_iterator>           indirect_pre_order_iterator;
typedef boost::indirect_iterator<beamline::post_order_iterator>          indirect_post_order_iterator;
typedef boost::indirect_iterator<beamline::deep_iterator>                indirect_deep_iterator;


typedef boost::indirect_iterator<beamline::reverse_iterator>             indirect_reverse_iterator;
typedef boost::indirect_iterator<beamline::reverse_pre_order_iterator>   indirect_reverse_pre_order_iterator;
typedef boost::indirect_iterator<beamline::reverse_post_order_iterator>  indirect_reverse_post_order_iterator;
typedef boost::indirect_iterator<beamline::reverse_deep_iterator>        indirect_reverse_deep_iterator;


indirect_iterator indirect_begin( beamline& bml ) { return bml.begin(); }
indirect_iterator   indirect_end( beamline& bml ) { return bml.end();   }

indirect_pre_order_iterator indirect_pre_begin( beamline& bml )  { return bml.pre_begin(); }
indirect_pre_order_iterator   indirect_pre_end( beamline& bml )  { return bml.pre_end();   }

indirect_post_order_iterator indirect_post_begin( beamline& bml ) { return bml.post_begin(); }
indirect_post_order_iterator   indirect_post_end( beamline& bml ) { return bml.post_end();   }

indirect_deep_iterator      indirect_deep_begin( beamline& bml ) { return bml.deep_begin(); }
indirect_deep_iterator        indirect_deep_end( beamline& bml ) { return bml.deep_end();   }

indirect_reverse_iterator indirect_rbegin( beamline& bml )                 { return bml.rbegin(); }
indirect_reverse_iterator   indirect_rend( beamline& bml )                 { return bml.rend();   }

indirect_reverse_pre_order_iterator indirect_rpre_begin( beamline& bml )   { return bml.rpre_begin(); }
indirect_reverse_pre_order_iterator   indirect_rpre_end( beamline& bml )   { return bml.rpre_end();   }

indirect_reverse_post_order_iterator indirect_rpost_begin( beamline& bml ) { return bml.rpost_begin(); }
indirect_reverse_post_order_iterator   indirect_rpost_end( beamline& bml ) { return bml.rpost_end();   }
  
indirect_reverse_deep_iterator      indirect_rdeep_begin( beamline& bml )  { return bml.rdeep_begin(); }
indirect_reverse_deep_iterator        indirect_rdeep_end( beamline& bml )  { return bml.rdeep_end();   }



void (beamline::*beamline_propagateParticle)     (Particle&      ) =  &beamline::propagate;
void (beamline::*beamline_propagateJetParticle)  (JetParticle&   ) =  &beamline::propagate;
void (beamline::*beamline_propagateParticleBunch)(ParticleBunch& ) =  &beamline::propagate;

void (beamline::*insert1) (  ElmPtr const& )                       =  &beamline::insert;
void (beamline::*append1) (  ElmPtr const& )                       =  &beamline::append;
void (beamline::*accept1) (  BmlVisitor& )                         =  &beamline::accept;

int (beamline::*countHowMany1)()       const                       =  &beamline::countHowMany;
int (beamline::*countHowManyDeeply1)() const                       =  &beamline::countHowManyDeeply;

bmlnElmnt& beamline_firstElement ( beamline& bml ) { return *bml.firstElement(); }
bmlnElmnt& beamline_lastElement  ( beamline& bml ) { return *bml.lastElement(); }

} // anonymous namespace

//------------------------------------------------------------------------------
// wrapper code
//------------------------------------------------------------------------------

void wrap_beamline() {

class_<beamline, bases<bmlnElmnt>, boost::shared_ptr<beamline> >beamline_("beamline", init<>() );

beamline_.def( init<const char*>() );
beamline_.def( init<const beamline& >() );
beamline_.def("__iter__", 
    range< return_value_policy<reference_existing_object> >( &indirect_deep_begin, &indirect_deep_end) ); // default iterator

beamline_.def("iterator", 
   range<return_value_policy<reference_existing_object> > (  &indirect_begin,      &indirect_end) );

beamline_.def("pre_order_iterator", 
    range<return_value_policy<reference_existing_object> >(  &indirect_pre_begin,  &indirect_pre_end ) );

beamline_.def("post_order_iterator", 
    range<return_value_policy<reference_existing_object> >(  &indirect_post_begin, &indirect_post_end ) );

beamline_.def("deep_iterator", 
    range<return_value_policy<reference_existing_object> >(  &indirect_deep_begin, &indirect_deep_end ) );

beamline_.def("reverse_iterator", 
   range<return_value_policy<reference_existing_object> > (  &indirect_rbegin,      &indirect_rend) );

beamline_.def("reverse_pre_order_iterator", 
    range<return_value_policy<reference_existing_object> >(  &indirect_rpre_begin,  &indirect_rpre_end ) );

beamline_.def("reverse_post_order_iterator", 
    range<return_value_policy<reference_existing_object> >(  &indirect_rpost_begin, &indirect_rpost_end ) );

beamline_.def("reverse_deep_iterator", 
    range<return_value_policy<reference_existing_object> >(  &indirect_rdeep_begin, &indirect_rdeep_end ) );

beamline_.def("clear",                  &beamline::clear);
beamline_.def("insert",                 insert1);
beamline_.def("append",                 append1 );
beamline_.def("flatten",                &beamline::flatten);

// beamline_.def("InsertElementsFromList",  &beamlineWrap::InsertElementsFromList);
// beamline_.def("InsertElementAt",         &beamline::InsertElementAt);

  // PROPAGATE PARTICLES

 beamline_.def("propagateParticle",      beamline_propagateParticle);
 beamline_.def("propagateJetParticle",   beamline_propagateJetParticle);
 beamline_.def("propagateParticleBunch", beamline_propagateParticleBunch);

 beamline_.def("setEnergy",      &beamline::setEnergy);
 beamline_.def("unTwiss",        &beamline::unTwiss);
 beamline_.def("eraseBarnacles", &beamline::eraseBarnacles);

 //beamline_.def("twiss", twiss1);
 //beamline_.def("twiss", twiss2);
 //beamline_.def("twiss", twiss3);

  // QUERIES

 //beamline_.def("whatisLattice",            whatisLattice1); 
 //beamline_.def("whatisLattice",            whatisLattice2);
 beamline_.def("countHowMany",               countHowMany1 );
 beamline_.def("countHowManyDeeply",         countHowManyDeeply1 );
 beamline_.def("isFlat",                     &beamline::isFlat);
 beamline_.def("empty",                      &beamline::empty);
 beamline_.def("depth",                      &beamline::depth);
 beamline_.def("contains",                   &beamline::contains);
 beamline_.def("firstElement",               &beamline_firstElement, return_value_policy<reference_existing_object>() );
 beamline_.def("lastElement",                &beamline_lastElement,  return_value_policy<reference_existing_object>() );
 beamline_.def("twissIsDone",                &beamline::twissIsDone);
 beamline_.def("setTwissIsDone",             &beamline::setTwissIsDone);
 beamline_.def("unsetTwissIsDone",           &beamline::unsetTwissIsDone);
 beamline_.def("Energy",                     &beamline::Energy);
 beamline_.def("OrbitLength",                &beamline::OrbitLength);
 beamline_.def("accept",                     accept1);
 //beamline_.def("flatten",&beamline::flatten,
 //     return_value_policy<reference_existing_object>());
 //beamline_.def("whatIsRing",               &beamline::whatIsRing)

}

