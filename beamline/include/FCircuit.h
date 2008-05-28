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
******                                                                
****** REVISION HISTORY
****** Mar 2007        ostiguy@fnal.gov
****** - added support for reference counted elements.
**************************************************************************
*************************************************************************/

#if 0

#ifndef FCIRCUIT_H
#define FCIRCUIT_H


#include <basic_toolkit/globaldefs.h>
#include <beamline/circuit.h>


class DLLEXPORT FCircuit : public Circuit {

public:

  FCircuit( const char* );
 ~FCircuit();

  void   switchOn();
  void   switchOff();
    
  void   set(double const&);
  double get() const;

  void   setCurrent( double const& );
  double getCurrent() const;

  void append( ElmPtr );

protected:

  double field_;

};

#endif // FCIRCUIT_H
#endif
