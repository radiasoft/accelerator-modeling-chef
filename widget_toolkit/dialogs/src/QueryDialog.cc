/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      QueryDialog.cc
******  Version:   3.0
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

#include <iostream>
#include <sstream>

#include <qapplication.h>
#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qrect.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#include <qvbox.h>
#include <qlayout.h>

#include "BeamlineExpressionTree.h"
#include "QueryDialog.h"

using namespace std;

QueryDialog::QueryDialog( QWidget* p, const char* n, WFlags f )
: QWidget(p,n,f)
{
  QGridLayout* qgl = new QGridLayout( this, 2, 3, 5 );

  for( int i = 0; i < 4; i++ ) {
    _queryPtr[i] = 0;
  }

  // Construct the operations buttons
  _opsBoxPtr = new QVBox(this);
    QPushButton* enterPtr = new QPushButton( "Enter", _opsBoxPtr );
    // enterPtr->setMinimumWidth( QApplication::desktop()->width()/9 );
    enterPtr->setMaximumWidth( 70 );
    connect( enterPtr, SIGNAL(pressed()),
             this,     SLOT(_opEnter()) );
    QPushButton* andPtr   = new QPushButton( "AND",   _opsBoxPtr );
    andPtr->setMaximumWidth( 70 );
    connect( andPtr,   SIGNAL(pressed()),
             this,     SLOT(_opAnd()) );
    QPushButton* orPtr    = new QPushButton( "OR",    _opsBoxPtr );
    orPtr->setMaximumWidth( 70 );
    connect( orPtr,    SIGNAL(pressed()),
             this,     SLOT(_opOr()) );
    QPushButton* notPtr   = new QPushButton( "NOT",   _opsBoxPtr );
    notPtr->setMaximumWidth( 70 );
    connect( notPtr,   SIGNAL(pressed()),
             this,     SLOT(_opNot()) );

    QPushButton* applyPtr = new QPushButton( "Apply",   _opsBoxPtr );
    applyPtr->setMaximumWidth( 70 );
    connect( applyPtr, SIGNAL(pressed()),
             this,     SLOT(_doFilter()) );
    QPushButton* closePtr = new QPushButton( "Close",   _opsBoxPtr );
    closePtr->setMaximumWidth( 70 );
    connect( closePtr, SIGNAL(pressed()),
             this,     SLOT(close()) );

    QWidget* opsFiller = new QWidget( _opsBoxPtr );
    opsFiller->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );

  // REMOVE: _opsBoxPtr->setCaption( "Ops" );
  _opsBoxPtr->setMargin(5);
  _opsBoxPtr->setSpacing(3);
  _opsBoxPtr->adjustSize();


  // Construct the Tab Dialog
   _tabWidgetPtr = new QTabWidget(this);

   _typeTabPtr = new QListBox( _tabWidgetPtr );
   _typeTabPtr->setMinimumWidth( QApplication::desktop()->width()/4 );
   _typeTabPtr->insertItem( "BBLens"               );
   _typeTabPtr->insertItem( "CF_rbend"             );
   _typeTabPtr->insertItem( "CF_sbend"             );
   _typeTabPtr->insertItem( "HPinger"              );
   _typeTabPtr->insertItem( "Pinger"               );
   _typeTabPtr->insertItem( "Slot"                 );
   _typeTabPtr->insertItem( "VPinger"              );
   _typeTabPtr->insertItem( "combinedFunction"     );
   _typeTabPtr->insertItem( "drift"                );
   _typeTabPtr->insertItem( "hkick"                );
   _typeTabPtr->insertItem( "hmonitor"             );
   _typeTabPtr->insertItem( "kick"                 );
   _typeTabPtr->insertItem( "marker"               );
   _typeTabPtr->insertItem( "monitor"              );
   _typeTabPtr->insertItem( "octupole"             );
   _typeTabPtr->insertItem( "quadrupole"           );
   _typeTabPtr->insertItem( "rbend"                );
   _typeTabPtr->insertItem( "rfcavity"             );
   _typeTabPtr->insertItem( "sbend"                );
   _typeTabPtr->insertItem( "sector"               );
   _typeTabPtr->insertItem( "sextupole"            );
   _typeTabPtr->insertItem( "srot"                 );
   _typeTabPtr->insertItem( "thin12pole"           );
   _typeTabPtr->insertItem( "thin14pole"           );
   _typeTabPtr->insertItem( "thin16pole"           );
   _typeTabPtr->insertItem( "thin18pole"           );
   _typeTabPtr->insertItem( "thinDecapole"         );
   _typeTabPtr->insertItem( "thinLamb"             );
   _typeTabPtr->insertItem( "thinMultipole"        );
   _typeTabPtr->insertItem( "thinOctupole"         );
   _typeTabPtr->insertItem( "thinQuad"             );
   _typeTabPtr->insertItem( "thinSeptum"           );
   _typeTabPtr->insertItem( "thinSextupole"        );
   _typeTabPtr->insertItem( "thinrfcavity"         );
   _typeTabPtr->insertItem( "vkick"                );
   _typeTabPtr->insertItem( "vmonitor"             );
   _typeTabPtr->setSelected( 0, true );
   _tabWidgetPtr->addTab( _typeTabPtr, "Type" );

   QVBox* nameTab = new QVBox( _tabWidgetPtr );
     QString nameString; // ??? necessary???
     _nameEdt = new QLineEdit( nameString, nameTab );
     _nameEdt->setFixedHeight( 20 );
     new QLabel( nameTab );
     nameTab->setMargin(5);
     nameTab->setSpacing(3);
     nameTab->adjustSize();
   _tabWidgetPtr->addTab( nameTab, "Name" );

   QVBox* lengthTab = new QVBox( _tabWidgetPtr );
     new QLabel( "Length is ", lengthTab );
     _lengthBtnGrp = new QHButtonGroup( lengthTab );
       _lengthBtnGrp->setExclusive( true );
         new QRadioButton( "<", _lengthBtnGrp );
       ( new QRadioButton( "=", _lengthBtnGrp ) )->setChecked(true);
         new QRadioButton( ">", _lengthBtnGrp );
       // QRadioButton* ltButton = new QRadioButton( "<", _lengthBtnGrp );
       // QRadioButton* eqButton = new QRadioButton( "=", _lengthBtnGrp );
       // QRadioButton* gtButton = new QRadioButton( ">", _lengthBtnGrp );
     QString lengthString; // ??? necessary ???
     _lengthEdt = new QLineEdit( lengthString, lengthTab );
       _lengthEdt->setFixedHeight( 20 );
     new QLabel( lengthTab );
     lengthTab->setMargin(5);
     lengthTab->setSpacing(3);
     lengthTab->adjustSize();
   _tabWidgetPtr->addTab( lengthTab, "Length" );

   QVBox* strengthTab = new QVBox( _tabWidgetPtr );
     new QLabel( "Strength is ", strengthTab );
     _strengthBtnGrp = new QHButtonGroup( strengthTab );
       _strengthBtnGrp->setExclusive( true );
         new QRadioButton( "<", _strengthBtnGrp );
       ( new QRadioButton( "=", _strengthBtnGrp ) )->setChecked(true);
         new QRadioButton( ">", _strengthBtnGrp );
       // QRadioButton* ltButton = new QRadioButton( "<", _strengthBtnGrp );
       // QRadioButton* eqButton = new QRadioButton( "=", _strengthBtnGrp );
       // QRadioButton* gtButton = new QRadioButton( ">", _strengthBtnGrp );
     QString strengthString; // ??? necessary ???
     _strengthEdt = new QLineEdit( strengthString, strengthTab );
       _strengthEdt->setFixedHeight( 20 );
     new QLabel( strengthTab );
     strengthTab->setMargin(5);
     strengthTab->setSpacing(3);
     strengthTab->adjustSize();
   _tabWidgetPtr->addTab( strengthTab, "Strength" );

   // REMOVE:   // A null string makes the OK button invisible.
   // REMOVE:   _tabWidgetPtr->setOkButton( QString() );

   // REMOVE:   _tabWidgetPtr->setApplyButton( "Select" );
   // REMOVE:   connect( _tabWidgetPtr, SIGNAL(applyButtonPressed()),
   // REMOVE:            this,          SLOT(_doFilter()) );
   // REMOVE:   _tabWidgetPtr->setCancelButton( "Close" );


   _tabWidgetPtr->adjustSize();
   // REMOVE: _tabWidgetPtr->setCaption( "Filters" );


   // Construct the query display.
   _displayPtr = new QTextEdit(this);
   _displayPtr->setReadOnly( true );
   // REMOVE: _displayPtr->setFixedWidth(512);
   _displayPtr->setMinimumWidth(256);

   // REMOVE: _displayPtr->adjustSize();
   // REMOVE: _displayPtr->setCaption( "Query buffers" );

   // REMOVE:   // Arrange the widgets on the screen and show them
   // REMOVE:   QRect geom_td = _tabDialogPtr->geometry();
   // REMOVE:   QRect geom_ob = _opsBoxPtr->geometry();
   // REMOVE:   QRect geom_dp = _displayPtr->geometry();

   // REMOVE:   int dx = geom_td.width() + geom_dp.width();

   // REMOVE:   int x = ( QApplication::desktop()->width()  - dx               ) / 2;
   // REMOVE:   int y = ( QApplication::desktop()->height() - geom_td.height() ) / 2;
   // REMOVE:   geom_td.moveTopLeft( QPoint(x,y) );
   // REMOVE:   _tabDialogPtr->setGeometry( geom_td );

   // REMOVE:   x += ( geom_td.width() + 10 );
   // REMOVE:   geom_ob.moveTopLeft( QPoint(x,y) );
   // REMOVE:   _opsBoxPtr->setGeometry( geom_ob );

   // REMOVE:   y += ( geom_td.height() - geom_ob.height() );
   // REMOVE:   // y += ( geom_ob.height() + 20 );
   // REMOVE:   geom_dp.moveTopLeft( QPoint(x,y) );
   // REMOVE:   _displayPtr->setGeometry( geom_dp );

   QWidget* fillerPtr = new QWidget( this );
   fillerPtr->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );
   
   // Install the widgets into the grid
   qgl->addMultiCellWidget( _tabWidgetPtr, 0, 1, 0, 0 );
   qgl->addWidget( _opsBoxPtr, 0, 1 );
   qgl->addWidget( fillerPtr, 0, 2 );
   qgl->addMultiCellWidget( _displayPtr, 1, 1, 1, 2 );

   // REMOVE: _tabWidgetPtr->show();
   // REMOVE: _opsBoxPtr->show();
   // REMOVE: _displayPtr->show();

   _updateDisplay();

   // this->setFixedSize( this->childrenRect().width(), 
   //                     this->childrenRect().height() );

   this->adjustSize();
   this->setCaption( "Beamline Query Dialog" );
   // this->show();
}


