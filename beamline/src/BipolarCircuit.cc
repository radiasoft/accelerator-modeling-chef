/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BipolarCircuit.cc
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
**************************************************************************
**************************************************************************
*************************************************************************/




#include <basic_toolkit/iosetup.h>
#include <beamline/BipolarCircuit.h>

using namespace std;

using FNAL::pcout;
using FNAL::pcerr;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BipolarCircuit::BipolarCircuit(const char* n) 
 : Circuit (n), field_(0.0) {} 


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BipolarCircuit::~BipolarCircuit() {}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BipolarCircuit::switchOn() {
  onOffSwitch_ = true;
  set( field_ );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BipolarCircuit::switchOff() {

  onOffSwitch_ = false;

  for (std::list<ElmPtr>::iterator it  = theList_.begin(); 
                                   it != theList_.end(); ++it) {
    (*it)-> setStrength( 0.0 );
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BipolarCircuit::set(double const& current) {

  
  field_       = current;
  onOffSwitch_ = true;

  for (std::list<ElmPtr>::iterator it  = theList_.begin(); 
                                   it != theList_.end(); ++it) {

    BarnacleList::iterator bit = (*it)->dataHook.find("Polarity");

    if( bit ==  (*it)->dataHook.end() ) {
      (*pcerr) << "BipolarCircuit: no polarity information for " <<
	(*it)->Name() << " Assuming +1 " << endl;
      (*it)-> setStrength( current );

    } else {
      (*pcerr) <<    "*** WARNING ***"
                   "\n*** WARNING *** " << __FILE__ << ",line: " << __LINE__
               <<  "\n*** WARNING *** void BipolarCircuit::set(double const& current)"
                   "\n*** WARNING *** BipolarCircuit.cc is old, obsolete code that SHOULD"
                   "\n*** WARNING *** HAVE BEEN ELIMINATED a long time ago.  The action"
                   "\n*** WARNING *** of this segment has been masked out."
                   "\n*** WARNING *** "
               << endl;
      #if 0
      *** FIX ME ***
      *** FIX ME ***  BipolarCircuit.cc is old, obsolete code that SHOULD
      *** FIX ME ***  HAVE BEEN ELIMINATED a long time ago.  Right now it
      *** FIX ME ***  does nothing more than produce an undefined reference
      *** FIX ME ***  because of the following line, which I am masking.
      *** FIX ME ***  - Leo Michelotti
      *** FIX ME ***
      (*it)->setStrength( current *  boost::any_cast<double>(bit->info) );
      #endif
    }
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double BipolarCircuit::get() const {
  return field_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BipolarCircuit::append( ElmPtr q ) {
   theList_.push_back( q );
   field_ = q->Strength();
}
