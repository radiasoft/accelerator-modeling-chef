/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
******                                    
******  File:      PointEdit.cc
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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


// ************************************
// 
// File: PointEdit.cc
// 
// Author: Leo Michelotti
// Date: May 24, 2001
// 
// ************************************


#include <stdlib.h>    // needed for atof
#include <math.h>

#include <qapplication.h>
#include <qhbox.h>

#include "PointEdit.h"

using namespace CHEF_domain;

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


