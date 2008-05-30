////////////////////////////////////////////////////////////
// 
// File:          PointEdit.h
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Thursday. May 24, 2001.       (original version)
// * Primary purpose: usage by Tracker.cc and other
//   similar widget files.
// 
// Friday.   May 30, 2008.       (this file)
// 
////////////////////////////////////////////////////////////
// 
// This file is an effective duplicate of widget_toolkit/tracking/
// include/PointEdit.h.  It is reproduced here because the widget_toolkit
// files do not appear in the $INSTALLDIR/include upon CHEF's
// installation.  That is an oversight which will be corrected, at
// which time this file will disappear.
// 
////////////////////////////////////////////////////////////

#ifndef POINTEDIT_H
#define POINTEDIT_H

#include <qlineedit.h>

class QHBox;

namespace AESOP_domain
{
class PointEdit : public QLineEdit
{
Q_OBJECT

public:
  PointEdit( const char*, QHBox* );
  ~PointEdit();

public slots:
  void _set_first ( double, double );
  void _set_second( double, double );
  void _txtchg( const QString& );
  void _retprs();

signals:
  void _new_value( double );

};

};  // end namespace AESOP

#endif // POINTEDIT_H
