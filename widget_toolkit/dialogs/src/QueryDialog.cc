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
    queryPtr_[i] = 0;
  }

  // Construct the operations buttons
    opsBoxPtr_ = new QVBox(this);
    QPushButton* enterPtr = new QPushButton( "Enter", opsBoxPtr_ );
    // enterPtr->setMinimumWidth( QApplication::desktop()->width()/9 );
    enterPtr->setMaximumWidth( 70 );
    connect( enterPtr, SIGNAL(pressed()),
             this,     SLOT(opEnter()) );
    QPushButton* andPtr   = new QPushButton( "AND",   opsBoxPtr_ );
    andPtr->setMaximumWidth( 70 );
    connect( andPtr,   SIGNAL(pressed()),
             this,     SLOT(opAnd()) );
    QPushButton* orPtr    = new QPushButton( "OR",    opsBoxPtr_ );
    orPtr->setMaximumWidth( 70 );
    connect( orPtr,    SIGNAL(pressed()),
             this,     SLOT(opOr()) );
    QPushButton* notPtr   = new QPushButton( "NOT",   opsBoxPtr_ );
    notPtr->setMaximumWidth( 70 );
    connect( notPtr,   SIGNAL(pressed()),
             this,     SLOT(opNot()) );

    QPushButton* applyPtr = new QPushButton( "Apply",   opsBoxPtr_ );
    applyPtr->setMaximumWidth( 70 );
    connect( applyPtr, SIGNAL(pressed()),
             this,     SLOT( doFilter()) );
    QPushButton* closePtr = new QPushButton( "Close",   opsBoxPtr_ );
    closePtr->setMaximumWidth( 70 );
    connect( closePtr, SIGNAL(pressed()),
             this,     SLOT(close()) );

    QWidget* opsFiller = new QWidget( opsBoxPtr_ );
    opsFiller->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );

  // REMOVE: opsBoxPtr->setCaption( "Ops" );
  opsBoxPtr_->setMargin(5);
  opsBoxPtr_->setSpacing(3);
  opsBoxPtr_->adjustSize();


  // Construct the Tab Dialog
   tabWidgetPtr_ = new QTabWidget(this);

   typeTabPtr_ = new QListBox( tabWidgetPtr_ );
   typeTabPtr_->setMinimumWidth( QApplication::desktop()->width()/4 );
   typeTabPtr_->insertItem( "BBLens"               );
   typeTabPtr_->insertItem( "CF_rbend"             );
   typeTabPtr_->insertItem( "CF_sbend"             );
   typeTabPtr_->insertItem( "HPinger"              );
   typeTabPtr_->insertItem( "Pinger"               );
   typeTabPtr_->insertItem( "Slot"                 );
   typeTabPtr_->insertItem( "VPinger"              );
   typeTabPtr_->insertItem( "combinedFunction"     );
   typeTabPtr_->insertItem( "drift"                );
   typeTabPtr_->insertItem( "hkick"                );
   typeTabPtr_->insertItem( "hmonitor"             );
   typeTabPtr_->insertItem( "kick"                 );
   typeTabPtr_->insertItem( "marker"               );
   typeTabPtr_->insertItem( "monitor"              );
   typeTabPtr_->insertItem( "octupole"             );
   typeTabPtr_->insertItem( "quadrupole"           );
   typeTabPtr_->insertItem( "rbend"                );
   typeTabPtr_->insertItem( "rfcavity"             );
   typeTabPtr_->insertItem( "sbend"                );
   typeTabPtr_->insertItem( "sector"               );
   typeTabPtr_->insertItem( "sextupole"            );
   typeTabPtr_->insertItem( "srot"                 );
   typeTabPtr_->insertItem( "thin12pole"           );
   typeTabPtr_->insertItem( "thin14pole"           );
   typeTabPtr_->insertItem( "thin16pole"           );
   typeTabPtr_->insertItem( "thin18pole"           );
   typeTabPtr_->insertItem( "thinDecapole"         );
   typeTabPtr_->insertItem( "thinLamb"             );
   typeTabPtr_->insertItem( "thinMultipole"        );
   typeTabPtr_->insertItem( "thinOctupole"         );
   typeTabPtr_->insertItem( "thinQuad"             );
   typeTabPtr_->insertItem( "thinSeptum"           );
   typeTabPtr_->insertItem( "thinSextupole"        );
   typeTabPtr_->insertItem( "thinrfcavity"         );
   typeTabPtr_->insertItem( "vkick"                );
   typeTabPtr_->insertItem( "vmonitor"             );
   typeTabPtr_->setSelected( 0, true );
   tabWidgetPtr_->addTab( typeTabPtr_, "Type" );

   QVBox* nameTab = new QVBox( tabWidgetPtr_ );
     QString nameString; // ??? necessary???
     nameEdt_ = new QLineEdit( nameString, nameTab );
     nameEdt_->setFixedHeight( 20 );
     new QLabel( nameTab );
     nameTab->setMargin(5);
     nameTab->setSpacing(3);
     nameTab->adjustSize();
     tabWidgetPtr_->addTab( nameTab, "Name" );

   QVBox* lengthTab = new QVBox( tabWidgetPtr_ );
     new QLabel( "Length is ", lengthTab );
       lengthBtnGrp_ = new QHButtonGroup( lengthTab );
       lengthBtnGrp_->setExclusive( true );
         new QRadioButton( "<", lengthBtnGrp_ );
       ( new QRadioButton( "=", lengthBtnGrp_ ) )->setChecked(true);
         new QRadioButton( ">", lengthBtnGrp_ );
       // QRadioButton* ltButton = new QRadioButton( "<", lengthBtnGrp_ );
       // QRadioButton* eqButton = new QRadioButton( "=", lengthBtnGrp_ );
       // QRadioButton* gtButton = new QRadioButton( ">", lengthBtnGrp_ );
     QString lengthString; // ??? necessary ???
     lengthEdt_ = new QLineEdit( lengthString, lengthTab );
     lengthEdt_->setFixedHeight( 20 );
     new QLabel( lengthTab );
     lengthTab->setMargin(5);
     lengthTab->setSpacing(3);
     lengthTab->adjustSize();
     tabWidgetPtr_->addTab( lengthTab, "Length" );

   QVBox* strengthTab = new QVBox( tabWidgetPtr_ );
     new QLabel( "Strength is ", strengthTab );
      strengthBtnGrp_ = new QHButtonGroup( strengthTab );
        strengthBtnGrp_->setExclusive( true );
         new QRadioButton( "<", strengthBtnGrp_ );
       ( new QRadioButton( "=", strengthBtnGrp_ ) )->setChecked(true);
         new QRadioButton( ">", strengthBtnGrp_ );
       // QRadioButton* ltButton = new QRadioButton( "<", strengthBtnGrp_ );
       // QRadioButton* eqButton = new QRadioButton( "=", strengthBtnGrp_ );
       // QRadioButton* gtButton = new QRadioButton( ">", strengthBtnGrp_ );
     QString strengthString; // ??? necessary ???
      strengthEdt_ = new QLineEdit( strengthString, strengthTab );
      strengthEdt_->setFixedHeight( 20 );
     new QLabel( strengthTab );
     strengthTab->setMargin(5);
     strengthTab->setSpacing(3);
     strengthTab->adjustSize();
     tabWidgetPtr_->addTab( strengthTab, "Strength" );


     tabWidgetPtr_->adjustSize();

   // Construct the query display.
    displayPtr_ = new QTextEdit(this);
    displayPtr_->setReadOnly( true );
    displayPtr_->setMinimumWidth(256);


   QWidget* fillerPtr = new QWidget( this );
   fillerPtr->setSizePolicy( QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored) );
   
   // Install the widgets into the grid
   qgl->addMultiCellWidget( tabWidgetPtr_, 0, 1, 0, 0 );
   qgl->addWidget( opsBoxPtr_, 0, 1 );
   qgl->addWidget( fillerPtr, 0, 2 );
   qgl->addMultiCellWidget( displayPtr_, 1, 1, 1, 2 );

   updateDisplay();

   adjustSize();
   setCaption( "Beamline Query Dialog" );
}


