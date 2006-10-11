/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      BeamlineBrowser.cc
******  Version:   3.3
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
#include <iosetup.h>

// REMOVE: #include <qdir.h>
// REMOVE: #include <qfile.h>
// REMOVE: #include <qfileinfo.h>
#include <qpixmap.h>
// REMOVE: #include <qevent.h>
// REMOVE: #include <qpoint.h>
#include <qmessagebox.h>
#include <qdragobject.h>
// REMOVE: #include <qmime.h>
// REMOVE: #include <qstrlist.h>
// REMOVE: #include <qstringlist.h>
#include <qapplication.h>
#include <qheader.h>
#include <qdialog.h>
#include <qpushbutton.h>
// REMOVE: #include <qradiobutton.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
// REMOVE: #include <qlineedit.h>
#include <qlayout.h>
#include <qtextbrowser.h>

#include <BeamlineBrowser.h>
// REMOVE: #include "PhysicsConstants.h>
#include <GenericException.h>
#include <BeamlineIterator.h>
#include <BeamlineContext.h>
#include <bmlfactory.h>
#include <BeamlineExpressionTree.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


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

// const char* BeamlineBrowser::bmr_black_xpm[19]={
// "  16 16 2 1",
// "# c #000000",
// ". c #ffffff",
// "...........##...",
// "....########....",
// "...########.....",
// "..##...#........",
// ".......#........",
// ".....#####......",
// ".....#####......",
// ".......#........",
// ".......#........",
// ".....#####......",
// ".....#####......",
// ".......#........",
// ".......#...##...",
// "....########....",
// "...########.....",
// "..##............"
// };
// 
// const char* BeamlineBrowser::bmr_orange_xpm[19]={
// "  16 16 2 1",
// "# c #ff8800",
// ". c #ffffff",
// "...........##...",
// "....########....",
// "...########.....",
// "..##...#........",
// ".......#........",
// ".....#####......",
// ".....#####......",
// ".......#........",
// ".......#........",
// ".....#####......",
// ".....#####......",
// ".......#........",
// ".......#...##...",
// "....########....",
// "...########.....",
// "..##............"
// };

const char* BeamlineBrowser::bmr_black_xpm[19]={
"  16 16 2 1",
"# c #000000",
". c #ffffff",
"................",
"....########....",
"...##########...",
"..##...#....##..",
".##....#.....##.",
"##...#####...##.",
"##...#####...##.",
"##.....#.....##.",
"##.....#.....##.",
"##...#####...##.",
"##...#####...##.",
"##.....#.....##.",
".##....#...##...",
"..##########....",
"...########.....",
"................"
};

