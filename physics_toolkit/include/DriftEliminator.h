/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      DriftEliminator.h
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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


/*
 * File: DriftEliminator.h
 * 
 * Header for class DriftEliminator
 * 
 * Concatenates drifts and removes passive
 * elements of zero length.
 * 
 * Leo Michelotti
 * May 17, 2001
 * 
 */

#ifndef DRIFTELIMINATOR_H
#define DRIFTELIMINATOR_H

#ifndef BMLVISITOR_H
#include "BmlVisitor.h"
#endif

class DriftEliminator : public ConstBmlVisitor
{
 public:
  DriftEliminator();
  DriftEliminator( const DriftEliminator& );
  ~DriftEliminator();

  void visitBeamline( const beamline* );
  void visitBmlnElmnt( const bmlnElmnt* );

  void visitMonitor( const monitor* ); 
  void visitMarker( const marker* ); 
  void visitDrift( const drift* ); 

  void visitSlot( const Slot* );

  beamline* beamlinePtr();
  // Invoking routine is responsible for
  // using this beamline before the 
  // DriftEliminator goes out of scope.

  beamline* clonedBeamlinePtr();
  // Invoking routine is responsible for
  // eliminating the cloned beamline.


 private: 
  beamline* _bmlPtr;
  drift*    _driftPtr;
  void      _handlePassiveElement( const bmlnElmnt* );
};


#endif // DRIFTELIMINATOR_H
