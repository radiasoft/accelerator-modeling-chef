/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.2
******                                    
******  File:      BeamlineBrowser.cc
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


#include <fstream>
#include <typeinfo>

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qmime.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <qheader.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtextbrowser.h>

#include "PhysicsConstants.h"
#include "GenericException.h"
#include "BeamlineIterator.h"
#include "bmlfactory.h"
#include "BeamlineBrowser.h"
#include "BeamlineContext.h"
#include "BeamlineExpressionTree.h"

// This undef is needed because of the compiler.
// #undef connect

using namespace CHEF_domain;
using namespace std;

const char* BeamlineBrowser::drift_xpm[19]={
"  16 16 2 1",
"# c #000000",
". c #ffffff",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##......."
};

const char* BeamlineBrowser::slot_xpm[19]={
"  16 16 2 1",
"# c #000000",
". c #ffffff",
".........##.....",
".......##.......",
".....####.......",
"...##..##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
".......##.......",
"...##..##.......",
".....####.......",
".......##.......",
".........##....."
};

const char* BeamlineBrowser::bml_black_xpm[19]={
"  16 16 2 1",
"# c #000000",
". c #ffffff",
".......#........",
".....#####......",
".....#####......",
".......#........",
".......#........",
".....#####......",
".....#####......",
".......#........",
".......#........",
".....#####......",
".....#####......",
".......#........",
".......#........",
".....#####......",
".....#####......",
".......#........"
};

const char* BeamlineBrowser::bml_orange_xpm[19]={
"  16 16 2 1",
"# c #ff8800",
". c #ffffff",
".......#........",
".....#####......",
".....#####......",
".......#........",
".......#........",
".....#####......",
".....#####......",
".......#........",
".......#........",
".....#####......",
".....#####......",
".......#........",
".......#........",
".....#####......",
".....#####......",
".......#........"
};

const char* BeamlineBrowser::dquad_xpm [19]={
"  16 16 2 1",
"# c #f00000",
". c #ffffff",
"................",
"................",
"...#########....",
"....#######.....",
".....#####......",
".....#####......",
"......###.......",
"......###.......",
"......###.......",
"......###.......",
".....#####......",
".....#####......",
"....#######.....",
"...#########....",
"................",
"................"
};

const char* BeamlineBrowser::fquad_xpm [19]={
"  16 16 2 1",
"# c #f00000",
". c #ffffff",
"................",
"................",
".......#........",
"......###.......",
".....#####......",
".....#####......",
"....#######.....",
"....#######.....",
"....#######.....",
"....#######.....",
".....#####......",
".....#####......",
"......###.......",
".......#........",
"................",
"................"
};

const char* BeamlineBrowser::sextupole_xpm [19]={
"  16 16 2 1",
"# c #ffff00",
". c #ffffff",
"................",
"................",
"................",
"....########....",
"...##########...",
"..############..",
".##############.",
"################",
".##############.",
"..############..",
"...##########...",
"....########....",
"................",
"................",
"................",
"................"
};

const char* BeamlineBrowser::dipole_xpm[19]={
"  16 16 2 1",
"# c #008000",
". c #ffffff",
"................",
"................",
"................",
".......##.......",
".......##.......",
"......####......",
"......####......",
".....######.....",
".....######.....",
".....######.....",
"....########....",
"....########....",
"...##########...",
"...##########...",
"..############..",
"................"
};

const char* BeamlineBrowser::elmnt_xpm[19]={
"  16 16 2 1",
"# c #000000",
". c #ffffff",
"................",
"................",
"................",
"................",
"................",
"................",
"......###.......",
".....#####......",
".....#####......",
"......###.......",
"................",
"................",
"................",
"................",
"................",
"................"
};


QPixmap* BeamlineBrowser::driftSymbol     = 0;
QPixmap* BeamlineBrowser::slotSymbol      = 0;
QPixmap* BeamlineBrowser::bmlBlackSymbol  = 0;
QPixmap* BeamlineBrowser::sextupoleSymbol = 0;
QPixmap* BeamlineBrowser::fquadSymbol     = 0;
QPixmap* BeamlineBrowser::dquadSymbol     = 0;
QPixmap* BeamlineBrowser::bmlOrangeSymbol = 0;
QPixmap* BeamlineBrowser::dipoleSymbol    = 0;
QPixmap* BeamlineBrowser::elmntSymbol     = 0;


// *****************************************************************************
// *
// * Class QBml
// *
// *****************************************************************************

QBml::QBml( QPixmap* px, QBmlRoot* rt, const char* s1, const char* s2 )
: QListViewItem( rt, s1, s2 ), _pix(px), _p(rt)
{
}

QBml::QBml( QPixmap* px, QListView* parent, const char* s1, const char* s2 )
: QListViewItem( parent, s1, s2 ), _pix(px), _p(0)
{
}

QBml::QBml( const QBml& x ) 
: QListViewItem( x )
{
  QMessageBox::information( 
          0, 
          "CHEF ERROR",
          "QBml Copy constructor called."
          );
  throw( GenericException( __FILE__, __LINE__, 
         "QBml::QBml( const QBml& x ) ", 
         "QBml copy constructor called." ) );
}


QBml::~QBml()
{
}


void QBml::setPixmap( QPixmap *px )
{
    _pix = px;
    setup();
    widthChanged( 0 );
    invalidateHeight();
    // REMOVE: repaint();
}


const QPixmap* QBml::pixmap( int i ) const
{
    if ( i )         // What is the point of this function?
      return 0;
    return _pix;
}


const QBmlRoot* QBml::bmlParent()
{
  return _p;
}


// *****************************************************************************
// *
// * Class QBmlElmt
// *
// *****************************************************************************

QBmlElmt::QBmlElmt( QBmlRoot* parent, /* const */ bmlnElmnt* q, double& s )
: QBml( 0, parent, q->Name(), q->Type() ),
  _azimuth(s), _myElement(q)
{
  QString str;
  str.setNum( ((double) s) );
  this->setText( 2, str );
}


QBmlElmt::~QBmlElmt()
{
  // delete _myElement; ???
}


QString QBmlElmt::fullName() const
{
  return QString( _myElement->Name() );
}


// *****************************************************************************
// *
// * Class QBmlRoot
// *
// *****************************************************************************

QBmlRoot::QBmlRoot( QListView* parent, /* const */ BeamlineContext* q, double& s )
: QBml( 0, parent, q->name(), "beamline" ), 
  _myBmlCon(q)
{
    QString str;
    str.setNum( ((double) s) );
    this->setText( 2, str );

    _myBeamline = ((beamline*) -1);
}


QBmlRoot::QBmlRoot( QListView* parent, /* const */ beamline* q, double& s )
: QBml( 0, parent, q->Name(), q->Type() ), 
  _myBeamline(0)
{
    QString str;
    str.setNum( ((double) s) );
    this->setText( 2, str );

    _myBmlCon = new BeamlineContext( true, q );
}


