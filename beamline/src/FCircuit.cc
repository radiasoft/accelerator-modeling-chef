/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      FCircuit.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - eliminated dependency on dlist
****** - use new-style Barnacles.
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <basic_toolkit/iosetup.h>
#include <beamline/FCircuit.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

FCircuit::FCircuit(const char* n)
: Circuit (n), field_(0.0) {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


FCircuit::~FCircuit() {}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FCircuit::switchOn() {

  onOffSwitch_ = true;

  for ( std::list<ElmPtr>::iterator it  = theList_.begin(); 
                                    it != theList_.end(); ++it ) {

    (*it) -> setStrength( field_ );
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FCircuit::switchOff() {

  onOffSwitch_ = false;

  for ( std::list<ElmPtr>::iterator it  = theList_.begin(); 
                                    it != theList_.end(); ++it ) {
    (*it)->setStrength(0.0);
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FCircuit::set(double const& field) {

  field_ = field;

  for ( std::list<ElmPtr>::iterator it  = theList_.begin(); 
                                    it != theList_.end(); ++it ) {
    (*it)->setStrength( field_ );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double FCircuit::get() const {
  return field_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double FCircuit::getCurrent() const {

    // Take as a representative element, the first one and use it's ItoField.

  double ItoFieldvalue = theList_.front()->IToField();

  double current = 0.0;

  if(ItoFieldvalue != 0.0) {
    current = field_/ItoFieldvalue;
   } else {
    (*pcerr) << "FCircuit::getCurrent(): ItoField conversion is zero for " 
         << name() << endl;
  }

  return current;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FCircuit::setCurrent(double const& current) {

  double ItoFieldvalue = 0.0;
  double lcurrent      = current;

  for ( std::list<ElmPtr>::iterator it  = theList_.begin(); 
                                    it != theList_.end(); ++it ) {

    ItoFieldvalue = (*it)->IToField();

    if(ItoFieldvalue != 0.0) {
      field_ = lcurrent * ItoFieldvalue;
    } else {
      (*pcerr) << "FCircuit::setCurrent(): ItoField conversion is zero for " 
	   << (*it)->Name() << endl;
      lcurrent = field_;
      return;
    }
    (*it)->setCurrent(lcurrent);
    (*pcout) << (*it)->Name() << " Strength= " << (*it)->Strength() << " Current= "
         << lcurrent  << " I2F= " << (*it)->IToField() << endl;
  };
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void FCircuit::append( ElmPtr q ) {
   
   theList_.push_back( q );
   field_ = q->Strength();
}

