/****************************************************************************
** Form implementation generated from reading ui file 'datadisplaybase.ui'
**
** Created: Sun Jan 9 21:19:48 2005
**      by: The User Interface Compiler ($Id: datadisplaybase.cpp,v 1.1 2005/01/25 17:34:56 ostiguy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "datadisplaybase.h"

#include <qvariant.h>
#include <qtable.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a DataDisplayBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
DataDisplayBase::DataDisplayBase( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "DataDisplayBase" );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    DataDisplayBaseLayout = new QHBoxLayout( centralWidget(), 11, 6, "DataDisplayBaseLayout"); 

    DataTable = new QTable( centralWidget(), "DataTable" );
    DataTable->setFrameShape( QTable::GroupBoxPanel );
    DataTable->setFrameShadow( QTable::Sunken );
    DataTable->setLineWidth( 2 );
    DataTable->setNumRows( 8000 );
    DataTable->setNumCols( 5 );
    DataTable->setReadOnly( TRUE );
    DataDisplayBaseLayout->addWidget( DataTable );

    // actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filenew" ) ) );
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "fileopen" ) ) );
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
    fileSaveAsAction = new QAction( this, "fileSaveAsAction" );
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "print" ) ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    viewMergeAction = new QAction( this, "viewMergeAction" );
    viewMergeAction->setToggleAction( TRUE );


    // toolbars
    toolBar = new QToolBar( QString(""), this, DockTop ); 

    fileSaveAction->addTo( toolBar );
    fileSaveAsAction->addTo( toolBar );
    filePrintAction->addTo( toolBar );
    fileExitAction->addTo( toolBar );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new QPopupMenu( this );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    View = new QPopupMenu( this );
    viewMergeAction->addTo( View );
    MenuBar->insertItem( QString(""), View, 2 );

    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( fileSaveAsAction, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( viewMergeAction, SIGNAL( toggled(bool) ), this, SLOT( viewMerge(bool) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DataDisplayBase::~DataDisplayBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DataDisplayBase::languageChange()
{
    setCaption( tr( "Form1" ) );
    fileNewAction->setText( tr( "New" ) );
    fileNewAction->setMenuText( tr( "&New" ) );
    fileNewAction->setAccel( tr( "Ctrl+N" ) );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileSaveAction->setText( tr( "Save" ) );
    fileSaveAction->setMenuText( tr( "&Save" ) );
    fileSaveAction->setAccel( tr( "Ctrl+S" ) );
    fileSaveAsAction->setText( tr( "Save As" ) );
    fileSaveAsAction->setMenuText( tr( "Save &As..." ) );
    fileSaveAsAction->setAccel( QString::null );
    filePrintAction->setText( tr( "Print" ) );
    filePrintAction->setMenuText( tr( "&Print..." ) );
    filePrintAction->setAccel( tr( "Ctrl+P" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    viewMergeAction->setText( tr( "Common Abscissae" ) );
    toolBar->setLabel( tr( "Tools" ) );
    if (MenuBar->findItem(1))
        MenuBar->findItem(1)->setText( tr( "&File" ) );
    if (MenuBar->findItem(2))
        MenuBar->findItem(2)->setText( tr( "View" ) );
}

void DataDisplayBase::fileNew()
{
    qWarning( "DataDisplayBase::fileNew(): Not implemented yet" );
}

void DataDisplayBase::fileOpen()
{
    qWarning( "DataDisplayBase::fileOpen(): Not implemented yet" );
}

void DataDisplayBase::fileSave()
{
    qWarning( "DataDisplayBase::fileSave(): Not implemented yet" );
}

void DataDisplayBase::fileSaveAs()
{
    qWarning( "DataDisplayBase::fileSaveAs(): Not implemented yet" );
}

void DataDisplayBase::filePrint()
{
    qWarning( "DataDisplayBase::filePrint(): Not implemented yet" );
}

void DataDisplayBase::fileExit()
{
    qWarning( "DataDisplayBase::fileExit(): Not implemented yet" );
}

void DataDisplayBase::viewMerge(bool)
{
    qWarning( "DataDisplayBase::viewMerge(bool): Not implemented yet" );
}