QBmlRoot::QBmlRoot( QBmlRoot * parent, /* const */ beamline* q, double& s )
: QBml( 0, parent, q->Name(), q->Type() ),
  _myBeamline(q), _myBmlCon(0)
{
    QString str;
    str.setNum( ((double) s) );
    this->setText( 2, str );

    if ( 0 == strcmp(q->Type(),"beamline") )
        setPixmap( BeamlineBrowser::bmlBlackSymbol );
    else
        setPixmap( BeamlineBrowser::elmntSymbol );
}


QBmlRoot::~QBmlRoot()
{
  if( -1 != ((int) _myBeamline) )
  { if( 0 != _myBmlCon   ) 
    { delete _myBmlCon;   
      if( 0 != _myBeamline ) { 
        ostringstream uic;
        uic << "An impossibility has occurred\nin file "
            << __FILE__
            << " at line " << __LINE__ ;
        QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
      }
    }
  }
}


void QBmlRoot::setOpen( bool o )
{
    if ( o )
        setPixmap( BeamlineBrowser::bmlOrangeSymbol );
    else
        setPixmap( BeamlineBrowser::bmlBlackSymbol );

    QListViewItem::setOpen( o );
}

void QBmlRoot::setup()
{
    setExpandable( TRUE );
    QListViewItem::setup();
}

QString QBmlRoot::fullName() const
{
    QString s;
    if( 0 == _p ) 
    { s = _myBmlCon->name();
    }
    else 
    { s = _myBeamline->Name();
    }
    return s;
}

// QString QBmlRoot::text( int column ) const
// {
//     static QString ret;
//     if( column == 0 ) {
//         return _myElement->Name();
//     }
//     else if( column == 1 ) {
//         return _myElement->Type();
//     }
//     else {
//         return QString( "Column 2" );
//     }
// }


const QBmlElmt* QBmlRoot::findSelectedElement() const
{
  QListViewItem* fc = this->firstChild();

  while( 0 != fc ) 
  {
    if(typeid(*fc) == typeid(QBmlElmt)) {
      if( fc->isSelected() ) {
        return (QBmlElmt*) fc;
      }
    }

    else if(typeid(*fc) == typeid(QBmlRoot)) {
      const QBmlElmt* ret = ((QBmlRoot*) fc)->findSelectedElement();
      if( 0 != ret ) { return ret; }
    }

    else {
      ostringstream uic;
      uic << "An impossibility has occurred\nin file "
          << __FILE__
          << " at line " << __LINE__ ;
      QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
      throw( GenericException( __FILE__, __LINE__, 
             "const QBmlElmt* QBmlRoot::findSelectedElement() const", 
             "Impossible! Neither QBmlElmt nor QBmlRoot!!" ) );
    }
    
    fc = fc->nextSibling();
  }

  return 0;
}


// *****************************************************************************
// *
// * Class BeamlineBrowser
// *
// *****************************************************************************

BeamlineBrowser::BeamlineBrowser( QWidget *parent, const char *name, bool sdo )
    : QListView( parent, name ), oldCurrent( 0 ),
      dropItem( 0 ), mousePressed( FALSE ), _lastClickedRootPtr(0)
{
  this->setSorting(-1);  // prevents sorting
  this->addColumn( "Name" );
  this->addColumn( "Type" );
  this->addColumn( "Azimuth" );
  this->setTreeStepSize( 20 );
  
  autoopen_timer = new QTimer( this );
  
  if ( !BeamlineBrowser::bmlBlackSymbol ) {
      BeamlineBrowser::driftSymbol = new QPixmap( drift_xpm );
      BeamlineBrowser::slotSymbol = new QPixmap( slot_xpm );
      BeamlineBrowser::bmlBlackSymbol = new QPixmap( bml_black_xpm );
      BeamlineBrowser::bmlOrangeSymbol = new QPixmap( bml_orange_xpm );
      BeamlineBrowser::sextupoleSymbol = new QPixmap( sextupole_xpm );
      BeamlineBrowser::fquadSymbol = new QPixmap( fquad_xpm );
      BeamlineBrowser::dquadSymbol = new QPixmap( dquad_xpm );
      BeamlineBrowser::dipoleSymbol = new QPixmap( dipole_xpm );
      BeamlineBrowser::elmntSymbol = new QPixmap( elmnt_xpm );
  }
  
  connect( this, SIGNAL( doubleClicked( QListViewItem * ) ),
           this, SLOT( slotFolderSelected( QListViewItem * ) ) );
  connect( this, SIGNAL( returnPressed( QListViewItem * ) ),
           this, SLOT( slotFolderSelected( QListViewItem * ) ) );
  // Shadowed: connect( this, SIGNAL( rightButtonClicked( QListViewItem*, const QPoint&, int ) ),
  // Shadowed:          this, SLOT( slotShowData( QListViewItem*, const QPoint&, int ) ) );
  
  setAcceptDrops( TRUE );
  viewport()->setAcceptDrops( TRUE );
  
  connect( autoopen_timer, SIGNAL( timeout() ),
           this, SLOT( openFolder() ) );

  this->setSelectionMode( QListView::Extended );
  this->resize( 400, 400 );
  this->setCaption( "CHEF: Beamline Browser" );
  this->setAllColumnsShowFocus( TRUE );

  if ( childCount() == 0 ) emit sig_browserIsEmpty();
  this->show();
}


BeamlineBrowser::~BeamlineBrowser()
{
}


const bmlnElmnt* BeamlineBrowser::getSelectedElement( BeamlineContext* bcPtr ) const
{
  if( 0 == bcPtr ) {
    QMessageBox::information( 
            0, 
            "BeamlineBrowser::getSelectedElement",
            "ERROR: Null pointer passed as argument."
            );
    return 0;
  }

  QBmlRoot* fc = dynamic_cast<QBmlRoot*>( this->firstChild() );
  // cout << "DGN: " << ( fc->_myBmlCon->cheatBmlPtr()->Name() )
  //      << endl;
  bool found = ( bcPtr == fc->_myBmlCon );
  while( !found ) {
    fc = dynamic_cast<QBmlRoot*>( fc->nextSibling() );
    // cout << "DGN: " << ( fc->_myBmlCon->cheatBmlPtr()->Name() )
    //      << endl;
    found = ( bcPtr == fc->_myBmlCon );
  }

  if( !found ) {
    QMessageBox::information( 
            0, 
            "BeamlineBrowser::getSelectedElement",
            "ERROR: Did not find selected beamline."
            );
    return 0;
  }

  const QBmlElmt* qbePtr = fc->findSelectedElement();
  if( qbePtr == 0 ) { return 0; }

  return qbePtr->cheatElementPtr();
}


