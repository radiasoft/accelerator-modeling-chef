//*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      EditDialog.h
****** 
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
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
****** Mar 2007   ostiguy@fnal.gov
****** - support for reference-counted elements and beamlines 
****** - visitor interface takes advantage of dynamic typing
******
*************************************************************************
*************************************************************************
************************************************************************/
#ifndef EDIT_DIALOG_H
#define EDIT_DIALOG_H


#include <beamline/BmlVisitor.h>
#include <beamline/BmlPtr.h>

struct editDialog : public BmlVisitor 
{
  void visit( bmlnElmnt&  );
  void visit( beamline&   );
  void visit( drift&      );
  void visit( Slot&       );
  void visit( rbend&      );
  void visit( quadrupole& );
  void visit( thinQuad&   );

  BmlContextPtr     _contextPtr;
};

#endif // EDIT_DIALOG_H
