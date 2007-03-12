/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      circuit.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
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
******  REVISION HISTORY:
****** 
******  Jan-Mar 2007:  ostiguy@fnal.gov
******
******  - eliminated dependence on dlist (c-style void* linked list)
******  - eliminated inheritance from container type
******  - support for smart pointers  
******  - cleanup of constructor interface  
******
******                                                              
**************************************************************************
*************************************************************************/


#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <string>
#include <list>

class DLLEXPORT Circuit {

public:

  Circuit( char const* name );
  virtual ~Circuit();

  virtual void switchOn()  = 0;
  virtual void switchOff() = 0;

  std::string  name() const;

  virtual void append( ElmPtr q ) = 0;

protected:

  std::string                 ident_;      // name of circuit
  bool                  onOffSwitch_;      // switch to turn current on and off

  std::list<ElmPtr>          theList_;

private:

  Circuit( Circuit const&); // forbidden  
  
} ;


#endif // CIRCUIT_H