void BeamlineBrowser::slotFolderSelected( QListViewItem *i )
{
  if( typeid(*i) == typeid(QBmlRoot) ) {
    QBmlRoot *dir = (QBmlRoot*)i;
    emit folderSelected( dir->fullName() );
  }
}


void BeamlineBrowser::slotShowData( QBmlRoot* toproot, QBml* item )
{
  infoWriter wr;
  wr._contextPtr = toproot->_myBmlCon;

  if( typeid(*item)==typeid(QBmlRoot) ) {
    if( 0 == ((QBmlRoot*) item)->_p ) {
      (  ((QBmlRoot*) item)  ->_myBmlCon)->accept(wr);
    }
    else {
      (  ((QBmlRoot*) item)  ->_myBeamline)->accept(wr);
    }
  }

  else if( typeid(*item)==typeid(QBmlElmt) ) {
    (((QBmlElmt*) item)->_myElement)->accept(wr);
  }

  // REMOVE: infoWriter wr;
  // REMOVE: if( typeid(*i)==typeid(QBmlRoot) ) {
  // REMOVE: }
  // REMOVE: else if( typeid(*i)==typeid(QBmlElmt) ) {
  // REMOVE:   (((QBmlElmt*) i)->_myElement)->accept(wr);
  // REMOVE: }
}


void BeamlineBrowser::openFolder()
{
    autoopen_timer->stop();
    if ( dropItem && !dropItem->isOpen() ) {
        dropItem->setOpen( TRUE );
        dropItem->repaint();
    }
}

static const int autoopenTime = 750;

void BeamlineBrowser::contentsDragEnterEvent( QDragEnterEvent *e )
{
    if ( !QUriDrag::canDecode(e) ) {
        e->ignore();
        return;
    }

    oldCurrent = currentItem();

    QListViewItem *i = itemAt( contentsToViewport(e->pos()) );
    if ( i ) {
        dropItem = i;
        autoopen_timer->start( autoopenTime );
    }
}

void BeamlineBrowser::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( !QUriDrag::canDecode(e) ) {
        e->ignore();
        return;
    }

    QPoint vp = contentsToViewport( ( (QDragMoveEvent*)e )->pos() );
    QListViewItem *i = itemAt( vp );
    if ( i ) {
        setSelected( i, TRUE );
        e->accept();
        if ( i != dropItem ) {
            autoopen_timer->stop();
            dropItem = i;
            autoopen_timer->start( autoopenTime );
        }
        switch ( e->action() ) {
        case QDropEvent::Copy:
            break;
        case QDropEvent::Move:
            e->acceptAction();
            break;
        case QDropEvent::Link:
            e->acceptAction();
            break;
        default:
            ;
        }
    } else {
        e->ignore();
        autoopen_timer->stop();
        dropItem = 0;
    }
}

void BeamlineBrowser::contentsDragLeaveEvent( QDragLeaveEvent * )
{
    autoopen_timer->stop();
    dropItem = 0;

    setCurrentItem( oldCurrent );
    setSelected( oldCurrent, TRUE );
}

void BeamlineBrowser::contentsDropEvent( QDropEvent *e )
{
    autoopen_timer->stop();

    if ( !QUriDrag::canDecode(e) ) {
        e->ignore();
        return;
    }

    QListViewItem *item = itemAt( contentsToViewport(e->pos()) );
    if ( item ) {

        QStrList lst;

        QUriDrag::decode( e, lst );

        QString str;

        switch ( e->action() ) {
            case QDropEvent::Copy:
            str = "Copy";
            break;
            case QDropEvent::Move:
            str = "Move";
            e->acceptAction();
            break;
            case QDropEvent::Link:
            str = "Link";
            e->acceptAction();
            break;
            default:
            str = "Unknown";
        }

        str += "\n\n";

        e->accept();

        for ( uint i = 0; i < lst.count(); ++i ) {
            QString filename = lst.at( i );
            str += filename + "\n";
        }
        str += QString( "\nTo\n\n   %1" )
               .arg( fullPath(item) );

        QMessageBox::information( this, "Drop target", str, "Not implemented" );
    } else
        e->ignore();

}

QString BeamlineBrowser::fullPath(QListViewItem* item)
{
    QString fullpath = item->text(0);
    while ( (item=item->parent()) ) {
        if ( item->parent() )
            fullpath = item->text(0) + "/" + fullpath;
        else
            fullpath = item->text(0) + fullpath;
    }
    return fullpath;
}

void BeamlineBrowser::contentsMousePressEvent( QMouseEvent* e )
{
  // Begin DGN: section
  // static bool firstTime = true;
  // if( firstTime ) {
  //   firstTime = false;
  //   SelectionMode sm = this->selectionMode();
  //   if( sm == QListView::Single )      { cout << "DGN: Single selection mode."   << endl; }
  //   else if( sm == QListView::Multi )       { cout << "DGN: Multi selection mode."    << endl; }
  //   else if( sm == QListView::Extended )    { cout << "DGN: Extended selection mode." << endl; }
  //   else if( sm == QListView::NoSelection ) { cout << "DGN: No selection mode."       << endl; }
  //   else { cout << "DGN: Selection mode not recognized." << endl; }
  // }
  // End DGN: section

  QListView::contentsMousePressEvent(e);

  QPoint p( contentsToViewport( e->pos() ) );
  QListViewItem* i = itemAt( p );
    if( 0 == i ) { return; }
  QBml* j = dynamic_cast<QBml*>(i);
    if( 0 == j ) { return; }

  QBmlRoot* topRoot = 0;
  if( typeid(*j) == typeid(QBmlRoot) ) 
  { topRoot = dynamic_cast<QBmlRoot*>(j);
  }

  QBmlRoot* parentPtr = ((QBmlRoot*) (j->bmlParent()));
  while( 0 != parentPtr ) 
  { topRoot   = parentPtr;
    parentPtr = ((QBmlRoot*) (parentPtr->bmlParent()));
  }

  if( e->button() == Qt::RightButton ) 
  { this->slotShowData( topRoot, j );
  }


  else if( e->button() == Qt::LeftButton ) 
  { if( typeid(*j) == typeid(QBmlRoot) ) 
    { _lastClickedRootPtr = dynamic_cast<QBmlRoot*>(j);
      emit sig_bmlLeftClicked( ((QBmlRoot*) j) -> _myBmlCon );
    }
    emit sig_bmlLeftClicked( j );
  }


  if( i ) 
  { // if the user clicked into the root decoration of the item, don't try to start a drag!
    if( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
        treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() ||
        p.x() < header()->cellPos( header()->mapToActual( 0 ) ) ) 
    { presspos = e->pos();
      mousePressed = TRUE;
    }
  }
}