const char* BeamlineBrowser::bmr_orange_xpm[19]={
"  16 16 2 1",
"# c #ff8800",
". c #ffffff",
"................",
"....########....",
"...##########...",
"..##...#....##..",
".##....#.....##.",
"##...#####...##.",
"##...#####...##.",
"##.....#.....##.",
"##.....#.....##.",
"##...#####...##.",
"##...#####...##.",
"##.....#.....##.",
".##....#...##...",
"..##########....",
"...########.....",
"................"
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
QPixmap* BeamlineBrowser::bmrBlackSymbol  = 0;
QPixmap* BeamlineBrowser::sextupoleSymbol = 0;
QPixmap* BeamlineBrowser::fquadSymbol     = 0;
QPixmap* BeamlineBrowser::dquadSymbol     = 0;
QPixmap* BeamlineBrowser::bmlOrangeSymbol = 0;
QPixmap* BeamlineBrowser::bmrOrangeSymbol = 0;
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


const QBmlRoot* QBml::topBmlParent()
{
  // Must not return null pointer.
  if( !_p ) { return dynamic_cast<QBmlRoot*>(this); }

  QBmlRoot* ret   = _p;
  QBmlRoot* tsts  = ret->_p;

  while( tsts ) {
    ret = tsts;
    tsts  = ret->_p;
  }

  return ret;
}


// *****************************************************************************
// *
// * Class QBmlElmt
// *
// *****************************************************************************

QBmlElmt::QBmlElmt( QBmlRoot* parent, /* const */ bmlnElmnt* q, double& s )
: QBml( 0, parent, q->Name(), q->Type() ),
  _myElement(q),
  _azimuth(s) 
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


QBmlRoot::QBmlRoot( QListView* parent, const Particle& prt, beamline* q, double& s )
: QBml( 0, parent, q->Name(), q->Type() ), 
  _myBeamline(0)
{
    QString str;
    str.setNum( ((double) s) );
    this->setText( 2, str );

    _myBmlCon = new BeamlineContext( prt, q, true );
}


QBmlRoot::QBmlRoot( QBmlRoot * parent, /* const */ beamline* q, double& s )
: 
   QBml( 0, parent, q->Name(), q->Type() ),
   _myBmlCon(0),
   _myBeamline(q) 
{
    QString str;
    str.setNum( ((double) s) );
    this->setText( 2, str );

    if( 0 == strcmp(q->Type(),"beamline") ) {
      setPixmap( BeamlineBrowser::bmlBlackSymbol );
    }
    else {
      setPixmap( BeamlineBrowser::elmntSymbol );
    }
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
  if( !_myBmlCon ) {
    if ( o ) {
      setPixmap( BeamlineBrowser::bmlOrangeSymbol );
    }
    else {
      setPixmap( BeamlineBrowser::bmlBlackSymbol );
    }
  }
  else if( _myBmlCon->isTreatedAsRing() ) {
    if ( o ) {
      setPixmap( BeamlineBrowser::bmrOrangeSymbol );
    }
    else {
      setPixmap( BeamlineBrowser::bmrBlackSymbol );
    }
  }
  else {
    if ( o ) {
      setPixmap( BeamlineBrowser::bmlOrangeSymbol );
    }
    else {
      setPixmap( BeamlineBrowser::bmlBlackSymbol );
    }
  }

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

BeamlineBrowser::BeamlineBrowser( QWidget *parent, const char *name, bool /*sdo*/ )
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
      BeamlineBrowser::bmrBlackSymbol = new QPixmap( bmr_black_xpm );
      BeamlineBrowser::bmrOrangeSymbol = new QPixmap( bmr_orange_xpm );
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
  bool found = ( bcPtr == fc->_myBmlCon );
  while( !found ) {
    fc = dynamic_cast<QBmlRoot*>( fc->nextSibling() );
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
      if( 0 != ( ((QBmlRoot*) j) -> _myBmlCon ) ) {
        emit sig_bmlLeftClicked( ((QBmlRoot*) j) -> _myBmlCon, ((QBmlRoot*) j) );
      }
    }
    emit sig_bmlLeftClicked( j );  // No longer necessary???
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


void BeamlineBrowser::resetPixmap( const BeamlineContext* bcPtr )
{
  if( 0 == bcPtr ) {
    QMessageBox::information( 0, "CHEF: WARNING", 
      "Null pointer passed to BeamlineContext::resetPixmap." 
      "\nNo action will be taken." );
    return;
  }

  QBmlRoot* ptrRoot = dynamic_cast<QBmlRoot*>( this->firstChild() );
  bool found = ( bcPtr == ptrRoot->_myBmlCon );
  while( !found ) {
    ptrRoot = dynamic_cast<QBmlRoot*>( ptrRoot->nextSibling() );
    found = ( bcPtr == ptrRoot->_myBmlCon );
  }

  if( !found ) {
    ostringstream uic;
    uic << "An impossibility has occurred!"
           "\nFile " << __FILE__ << " at line " << __LINE__
        << "\ninside BeamlineContext::resetPixmap."
        << "\nCould not find indicated BeamlineContext."
           "\nNo action will be taken.";
    QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }

  if( bcPtr->isTreatedAsRing() ) {
    if( ptrRoot->isOpen() ) {
      ptrRoot->setPixmap( BeamlineBrowser::bmrOrangeSymbol );
    }
    else {
      ptrRoot->setPixmap( BeamlineBrowser::bmrBlackSymbol );
    }
  }
  else {
    if( ptrRoot->isOpen() ) {
      ptrRoot->setPixmap( BeamlineBrowser::bmlOrangeSymbol );
    }
    else {
      ptrRoot->setPixmap( BeamlineBrowser::bmlBlackSymbol );
    }
  }

  this->repaint();
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

  BeamlineContext* www = 0;
  if( 0 == strcmp( "PROTON", bf.getParticleType() ) ) {
    www = new BeamlineContext( Proton(pBml->Energy()), pBml, false );
  }
  else if( 0 == strcmp( "POSITRON", bf.getParticleType() ) ) {
    www = new BeamlineContext( Positron(pBml->Energy()), pBml, false );
  }
  else {
    ostringstream uic;
    uic << "Particle type "
        << bf.getParticleType()
        << " not recognized.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    throw GenericException( __FILE__, __LINE__, 
          "BeamlineBrowser::readMADFile",
          uic.str().c_str() ) ;
  }

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
  if( !ptr ) {
    ostringstream uic;
    uic << "Null pointer was passed to BeamlineBrowser::displayBeamline"
           "\nFile "
        << __FILE__
        << " at line " << __LINE__ 
        << "\nException will be thrown.";
    QMessageBox::information( 0, "CHEF: ERROR", uic.str().c_str() );
    throw( GenericException( __FILE__, __LINE__, 
           "void BeamlineBrowser::displayBeamline( const BeamlineContext* ptr )",
           "Null pointer passed as argument." ) );
    return;
  }

  double s = ptr->sumLengths();
  QString str1, str2;

  QBmlRoot* root = new QBmlRoot( this, (BeamlineContext*) ptr, s );
  if( ptr->isTreatedAsRing() ) {
    root->setPixmap( BeamlineBrowser::bmrBlackSymbol ); 
  }
  else {
    root->setPixmap( BeamlineBrowser::bmlBlackSymbol );
  }
  _topLevelItems.insert( (void*) root );

  str2.setNum(s);

  ReverseBeamlineIterator rbi( *ptr->cheatBmlPtr() );
  _displayLine( root, rbi, s );

  str1.setNum( s );
  root->setText( 2, str1 + "-" + str2 );

  this->setSelected ( root, true );
  this->setCurrentItem( root );
  this->ensureItemVisible( root );

  if( 0 != ( root->_myBmlCon ) ) {
    emit sig_bmlLeftClicked( root->_myBmlCon, root );
  }
  else {
    ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nBeamlineBrowser::displayBeamline"
           "\nAn impossibility has occurred:"
           "\ncreation of a null BeamlineContext.";
    QMessageBox::information( 0, "BeamlineBrowser: WARNING", uic.str().c_str() );
  }
  emit sig_bmlLeftClicked( root );  // No longer necessary???

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
      ReverseBeamlineIterator newbi( *static_cast<beamline*>(q) );
      str2.setNum(s);
      BeamlineBrowser::_displayLine( newBml, newbi, s );
      str1.setNum( s );
      newBml->setText( 2, str1 + "-" + str2 );
    }
    else {
      if( firstTime ) {
        (*pcerr) << "*** WARNING ***                              \n"
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
// REMOVE         (*pcout) << ++i << " !  " << qbmlelmtPtr->fullName() << endl;
// REMOVE       }
// REMOVE       else {
// REMOVE         (*pcout) << ++i << "    " << qbmlelmtPtr->fullName() << endl;
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
        qbmlPtr->repaint();  // See note below.
        ret = 0;
      }
    }
    ++looker;

  }

  // Note: Repainting each individual item is necessary. 
  //   Without it, if the tree structure has been expanded,
  //   not all selected items get painted correctly, and
  //   those that do are painted sloppily. This must be a
  //   bug in the Qt graphics widget.
  // I tried adding a line at the end: either either
  //   "this->repaint();" or "this->update();" Neither one
  //   fixed the problem. It was fixed only by repainting each
  //   individual item as it was selected.
  // The Qt manual advises using update() rather than
  //   repaint(). The latter is more immediate, but the
  //   former is less dangerous and allegedly has reduced 
  //   flicker. However, repaint() seems to work fine.
  // 
  // - Leo Michelotti  
  //   December 10, 2004

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

  if( 0 == startpoint ) {
    ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nNull argument passed to function findAllSelected.";
    QMessageBox::information( 0, "BeamlineBrowser: WARNING", uic.str().c_str() );
    return ret;
  }

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


