/****************************************************************************
** Form implementation generated from reading ui file 'tunedialogbase.ui'
**
** Created: Sun Jan 9 21:19:54 2005
**      by: The User Interface Compiler ($Id: tunedialogbase.cpp,v 1.1 2005/01/25 17:34:56 ostiguy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "tunedialogbase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a TuneDialogBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TuneDialogBase::TuneDialogBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "TuneDialogBase" );
    setSizeGripEnabled( TRUE );

    QWidget* privateLayoutWidget = new QWidget( this, "Layout1" );
    privateLayoutWidget->setGeometry( QRect( 10, 100, 340, 27 ) );
    Layout1 = new QHBoxLayout( privateLayoutWidget, 0, 6, "Layout1"); 
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( privateLayoutWidget, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setGeometry( QRect( 160, 10, 340, 34 ) );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setGeometry( QRect( 160, 50, 140, 33 ) );

    vTuneLineEdit = new QLineEdit( this, "vTuneLineEdit" );
    vTuneLineEdit->setGeometry( QRect( 20, 60, 120, 20 ) );
    vTuneLineEdit->setReadOnly( TRUE );

    hTuneLineEdit = new QLineEdit( this, "hTuneLineEdit" );
    hTuneLineEdit->setGeometry( QRect( 20, 20, 120, 20 ) );
    hTuneLineEdit->setReadOnly( TRUE );
    languageChange();
    resize( QSize(358, 143).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
TuneDialogBase::~TuneDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TuneDialogBase::languageChange()
{
    setCaption( tr( "EigenTunes" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    textLabel1->setText( tr( "Horizontal EigenTune" ) );
    textLabel2->setText( tr( "Vertical EigenTune" ) );
}