QueryDialog::~QueryDialog()
{
  for( int i = 0; i < 4; i++ ) {
    if( 0 != queryPtr_[i] )     { delete queryPtr_[i]; }
  }
  if( tabWidgetPtr_ ) { delete  tabWidgetPtr_; }
  if( opsBoxPtr_    ) { delete  opsBoxPtr_;    }
  if( displayPtr_   ) { delete  displayPtr_;   }
}


const BoolNode& QueryDialog::query()
{
  static BoolNullNode nothing;

  if( !queryPtr_[0] ) {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line " << __LINE__
         << "\n*** WARNING *** No query has been constructed. Your program"
            "\n*** WARNING *** will probably abort soon."
         << endl;
    return nothing;
  }

  return *( queryPtr_[0]);
}



// Slots ...
void QueryDialog::opEnter()
{
  static QString typeLabel( "Type" );
  static QString nameLabel( "Name" );
  static QString lengthLabel( "Length" );
  static QString strengthLabel( "Strength" );

  static bool ok = false;

  // Shift
  if( 0 != queryPtr_[3] ) { delete queryPtr_[3]; }
  for( int i = 3; i >=1; i-- ) {
    queryPtr_[i] = queryPtr_[i-1];
  }
  queryPtr_[0] = 0;

  // Process tabs
  QString filterName( tabWidgetPtr_->tabLabel( tabWidgetPtr_->currentPage() ) );

  if( filterName == typeLabel ) {
    queryPtr_[0] = new TypeNode( ( typeTabPtr_->selectedItem()->text()).ascii() );
    // Happily, the text is copied.
  }

  else if( filterName == nameLabel ) {
    queryPtr_[0] = new NameNode( nameEdt_->text().ascii() );
    // Happily, the text is copied.
  }

  else if( filterName == lengthLabel ) {
    double l = lengthEdt_->text().toDouble(&ok);
    if(ok) {
      if(  lengthBtnGrp_->selected()->text() == "=" ) 
      { queryPtr_[0] = new LengthEqNode( l ); }
      else if(  lengthBtnGrp_->selected()->text() == "<" ) 
      { queryPtr_[0] = new LengthLtNode( l ); }
      else if(  lengthBtnGrp_->selected()->text() == ">" ) 
      { queryPtr_[0] = new LengthGtNode( l ); }
      else 
      { QMessageBox::information( 0, "QueryDialog::opEnter()",
            "The impossible has occurred!!" ); 
      }
    }
  }

  else if( filterName == strengthLabel ) {
    double s = strengthEdt_->text().toDouble(&ok);
    if(ok) {
      if(  strengthBtnGrp_->selected()->text() == "=" ) 
      { queryPtr_[0] = new StrengthEqNode( s ); }
      else if(  strengthBtnGrp_->selected()->text() == "<" ) 
      { queryPtr_[0] = new StrengthLtNode( s ); }
      else if(  strengthBtnGrp_->selected()->text() == ">" ) 
      { queryPtr_[0] = new StrengthGtNode( s ); }
      else 
      { QMessageBox::information( 0, "QueryDialog::opEnter()",
            "The impossible has occurred!!!" ); 
      }
    }
  }

  else {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** Unrecognized tab label: " << filterName
         << endl;
  }

  updateDisplay();
}


