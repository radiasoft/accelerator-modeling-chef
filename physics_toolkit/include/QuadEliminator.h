/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.1
******                                    
******  File:      QuadEliminator.h
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
 * File: QuadEliminator.h
 * 
 * Header for class QuadEliminator
 * 
 * Concatenates quads and removes passive
 * elements of zero length.
 * 
 * Leo Michelotti
 * March 11, 2002
 * 
 */

#ifndef QUADELIMINATOR_H
#define QUADELIMINATOR_H

#ifndef BMLVISITOR_H
#include "BmlVisitor.h"
#endif

class QuadEliminator : public ConstBmlVisitor
{
 public:
  QuadEliminator();
  QuadEliminator( const QuadEliminator& );
  ~QuadEliminator();

  void visitBeamline  ( const beamline*   );
  void visitBmlnElmnt ( const bmlnElmnt*  );
  void visitQuadrupole( const quadrupole* ); 

  beamline* beamlinePtr();
  // Invoking routine is responsible for
  // using this beamline before the 
  // QuadEliminator goes out of scope.

  beamline* clonedBeamlinePtr();
  // Invoking routine is responsible for
  // eliminating the cloned beamline.


 private: 
  beamline*   _bmlPtr;
  quadrupole* _quadPtr;
};


#endif // QUADELIMINATOR_H
