/**********************************************************************/
/*                                                                    */
/* File:           EditDialog.h                                       */
/*                                                                    */ 
/* Authors:        Leo Michelotti                                     */
/*                 michelotti@fnal.gov                                */     
/*                                                                    */ 
/* Creation Date:  July 26, 2005                                      */
/*                                                                    */ 
/* Copyright:      (c) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/


#ifndef EDIT_DIALOG_H
#define EDIT_DIALOG_H

class BeamlineContext;

#include "BmlVisitor.h"

struct editDialog : public BmlVisitor 
{
  void visitBmlnElmnt  ( bmlnElmnt*  );
  void visitDrift      ( drift*      );
  void visitSlot       ( Slot*       );
  void visitRbend      ( rbend*      );
  void visitQuadrupole ( quadrupole* );
  void visitThinQuad   ( thinQuad*   );

  BeamlineContext*     _contextPtr;
};

#endif // EDIT_DIALOG_H
