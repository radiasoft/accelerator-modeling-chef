/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      LayoutVisitor.h
******  Version:   1.1
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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
****** July 2007 ostiguy@fnal.gov
****** - take advantage of dynamic type resolution for visit functions
****** - element predicate based on general function object   
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
 * Version 1.1  June 27, 2003
 * 
 */

#ifndef LAYOUTVISITOR_H
#define LAYOUTVISITOR_H

#include <beamline/BmlVisitor.h>
#include <beamline/bmlnElmnt.h>

#include <fstream>

class LayoutVisitor : public BmlVisitor {

 public:

  LayoutVisitor( double, double, double, double );
 ~LayoutVisitor();

  void visit( bmlnElmnt&  );
  void visit( quadrupole& );
  void visit( rbend&      );
  void visit( sbend&      );
  void visit( CF_rbend&   );
  void visit( CF_sbend&   );
  void visit( sextupole&  );
  void visit( sector&     );

  int  getErrorCode() const;
  int  openFile( const char* );
  int  closeFile();

  void setDiscriminator( boost::function< bool( bmlnElmnt&) >, double = 0.0 );

  // Error codes

  static const int OKAY;
  static const int SECTORVISITED;
  static const int NOFILEOPENED;

 private:

  LayoutVisitor( LayoutVisitor const& ); // forbidden

  std::ofstream * streamPtr_; 

  void visit_bend( bmlnElmnt& );
  void processSpecialElement( bmlnElmnt& );

  int  errorCode_;

  double s_;
  double baseline_;
  double bendHeight_;
  double quadHeight_;
  double sextHeight_;
  double specialHeight_;

  boost::function< bool( bmlnElmnt&) > discriminator_; 

};

#endif // LAYOUTVISITOR_H
