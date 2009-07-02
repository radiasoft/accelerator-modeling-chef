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
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Authors:    Jean-Francois Ostiguy  ostiguy@fnal.gov             
******              Leo Michelotti         michelotti@fnal.gov 
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
#include <DialogElmInfo.h>
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
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();
  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "No of elements",     "",         QString("%1").arg( x.countHowManyDeeply()) );
  w->addPropertyRow( "Reference Momentum", "[GeV/c]",  QString("%1").arg( x.Momentum()) );
 
  w->show();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( bmlnElmnt const& x )
{

  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();
  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Length",   "[m]",         QString("%1").arg(  x.Length()) );
  w->addPropertyRow( "Strength", "unspecified", QString("%1").arg(x.Strength()) );
 
  w->show();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( drift const& x )
{  
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Length",   "[m]",         QString("%1").arg(  x.Length()) );
  w->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( Slot const& x )
{
  
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();
  
  Vector uso =  x.getInFrame().getOrigin();
  Vector ux =   x.getInFrame().getxAxis();
  Vector uy =   x.getInFrame().getyAxis();
  Vector us =   x.getInFrame().getzAxis();

  Vector dso =  x.getOutFrame().getOrigin();
  Vector dx =   x.getOutFrame().getxAxis();
  Vector dy =   x.getOutFrame().getyAxis();
  Vector ds =   x.getOutFrame().getzAxis();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );

  w->setCaption(cap);
  w->addPropertyRow( "Upstream   Origin", "[m]",  QString("(%1, %2, %3)").arg( uso[0]).arg(uso[1]).arg(uso[2]) );
  w->addPropertyRow( "Upstream   U-Axis", "",     QString("(%1, %2, %3)").arg(  ux[0]).arg(ux[1]).arg(ux[2])   );
  w->addPropertyRow( "Upstream   V-Axis", "",     QString("(%1, %2, %3)").arg(  uy[0]).arg(uy[1]).arg(uy[2])   );
  w->addPropertyRow( "Upstream   W-Axis", "",     QString("(%1, %2, %3)").arg(  us[0]).arg(us[1]).arg(us[2])   );

  w->addPropertyRow( "Downstream  Origin", "[m]",QString("(%1, %2, %3)").arg( dso[0]).arg(dso[1]).arg(dso[2])  );
  w->addPropertyRow( "Downstream  U-Axis", "",   QString("(%1, %2, %3)").arg(  dx[0]).arg(dx[1]).arg(dx[2] )   );
  w->addPropertyRow( "Downstream  V-Axis", "",   QString("(%1, %2, %3)").arg(  dy[0]).arg(dy[1]).arg(dy[2] )   );
  w->addPropertyRow( "Downstream  W-Axis", "",   QString("(%1, %2, %3)").arg(  ds[0]).arg(ds[1]).arg(ds[2] )   );

  w->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( sbend const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Length",         "[m]",         QString("%1").arg(    x.Length()) );
  w->addPropertyRow( "Magnetic Field", "[T]",         QString("%1").arg(  x.Strength()) );
  w->show();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( CF_sbend const& x )
{

  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Length",        "[m]",         QString("%1").arg(    x.Length()) );
  w->addPropertyRow( "Bend Field",    "[T]",         QString("%1").arg(  x.Strength()) );
  w->addPropertyRow( "Gradient",      "[T/m]",       QString("%1").arg(  x.getQuadrupole()/x.Length()) );

  w->show();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 

void BmlBrowser::visit( rbend const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Length",              "[m]",         QString("%1").arg(    x.Length()) );
  w->addPropertyRow( "Bend Field",          "[T]",         QString("%1").arg(  x.Strength()) );
  w->addPropertyRow( "Nominal Entry Angle", "[mrad]",     QString("%1").arg(  1000.0 * x.getPoleFaceAngle() ));

  w->show();

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void BmlBrowser::visit( CF_rbend const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Length",             "[m]",        QString("%1").arg(    x.Length()) );
  w->addPropertyRow( "Bend Field",         "[T]",        QString("%1").arg(  x.Strength()) );
  w->addPropertyRow( "Gradient",           "[T/m]",      QString("%1").arg(  x.getQuadrupole() / x.Length()  ));
  w->addPropertyRow( "Roll Angle",         "[mrad]",     QString("%1").arg(  x.alignment().roll()*1000.0 ) );
  w->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( quadrupole const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Length",           "[m]",         QString("%1").arg(    x.Length()) );
  w->addPropertyRow( "Gradient",         "[T/m]",       QString("%1").arg(  x.Strength()) );
  w->addPropertyRow( "Roll Angle",        "[mrad]",      QString("%1").arg(  x.alignment().roll()*1000.0 ) );
  w->show();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( thinQuad const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Integrated Gradient", "[T]",         QString("%1").arg(  x.Strength()) );
  w->addPropertyRow( "Roll Angle",          "[mrad]",      QString("%1").arg(  x.alignment().roll()*1000.0 ) );
  w->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( sextupole const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "Strength",  "[T/m**2]", QString("%1").arg(  x.Strength()) );
  w->show();

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( marker const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->show();

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

void BmlBrowser::visit( Monitor const& x )
{
  QString cap = QString("%1 %2" ).arg( x.Type() ).arg( x.Name());

  QWidget* pw = parentWidget()->parentWidget();

  DialogElmInfo* w = new DialogElmInfo( pw, 0, true, Qt::WDestructiveClose );
  w->setCaption(cap);
  w->addPropertyRow( "x",    "[mm]", QString("%1").arg(1000.0*x.hposition()) );
  w->addPropertyRow( "y",    "[mm]", QString("%1").arg(1000.0*x.vposition()) );
  w->addPropertyRow( "p_x/p",    "", QString("%1").arg(x.npx()) );
  w->addPropertyRow( "p_y/p",    "", QString("%1").arg(x.npy()) );
  w->show();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlBrowser::visit( LinacCavity const& x )
{ 
  visit( static_cast<bmlnElmnt const&>(x) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

