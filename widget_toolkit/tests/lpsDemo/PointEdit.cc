////////////////////////////////////////////////////////////
// 
// File:          PointEdit.cc
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
// src/PointEdit.cc.  It will be removed from this directory
// following correction of an oversight described in the file
// PointEdit.h in this directory.
// 
////////////////////////////////////////////////////////////

#include <stdlib.h>    // needed for atof
#include <math.h>

#include <qapplication.h>
#include <qhbox.h>

#include <PointEdit.h>

using namespace AESOP_domain;

// -------------------------------
// Implementation: class PointEdit
// -------------------------------

PointEdit::PointEdit( const char* str, QHBox* parent )
: QLineEdit( QString(str), parent )
{
}

PointEdit::~PointEdit()
{
}

void PointEdit::_set_first( double x, double )
{
  static QString qstr;
  qstr.setNum(x);
  this->setText( qstr );
}

void PointEdit::_set_second( double, double x )
{
  static QString qstr;
  qstr.setNum(x);
  this->setText( qstr );
}


void PointEdit::_txtchg( const QString& s )
{
  QPalette theColors( this->palette() );
  theColors.setColor( QPalette::Normal, QColorGroup::Text, red );

  this->setPalette( theColors );
  this->repaint();
  QApplication::flushX();

  disconnect( this,  SIGNAL(textChanged( const QString& )),
              this,  SLOT(_txtchg( const QString& )) );
}


void PointEdit::_retprs()
{
  this->unsetPalette();
  this->repaint();
  QApplication::flushX();

  connect( this,  SIGNAL(textChanged( const QString& )),
           this,  SLOT(_txtchg( const QString& )) );

  emit _new_value( atof(this->text()) );
}


