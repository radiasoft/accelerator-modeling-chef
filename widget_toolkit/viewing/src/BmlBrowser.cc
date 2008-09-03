/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      BmlBrowser.cc
******                                                                
****** Copyright (c) Universities Research Association, Inc./ Fermilab    
******                 All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******
******                                                                
**************************************************************************
*************************************************************************/


#include <fstream>
#include <typeinfo>
#include <iosetup.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qheader.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include <BmlBrowser.h>
#include <GenericException.h>
#include <bmlfactory/MAD8Factory.h>
#include <BeamlineExpressionTree.h>
#include <beamline/beamline.h>
#include <beamline/beamline_elements.h>
#include <beamline/Alignment.h>


using namespace std;

using FNAL::pcerr;
using FNAL::pcout;

namespace {

struct IsBeamline {
  bool operator()( ElmPtr const& elm) { return elm->isBeamline(); }
}; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlBrowser::BmlBrowser( QWidget *parent, char const* name, WFlags f)
: QListView( parent, name, f ), ConstBmlVisitor()
{
  setSorting(-1);  // prevents sorting
  addColumn( "Name" );
  addColumn( "Type" );
  addColumn( "Azimuth" );
  setTreeStepSize( 20 );
  
  
  setAcceptDrops( false );
  viewport()->setAcceptDrops( false );
  
  setSelectionMode( QListView::Extended );
  setRootIsDecorated( true );

  resize( 400, 400 );

  setCaption( "CHEF: Beamline Browser" );
  setAllColumnsShowFocus( true );

  if ( childCount() == 0 ) emit signalBrowserIsEmpty();

  show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlBrowser::~BmlBrowser()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::slotShowData( QElm* item )
{
  item->elm()->accept(*this); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::contentsMousePressEvent( QMouseEvent* e )
{
  QListView::contentsMousePressEvent(e);

  QPoint p( contentsToViewport( e->pos() ) );
  QListViewItem* item = itemAt( p );

  if (!item) return; 

  QElm* qelm = static_cast<QElm*>(item);

  if( e->button() == Qt::RightButton )  { 
    slotShowData( qelm ); 
  }
  else if( e->button() == Qt::LeftButton )  { 
     emit signalElmLeftClicked( qelm );  
  }
 
  return;
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::resetPixmap( ConstBmlPtr bml )
{
  using namespace boost;

  if( !bml ) {
    QMessageBox::information( 0, "CHEF: WARNING", 
      "Null pointer passed to BmlBrowser::resetPixmap." 
      "\nNo action will be taken." );
    return;
  }

  QElm* node = dynamic_cast<QElm*>( firstChild() );

  bool found = ( bml == dynamic_pointer_cast<beamline>(node->elm()) );

  while( !found ) {
    node  =   dynamic_cast<QElm*>( node->nextSibling() );
    found = ( bml == dynamic_pointer_cast<beamline>(node->elm()) );
  }


  if( !found ) {
    ostringstream uic;
    uic << "An impossibility has occurred!"
           "\nFile " << __FILE__ << " at line " << __LINE__
        << "\ninside BmlBrowser::resetPixmap."
        << "\nCould not find indicated Beamline"
           "\nNo action will be taken.";
    QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }

  if( bml->getLineMode() == beamline::ring )  {
    if( node->isOpen() ) {
        node->setPixmap( 0, QElm::bmrOrangeSymbol() );
    }
    else {
        node->setPixmap( 0, QElm::bmrBlackSymbol() );
    }
  }
  else {
    if( node->isOpen() ) {
        node->setPixmap( 0, QElm::bmlOrangeSymbol() );
    }
    else {
       node->setPixmap( 0, QElm::bmlBlackSymbol()  );
    }
  }

  repaint();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::displayBeamline( BmlPtr bml ) 
{
  using namespace boost;
  if( !bml ) {
    ostringstream uic;
    uic << "Null pointer was passed to BmlBrowser::displayBeamline"
           "\nFile "
        << __FILE__
        << " at line " << __LINE__ 
        << "\nException will be thrown.";
    QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
    throw( GenericException( __FILE__, __LINE__, 
           "void BmlBrowser::displayBeamline( ConstBmlPtr )",
           "Null pointer passed as argument." ) );
    return;
  }

  QElm* root = new QElm( bml, 0.0, this, bml->Name(), bml->Type() );

  if( bml->getLineMode() == beamline::ring )  {
    root->setPixmap( 0, QElm::bmrBlackSymbol() ); 
  }
  else {
    root->setPixmap( 0, QElm::bmlBlackSymbol() );
  }

  beamline::reverse_iterator rbi = bml->rbegin();

  double s  = bml->Length();
  double s2 = s;

  displayLine( root,  bml,  rbi,  s );

  root->setText( 2, QString().setNum(s) + "-" +  QString().setNum(s2) );

       clearSelection();
       setSelected ( root, true );
     setCurrentItem( root );
  ensureItemVisible( root );

  emit signalElmLeftClicked( root );

  if ( childCount() == 1 ) emit signalBrowserIsNotEmpty();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::displayLine( QElm* parent, BmlPtr bml, beamline::reverse_iterator bi, double& s )
{

  using namespace boost;

  ElmPtr q;

  for ( ;  bi != bml->rend(); ++bi )
  {
    q = (*bi);

    if( q->isBeamline() ) {

      BmlPtr qbml =  dynamic_pointer_cast<beamline>(q);  

      QElm* newBml = new QElm( qbml, s, parent, qbml->Name(), qbml->Type());
      newBml->setPixmap( 0, QElm::bmlBlackSymbol() );

      double s2 = s;
      BmlBrowser::displayLine( newBml,  qbml,  qbml->rbegin(), s );

      newBml->setText( 2, QString().setNum(s) + "-" +  QString().setNum(s2) );
    }
    else {

      QElm* newElmnt = new QElm( q, s, parent, q->Name(), q->Type() );
      
      newElmnt->setText( 2, QString().setNum(s) );

      s -= q->Length();  

      if( std::abs(s) <= 1.0e-8 ) { s = 0.0; }

      if(  strstr(q->Type(), "bend") ) {
        newElmnt->setPixmap( 0, QElm::dipoleSymbol() );
      }
      else if(  strstr( q->Type(), "quad")  || 
                strstr( q->Type(), "Quad")     ) {

        if( q->Strength() >= 0.0 ) {
          newElmnt->setPixmap(0, QElm::fquadSymbol() ); 
        }
        else {
          newElmnt->setPixmap(0, QElm::dquadSymbol() ); 
        }
      }
      else if( strstr(q->Type(), "sext") || 
               strstr(q->Type(), "Sext")     ) {
        newElmnt->setPixmap(0, QElm::sextupoleSymbol() ); 
      }
      else if( !strcmp(q->Type(), "drift" ) ) {
        newElmnt->setPixmap(0, QElm::driftSymbol() ); 
      }
      else if( !strcmp(q->Type(), "Slot" ) ) {
        newElmnt->setPixmap(0, QElm::slotSymbol() ); 
      }
      else if( !strcmp(q->Type(), "LinacCavity" ) ) {
        newElmnt->setPixmap(0, QElm::lcavSymbol() ); 
      }
      else {
        newElmnt->setPixmap(0, QElm::elmntSymbol() );
      }
    }
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::removeBeamline( BmlPtr bml )
{

  if( !bml ) return;

  QListViewItem* item = firstChild();

  if (!item) return;  // browser is empty, we are done. 
 
  do { 
    
     QElm* qelm = static_cast<QElm*>(item);
     if ( qelm->elm() == bml ) {
        takeItem(item);
        delete item;
        return;
      }
  } while ( (item = item->nextSibling()) );

  if ( childCount() == 0 ) emit signalBrowserIsEmpty();

}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> BmlBrowser::selectElements( ElmPtr toplevel, BoolNode const&  query )
{

  // *** find and select elements that satisfy query within a specific top-level line.

  std::list<ElmPtr> selection;

  // find the top-level element line 
 
  QListViewItem* node  = firstChild();
  QListViewItem* start = 0;

  while ( node ) {
     QElm* qelm = dynamic_cast<QElm*>(node); 
     if ( qelm->elm() == toplevel ) {
          start = node;
          break;
     }
     node = node->nextSibling();
  }        

  if (!start ) return selection;  

  QListViewItemIterator it( start  );

  do { 

    QElm* qelm = dynamic_cast<QElm*>(*it);

    // Apply the test and, if passed, add element pointer to the list.

    if( query.evaluate( *qelm->elm() ) ) {  
        selection.push_back( qelm->elm() );
        qelm->setSelected(true);
        qelm->repaint();
    }
   

   } while( (*(++it) ));

  repaint();

  return selection;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> BmlBrowser::getSelection() const
{
  // *** returns all selected elements ( beamlines are elements too ! )

  std::list<ElmPtr> selection;

  QListViewItemIterator it( firstChild() );

  do { 

    if( !( (*it)->isSelected() ) ) continue;

    QElm* qelm = dynamic_cast<QElm*>(*it);
  
    selection.push_back( qelm->elm() );

   } while( (*(++it) ));

  return selection;  

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> BmlBrowser::getTopLevelSelection()  const 
{
  // Synopsis: returns only selected top-level beamlines 
 
  std::list<ElmPtr> selection;

  QListViewItem* node = firstChild();

  while ( node ) {
     QElm* qelm = dynamic_cast<QElm*>(node); 
     if (qelm->isSelected() ) { 
       selection.push_back( qelm->elm() );
     }
     node = node->nextSibling();
  }        

  return selection;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::list<ElmPtr> BmlBrowser::getElmSelection()       const
{
  // Synopsis: returns all selected "simple" elements (i.e. excludes beamlines); 

  std::list<ElmPtr> selection = getSelection();

  // remove beamlines from the selection ...  

  std::list<ElmPtr>::iterator it;
  while ( (it = std::find_if( selection.begin(), selection.end(), IsBeamline() )) != selection.end() ) {
    selection.erase(it);
  }

  return selection; 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( beamline const& x )
{
  QWidget* pw = parentWidget()->parentWidget();

  QString cap = QString("Beamline %1" ).arg( x.Name() ); 
  QString msg = QString( "%1 elements. \nReference momentum = %2 GeV/c ").arg( x.countHowManyDeeply() ).arg(x.Momentum() );
 
  QMessageBox::information( pw, cap, msg );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( bmlnElmnt const& x )
{

  QWidget* pw = parentWidget()->parentWidget();
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());
  QString msg = QString("Length = %1; Strength = %2" ).arg(x.Length()).arg(x.Strength());
  QMessageBox::information( pw, cap, msg );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( drift const& x )
{  
  QWidget* pw = parentWidget()->parentWidget();

  QDialog* wpu = new QDialog( pw, 0, false, Qt::WDestructiveClose );
     QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Length [m] = ", qhb1 );
        new QLabel( QString().setNum( x.Length()), qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb2 );
          closeBtn->setDefault( true );
          QObject::connect( closeBtn,  SIGNAL( pressed()),
                   wpu,       SLOT(   close()) );
      qhb2->setMargin(10);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

 std::cout << "wpu parent name = " 
           << wpu->parentWidget()->name() << std::endl; 

  wpu->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( Slot const& x )
{
  
  QWidget* pw = parentWidget()->parentWidget();

  QString xstr, ystr, zstr;
  QString lparen("( ");
  QString rparen(" )");
  QString comma(", ");
  
  QDialog* wpu = new QDialog( pw,0,false,Qt::WDestructiveClose);

    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 5, 3, 5, 10 );

         qgl->addWidget( new QLabel( QString("Upstream"),   qwa ), 0, 1 );
         qgl->addWidget( new QLabel( QString("Downstream"), qwa ), 0, 2 );
         
         qgl->addWidget( new QLabel( QString("Origin   "), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("U   "), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString("V   "), qwa ), 3, 0 );
         qgl->addWidget( new QLabel( QString("W   "), qwa ), 4, 0 );

         xstr.setNum( (x.getInFrame().getOrigin())[0] );
         ystr.setNum( (x.getInFrame().getOrigin())[1] );
         zstr.setNum( (x.getInFrame().getOrigin())[2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 1 );
         xstr.setNum( (x.getOutFrame().getOrigin())[0] );
         ystr.setNum( (x.getOutFrame().getOrigin())[1] );
         zstr.setNum( (x.getOutFrame().getOrigin())[2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 2 );

         xstr.setNum( (x.getInFrame().getxAxis()) [0] );
         ystr.setNum( (x.getInFrame().getxAxis()) [1] );
         zstr.setNum( (x.getInFrame().getxAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 1 );
         xstr.setNum( (x.getInFrame().getyAxis()) [0] );
         ystr.setNum( (x.getInFrame().getyAxis()) [1] );
         zstr.setNum( (x.getInFrame().getyAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 1 );
         xstr.setNum( (x.getInFrame().getzAxis()) [0] );
         ystr.setNum( (x.getInFrame().getzAxis()) [1] );
         zstr.setNum( (x.getInFrame().getzAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 1 );
    
         xstr.setNum( (x.getOutFrame().getxAxis()) [0] );
         ystr.setNum( (x.getOutFrame().getxAxis()) [1] );
         zstr.setNum( (x.getOutFrame().getxAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 2 );
         xstr.setNum( (x.getOutFrame().getyAxis()) [0] );
         ystr.setNum( (x.getOutFrame().getyAxis()) [1] );
         zstr.setNum( (x.getOutFrame().getyAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 2 );
         xstr.setNum( (x.getOutFrame().getzAxis()) [0] );
         ystr.setNum( (x.getOutFrame().getzAxis()) [1] );
         zstr.setNum( (x.getOutFrame().getzAxis()) [2] );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 2 );
    
      qwa->adjustSize();

      QHBox* qhb99 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb99 );
          QObject::connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb99->setMargin(5);
      qhb99->setSpacing(3);
      qhb99->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( sbend const& x )
{
  QWidget* pw = parentWidget()->parentWidget();

  QDialog* wpu = new QDialog( pw,0,false,Qt::WDestructiveClose);
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( QString("Length [m]: "), qhb1 );
        QString stlen;
        stlen.setNum( x.Length() );
        new QLabel( stlen, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        new QLabel( QString("Magnetic field [T]: "), qhb2 );
        QString sts;
        sts.setNum( x.Strength() );
        new QLabel( sts, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb3 );
          QObject::connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, because of the flag Qt::WDestructiveClose.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.
  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( CF_sbend const& x )
{

  QWidget* pw = parentWidget()->parentWidget();

  QMessageBox::information( 
    pw, 
    QString("%1 : %2" ).arg( x.Type() ).arg(x.Name()),
    QString("Length [m]: %1 Bend field [T]: %2 Quad gradient [T/m]: %3 ").arg(x.Length()).arg(x.Strength()).arg( x.getQuadrupole()/x.Length() )
  );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void BmlBrowser::visit( rbend const& x )
{
  QWidget* pw = parentWidget()->parentWidget();

  QDialog* wpu = new QDialog( pw,0,false,Qt::WDestructiveClose);
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 3, 3, 5 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [m]: "), qwa ), 0, 1 );
           QString stlen;
           stlen.setNum( x.Length() );
         qgl->addWidget( new QLabel( stlen, qwa ), 0, 2 );

         qgl->addWidget( new QLabel( QString("Magnetic field"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [T]: "), qwa ), 1, 1 );
           QString sts;
           sts.setNum( x.Strength() );
         qgl->addWidget( new QLabel( sts, qwa ), 1, 2 );

         qgl->addWidget( new QLabel( QString("Nominal entry angle"), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]: "), qwa ), 2, 1 );
           sts.setNum( 1000.0 * x.getPoleFaceAngle() );
         qgl->addWidget( new QLabel( sts, qwa ), 2, 2 );

       qwa->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb4 );
          closeBtn->setDefault( true );
          QObject::connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void BmlBrowser::visit( CF_rbend const& x )
{

  QWidget* pw = parentWidget()->parentWidget();

  QDialog* wpu = new QDialog( pw,0,false,Qt::WDestructiveClose);
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( QString("Field [T]: "), qhb1 );
        QString sts;
        sts.setNum( x.Strength() );
        new QLabel( sts, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        new QLabel( QString("Gradient [T/m]: "), qhb4 );
        QString stg;
        stg.setNum( x.getQuadrupole() / x.Length() );
        new QLabel( stg, qhb4 );
      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        new QLabel( QString("Roll angle [mrad]: "), qhb2 );
        alignmentData ad(x.Alignment());
        QString str;
        str.setNum( 1000.*(ad.roll /*[rad]*/) );
        new QLabel( str, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb3 );
          QObject::connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( quadrupole const& x )
{
  QWidget* pw = parentWidget()->parentWidget();

  QDialog* wpu = new QDialog( pw,0,false,Qt::WDestructiveClose);
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( QString("Gradient [T/m]: "),    qhb1 );
        new QLabel( QString().setNum(x.Strength()), qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        new QLabel( QString("Length: "), qhb2 );
        new QLabel( QString().setNum( x.Length() ), qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        new QLabel( QString("Roll angle [mrad]: "), qhb3 );
        alignmentData ad(x.Alignment());
        new QLabel( QString().setNum( 1000.*(ad.roll /*[rad]*/) ), qhb3 );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb4 );
          closeBtn->setDefault( true );
          QObject::connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString("%1 : %2").arg( x.Type() ).arg( x.Name() ));
  wpu->adjustSize();

  wpu->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( thinQuad const& x )
{
  QWidget* pw = parentWidget()->parentWidget();

  QString theValue;
  QDialog* wpu = new QDialog( pw,0,false,Qt::WDestructiveClose);
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Integrated gradient"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [T]  "), qwa ), 0, 1 );
           theValue.setNum( x.Strength() );
         qgl->addWidget( new QLabel( theValue, qwa ), 0, 2 );

         qgl->addWidget( new QLabel( QString("Roll angle"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]  "), qwa ), 1, 1 );
           theValue.setNum( 1000.*(x.Alignment().roll /*[rad]*/) );
         qgl->addWidget( new QLabel( theValue, qwa ), 1, 2 );
      qwa->adjustSize();

      QHBox* qhb = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb );
          closeBtn->setDefault( true );
          QObject::connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( sextupole const& x )
{
  QWidget* pw = parentWidget()->parentWidget();

  QMessageBox::information( 
     pw, 
     QString("%1 : %2").arg(x.Type()).arg(x.Name()),
     QString("Length: %1 \nStrength: %2 [T/m**2]:").arg(x.Length() ).arg(x.Strength() ) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( marker const& x )
{
 
  QWidget* pw = parentWidget()->parentWidget();

  QMessageBox::information( 
          pw, 
          QString(x.Type())+QString(": ")+QString(x.Name()),
          "What do you expect here?\nIt's a marker!"
          );
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( sector const& x )
{
  QWidget* pw = parentWidget()->parentWidget();

  Mapping theMap( x.getMap() );
  MatrixD M( theMap.jacobian() );

  ostringstream zlorfik;
  zlorfik << M;
  zlorfik << "\nBegin the mapping " << endl;
  zlorfik << theMap << endl;
  zlorfik << "\nEnd the mapping " << endl;
  zlorfik << '\0';

  string zlorfik_str( zlorfik.str() );

  QString buf( zlorfik_str.c_str() );

  QDialog* wpu = new QDialog( pw,0,false,Qt::WDestructiveClose);

    QVBox* qvb = new QVBox( wpu );

    QTextBrowser* qtv = new QTextBrowser( qvb );
      qtv->setSource( buf );
      int drawArea = (50*QApplication::desktop()->height()*
                         QApplication::desktop()->width())/100;
      int fixedWidth = (int) sqrt( (double) drawArea );
      qtv->setFixedSize( fixedWidth, fixedWidth );

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* doneBtn = new QPushButton( "Close", qhb3 );
          doneBtn->setDefault( true );
          QObject::connect( doneBtn, SIGNAL(pressed()),
                   wpu,     SLOT(close()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( monitor const& x )
{

  QWidget* pw = parentWidget()->parentWidget();

  // Set up the display widget
  QString theValue;

  QDialog* wpu = new QDialog( pw, 0, false,Qt::WDestructiveClose);
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 4, 4, 5 );

         qgl->addWidget( new QLabel( QString("x"),     qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString("[mm]"),  qwa ), 0, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 0, 2 );
           theValue.setNum( 1000.0*x.hposition() );
         qgl->addWidget( new QLabel( theValue,         qwa ), 0, 3 );

         qgl->addWidget( new QLabel( QString("y"),     qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("[mm]"),  qwa ), 1, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 1, 2 );
           theValue.setNum( 1000.0*x.vposition());
         qgl->addWidget( new QLabel( theValue,         qwa ), 1, 3 );

         qgl->addWidget( new QLabel( QString("p_x/p"), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString("[mrad]"),qwa ), 2, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 2, 2 );
           theValue.setNum( 1000.0*x.npx() );
         qgl->addWidget( new QLabel( theValue,         qwa ), 2, 3 );

         qgl->addWidget( new QLabel( QString("p_y/p"), qwa ), 3, 0 );
         qgl->addWidget( new QLabel( QString("[mrad]"),qwa ), 3, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 3, 2 );
           theValue.setNum( 1000.0*x.npy() );
         qgl->addWidget( new QLabel( theValue,         qwa ), 3, 3 );

       qwa->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb4 );
          QObject::connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(close()) );

      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( LinacCavity const& x )
{ 
  visit( static_cast<bmlnElmnt const&>(x) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



