/****************************************************************************
** Form implementation generated from reading ui file 'plotpropertiesdialog.ui'
**
** Created: Wed Jul 7 16:21:13 2004
**      by: The User Interface Compiler ($Id: plotpropertiesdialog.cpp,v 1.2 2004/07/16 21:46:14 michelot Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "plotpropertiesdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../plotpropertiesdialog.ui.h"
/*
 *  Constructs a PlotPropertiesDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PlotPropertiesDialog::PlotPropertiesDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PlotPropertiesDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 0, 0 ) );
    setMaximumSize( QSize( 32000, 32000 ) );
    setBaseSize( QSize( 600, 400 ) );
    setPaletteBackgroundColor( QColor( 212, 208, 200 ) );
    setSizeGripEnabled( TRUE );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setGeometry( QRect( 300, 260, 50, 30 ) );
    QFont buttonCancel_font(  buttonCancel->font() );
    buttonCancel->setFont( buttonCancel_font ); 
    buttonCancel->setAutoDefault( TRUE );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setGeometry( QRect( 240, 260, 50, 30 ) );
    QFont buttonOk_font(  buttonOk->font() );
    buttonOk->setFont( buttonOk_font ); 
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );

    apply_pushButton = new QPushButton( this, "apply_pushButton" );
    apply_pushButton->setGeometry( QRect( 180, 260, 50, 30 ) );
    QFont apply_pushButton_font(  apply_pushButton->font() );
    apply_pushButton->setFont( apply_pushButton_font ); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setGeometry( QRect( 10, 260, 50, 30 ) );
    buttonHelp->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 50, buttonHelp->sizePolicy().hasHeightForWidth() ) );
    QFont buttonHelp_font(  buttonHelp->font() );
    buttonHelp->setFont( buttonHelp_font ); 
    buttonHelp->setAutoDefault( TRUE );

    tabWidget = new QTabWidget( this, "tabWidget" );
    tabWidget->setGeometry( QRect( 10, 10, 340, 240 ) );
    tabWidget->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, tabWidget->sizePolicy().hasHeightForWidth() ) );
    tabWidget->setPaletteBackgroundColor( QColor( 212, 208, 200 ) );
    QPalette pal;
    QColorGroup cg;
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 223, 223, 223) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 212, 208, 200) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    cg.setColor( QColorGroup::Link, black );
    cg.setColor( QColorGroup::LinkVisited, black );
    pal.setActive( cg );
    cg.setColor( QColorGroup::Foreground, black );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, black );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, black );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 212, 208, 200) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    cg.setColor( QColorGroup::Link, QColor( 0, 0, 255) );
    cg.setColor( QColorGroup::LinkVisited, QColor( 255, 0, 255) );
    pal.setInactive( cg );
    cg.setColor( QColorGroup::Foreground, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Button, QColor( 192, 192, 192) );
    cg.setColor( QColorGroup::Light, white );
    cg.setColor( QColorGroup::Midlight, QColor( 220, 220, 220) );
    cg.setColor( QColorGroup::Dark, QColor( 96, 96, 96) );
    cg.setColor( QColorGroup::Mid, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Text, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::BrightText, white );
    cg.setColor( QColorGroup::ButtonText, QColor( 128, 128, 128) );
    cg.setColor( QColorGroup::Base, white );
    cg.setColor( QColorGroup::Background, QColor( 212, 208, 200) );
    cg.setColor( QColorGroup::Shadow, black );
    cg.setColor( QColorGroup::Highlight, QColor( 0, 0, 128) );
    cg.setColor( QColorGroup::HighlightedText, white );
    cg.setColor( QColorGroup::Link, QColor( 0, 0, 255) );
    cg.setColor( QColorGroup::LinkVisited, QColor( 255, 0, 255) );
    pal.setDisabled( cg );
    tabWidget->setPalette( pal );
    QFont tabWidget_font(  tabWidget->font() );
    tabWidget->setFont( tabWidget_font ); 

    Widget8 = new QWidget( tabWidget, "Widget8" );

    checkBox3_2 = new QCheckBox( Widget8, "checkBox3_2" );
    checkBox3_2->setGeometry( QRect( 20, 50, 130, 20 ) );

    textLabel1_2 = new QLabel( Widget8, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 20, 80, 80, 20 ) );

    comboBox1 = new QComboBox( FALSE, Widget8, "comboBox1" );
    comboBox1->setGeometry( QRect( 110, 80, 90, 20 ) );

    textLabel2_2 = new QLabel( Widget8, "textLabel2_2" );
    textLabel2_2->setGeometry( QRect( 20, 110, 60, 20 ) );

    comboBox2 = new QComboBox( FALSE, Widget8, "comboBox2" );
    comboBox2->setGeometry( QRect( 110, 110, 90, 20 ) );

    comboBox3 = new QComboBox( FALSE, Widget8, "comboBox3" );
    comboBox3->setGeometry( QRect( 110, 140, 92, 20 ) );

    textLabel1_3 = new QLabel( Widget8, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 20, 140, 60, 20 ) );

    checkBox_gridLines = new QCheckBox( Widget8, "checkBox_gridLines" );
    checkBox_gridLines->setGeometry( QRect( 20, 20, 120, 20 ) );
    QFont checkBox_gridLines_font(  checkBox_gridLines->font() );
    checkBox_gridLines->setFont( checkBox_gridLines_font ); 
    tabWidget->insertTab( Widget8, QString("") );

    TabPage = new QWidget( tabWidget, "TabPage" );

    textLabel1 = new QLabel( TabPage, "textLabel1" );
    textLabel1->setGeometry( QRect( 10, 30, 40, 20 ) );

    lineEdit1 = new QLineEdit( TabPage, "lineEdit1" );
    lineEdit1->setGeometry( QRect( 60, 30, 60, 20 ) );

    lineEdit3 = new QLineEdit( TabPage, "lineEdit3" );
    lineEdit3->setGeometry( QRect( 60, 60, 60, 20 ) );

    textLabel2 = new QLabel( TabPage, "textLabel2" );
    textLabel2->setGeometry( QRect( 10, 60, 40, 20 ) );

    textLabel5 = new QLabel( TabPage, "textLabel5" );
    textLabel5->setGeometry( QRect( 10, 0, 110, 20 ) );
    QFont textLabel5_font(  textLabel5->font() );
    textLabel5_font.setBold( TRUE );
    textLabel5->setFont( textLabel5_font ); 

    textLabel6 = new QLabel( TabPage, "textLabel6" );
    textLabel6->setGeometry( QRect( 10, 110, 57, 20 ) );

    lineEdit5 = new QLineEdit( TabPage, "lineEdit5" );
    lineEdit5->setGeometry( QRect( 60, 110, 60, 20 ) );

    textLabel3 = new QLabel( TabPage, "textLabel3" );
    textLabel3->setGeometry( QRect( 160, 30, 57, 20 ) );

    textLabel4 = new QLabel( TabPage, "textLabel4" );
    textLabel4->setGeometry( QRect( 160, 60, 57, 20 ) );

    lineEdit6 = new QLineEdit( TabPage, "lineEdit6" );
    lineEdit6->setGeometry( QRect( 60, 140, 60, 20 ) );

    textLabel7 = new QLabel( TabPage, "textLabel7" );
    textLabel7->setGeometry( QRect( 10, 140, 40, 20 ) );

    textLabel10 = new QLabel( TabPage, "textLabel10" );
    textLabel10->setGeometry( QRect( 10, 90, 80, 16 ) );
    QFont textLabel10_font(  textLabel10->font() );
    textLabel10_font.setBold( TRUE );
    textLabel10->setFont( textLabel10_font ); 

    textLabel5_2 = new QLabel( TabPage, "textLabel5_2" );
    textLabel5_2->setGeometry( QRect( 160, 0, 90, 20 ) );
    QFont textLabel5_2_font(  textLabel5_2->font() );
    textLabel5_2_font.setBold( TRUE );
    textLabel5_2->setFont( textLabel5_2_font ); 

    textLabel11 = new QLabel( TabPage, "textLabel11" );
    textLabel11->setGeometry( QRect( 160, 90, 100, 20 ) );
    QFont textLabel11_font(  textLabel11->font() );
    textLabel11_font.setBold( TRUE );
    textLabel11->setFont( textLabel11_font ); 

    textLabel9 = new QLabel( TabPage, "textLabel9" );
    textLabel9->setGeometry( QRect( 160, 140, 40, 20 ) );

    lineEdit8 = new QLineEdit( TabPage, "lineEdit8" );
    lineEdit8->setGeometry( QRect( 200, 140, 60, 20 ) );

    textLabel8 = new QLabel( TabPage, "textLabel8" );
    textLabel8->setGeometry( QRect( 160, 110, 32, 20 ) );

    lineEdit7 = new QLineEdit( TabPage, "lineEdit7" );
    lineEdit7->setGeometry( QRect( 200, 110, 60, 20 ) );

    lineEdit4 = new QLineEdit( TabPage, "lineEdit4" );
    lineEdit4->setGeometry( QRect( 200, 60, 60, 20 ) );

    lineEdit2 = new QLineEdit( TabPage, "lineEdit2" );
    lineEdit2->setGeometry( QRect( 200, 30, 60, 20 ) );

    checkBox3 = new QCheckBox( TabPage, "checkBox3" );
    checkBox3->setGeometry( QRect( 10, 190, 84, 20 ) );

    checkBox4 = new QCheckBox( TabPage, "checkBox4" );
    checkBox4->setGeometry( QRect( 10, 170, 85, 20 ) );

    checkBox6 = new QCheckBox( TabPage, "checkBox6" );
    checkBox6->setGeometry( QRect( 160, 190, 85, 20 ) );

    checkBox5 = new QCheckBox( TabPage, "checkBox5" );
    checkBox5->setGeometry( QRect( 160, 170, 85, 20 ) );
    tabWidget->insertTab( TabPage, QString("") );

    Widget9 = new QWidget( tabWidget, "Widget9" );

    listBox1 = new QListBox( Widget9, "listBox1" );
    listBox1->setGeometry( QRect( 160, 20, 130, 52 ) );

    textLabel12 = new QLabel( Widget9, "textLabel12" );
    textLabel12->setGeometry( QRect( 10, 30, 130, 20 ) );
    QFont textLabel12_font(  textLabel12->font() );
    textLabel12_font.setBold( TRUE );
    textLabel12->setFont( textLabel12_font ); 
    tabWidget->insertTab( Widget9, QString("") );

    TabPage_2 = new QWidget( tabWidget, "TabPage_2" );
    tabWidget->insertTab( TabPage_2, QString("") );
    languageChange();
    resize( QSize(366, 298).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( checkBox_gridLines, SIGNAL( stateChanged(int) ), this, SLOT( checkBox_gridLines_stateChanged(int) ) );
    connect( apply_pushButton, SIGNAL( clicked() ), this, SLOT( plotPropertiesApply() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PlotPropertiesDialog::~PlotPropertiesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PlotPropertiesDialog::languageChange()
{
    setCaption( tr( "Plot Properties" ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    apply_pushButton->setText( tr( "Apply" ) );
    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    checkBox3_2->setText( tr( "Show Data Points" ) );
    textLabel1_2->setText( tr( "Line thickness" ) );
    textLabel2_2->setText( tr( "Line Style" ) );
    textLabel1_3->setText( tr( "Symbols" ) );
    checkBox_gridLines->setText( tr( "Show Grid Lines" ) );
    tabWidget->changeTab( Widget8, tr( "General" ) );
    textLabel1->setText( tr( "X min" ) );
    textLabel2->setText( tr( "X max" ) );
    textLabel5->setText( tr( "Bottom X-axis" ) );
    textLabel6->setText( tr( "Y min" ) );
    textLabel3->setText( tr( "X min" ) );
    textLabel4->setText( tr( "X max" ) );
    textLabel7->setText( tr( "Y max" ) );
    textLabel10->setText( tr( "Left Y-axis" ) );
    textLabel5_2->setText( tr( "Top X-axis" ) );
    textLabel11->setText( tr( "Right Y-axis" ) );
    textLabel9->setText( tr( "Y max" ) );
    textLabel8->setText( tr( "Y min" ) );
    checkBox3->setText( tr( "Auto Scale" ) );
    checkBox4->setText( tr( "Enable" ) );
    checkBox6->setText( tr( "Auto Scale" ) );
    checkBox5->setText( tr( "Enable" ) );
    tabWidget->changeTab( TabPage, tr( "Scaling" ) );
    listBox1->clear();
    listBox1->insertItem( tr( "Red" ) );
    listBox1->insertItem( tr( "Green" ) );
    listBox1->insertItem( tr( "Blue" ) );
    listBox1->insertItem( tr( "Custom" ) );
    textLabel12->setText( tr( "Canvas Background Color" ) );
    tabWidget->changeTab( Widget9, tr( "Colors" ) );
    tabWidget->changeTab( TabPage_2, tr( "Fonts" ) );
}