void BeamlineBrowser::contentsMouseMoveEvent( QMouseEvent* e )
{
    if ( mousePressed && ( presspos - e->pos() ).manhattanLength() > QApplication::startDragDistance() ) {
        mousePressed = FALSE;
        QListViewItem *item = itemAt( contentsToViewport(presspos) );
        if ( item ) {
            QString source = fullPath(item);
            if ( QFile::exists(source) ) {
                QUriDrag* ud = new QUriDrag(viewport());
                ud->setUnicodeUris( source );
                if ( ud->drag() )
                    QMessageBox::information( this, "Drag source",
                                              QString("Delete ")+source, "Not implemented" );
            }
        }
    }
}

void BeamlineBrowser::contentsMouseReleaseEvent( QMouseEvent * )
{
    mousePressed = FALSE;
}

void BeamlineBrowser::setDir( const QString &s )
{
    QListViewItemIterator it( this );
    ++it;
    for ( ; it.current(); ++it ) {
        it.current()->setOpen( FALSE );
    }

    QStringList lst( QStringList::split( "/", s ) );
    QListViewItem *item = firstChild();
    QStringList::Iterator it2 = lst.begin();
    for ( ; it2 != lst.end(); ++it2 ) {
        while ( item ) {
            if ( item->text( 0 ) == *it2 ) {
                item->setOpen( TRUE );
                break;
            }
            item = item->itemBelow();
        }
    }

    if ( item )
        setCurrentItem( item );
}


// 
// Readers
// 

BeamlineContext* BeamlineBrowser::readMADFile( const char* fileName, 
                                               const char* lineName, 
                                               double      brho )
{
  bmlfactory bf( fileName, brho );
  beamline* pBml = bf.create_beamline( lineName ); 

  BeamlineContext* www = new BeamlineContext( false, pBml );
  www->setClonedFlag( true );
  displayBeamline( www );

  emit sig_newContext( www );
  return www;
}


// void BeamlineBrowser::displayBeamline( const beamline* ptr )
// {
//   double s = 0.0;
//   QString str1, str2;
// 
//   { // Begin local scope ...
//     DeepBeamlineIterator dbi( ptr );
//     bmlnElmnt* q;
//     while( q = dbi++ ) {
//       s += q->Length();
//     }
//   } // End local scope ...
// 
//   QBmlRoot* root = new QBmlRoot( this, (beamline*) ptr, s );
//   root->setPixmap( BeamlineBrowser::bmlBlackSymbol );
//   _topLevelItems.insert( (void*) root );
// 
//   str2.setNum(s);
// 
//   ReverseBeamlineIterator rbi( ptr );
//   _displayLine( root, rbi, s );
// 
//   str1.setNum( s );
//   root->setText( 2, str1 + "-" + str2 );
// }


void BeamlineBrowser::displayBeamline( const BeamlineContext* ptr )
{
  double s = ptr->sumLengths();
  QString str1, str2;

  QBmlRoot* root = new QBmlRoot( this, (BeamlineContext*) ptr, s );
  root->setPixmap( BeamlineBrowser::bmlBlackSymbol );
  _topLevelItems.insert( (void*) root );

  str2.setNum(s);

  ReverseBeamlineIterator rbi( ptr->cheatBmlPtr() );
  _displayLine( root, rbi, s );

  str1.setNum( s );
  root->setText( 2, str1 + "-" + str2 );

  this->setSelected ( root, true );
  emit sig_bmlLeftClicked( root->_myBmlCon );
  emit sig_bmlLeftClicked( root );

  if ( childCount() == 1 ) emit sig_browserIsNotEmpty();
}


void BeamlineBrowser::_displayLine( QBmlRoot* root, 
                                    ReverseBeamlineIterator& bi, 
                                    double& s )
{
  static bmlnElmnt* q;
  static bool firstTime = true;
  QString str1, str2;

  while(( q = bi++ )) {
    if( 0 == strcmp("beamline",q->Type()) ) {
      QBmlRoot* newBml = new QBmlRoot( root, (beamline*) q , s);
      newBml->setPixmap( BeamlineBrowser::bmlBlackSymbol );
      ReverseBeamlineIterator newbi( (beamline*) q );
      str2.setNum(s);
      BeamlineBrowser::_displayLine( newBml, newbi, s );
      str1.setNum( s );
      newBml->setText( 2, str1 + "-" + str2 );
    }
    else {
      if( firstTime ) {
        cerr << "*** WARNING ***                              \n"
                "*** WARNING *** BeamlineBrowser::_displayLine    \n"
                "*** WARNING *** bmlnElmnt::Length() function \n"
                "*** WARNING *** will give incorrect answers  \n"
                "*** WARNING *** for arclength of rbends.     \n"
                "*** WARNING ***                              \n"
             << endl;
        firstTime = false;
      }

      QBmlElmt* newElmnt = new QBmlElmt( root, q, s );
      s -= q->Length();  // ??? Fix this so it works with rbends too.
      if( fabs(s) <= 1.0e-8 ) { s = 0.0; }

      if( !(0 == strstr(q->Type(), "bend")) ) {
        newElmnt->setPixmap( BeamlineBrowser::dipoleSymbol );
      }
      else if( !(0 == strstr(q->Type(), "quad")) || 
               !(0 == strstr(q->Type(), "Quad"))     ) {
        if( q->Strength() >= 0.0 ) {
          newElmnt->setPixmap( BeamlineBrowser::fquadSymbol ); 
        }
        else {
          newElmnt->setPixmap( BeamlineBrowser::dquadSymbol ); 
        }
      }
      else if( !(0 == strstr(q->Type(), "sext")) || 
               !(0 == strstr(q->Type(), "Sext"))     ) {
        newElmnt->setPixmap( BeamlineBrowser::sextupoleSymbol ); 
      }
      else if( 0 == strcmp(q->Type(), "drift" ) ) {
        newElmnt->setPixmap( BeamlineBrowser::driftSymbol ); 
      }
      else if( 0 == strcmp(q->Type(), "Slot" ) ) {
        newElmnt->setPixmap( BeamlineBrowser::slotSymbol ); 
      }
      else {
        newElmnt->setPixmap( BeamlineBrowser::elmntSymbol );
      }
    }
  }
}


int BeamlineBrowser::removeBeamline( BeamlineContext* ptr )
{
  if( 0 == ptr ) { return 1; }
  if( typeid(*ptr) != typeid(BeamlineContext) ) { return 2; }

  slist_iterator getNext( _topLevelItems );
  QBmlRoot* q;
  QBmlRoot* w;
  bool notfound = true;
  while( notfound && ( 0 != ( q = (QBmlRoot*) getNext() ) ) ) {
    if( q->_myBmlCon == ptr ) { 
      w = q;   // Just paranoia
      notfound = false;
    }
  }

  if( notfound ) { return 3; }
  else {
    this->takeItem(w);
    _topLevelItems.remove( (void*) w );
    delete w;
  }

  if ( childCount() == 0 ) emit sig_browserIsEmpty();

  return 0;
}