QueryDialog::~QueryDialog()
{
  for( int i = 0; i < 4; i++ ) {
    if( 0 != _queryPtr[i] )     { _queryPtr[i]->eliminate(); }
  }
  if( 0 != _tabWidgetPtr ) { delete _tabWidgetPtr; }
  if( 0 != _opsBoxPtr    ) { delete _opsBoxPtr;    }
  if( 0 != _displayPtr   ) { delete _displayPtr;   }
}


const BoolNode& QueryDialog::query()
{
  static BoolNullNode nothing;

  if( 0 == _queryPtr[0] ) {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line " << __LINE__
         << "\n*** WARNING *** No query has been constructed. Your program"
            "\n*** WARNING *** will probably abort soon."
         << endl;
    return nothing;
  }

  return *(_queryPtr[0]);
}


// REMOVE:int QueryDialog::exec()
// REMOVE:{ 
// REMOVE:  if( 0 != _tabWidgetPtr ) { return _tabWidgetPtr->exec(); }
// REMOVE:  else { return -1; }
// REMOVE:}


// Slots ...
void QueryDialog::_opEnter()
{
  static QString typeLabel( "Type" );
  static QString nameLabel( "Name" );
  static QString lengthLabel( "Length" );
  static QString strengthLabel( "Strength" );

  static bool ok = false;

  // Shift
  if( 0 != _queryPtr[3] ) { _queryPtr[3]->eliminate(); }
  for( int i = 3; i >=1; i-- ) {
    _queryPtr[i] = _queryPtr[i-1];
  }
  _queryPtr[0] = 0;

  // Process tabs
  QString filterName( _tabWidgetPtr->tabLabel( _tabWidgetPtr->currentPage() ) );

  if( filterName == typeLabel ) {
    _queryPtr[0] = new TypeNode( (_typeTabPtr->selectedItem()->text()).ascii() );
    // Happily, the text is copied.
  }

  else if( filterName == nameLabel ) {
    _queryPtr[0] = new NameNode( _nameEdt->text().ascii() );
    // Happily, the text is copied.
  }

  else if( filterName == lengthLabel ) {
    double l = _lengthEdt->text().toDouble(&ok);
    if(ok) {
      if( _lengthBtnGrp->selected()->text() == "=" ) 
      { _queryPtr[0] = new LengthEqNode( l ); }
      else if( _lengthBtnGrp->selected()->text() == "<" ) 
      { _queryPtr[0] = new LengthLtNode( l ); }
      else if( _lengthBtnGrp->selected()->text() == ">" ) 
      { _queryPtr[0] = new LengthGtNode( l ); }
      else 
      { QMessageBox::information( 0, "QueryDialog::_opEnter()",
            "The impossible has occurred!!" ); 
      }
    }
  }

  else if( filterName == strengthLabel ) {
    double s = _strengthEdt->text().toDouble(&ok);
    if(ok) {
      if( _strengthBtnGrp->selected()->text() == "=" ) 
      { _queryPtr[0] = new StrengthEqNode( s ); }
      else if( _strengthBtnGrp->selected()->text() == "<" ) 
      { _queryPtr[0] = new StrengthLtNode( s ); }
      else if( _strengthBtnGrp->selected()->text() == ">" ) 
      { _queryPtr[0] = new StrengthGtNode( s ); }
      else 
      { QMessageBox::information( 0, "QueryDialog::_opEnter()",
            "The impossible has occurred!!!" ); 
      }
    }
  }

  // REMOVE: else if( filterName == strengthLabel ) {
  // REMOVE:   double s = _strengthEdt->text().toDouble(&ok);
  // REMOVE:   if(ok) { _queryPtr[0] = new StrengthEqNode( s ); }
  // REMOVE: }
  // REMOVE: 
  else {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** Unrecognized tab label: " << filterName
         << endl;
  }

  _updateDisplay();
  // REMOVE: cout << "The filter: ";
  // REMOVE: _queryPtr[0]->writeTo(cout);
  // REMOVE: cout << endl;
}