//////////////////////////////////////////////////
// BeamlineBrowser::infoWriter implementation
/////////////////////////////////////////////////

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
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog;
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Length [m] = ", qhb1 );
        QString stl;
        stl.setNum( x->Length() );
        new QLabel( stl, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb2 );
          closeBtn->setDefault( true );
          connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
	  // REMOVE:       QPushButton* editBtn = new QPushButton( "Edit", qhb2 );
	  // REMOVE:         connect( editBtn, SIGNAL(pressed()),
	  // REMOVE:                  wpu,     SLOT(accept()) );
      qhb2->setMargin(10);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: int returnCode = wpu->exec();
  // REMOVE: // Note: when reject is activated, wpu and all its subwidgets
  // REMOVE: //       will be deleted, if the flag Qt::WDestructiveClose is used.
  // REMOVE: //       This is confirmed by changing these from pointers to objects.
  // REMOVE: //       A warning message is issued, when exiting this scope, that
  // REMOVE: //       the objects are deleted twice.

  // REMOVE: if( returnCode == QDialog::Accepted ) {
  // REMOVE:   editDialog edg;
  // REMOVE:   edg._contextPtr = _contextPtr;
  // REMOVE:   (const_cast<drift*>(x))->accept(edg);
  // REMOVE: }

  // REMOVE: delete wpu;
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

  
  // REMOVE: { // Old version
  // REMOVE: QString sts;
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
  // REMOVE:   QVBox* qvb = new QVBox( wpu );
  // REMOVE:     QHBox* qhbFrames = new QHBox( qvb );
  // REMOVE: 
  // REMOVE:       QVBox* qvbIn = new QVBox( qhbFrames );
  // REMOVE:         new QLabel( "Upstream frame", qvbIn );
  // REMOVE: 
  // REMOVE:         QHBox* qhboIn = new QHBox( qvbIn );
  // REMOVE:           new QLabel( QString("O: "), qhboIn );
  // REMOVE:           sts.setNum( (x->getInFrame().getOrigin()) (0) );
  // REMOVE:           new QLabel( sts, qhboIn );
  // REMOVE:           sts.setNum( (x->getInFrame().getOrigin()) (1) );
  // REMOVE:           new QLabel( sts, qhboIn );
  // REMOVE:           sts.setNum( (x->getInFrame().getOrigin()) (2) );
  // REMOVE:           new QLabel( sts, qhboIn );
  // REMOVE:         qhboIn->setMargin(5);
  // REMOVE:         qhboIn->setSpacing(3);
  // REMOVE:         qhboIn->adjustSize();
  // REMOVE:   
  // REMOVE:         QHBox* qhb3 = new QHBox( qvbIn );
  // REMOVE:           QString sts0( "U: ( " );
  // REMOVE:           sts.setNum( (x->getInFrame().getxAxis()) (0) );
  // REMOVE:           sts0 = sts0 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getInFrame().getxAxis()) (1) );
  // REMOVE:           sts0 = sts0 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getInFrame().getxAxis()) (2) );
  // REMOVE:           sts0 = sts0 + sts + " )";
  // REMOVE:           new QLabel( sts0, qhb3 );
  // REMOVE:         qhb3->setMargin(5);
  // REMOVE:         qhb3->setSpacing(3);
  // REMOVE:         qhb3->adjustSize();
  // REMOVE:   
  // REMOVE:         QHBox* qhb4 = new QHBox( qvbIn );
  // REMOVE:           QString sts1( "V: ( " );
  // REMOVE:           sts.setNum( (x->getInFrame().getyAxis()) (0) );
  // REMOVE:           sts1 = sts1 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getInFrame().getyAxis()) (1) );
  // REMOVE:           sts1 = sts1 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getInFrame().getyAxis()) (2) );
  // REMOVE:           sts1 = sts1 + sts + " )";
  // REMOVE:           new QLabel( sts1, qhb4 );
  // REMOVE:         qhb4->setMargin(5);
  // REMOVE:         qhb4->setSpacing(3);
  // REMOVE:         qhb4->adjustSize();
  // REMOVE:   
  // REMOVE:         QHBox* qhb5 = new QHBox( qvbIn );
  // REMOVE:           QString sts2( "W: ( " );
  // REMOVE:           sts.setNum( (x->getInFrame().getzAxis()) (0) );
  // REMOVE:           sts2 = sts2 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getInFrame().getzAxis()) (1) );
  // REMOVE:           sts2 = sts2 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getInFrame().getzAxis()) (2) );
  // REMOVE:           sts2 = sts2 + sts + " )";
  // REMOVE:           new QLabel( sts2, qhb5 );
  // REMOVE:         qhb5->setMargin(5);
  // REMOVE:         qhb5->setSpacing(3);
  // REMOVE:         qhb5->adjustSize();
  // REMOVE: 
  // REMOVE:       qvbIn->setMargin(5);
  // REMOVE:       qvbIn->setSpacing(3);
  // REMOVE:       qvbIn->adjustSize();
  // REMOVE: 
  // REMOVE:       QVBox* qvbOut = new QVBox( qhbFrames );
  // REMOVE:         new QLabel( "Downstream frame", qvbOut );
  // REMOVE: 
  // REMOVE:         QHBox* qhbo = new QHBox( qvbOut );
  // REMOVE:           new QLabel( QString("O: "), qhbo );
  // REMOVE:           sts.setNum( (x->getOutFrame().getOrigin()) (0) );
  // REMOVE:           new QLabel( sts, qhbo );
  // REMOVE:           sts.setNum( (x->getOutFrame().getOrigin()) (1) );
  // REMOVE:           new QLabel( sts, qhbo );
  // REMOVE:           sts.setNum( (x->getOutFrame().getOrigin()) (2) );
  // REMOVE:           new QLabel( sts, qhbo );
  // REMOVE:         qhbo->setMargin(5);
  // REMOVE:         qhbo->setSpacing(3);
  // REMOVE:         qhbo->adjustSize();
  // REMOVE:   
  // REMOVE:         QHBox* qhb0 = new QHBox( qvbOut );
  // REMOVE:           sts0 = "U: ( ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getxAxis()) (0) );
  // REMOVE:           sts0 = sts0 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getxAxis()) (1) );
  // REMOVE:           sts0 = sts0 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getxAxis()) (2) );
  // REMOVE:           sts0 = sts0 + sts + " )";
  // REMOVE:           new QLabel( sts0, qhb0 );
  // REMOVE:         qhb0->setMargin(5);
  // REMOVE:         qhb0->setSpacing(3);
  // REMOVE:         qhb0->adjustSize();
  // REMOVE:   
  // REMOVE:         QHBox* qhb1 = new QHBox( qvbOut );
  // REMOVE:           sts1 = "V: ( ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getyAxis()) (0) );
  // REMOVE:           sts1 = sts1 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getyAxis()) (1) );
  // REMOVE:           sts1 = sts1 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getyAxis()) (2) );
  // REMOVE:           sts1 = sts1 + sts + " )";
  // REMOVE:           new QLabel( sts1, qhb1 );
  // REMOVE:         qhb1->setMargin(5);
  // REMOVE:         qhb1->setSpacing(3);
  // REMOVE:         qhb1->adjustSize();
  // REMOVE:   
  // REMOVE:         QHBox* qhb2 = new QHBox( qvbOut );
  // REMOVE:           sts2 = "W: ( ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getzAxis()) (0) );
  // REMOVE:           sts2 = sts2 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getzAxis()) (1) );
  // REMOVE:           sts2 = sts2 + sts + ", ";
  // REMOVE:           sts.setNum( (x->getOutFrame().getzAxis()) (2) );
  // REMOVE:           sts2 = sts2 + sts + " )";
  // REMOVE:           new QLabel( sts2, qhb2 );
  // REMOVE:         qhb2->setMargin(5);
  // REMOVE:         qhb2->setSpacing(3);
  // REMOVE:         qhb2->adjustSize();
  // REMOVE: 
  // REMOVE:       qvbOut->setMargin(5);
  // REMOVE:       qvbOut->setSpacing(3);
  // REMOVE:       qvbOut->adjustSize();
  // REMOVE: 
  // REMOVE:     qhbFrames->setMargin(5);
  // REMOVE:     qhbFrames->setSpacing(3);
  // REMOVE:     qhbFrames->adjustSize();
  // REMOVE: 
  // REMOVE:     QHBox* qhb99 = new QHBox( qvb );
  // REMOVE:       QPushButton* cancelBtn = new QPushButton( "Close", qhb99 );
  // REMOVE:         connect( cancelBtn, SIGNAL(pressed()),
  // REMOVE:                  wpu,       SLOT(close()) );
  // REMOVE:     qhb99->setMargin(5);
  // REMOVE:     qhb99->setSpacing(3);
  // REMOVE:     qhb99->adjustSize();
  // REMOVE: 
  // REMOVE:   qvb->adjustSize();
  // REMOVE: 
  // REMOVE: wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  // REMOVE: wpu->adjustSize();
  // REMOVE: 
  // REMOVE: wpu->exec();
  // REMOVE: 
  // REMOVE: delete wpu;
  // REMOVE: }

  { // New version
  QString xstr, ystr, zstr;
  QString lparen("( ");
  QString rparen(" )");
  QString comma(", ");
  
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );

    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 5, 3, 5, 10 );

         qgl->addWidget( new QLabel( QString("Upstream"),   qwa ), 0, 1 );
         qgl->addWidget( new QLabel( QString("Downstream"), qwa ), 0, 2 );
         
         qgl->addWidget( new QLabel( QString("Origin   "), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("U   "), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString("V   "), qwa ), 3, 0 );
         qgl->addWidget( new QLabel( QString("W   "), qwa ), 4, 0 );

         xstr.setNum( (x->getInFrame().getOrigin())(0) );
         ystr.setNum( (x->getInFrame().getOrigin())(1) );
         zstr.setNum( (x->getInFrame().getOrigin())(2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 1 );
         xstr.setNum( (x->getOutFrame().getOrigin())(0) );
         ystr.setNum( (x->getOutFrame().getOrigin())(1) );
         zstr.setNum( (x->getOutFrame().getOrigin())(2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 2 );

         xstr.setNum( (x->getInFrame().getxAxis()) (0) );
         ystr.setNum( (x->getInFrame().getxAxis()) (1) );
         zstr.setNum( (x->getInFrame().getxAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 1 );
         xstr.setNum( (x->getInFrame().getyAxis()) (0) );
         ystr.setNum( (x->getInFrame().getyAxis()) (1) );
         zstr.setNum( (x->getInFrame().getyAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 1 );
         xstr.setNum( (x->getInFrame().getzAxis()) (0) );
         ystr.setNum( (x->getInFrame().getzAxis()) (1) );
         zstr.setNum( (x->getInFrame().getzAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 1 );
    
         xstr.setNum( (x->getOutFrame().getxAxis()) (0) );
         ystr.setNum( (x->getOutFrame().getxAxis()) (1) );
         zstr.setNum( (x->getOutFrame().getxAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 2 );
         xstr.setNum( (x->getOutFrame().getyAxis()) (0) );
         ystr.setNum( (x->getOutFrame().getyAxis()) (1) );
         zstr.setNum( (x->getOutFrame().getyAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 2 );
         xstr.setNum( (x->getOutFrame().getzAxis()) (0) );
         ystr.setNum( (x->getOutFrame().getzAxis()) (1) );
         zstr.setNum( (x->getOutFrame().getzAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 2 );
    
      qwa->adjustSize();

      QHBox* qhb99 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb99 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb99->setMargin(5);
      qhb99->setSpacing(3);
      qhb99->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
  }
}


void BeamlineBrowser::infoWriter::visitSbend( const sbend* x )
{
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( QString("Length [m]: "), qhb1 );
        QString stlen;
        stlen.setNum( x->Length() );
        new QLabel( stlen, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        new QLabel( QString("Magnetic field [T]: "), qhb2 );
        QString sts;
        sts.setNum( x->Strength() );
        new QLabel( sts, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb3 );
          connect( cancelBtn, SIGNAL(pressed()),
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
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
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


void BeamlineBrowser::infoWriter::visitRbend( const rbend* x )
{
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 3, 3, 5 );

         qgl->addWidget( new QLabel( QString("Length"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [m]: "), qwa ), 0, 1 );
           QString stlen;
           stlen.setNum( x->Length() );
         qgl->addWidget( new QLabel( stlen, qwa ), 0, 2 );

         qgl->addWidget( new QLabel( QString("Magnetic field"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [T]: "), qwa ), 1, 1 );
           QString sts;
           sts.setNum( x->Strength() );
         qgl->addWidget( new QLabel( sts, qwa ), 1, 2 );

         qgl->addWidget( new QLabel( QString("Nominal entry angle"), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]: "), qwa ), 2, 1 );
           sts.setNum( 1000.0 * x->getPoleFaceAngle() );
         qgl->addWidget( new QLabel( sts, qwa ), 2, 2 );

       qwa->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb4 );
          closeBtn->setDefault( true );
          connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
	  // REMOVE:       QPushButton* editBtn = new QPushButton( "Edit", qhb4 );
	  // REMOVE:         connect( editBtn, SIGNAL(pressed()),
	  // REMOVE:                  wpu,     SLOT(accept()) );

      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: int returnCode = wpu->exec();

  // REMOVE: if( returnCode == QDialog::Accepted ) {
  // REMOVE:   editDialog edg;
  // REMOVE:   edg._contextPtr = _contextPtr;
  // REMOVE:   ((rbend*) x)->accept(edg);
  // REMOVE: }

  // REMOVE: delete wpu;
}


// ??? The argument is not const. It's not supposed to be.
// ??? Change the argument???
void BeamlineBrowser::infoWriter::visitCF_rbend( const CF_rbend* x )
{
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( QString("Field [T]: "), qhb1 );
        QString sts;
        sts.setNum( x->Strength() );
        new QLabel( sts, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        new QLabel( QString("Gradient [T/m]: "), qhb4 );
        QString stg;
        stg.setNum( x->getQuadrupole() / x->Length() );
        new QLabel( stg, qhb4 );
      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        new QLabel( QString("Roll angle [mrad]: "), qhb2 );
        alignmentData ad(x->Alignment());
        QString str;
        str.setNum( 1000.*(ad.tilt /*[rad]*/) );
        new QLabel( str, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb3 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
}


void BeamlineBrowser::infoWriter::visitQuadrupole( const quadrupole* x )
{
  QString theValue;
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog;
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( QString("Gradient [T/m]: "), qhb1 );
        theValue.setNum( x->Strength() );
        new QLabel( theValue, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        new QLabel( QString("Length: "), qhb2 );
        theValue.setNum( x->Length() );
        new QLabel( theValue, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        new QLabel( QString("Roll angle [mrad]: "), qhb3 );
        alignmentData ad(x->Alignment());
        theValue.setNum( 1000.*(ad.tilt /*[rad]*/) );
        new QLabel( theValue, qhb3 );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb4 );
          closeBtn->setDefault( true );
          connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
	  // REMOVE:       QPushButton* editBtn = new QPushButton( "Edit", qhb4 );
	  // REMOVE:         connect( editBtn, SIGNAL(pressed()),
	  // REMOVE:                  wpu,     SLOT(accept()) );
      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: int returnCode = wpu->exec();
  // REMOVE: // Note: when reject is activated, wpu and all its subwidgets
  // REMOVE: //       will be deleted, if the flag Qt::WDestructiveClose is used.
  // REMOVE: //       This is confirmed by changing these from pointers to objects.
  // REMOVE: //       A warning message is issued, when exiting this scope, that
  // REMOVE: //       the objects are deleted twice.

  // REMOVE: if( returnCode == QDialog::Accepted ) {
  // REMOVE:   editDialog edg;
  // REMOVE:   edg._contextPtr = _contextPtr;
  // REMOVE:   ((quadrupole*) x)->accept(edg);
  // REMOVE: }

  // REMOVE: delete wpu;
}


void BeamlineBrowser::infoWriter::visitThinQuad( const thinQuad* x )
{
  QString theValue;
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Integrated gradient"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [T]  "), qwa ), 0, 1 );
           theValue.setNum( x->Strength() );
         qgl->addWidget( new QLabel( theValue, qwa ), 0, 2 );

         qgl->addWidget( new QLabel( QString("Roll angle"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]  "), qwa ), 1, 1 );
           theValue.setNum( 1000.*(x->Alignment().tilt /*[rad]*/) );
         qgl->addWidget( new QLabel( theValue, qwa ), 1, 2 );
      qwa->adjustSize();

      QHBox* qhb = new QHBox( qvb );
        QPushButton* closeBtn = new QPushButton( "Close", qhb );
          closeBtn->setDefault( true );
          connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
	  // REMOVE:       QPushButton* editBtn = new QPushButton( "Edit", qhb );
	  // REMOVE:         connect( editBtn, SIGNAL(pressed()),
	  // REMOVE:                  wpu,     SLOT(accept()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: int returnCode = wpu->exec();
  // REMOVE: // Note: when reject is activated, wpu and all its subwidgets
  // REMOVE: //       will be deleted, if the flag Qt::WDestructiveClose is used.
  // REMOVE: //       This is confirmed by changing these from pointers to objects.
  // REMOVE: //       A warning message is issued, when exiting this scope, that
  // REMOVE: //       the objects are deleted twice.

  // REMOVE: if( returnCode == QDialog::Accepted ) {
  // REMOVE:   editDialog edg;
  // REMOVE:   edg._contextPtr = _contextPtr;
  // REMOVE:   ((thinQuad*) x)->accept(edg);
  // REMOVE: }

  // REMOVE: delete wpu;
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


// REMOVE:  void BeamlineBrowser::infoWriter::visitSector( const sector* x )
// REMOVE:  {
// REMOVE:    MatrixD M( x->getMap().Jacobian() );
// REMOVE:  
// REMOVE:    QString matrixElement;
// REMOVE:    QString matrixDisplay;
// REMOVE:    const QString spacer("  "); 
// REMOVE:    const QString eol("\n");
// REMOVE:  
// REMOVE:    for( int i = 0; i < M.rows(); i++ ) {
// REMOVE:      for( int j = 0; j < M.cols(); j++ ) {
// REMOVE:        matrixElement.setNum( M(i,j) );
// REMOVE:        matrixDisplay = matrixDisplay + matrixElement + spacer;
// REMOVE:      }
// REMOVE:      matrixDisplay = matrixDisplay + eol;
// REMOVE:    }
// REMOVE:  
// REMOVE:    QDialog* wpu = new QDialog( 0, 0, true );
// REMOVE:  
// REMOVE:      QVBox* qvb = new QVBox( wpu );
// REMOVE:  
// REMOVE:        QTextView* qtv = new QTextView( matrixDisplay, QString::null, qvb );
// REMOVE:        int drawArea = (50*QApplication::desktop()->height()*
// REMOVE:                           QApplication::desktop()->width())/100;
// REMOVE:        int fixedWidth = (int) sqrt( (double) drawArea );
// REMOVE:        qtv->setFixedSize( fixedWidth, fixedWidth );
// REMOVE:  
// REMOVE:        QHBox* qhb3 = new QHBox( qvb );
// REMOVE:          QPushButton* doneBtn = new QPushButton( "Done", qhb3 );
// REMOVE:            doneBtn->setDefault( true );
// REMOVE:            connect( doneBtn, SIGNAL(pressed()),
// REMOVE:                     wpu,     SLOT(accept()) );
// REMOVE:        qhb3->setMargin(5);
// REMOVE:        qhb3->setSpacing(3);
// REMOVE:        qhb3->adjustSize();
// REMOVE:  
// REMOVE:      qvb->adjustSize();
// REMOVE:  
// REMOVE:    wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
// REMOVE:    wpu->adjustSize();
// REMOVE:  
// REMOVE:    wpu->exec();
// REMOVE:    delete wpu;
// REMOVE:  }


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

  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );

    QVBox* qvb = new QVBox( wpu );

    QTextBrowser* qtv = new QTextBrowser( qvb );
      qtv->setSource( buf );
      // QTextView* qtv = new QTextView( outStream.str(), QString::null, qvb );
      int drawArea = (50*QApplication::desktop()->height()*
                         QApplication::desktop()->width())/100;
      int fixedWidth = (int) sqrt( (double) drawArea );
      qtv->setFixedSize( fixedWidth, fixedWidth );

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* doneBtn = new QPushButton( "Close", qhb3 );
          doneBtn->setDefault( true );
          connect( doneBtn, SIGNAL(pressed()),
                   wpu,     SLOT(close()) );
      qhb3->setMargin(5);
      qhb3->setSpacing(3);
      qhb3->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
}


// REMOVE: void BeamlineBrowser::infoWriter::visitMonitor( const monitor* x )
// REMOVE: {
// REMOVE:   QString cap( "monitor: " );
// REMOVE:   cap += QString( x->Name() );
// REMOVE: 
// REMOVE:   if( 6 != Particle::PSD ) {
// REMOVE:     cap += ": *** ERROR *** ";
// REMOVE:     QMessageBox::information( 0, cap,
// REMOVE:                               "SORRY: Current version of software"
// REMOVE:                               "\nrequires 6 dimensional phase space." );
// REMOVE:     return;
// REMOVE:   }
// REMOVE: 
// REMOVE:   int n = Particle::PSD;
// REMOVE:   double w[n];
// REMOVE:   x->getState(w);  // This is the only statement that requires
// REMOVE:                    //   six-dimensional phase space. 
// REMOVE: 
// REMOVE:   ostringstream uic;
// REMOVE:   uic << "x     [mm]  : " << 1000.0*w[ Particle::_x()  ] << endl;
// REMOVE:   uic << "y     [mm]  : " << 1000.0*w[ Particle::_y()  ] << endl;
// REMOVE:   uic << "p_x/p [mrad]: " << 1000.0*w[ Particle::_xp() ] << endl;
// REMOVE:   uic << "p_y/p [mrad]: " << 1000.0*w[ Particle::_yp() ] << endl;
// REMOVE: 
// REMOVE:   cap += ": registers";
// REMOVE:   QMessageBox::information( 0, cap, uic.str().c_str() );
// REMOVE: }


void BeamlineBrowser::infoWriter::visitMonitor( const monitor* x )
{
  if( 6 != Particle::PSD ) {
    QMessageBox::warning( 0, QString( "monitor: " ) + QString( x->Name() ),
                             "SORRY: Current version of software"
                             "\nrequires 6 dimensional phase space." );
    return;
  }

  // Load the coordinates into an array
  int n = Particle::PSD;
  double w[n];
  x->getState(w);  // This is the only statement that requires
                   //   six-dimensional phase space. 

  // Set up the display widget
  QString theValue;
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 4, 4, 5 );

         qgl->addWidget( new QLabel( QString("x"),     qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString("[mm]"),  qwa ), 0, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 0, 2 );
           theValue.setNum( 1000.0*w[ Particle::_x()  ] );
         qgl->addWidget( new QLabel( theValue,         qwa ), 0, 3 );

         qgl->addWidget( new QLabel( QString("y"),     qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("[mm]"),  qwa ), 1, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 1, 2 );
           theValue.setNum( 1000.0*w[ Particle::_y()  ] );
         qgl->addWidget( new QLabel( theValue,         qwa ), 1, 3 );

         qgl->addWidget( new QLabel( QString("p_x/p"), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString("[mrad]"),qwa ), 2, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 2, 2 );
           theValue.setNum( 1000.0*w[ Particle::_xp() ] );
         qgl->addWidget( new QLabel( theValue,         qwa ), 2, 3 );

         qgl->addWidget( new QLabel( QString("p_y/p"), qwa ), 3, 0 );
         qgl->addWidget( new QLabel( QString("[mrad]"),qwa ), 3, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 3, 2 );
           theValue.setNum( 1000.0*w[ Particle::_yp() ] );
         qgl->addWidget( new QLabel( theValue,         qwa ), 3, 3 );

       qwa->adjustSize();

      QHBox* qhb4 = new QHBox( qvb );
        QPushButton* cancelBtn = new QPushButton( "Close", qhb4 );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(close()) );

      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

    qvb->adjustSize();
  wpu->setCaption( QString(x->Type())+QString(": ")+QString(x->Name()) );
  wpu->adjustSize();

  // Show the information and exit
  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
}
