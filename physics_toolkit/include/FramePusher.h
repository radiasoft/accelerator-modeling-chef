/*
 * File: FramePusher.h
 * 
 * Header for class FramePusher.
 * 
 * Moves a Frame around a beamline, 
 * in anticipation of drawing the footprint.
 * 
 * Leo Michelotti
 * May 11, 2001
 * 
 */

#ifndef FRAMEPUSHER_H
#define FRAMEPUSHER_H

#ifndef BMLVISITOR_H
#include "BmlVisitor.h"
#endif

#ifndef FRAME_H
#include "Frame.h"
#endif


class FramePusher : public BmlVisitor
{
 public:
  FramePusher();
  FramePusher( const Frame& );
  FramePusher( const FramePusher& );
  ~FramePusher();

  void visitBmlnElmnt( bmlnElmnt* );

  void visitSlot     ( Slot*     );
  void visitSbend    ( sbend*    );
  void visitCF_sbend ( CF_sbend* );
  void visitSector   ( sector*   );

  Frame getFrame();

 private: 
  Frame _frame;
  int _errorCode;

  static const int OKAY;
  static const int SECTORVISITED;
};

#include "FramePusher.icc"

#endif // FRAMEPUSHER_H
