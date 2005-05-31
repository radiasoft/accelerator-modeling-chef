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


#ifndef BIPOLARCIRCUIT_H
#define BIPOLARCIRCUIT_H

#ifndef CIRCUIT_H
#include "circuit.h"
#endif

struct PolarityBarn : public BarnacleData {
  double polarity;
  PolarityBarn(double x) : polarity(x) {}
  PolarityBarn(const PolarityBarn& x): BarnacleData() {polarity = x.polarity;}
  ~PolarityBarn() {}
};

class BipolarCircuit : public circuit {
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