void QueryDialog::opAnd()
{
  static BoolNode* temp;
  static int i;
  if( ( 0 != queryPtr_[0] ) && ( 0 != queryPtr_[1] ) ) {
    temp = new AndNode( (queryPtr_[0]), (queryPtr_[1]) );
    queryPtr_[0] = temp;
    for( i = 1; i <=2; i++ ) {
      queryPtr_[i] = queryPtr_[i+1];
    }
  }

  updateDisplay();

}


void QueryDialog::opOr()
{
  static BoolNode* temp;
  static int i;
  if( ( 0 != queryPtr_[0] ) && ( 0 != queryPtr_[1] ) ) {
    temp = new OrNode( queryPtr_[0], queryPtr_[1] );
    queryPtr_[0] = temp;
    for( i = 1; i <=2; i++ ) {
      queryPtr_[i] = queryPtr_[i+1];
    }
  }

  updateDisplay();

}


void QueryDialog::opNot()
{
  static BoolNode* temp;
  if( queryPtr_[0] ) {
    temp = new NotNode( queryPtr_[0] );
    queryPtr_[0] = temp;
  }

   updateDisplay();
}


void QueryDialog::updateDisplay()
{
  if( displayPtr_ ) {
    ostringstream writer;
    for( int i = 0; i < 4; i++ ) {
      writer << "Q." << i << ":  ";
      if( 0 != queryPtr_[i] ) { queryPtr_[i]->writeTo( writer ); }
      writer << "\n";
    }
     displayPtr_->setText( QString(writer.str().c_str() ) );
  }
}


void QueryDialog::doFilter()
{
  if( 0 != queryPtr_[0] ) { emit useThis( *(queryPtr_[0]) ); }
}


