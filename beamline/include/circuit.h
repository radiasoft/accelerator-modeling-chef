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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef CIRCUIT_H
#define CIRCUIT_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

class circuit : public dlist
{
protected:
  char onOffSwitch;     // switch to turn current on and off
  char* ident;          // name of circuit
  int numElm;           // number of elements in the circuit
public:
  circuit();
  circuit( const char* );
  circuit( bmlnElmnt* );
  circuit( const char*, bmlnElmnt* );
  circuit(const circuit&);
  virtual ~circuit();
  virtual void switchOn();
  virtual void switchOff();
  virtual void set( void* );
  virtual void get( void* );
  char* getName();

  void append( bmlnElmnt* q );
} ;

#endif // CIRCUIT_H