// REMOVE int BeamlineBrowser::findElement( QBml* startpoint, 
// REMOVE                                   const QString& name )
// REMOVE {
// REMOVE   QBml*     qbmlPtr;
// REMOVE   QBmlElmt* qbmlelmtPtr; 
// REMOVE   QBmlRoot* qbmlrootPtr;
// REMOVE 
// REMOVE   int i = 0;
// REMOVE   QListViewItemIterator looker( dynamic_cast<QListViewItem*>(startpoint) );
// REMOVE   while( 0 != (  qbmlPtr = dynamic_cast<QBml*>(looker.current()) )) {
// REMOVE     if( 0 != ( qbmlelmtPtr = dynamic_cast<QBmlElmt*>( qbmlPtr ) )) {
// REMOVE       if( qbmlelmtPtr->fullName() == name ) {
// REMOVE         cout << ++i << " !  " << qbmlelmtPtr->fullName() << endl;
// REMOVE       }
// REMOVE       else {
// REMOVE         cout << ++i << "    " << qbmlelmtPtr->fullName() << endl;
// REMOVE       }
// REMOVE     }
// REMOVE     ++looker;
// REMOVE   }
// REMOVE   return 1;
// REMOVE }

int BeamlineBrowser::findElement( QBml*                startpoint, 
                                  const BoolNode*      queryPtr, 
                                  QPtrList<bmlnElmnt>& foundElements )
{
  return this->findElement( startpoint, *queryPtr, foundElements );
}


int BeamlineBrowser::findElement( QBml*                startpoint, 
                                  const BoolNode&      query, 
                                  QPtrList<bmlnElmnt>& foundElements )
{
  int        ret = 1;
  QBml*      qbmlPtr;
  QBmlElmt*  qbmlelmtPtr; 
  // REMOVE: QBmlRoot*  qbmlrootPtr;
  const bmlnElmnt* elementPtr;
  bool started = false;

  QListViewItemIterator looker( dynamic_cast<QListViewItem*>(startpoint) );
  while( 0 != ( qbmlPtr = dynamic_cast<QBml*>(looker.current()) )) {

    // This assures that the search will not continue
    // beyond a single top-level tree or the selected
    // element closest to the last left-clicked element.
    if( started ) {
      if(    ((void*)(qbmlPtr->parent()) == 0) 
          || ((void*)(qbmlPtr->parent()) == this)
          || qbmlPtr->isSelected()                ) 
      { 
        // REMOVE: cout << "DGN: parent = " << ((int) (qbmlPtr->parent())) << endl;
        break; 
      }
    }
    else {
      started = true;
    }

    // Apply the test and, if passed, add element pointer to the list.
    if( 0 != ( qbmlelmtPtr = dynamic_cast<QBmlElmt*>( qbmlPtr ) )) {
      elementPtr = qbmlelmtPtr->cheatElementPtr();
      if( query.evaluate( elementPtr ) ) {
        foundElements.append( elementPtr );
        qbmlPtr->setSelected(true);
        this->ensureItemVisible( qbmlPtr );
        ret = 0;
      }
    }
    ++looker;

  }
  return ret;
}

// REMOVE: int BeamlineBrowser::findElement( QBml*                startpoint, 
// REMOVE:                                   const BoolNode&      query, 
// REMOVE:                                   QPtrList<bmlnElmnt>& foundElements )
// REMOVE: {
// REMOVE:   int        ret = 1;
// REMOVE:   QBmlElmt*  qbmlelmtPtr; 
// REMOVE:   const bmlnElmnt* elementPtr;
// REMOVE: 
// REMOVE:   QListViewItem* fc = dynamic_cast<QListViewItem*>(startpoint);
// REMOVE: 
// REMOVE:   while( 0 != fc ) {
// REMOVE:     if( 0 != ( qbmlelmtPtr = dynamic_cast<QBmlElmt*>( fc ) )) {
// REMOVE:       elementPtr = qbmlelmtPtr->cheatElementPtr();
// REMOVE:       if( query.evaluate( elementPtr ) ) {
// REMOVE:         foundElements.append( elementPtr );
// REMOVE:         ret = 0;
// REMOVE:       }
// REMOVE:       else {
// REMOVE:       }
// REMOVE:     }
// REMOVE:     fc = fc->nextSibling();
// REMOVE:   }
// REMOVE:   return ret;
// REMOVE: }


QPtrList<bmlnElmnt> BeamlineBrowser::findAllSelected( QBmlRoot* startpoint ) const
{
  QPtrList<bmlnElmnt> ret;
  QBml*      qbmlPtr;
  QBmlElmt*  qbmlelmtPtr;
  bool started = false;

  QListViewItemIterator looker( dynamic_cast<QListViewItem*>(startpoint) );
  while( 0 != ( qbmlPtr = dynamic_cast<QBml*>(looker.current()) )) {

    // This assures that the search will not continue
    // beyond a single top-level tree.

    if( started ) {
      if(    ((void*)(qbmlPtr->parent()) == 0   ) 
	  || ((void*)(qbmlPtr->parent()) == this) )
      { 
        break; 
      }
    }
    else {
      started = true;
    }

    // Apply the test and, if passed, add element pointer to the list.
    if( qbmlPtr->isSelected() ) {
      if( 0 != ( qbmlelmtPtr = dynamic_cast<QBmlElmt*>(qbmlPtr) ) ) {
        ret.append( qbmlelmtPtr->cheatElementPtr() );
      }
    }

    // Repeat
    ++looker;
  }

  return ret;  
}


//
// BeamlineBrowser::infoWriter implementation
// 

void BeamlineBrowser::infoWriter::visitBeamline( const beamline* x )
{
  QString cap( "Beamline" );
  cap += QString( " " );
  cap += QString( x->Name() );

  QString stl;
  QString msg;
  msg.setNum( x->countHowManyDeeply() );
  msg += QString( " elements. \nReference energy = " );
  stl.setNum( x->Energy() );
  msg += stl;
  msg += QString( " Gev" );

  QMessageBox::information( 0, cap, msg );
}


void BeamlineBrowser::infoWriter::visitBmlnElmnt( const bmlnElmnt* x )
{
  // This code is similar to what appears in 
  // SiteViewer::Wndw::mousePressEvent
  // Keep it that way!
  QString cap( x->Type() );
  cap += QString( " " );
  cap += QString( x->Name() );

  QString msg( "Length = " );
  QString stl;
  stl.setNum( x->Length() );
  msg += stl;
  stl.setNum( x->Strength() );
  msg += QString( ";  Strength = " );
  msg += stl;

  QMessageBox::information( 0, cap, msg );
}