void QueryDialog::_opAnd()
{
  static BoolNode* temp;
  static int i;
  if( ( 0 != _queryPtr[0] ) && ( 0 != _queryPtr[1] ) ) {
    temp = new AndNode( *(_queryPtr[0]), *(_queryPtr[1]) );
    _queryPtr[0] = temp;
    for( i = 1; i <=2; i++ ) {
      _queryPtr[i] = _queryPtr[i+1];
    }
  }

  _updateDisplay();
  // REMOVE: cout << "The filter: ";
  // REMOVE: _queryPtr[0]->writeTo(cout);
  // REMOVE: cout << endl;
}


void QueryDialog::_opOr()
{
  static BoolNode* temp;
  static int i;
  if( ( 0 != _queryPtr[0] ) && ( 0 != _queryPtr[1] ) ) {
    temp = new OrNode( *(_queryPtr[0]), *(_queryPtr[1]) );
    _queryPtr[0] = temp;
    for( i = 1; i <=2; i++ ) {
      _queryPtr[i] = _queryPtr[i+1];
    }
  }

  _updateDisplay();
  // REMOVE: cout << "The filter: ";
  // REMOVE: _queryPtr[0]->writeTo(cout);
  // REMOVE: cout << endl;
}


void QueryDialog::_opNot()
{
  static BoolNode* temp;
  if( 0 != _queryPtr[0] ) {
    temp = new NotNode( *(_queryPtr[0]) );
    _queryPtr[0] = temp;
  }

  _updateDisplay();
  // REMOVE: cout << "The filter: ";
  // REMOVE: _queryPtr[0]->writeTo(cout);
  // REMOVE: cout << endl;
}


void QueryDialog::_updateDisplay()
{
  if( 0 != _displayPtr ) {
    ostringstream writer;
    for( int i = 0; i < 4; i++ ) {
      writer << "Q." << i << ":  ";
      if( 0 != _queryPtr[i] ) { _queryPtr[i]->writeTo( writer ); }
      writer << "\n";
    }
    _displayPtr->setText( QString(writer.str().c_str() ) );
  }
}


void QueryDialog::_doFilter()
{
  if( 0 != _queryPtr[0] ) { emit _useThis( *(_queryPtr[0]) ); }
}


