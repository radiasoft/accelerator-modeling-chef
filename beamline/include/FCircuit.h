/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      FCircuit.h
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


#ifndef FCIRCUIT_H
#define FCIRCUIT_H

#ifndef CIRCUIT_H
#include "circuit.h"
#endif

class FCircuit : public circuit {
protected:
  double field;
public:
  FCircuit();
  ~FCircuit();
  FCircuit( const char* );
  FCircuit( bmlnElmnt* );
  FCircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
  void setCurrent( void* );
  void getCurrent( void* );
  void append( bmlnElmnt* );
};

#endif // FCIRCUIT_H