void BeamlineBrowser::infoWriter::visitDrift( const drift* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
  // QDialog* wpu = new QDialog( 0, 0, true, Qt::WDestructiveClose );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlb = new QLabel( "Length", qhb1 );
        QString stl;
        stl.setNum( x->Length() );
        QLineEdit* qle = new QLineEdit( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();
      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb2 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();
    qvb->adjustSize();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, because of the flag Qt::WDestructiveClose.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( stl != qle->text() ) {
      bool ok;
      double newLength = (qle->text()).toDouble( &ok );
      if( ok ) {
        if( 0 != _contextPtr->setLength( (drift*) x, newLength ) ) {
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": drift not found in context."
               << endl;
	}
      }
    }
  }

  delete wpu;
}


void BeamlineBrowser::infoWriter::visitSlot( const Slot* x )
{
  /*
  QString stl;
  stl.setNum( x->Length() );
  QMessageBox::information( 
          0, 
          QString(x->Type())+QString(": ")+QString(x->Name()),
          QString("Length [m]: ")+stl
          );
  */

  QString sts;
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhbFrames = new QHBox( qvb );

        QVBox* qvbIn = new QVBox( qhbFrames );
          new QLabel( "Upstream frame", qvbIn );

      	  QHBox* qhboIn = new QHBox( qvbIn );
      	    new QLabel( QString("O: "), qhboIn );
      	    sts.setNum( (x->getInFrame().getOrigin()) (0) );
      	    new QLineEdit( sts, qhboIn );
      	    sts.setNum( (x->getInFrame().getOrigin()) (1) );
      	    new QLineEdit( sts, qhboIn );
      	    sts.setNum( (x->getInFrame().getOrigin()) (2) );
      	    new QLineEdit( sts, qhboIn );
      	  qhboIn->setMargin(5);
      	  qhboIn->setSpacing(3);
      	  qhboIn->adjustSize();
    
      	  QHBox* qhb3 = new QHBox( qvbIn );
      	    QString sts0( "U: ( " );
      	    sts.setNum( (x->getInFrame().getxAxis()) (0) );
      	    sts0 = sts0 + sts + ", ";
      	    sts.setNum( (x->getInFrame().getxAxis()) (1) );
      	    sts0 = sts0 + sts + ", ";
      	    sts.setNum( (x->getInFrame().getxAxis()) (2) );
      	    sts0 = sts0 + sts + " )";
      	    new QLabel( sts0, qhb3 );
      	  qhb3->setMargin(5);
      	  qhb3->setSpacing(3);
      	  qhb3->adjustSize();
    
      	  QHBox* qhb4 = new QHBox( qvbIn );
      	    QString sts1( "V: ( " );
      	    sts.setNum( (x->getInFrame().getyAxis()) (0) );
      	    sts1 = sts1 + sts + ", ";
      	    sts.setNum( (x->getInFrame().getyAxis()) (1) );
      	    sts1 = sts1 + sts + ", ";
      	    sts.setNum( (x->getInFrame().getyAxis()) (2) );
      	    sts1 = sts1 + sts + " )";
      	    new QLabel( sts1, qhb4 );
      	  qhb4->setMargin(5);
      	  qhb4->setSpacing(3);
      	  qhb4->adjustSize();
    
      	  QHBox* qhb5 = new QHBox( qvbIn );
      	    QString sts2( "W: ( " );
      	    sts.setNum( (x->getInFrame().getzAxis()) (0) );
      	    sts2 = sts2 + sts + ", ";
      	    sts.setNum( (x->getInFrame().getzAxis()) (1) );
      	    sts2 = sts2 + sts + ", ";
      	    sts.setNum( (x->getInFrame().getzAxis()) (2) );
      	    sts2 = sts2 + sts + " )";
      	    new QLabel( sts2, qhb5 );
      	  qhb5->setMargin(5);
      	  qhb5->setSpacing(3);
      	  qhb5->adjustSize();

        qvbIn->setMargin(5);
        qvbIn->setSpacing(3);
        qvbIn->adjustSize();

        QVBox* qvbOut = new QVBox( qhbFrames );
          new QLabel( "Downstream frame", qvbOut );

      	  QHBox* qhbo = new QHBox( qvbOut );
      	    QLabel* qlbo = new QLabel( QString("O: "), qhbo );
      	    sts.setNum( (x->getOutFrame().getOrigin()) (0) );
      	    QLineEdit* qleo0 = new QLineEdit( sts, qhbo );
      	    sts.setNum( (x->getOutFrame().getOrigin()) (1) );
      	    QLineEdit* qleo1 = new QLineEdit( sts, qhbo );
      	    sts.setNum( (x->getOutFrame().getOrigin()) (2) );
      	    QLineEdit* qleo2 = new QLineEdit( sts, qhbo );
      	  qhbo->setMargin(5);
      	  qhbo->setSpacing(3);
      	  qhbo->adjustSize();
    
      	  QHBox* qhb0 = new QHBox( qvbOut );
      	    sts0 = "U: ( ";
      	    sts.setNum( (x->getOutFrame().getxAxis()) (0) );
      	    sts0 = sts0 + sts + ", ";
      	    sts.setNum( (x->getOutFrame().getxAxis()) (1) );
      	    sts0 = sts0 + sts + ", ";
      	    sts.setNum( (x->getOutFrame().getxAxis()) (2) );
      	    sts0 = sts0 + sts + " )";
      	    QLabel* qlb0 = new QLabel( sts0, qhb0 );
      	  qhb0->setMargin(5);
      	  qhb0->setSpacing(3);
      	  qhb0->adjustSize();
    
      	  QHBox* qhb1 = new QHBox( qvbOut );
      	    sts1 = "V: ( ";
      	    sts.setNum( (x->getOutFrame().getyAxis()) (0) );
      	    sts1 = sts1 + sts + ", ";
      	    sts.setNum( (x->getOutFrame().getyAxis()) (1) );
      	    sts1 = sts1 + sts + ", ";
      	    sts.setNum( (x->getOutFrame().getyAxis()) (2) );
      	    sts1 = sts1 + sts + " )";
      	    QLabel* qlb1 = new QLabel( sts1, qhb1 );
      	  qhb1->setMargin(5);
      	  qhb1->setSpacing(3);
      	  qhb1->adjustSize();
    
      	  QHBox* qhb2 = new QHBox( qvbOut );
      	    sts2 = "W: ( ";
      	    sts.setNum( (x->getOutFrame().getzAxis()) (0) );
      	    sts2 = sts2 + sts + ", ";
      	    sts.setNum( (x->getOutFrame().getzAxis()) (1) );
      	    sts2 = sts2 + sts + ", ";
      	    sts.setNum( (x->getOutFrame().getzAxis()) (2) );
      	    sts2 = sts2 + sts + " )";
      	    QLabel* qlb2 = new QLabel( sts2, qhb2 );
      	  qhb2->setMargin(5);
      	  qhb2->setSpacing(3);
      	  qhb2->adjustSize();

        qvbOut->setMargin(5);
        qvbOut->setSpacing(3);
        qvbOut->adjustSize();

      qhbFrames->setMargin(5);
      qhbFrames->setSpacing(3);
      qhbFrames->adjustSize();

      QHBox* qhb99 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb99 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb99 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb99->setMargin(5);
      qhb99->setSpacing(3);
      qhb99->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    cout << "Dialog accepted!" << endl;
  }
  else {
    cout << "Dialog rejected!" << endl;
  }  

  delete wpu;
}


