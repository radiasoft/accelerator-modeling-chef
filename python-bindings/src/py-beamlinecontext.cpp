#include <boost/python.hpp>

#include <bmlnElmnt.h>
#include <beamline.h>
#include <Matrix.h>
#include "BeamlineContext.h"
#include <iostream>

#define BOOST_PYTHON_STATIC_MODULE


   // class LattFuncSage;
   // class LattFuncSage::lattRing;
   // class LattFuncSage::lattFunc;


void wrap_beamlinecontext() {

using namespace boost::python;

class_<BeamlineContext>( "BeamlineContext", init<bool, beamline*>() )
      
.def( "assign",                         &BeamlineContext::assign )                    //
//.def( "accept",                       &BeamlineContext::accept )                    //  ( ConstBmlVisitor& ) const;
//.def  "accept",                       &BeamlineContext::accept )                    //  ( BmlVisitor& ) 
.def( "setClonedFlag",                  &BeamlineContext::setClonedFlag ) 
.def( "getClonedFlag",                  &BeamlineContext::getClonedFlag )
.def( "writeTree",                      &BeamlineContext::writeTree )
.def( "name",                           &BeamlineContext::name)                             // const char* name() const;
.def( "peekAt",                         &BeamlineContext::peekAt)                           // void peekAt( double& s, Particle* = 0 ) const;
.def( "sumLengths",                     &BeamlineContext::sumLengths)                       // double sumLengths() const;
.def( "setLength",                      &BeamlineContext::setLength )                       // int setLength   ( bmlnElmnt*, double );
.def( "setStrength",                    &BeamlineContext::setStrength )                     // int setStrength ( bmlnElmnt*, double );
//.def("setAlignment",                  &BeamlineContext::setAlignment )                    // int setAlignment( bmlnElmnt*, const alignmentData& );
//.def("setAlignment",                  &BeamlineContext::setAlignment)                     //int setAlignment( beamline::Criterion&, const alignmentData& );

.def( "processElements",                &BeamlineContext::processElements)                  // int processElements( beamline::Action& ); 
.def( "setAvgInvariantEmittance",       &BeamlineContext::setAvgInvariantEmittance )        // void setAvgInvariantEmittance( double, double );
.def( "getEnergy",                      &BeamlineContext::getEnergy)                        // double getEnergy() const;
.def( "countHowManyDeeply",             &BeamlineContext::countHowManyDeeply)               // int countHowManyDeeply() const;
//.def("getAlignmentData",              &BeamlineContext::getAlignmentData)                 // alignmentData getAlignmentData( const bmlnElmnt* ) const;
//.def("cloneBeamline",                 &BeamlineContext::cloneBeamline)                    // beamline* cloneBeamline() const;
//.def("getOneTurnMap",                 &BeamlineContext::getOneTurnMap )                   // Mapping getOneTurnMap();
.def( "getHorizontalFracTune",          &BeamlineContext::getHorizontalFracTune)            // double getHorizontalFracTune();
.def( "getVerticalFracTune",            &BeamlineContext::getVerticalFracTune)
.def( "getHorizontalEigenTune",         &BeamlineContext::getHorizontalEigenTune)             //double getHorizontalEigenTune()
.def( "getVerticalEigenTune",           &BeamlineContext::getVerticalEigenTune)               // double getVerticalEigenTune()
.def( "getLattFuncPtr",                 &BeamlineContext::getLattFuncPtr, 
                                        return_value_policy<reference_existing_object>() )    // const LattFuncSage::lattFunc* getLattFuncPtr( int )
.def("getETFuncPtr",                    &BeamlineContext::getETFuncPtr,  
                                          return_value_policy<reference_existing_object>() )  //const EdwardsTengSage::Info* getETFuncPtr( int )
.def("getCovFuncPtr",                   &BeamlineContext::getCovFuncPtr, 
                                          return_value_policy<reference_existing_object>() )   //const CovarianceSage::Info* getCovFuncPtr( int )
.def("getDispersionPtr",                &BeamlineContext::getDispersionPtr, 
                                          return_value_policy<reference_existing_object>() )   //const DispersionSage::Info* getDispersionPtr( int )


//.def("equilibriumCovariance",         &BeamlineContext::equilibriumCovariance, 
//                                          return_value_policy<reference_existing_object>() )   // MatrixD equilibriumCovariance();
//.def("equilibriumCovariance",         &BeamlineContext::equilibriumCovariance, 
//                                          return_value_policy<reference_existing_object>() )   // MatrixD equilibriumCovariance( double, double );
.def("addHTuneCorrector",               &BeamlineContext::addHTuneCorrector)                   // int addHTuneCorrector( const bmlnElmnt* );
.def("addVTuneCorrector",               &BeamlineContext::addVTuneCorrector)                   // int addVTuneCorrector( const bmlnElmnt* );
.def("changeTunesBy",                   &BeamlineContext::changeTunesBy)                       // int changeTunesBy( double, double );

.def("beginIterator",                   &BeamlineContext::beginIterator)                       // int beginIterator();
.def("beginDeepIterator",               &BeamlineContext::beginDeepIterator)                   // int beginDeepIterator();
.def("beginReverseIterator",            &BeamlineContext::beginReverseIterator)                // int beginReverseIterator();
.def("beginDeepReverseIterator",        &BeamlineContext::beginDeepReverseIterator)            // int beginDeepReverseIterator();

.def("endIterator",                     &BeamlineContext::endIterator)                         // void endIterator();
.def("endDeepIterator",                 &BeamlineContext::endDeepIterator)                     // void endDeepIterator();
.def("endReverseIterator",              &BeamlineContext::endReverseIterator)                  // void endReverseIterator();
.def("endDeepReverseIterator",          &BeamlineContext::endDeepReverseIterator)              // void endDeepReverseIterator();

.def("i_next",                          &BeamlineContext::i_next,    
                                           return_value_policy<reference_existing_object>() )     // const bmlnElmnt* i_next();
.def("di_next",                         &BeamlineContext::di_next,   
                                           return_value_policy<reference_existing_object>() )     // const bmlnElmnt* di_next();
.def("ri_next",                         &BeamlineContext::ri_next,   
                                           return_value_policy<reference_existing_object>() )     // const bmlnElmnt* ri_next();
.def("dri_next",                        &BeamlineContext::dri_next,  
                                           return_value_policy<reference_existing_object>() )     // const bmlnElmnt* dri_next();

.def("i_reset",                         &BeamlineContext::i_reset)                               // void i_reset();
.def("di_reset",                        &BeamlineContext::di_reset)                              // void di_reset();
.def("ri_reset",                        &BeamlineContext::ri_reset)                              // void ri_reset();
.def("dri_reset",                       &BeamlineContext::dri_reset)                             // void dri_reset();

  ;

}
