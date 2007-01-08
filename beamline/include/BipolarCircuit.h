/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      BipolarCircuit.h
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
******                                                                
**************************************************************************
*************************************************************************/


#ifndef BIPOLARCIRCUIT_H
#define BIPOLARCIRCUIT_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/dlist.h>
#include <beamline/circuit.h>


class  DLLEXPORT  BipolarCircuit : public circuit {
protected:
  double field;
public:
  BipolarCircuit();
  ~BipolarCircuit();
  BipolarCircuit( const char* );
  BipolarCircuit( bmlnElmnt* );
  BipolarCircuit( const char*, bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );
  void append( bmlnElmnt* );

};

#endif // BIPOLARCIRCUIT_H
