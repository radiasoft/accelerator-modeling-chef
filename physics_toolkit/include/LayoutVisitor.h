/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   ?.?
******                                    
******  File:      LayoutVisitor.h
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
 * File: LayoutVisitor.h
 * 
 * Header for class LayoutVisitor
 * 
 * Provides coordinates for simple 2D diagram
 * of the beamline. FramePusher should be used
 * for more detailed site maps.
 * 
 * Leo Michelotti
 * May 7, 2002
 * 
 */

#ifndef LAYOUTVISITOR_H
#define LAYOUTVISITOR_H

#ifndef BMLVISITOR_H
#include "BmlVisitor.h"
#endif

#include <fstream>

class LayoutVisitor : public BmlVisitor
{
 public:
  LayoutVisitor( double, double, double, double );
  LayoutVisitor( const LayoutVisitor& );
  ~LayoutVisitor();

  void visitBmlnElmnt ( bmlnElmnt*  );
  void visitQuadrupole( quadrupole* );
  void visitRbend     ( rbend*      );
  void visitSbend     ( sbend*      );
  void visitCF_rbend  ( CF_rbend*   );
  void visitCF_sbend  ( CF_sbend*   );
  void visitSextupole ( sextupole*  );
  void visitSector    ( sector*     );

  int  getErrorCode() const;
  int  openFile( const char* );
  int  closeFile();

  // Error codes
  static const int OKAY;
  static const int SECTORVISITED;
  static const int NOFILEOPENED;

 private:
  double _s;
  double _baseline;
  double _bendHeight;
  double _quadHeight;
  double _sextHeight;

  std::ofstream* _streamPtr;

  void _visit_bend( bmlnElmnt* );

  int    _errorCode;
};

#endif // LAYOUTVISITOR_H
