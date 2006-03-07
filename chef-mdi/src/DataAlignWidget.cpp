/**********************************************************************/
/*                                                                    */
/* File:           DataAlignWidget.cc                                 */
/*                                                                    */ 
/* Authors:        Leo Michelotti                                     */
/*                 michelotti@fnal.gov                                */     
/*                                                                    */ 
/* Creation Date:  August 25, 2005                                    */
/* Current rev:    August 25, 2005                                    */
/*                                                                    */ 
/* Copyright:      (c) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/

#include <iostream>
#include <fstream>

#include "DataAlignWidget.h"
#include "BeamlineContext.h"

#include <qnamespace.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

using namespace std;


DataAlignWidget::DataAlignWidget( BeamlineContext& x, QWidget* parent, const char* name, WFlags atts )
:   QDialog( parent, name, false, atts )
  , _bmlConPtr(&x)
{
  _finishConstructor();
}


void DataAlignWidget::_finishConstructor()
{
  _qvbPtr = new QVBox( this );

    QLabel* qlbPtr = new QLabel( "Just a placeholder", _qvbPtr );
    qlbPtr->setMinimumHeight(50);
    qlbPtr->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    QHBox* qhbPtr = new QHBox( _qvbPtr );
      QPushButton* closeBtn = new QPushButton( "Close", qhbPtr );
        connect( closeBtn, SIGNAL(released()),
                      this,  SLOT(close()) );
      QPushButton* cancelBtn = new QPushButton( "Cancel", qhbPtr );
        connect( cancelBtn, SIGNAL(released()),
                      this,  SLOT(cancel()) );
      QPushButton* applyBtn = new QPushButton( "Apply", qhbPtr );
        connect( applyBtn, SIGNAL(pressed()),
                      this,  SLOT(apply()) );

      cancelBtn->setDefault( false );
      applyBtn ->setDefault( false );
      closeBtn ->setDefault( true  );

    qhbPtr->setMargin(5);
    qhbPtr->setSpacing(3);
    qhbPtr->adjustSize();

  _qvbPtr->adjustSize();

  this->setCaption( "Align to data" );
  this->adjustSize();
}


DataAlignWidget::DataAlignWidget()
: QDialog( 0, 0, true )
{
  QMessageBox::critical( 0, "DataAlignWidget", 
                            "Default constructor must not be invoked."
                            "\nThis object will fail."                 );
}


DataAlignWidget::DataAlignWidget( const DataAlignWidget& )
: QDialog( 0, 0, true )
{
  QMessageBox::critical( 0, "DataAlignWidget", 
                            "Program will probably crash."
                            "\nDataAlignWidget copy constructor called.");
}


DataAlignWidget::~DataAlignWidget()
{
}


void DataAlignWidget::resizeEvent( QResizeEvent* qrePtr )
{
  _qvbPtr->resize( qrePtr->size().width(), qrePtr->size().height() );
}


// *****
// SLOTS
// *****


void DataAlignWidget::cancel()
{
}


void DataAlignWidget::apply()
{
}