void BeamlineBrowser::infoWriter::visitSbend( const sbend* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlb2 = new QLabel( QString("Length [m]: "), qhb1 );
        QString stlen;
        stlen.setNum( x->Length() );
        QLineEdit* qle1 = new QLineEdit( stlen, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QLabel* qlb = new QLabel( QString("Magnetic field [T]: "), qhb2 );
        QString sts;
        sts.setNum( x->Strength() );
        QLineEdit* qle2 = new QLineEdit( sts, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb3 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb3 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, because of the flag Qt::WDestructiveClose.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( stlen != qle1->text() ) {
      bool ok;
      double newLength = (qle1->text()).toDouble( &ok );
      if( ok ) {
        if( 0 != _contextPtr->setLength( (sbend*) x, newLength ) ) { 
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": sbend not found in context."
               << endl;
	}
      }
    }
    if( sts != qle2->text() ) {
      bool ok;
      double newStrength = (qle2->text()).toDouble( &ok );
      if( ok ) {
        if( 0 != _contextPtr->setStrength( (sbend*) x, newStrength ) ) { 
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": sbend not found in context."
               << endl;
	}
      }
    }
  }

  delete wpu;

  // =========================================================
  // QString stl;
  // QString sts;
  // stl.setNum( x->Length() );
  // sts.setNum( x->Strength() );
  // QMessageBox::information( 
  //         0, 
  //         QString(x->Type())+QString(": ")+QString(x->Name()),
  //         QString("Length [m]: ")+stl+QString("  Bend field [T]: ")+sts
  //         );
}


void BeamlineBrowser::infoWriter::visitCF_sbend( const CF_sbend* x )
{
  QString stl;
  QString sts;
  QString stq;
  stl.setNum( x->Length() );
  sts.setNum( x->Strength() );
  stq.setNum( x->getQuadrupole()/x->Length() );  
  QMessageBox::information( 
          0, 
          QString(x->Type())+QString(": ")+QString(x->Name()),
          QString("Length [m]: ")+stl+
          QString("  Bend field [T]: ")+sts+
          QString("  Quad gradient [T/m]: ")+stq
          );
}


// REMOVE: void BeamlineBrowser::infoWriter::visitCF_rbend( const CF_rbend* x )
// REMOVE: {
// REMOVE:   QString stl;
// REMOVE:   QString sts;
// REMOVE:   QString stq;
// REMOVE:   stl.setNum( x->Length() );
// REMOVE:   sts.setNum( x->Strength() );
// REMOVE:   stq.setNum( x->getQuadrupole()/x->Length() );
// REMOVE:   QMessageBox::information( 
// REMOVE:           0, 
// REMOVE:           QString(x->Type())+QString(": ")+QString(x->Name()),
// REMOVE:           QString("Length [m]: ")+stl+
// REMOVE:           QString("\nBend field [T]: ")+sts+
// REMOVE:           QString("\nQuad gradient [T/m]: ")+stq
// REMOVE:           );
// REMOVE: }


// ??? The argument is not const. It's not supposed to be.
// ??? Change the argument???
void BeamlineBrowser::infoWriter::visitCF_rbend( const CF_rbend* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlb = new QLabel( QString("Field [T]: "), qhb1 );
        QString sts;
        sts.setNum( x->Strength() );
        QLineEdit* qle = new QLineEdit( sts, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        QLabel* qlb4 = new QLabel( QString("Gradient [T/m]: "), qhb4 );
        QString stg;
        stg.setNum( x->getQuadrupole() / x->Length() );
        QLineEdit* qle4 = new QLineEdit( stg, qhb4 );
      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QLabel* qlb2 = new QLabel( QString("Roll angle [mrad]: "), qhb2 );
        alignmentData ad(x->Alignment());
        QString str;
        str.setNum( 1000.*(ad.tilt /*[rad]*/) );
        QLineEdit* qle2 = new QLineEdit( str, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb3 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb3 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();


  // Change dipole field
  if( returnCode == QDialog::Accepted ) {
    if( sts != qle->text() ) {
      bool ok;
      double newStrength = (qle->text()).toDouble( &ok );
      if( ok ) {
        // if( 0 != _contextPtr->setStrength( (quadrupole*) x, newStrength ) ) {
        //   cerr << "*** WARNING *** File "
        //        << __FILE__ 
        //        << ", Line "
        //        << __LINE__
        //        << ": quadrupole not found in context."
        //        << endl;
        // }
        
        QMessageBox::information( 0, "CHEF: SORRY",
                                  "Field change not implemented." );
      }
    }

    // Change quadrupole field
    if( stg != qle4->text() ) {
      bool ok;
      double newQuadStrength = (qle4->text()).toDouble( &ok );
      newQuadStrength *= x->Length();
      if( ok ) {
        // if( 0 != _contextPtr->setQuadrupole( (quadrupole*) x, newStrength ) ) {
        //   cerr << "*** WARNING *** File "
        //        << __FILE__ 
        //        << ", Line "
        //        << __LINE__
        //        << ": quadrupole not found in context."
        //        << endl;
        // }
        
        QMessageBox::information( 0, "CHEF: SORRY",
                                  "Field change not implemented." );
      }
    }

    // Change alignment
    if( str != qle2->text() ) {
      bool ok;
      ad.tilt /*[rad]*/ = 0.001*((qle2->text()).toDouble( &ok ) /*[mrad]*/);
      if( ok ) {
        if( 0 != _contextPtr->setAlignment( (CF_rbend*) x,  ad ) ) {
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": CF_rbend not found in context."
               << endl;
	}
      }
    }

  }

  delete wpu;
}


void BeamlineBrowser::infoWriter::visitQuadrupole( const quadrupole* x )
{
  // QString stl;
  // QString sts;
  // stl.setNum( x->Length() );
  // sts.setNum( x->Strength() );
  // QMessageBox::information( 
  //         0, 
  //         QString(x->Type())+QString(": ")+QString(x->Name()),
  //         QString("Length: ")+stl+QString("\nGradient [T/m]: ")+sts
  //         );

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlb = new QLabel( QString("Gradient [T/m]: "), qhb1 );
        QString sts;
        sts.setNum( x->Strength() );
        QLineEdit* qle = new QLineEdit( sts, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QLabel* qlb2 = new QLabel( QString("Roll angle [mrad]: "), qhb2 );
        alignmentData ad(x->Alignment());
        QString str;
        str.setNum( 1000.*(ad.tilt /*[rad]*/) );
        QLineEdit* qle2 = new QLineEdit( str, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb3 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb3 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( sts != qle->text() ) {
      bool ok;
      double newStrength = (qle->text()).toDouble( &ok );
      if( ok ) {
        if( 0 != _contextPtr->setStrength( (quadrupole*) x, newStrength ) ) {
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": quadrupole not found in context."
               << endl;
	}
      }
    }

    if( str != qle2->text() ) {
      bool ok;
      ad.tilt /*[rad]*/ = 0.001*((qle2->text()).toDouble( &ok ) /*[mrad]*/);
      if( ok ) {
        if( 0 != _contextPtr->setAlignment( (quadrupole*) x,  ad ) ) {
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": quadrupole not found in context."
               << endl;
	}
      }
    }

  }

  delete wpu;
}


void BeamlineBrowser::infoWriter::visitThinQuad( const thinQuad* x )
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlb = new QLabel( QString("Integrated gradient [T]: "), qhb1 );
        QString sts;
        sts.setNum( x->Strength() );
        QLineEdit* qle = new QLineEdit( sts, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QLabel* qlb2 = new QLabel( QString("Roll angle [mrad]: "), qhb2 );
        alignmentData ad(x->Alignment());
        QString str;
        str.setNum( 1000.*(ad.tilt /*[rad]*/) );
        QLineEdit* qle2 = new QLineEdit( str, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhb3 );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhb3 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();
  // Note: when reject is activated, wpu and all its subwidgets
  //       will be deleted, because of the flag Qt::WDestructiveClose.
  //       This is confirmed by changing these from pointers to objects.
  //       A warning message is issued, when exiting this scope, that
  //       the objects are deleted twice.
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    if( sts != qle->text() ) {
      bool ok;
      double newStrength = (qle->text()).toDouble( &ok );
      if( ok ) {
        if( 0 != _contextPtr->setStrength( (thinQuad*) x, newStrength ) ) {
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": thinQuad not found in context."
               << endl;
	}
      }
    }

    if( str != qle2->text() ) {
      bool ok;
      ad.tilt /*[rad]*/ = 0.001*((qle2->text()).toDouble( &ok ) /*[mrad]*/);
      if( ok ) {
        if( 0 != _contextPtr->setAlignment( (thinQuad*) x, ad ) ) {
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": thinQuad not found in context."
               << endl;
	}
      }
    }

  }

  delete wpu;
}


void BeamlineBrowser::infoWriter::visitSextupole( const sextupole* x )
{
  QString stl;
  QString sts;
  stl.setNum( x->Length() );
  sts.setNum( x->Strength() );
  QMessageBox::information( 
          0, 
          QString(x->Type())+QString(": ")+QString(x->Name()),
          QString("Length: ")+stl+QString("\nStrength [T/m**2]: ")+sts
          );
}


void BeamlineBrowser::infoWriter::visitMarker( const marker* x )
{
  QString stl;
  QString sts;
  stl.setNum( x->Length() );
  sts.setNum( x->Strength() );
  QMessageBox::information( 
          0, 
          QString(x->Type())+QString(": ")+QString(x->Name()),
          "What do you expect here?\nIt's a marker!"
          );
}


// void BeamlineBrowser::infoWriter::visitSector( const sector* x )
// {
//   MatrixD M( x->getMap().Jacobian() );
// 
//   QString matrixElement;
//   QString matrixDisplay;
//   const QString spacer("  "); 
//   const QString eol("\n");
// 
//   for( int i = 0; i < M.rows(); i++ ) {
//     for( int j = 0; j < M.cols(); j++ ) {
//       matrixElement.setNum( M(i,j) );
//       matrixDisplay = matrixDisplay + matrixElement + spacer;
//     }
//     matrixDisplay = matrixDisplay + eol;
//   }
// 
//   QDialog* wpu = new QDialog( 0, 0, true );
// 
//     QVBox* qvb = new QVBox( wpu );
// 
//       QTextView* qtv = new QTextView( matrixDisplay, QString::null, qvb );
//       int drawArea = (50*QApplication::desktop()->height()*
//                          QApplication::desktop()->width())/100;
//       int fixedWidth = (int) sqrt( (double) drawArea );
//       qtv->setFixedSize( fixedWidth, fixedWidth );
// 
//       QHBox* qhb3 = new QHBox( qvb );
//         QPushButton* doneBtn = new QPushButton( "Done", qhb3 );
//           doneBtn->setDefault( true );
//           connect( doneBtn, SIGNAL(pressed()),
//                    wpu,     SLOT(accept()) );
//       qhb3->setMargin(5);
//       qhb3->setSpacing(3);
//       qhb3->adjustSize();
// 
//     qvb->adjustSize();
// 
//   wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
//   wpu->adjustSize();
// 
//   wpu->exec();
//   delete wpu;
// }


void BeamlineBrowser::infoWriter::visitSector( const sector* x )
{
  Mapping theMap( x->getMap() );
  MatrixD M( theMap.Jacobian() );

  ostringstream zlorfik;
  zlorfik << M;
  zlorfik << "\nBegin the mapping " << endl;
  zlorfik << theMap << endl;
  zlorfik << "\nEnd the mapping " << endl;
  zlorfik << '\0';

  string zlorfik_str( zlorfik.str() );

  QString buf( zlorfik_str.c_str() );
  // QTextOStream outStream( &buf );
  // outStream << M;
  // outStream << "\nBegin the mapping " << endl;
  // outStream << theMap << endl;
  // outStream << "\nEnd the mapping " << endl;
  // outStream << '\0';

  QDialog* wpu = new QDialog( 0, 0, true );

    QVBox* qvb = new QVBox( wpu );

    QTextBrowser* qtv = new QTextBrowser( qvb );
      qtv->setSource( buf );
      // QTextView* qtv = new QTextView( outStream.str(), QString::null, qvb );
      int drawArea = (50*QApplication::desktop()->height()*
                         QApplication::desktop()->width())/100;
      int fixedWidth = (int) sqrt( (double) drawArea );
      qtv->setFixedSize( fixedWidth, fixedWidth );

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* doneBtn = new QPushButton( "Done", qhb3 );
          doneBtn->setDefault( true );
          connect( doneBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->exec();
  delete wpu;
}
