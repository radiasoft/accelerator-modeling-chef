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
******Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
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
****** REVISION HISTORY
****** Mar 2007     ostiguy@fnal
****** - support for reference counted elements and beamlines
******                                                                
**************************************************************************
*************************************************************************/


#include <fstream>
#include <typeinfo>
#include <iosetup.h>

#include <qpixmap.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qapplication.h>
#include <qheader.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtextbrowser.h>

#include <BeamlineBrowser.h>
#include <GenericException.h>
#include <BeamlineContext.h>
#include <bmlfactory/MAD8Factory.h>
#include <BeamlineExpressionTree.h>
#include <beamline/drift.h>
#include <beamline/Slot.h>
#include <beamline/marker.h>
#include <beamline/sextupole.h>
#include <beamline/quadrupole.h>
#include <beamline/monitor.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/sector.h>
#include <beamline/Alignment.h>


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
: QListViewItem( rt, s1, s2 ), pix_(px), p_(rt)
{}

QBml::QBml( QPixmap* px, QListView* parent, const char* s1, const char* s2 )
: QListViewItem( parent, s1, s2 ), pix_(px), p_(0)
{
}

QBml::QBml( QBml const& x ) 
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
{}


void QBml::setPixmap( QPixmap *px )
{
    pix_ = px;
    setup();
    widthChanged( 0 );
    invalidateHeight();
}


const QPixmap* QBml::pixmap( int i ) const
{
    if ( i )         // What is the point of this function?
      return 0;
    return pix_;
}


const QBmlRoot* QBml::bmlParent()
{
  return p_;
}


const QBmlRoot* QBml::topBmlParent()
{
  // Must not return null pointer.
  if( !p_ ) { return dynamic_cast<QBmlRoot*>(this); }

  QBmlRoot* ret   = p_;
  QBmlRoot* tsts  = ret->p_;

  while( tsts ) {
    ret = tsts;
    tsts  = ret->p_;
  }

  return ret;
}


// *****************************************************************************
// *
// * Class QBmlElmt
// *
// *****************************************************************************

QBmlElmt::QBmlElmt( QBmlRoot* parent, ConstElmPtr const& q, double& s )
: QBml( 0, parent, q->Name().c_str(), q->Type() ),
   myElement_(q),
   azimuth_(s) 
{
  QString str;
  str.setNum( s );
  setText( 2, str );
}


QBmlElmt::~QBmlElmt()
{}


QString QBmlElmt::fullName() const
{
  return QString( myElement_->Name() );
}


// *****************************************************************************
// *
// * Class QBmlRoot
// *
// *****************************************************************************

QBmlRoot::QBmlRoot( QListView* parent, BmlContextPtr q, double& s )
: QBml( 0, parent, q->name().c_str(), "beamline" ), 
   myBmlCon_(q)
{
    QString str;
    str.setNum( ((double) s) );
    this->setText( 2, str );

    myBeamline_ = BmlPtr();
}


QBmlRoot::QBmlRoot( QListView* parent, Particle const& particle, BmlPtr q, double& s )
: QBml( 0, parent, q->Name().c_str(), q->Type() ), 
   myBeamline_()
{
    QString str;
    str.setNum( ((double) s) );
    this->setText( 2, str );

    myBmlCon_ = BmlContextPtr(new BeamlineContext( particle, q) );
}


QBmlRoot::QBmlRoot( QBmlRoot * parent, BmlPtr q, double& s )
: 
   QBml( 0, parent, q->Name().c_str(), q->Type() ),
   myBmlCon_(),
   myBeamline_(q) 
{
    QString str;
    str.setNum( ((double) s) );
    setText( 2, str );

    if( 0 == strcmp(q->Type(),"beamline") ) {
      setPixmap( BeamlineBrowser::bmlBlackSymbol );
    }
    else {
      setPixmap( BeamlineBrowser::elmntSymbol );
    }
}


QBmlRoot::~QBmlRoot()
  {}


void QBmlRoot::setOpen( bool o )
{
  if( !myBmlCon_ ) {
    if ( o ) {
      setPixmap( BeamlineBrowser::bmlOrangeSymbol );
    }
    else {
      setPixmap( BeamlineBrowser::bmlBlackSymbol );
    }
  }
  else if( myBmlCon_->isTreatedAsRing() ) {
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
    if( !p_ ) 
    { s = myBmlCon_->name();
    }
    else 
    { s = myBeamline_->Name();
    }
    return s;
}


QBmlElmt const* QBmlRoot::findSelectedElement() const
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
      dropItem( 0 ), mousePressed( FALSE ), lastClickedRootPtr_(0)
{
  this->setSorting(-1);  // prevents sorting
  this->addColumn( "Name" );
  this->addColumn( "Type" );
  this->addColumn( "Azimuth" );
  this->setTreeStepSize( 20 );
  
  autoopen_timer = new QTimer( this );
  
  if ( !BeamlineBrowser::bmlBlackSymbol ) {
      BeamlineBrowser::driftSymbol     = new QPixmap( drift_xpm );
      BeamlineBrowser::slotSymbol      = new QPixmap( slot_xpm );
      BeamlineBrowser::bmlBlackSymbol  = new QPixmap( bml_black_xpm );
      BeamlineBrowser::bmlOrangeSymbol = new QPixmap( bml_orange_xpm );
      BeamlineBrowser::bmrBlackSymbol  = new QPixmap( bmr_black_xpm );
      BeamlineBrowser::bmrOrangeSymbol = new QPixmap( bmr_orange_xpm );
      BeamlineBrowser::sextupoleSymbol = new QPixmap( sextupole_xpm );
      BeamlineBrowser::fquadSymbol     = new QPixmap( fquad_xpm );
      BeamlineBrowser::dquadSymbol     = new QPixmap( dquad_xpm );
      BeamlineBrowser::dipoleSymbol    = new QPixmap( dipole_xpm );
      BeamlineBrowser::elmntSymbol     = new QPixmap( elmnt_xpm );
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


ConstElmPtr BeamlineBrowser::getSelectedElement( BmlContextPtr bcPtr ) const
{
  if( !bcPtr ) {
    QMessageBox::information( 
            0, 
            "BeamlineBrowser::getSelectedElement",
            "ERROR: Null pointer passed as argument."
            );
    return ConstElmPtr();
  }

  QBmlRoot* fc = dynamic_cast<QBmlRoot*>( firstChild() );
  bool found = ( bcPtr == fc->myBmlCon_ );
  while( !found ) {
    fc = dynamic_cast<QBmlRoot*>( fc->nextSibling() );
    found = ( bcPtr == fc->myBmlCon_ );
  }

  if( !found ) {
    QMessageBox::information( 
            0, 
            "BeamlineBrowser::getSelectedElement",
            "ERROR: Did not find selected beamline."
            );
    return ConstElmPtr();
  }

  const QBmlElmt* qbePtr = fc->findSelectedElement();
  if( !qbePtr ) { return ConstElmPtr(); }

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
  wr._contextPtr = toproot->myBmlCon_;

  if( typeid(*item)==typeid(QBmlRoot) ) {
    
    if( !static_cast<QBmlRoot*>(item)->p_ ) 
    {
         static_cast<QBmlRoot*>(item)->myBmlCon_->accept(wr);
    }
    else {
         static_cast<QBmlRoot*>(item)->myBeamline_->accept(wr);
    }
  }

  else if( typeid(*item)==typeid(QBmlElmt) ) {
     static_cast<QBmlElmt*>(item)->myElement_->accept(wr);
  }
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
    { lastClickedRootPtr_ = dynamic_cast<QBmlRoot*>(j);
      if( 0 != ( ((QBmlRoot*) j) -> myBmlCon_ ) ) {
        emit sig_bmlLeftClicked( ((QBmlRoot*) j) -> myBmlCon_, ((QBmlRoot*) j) );
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


void BeamlineBrowser::resetPixmap( ConstBmlContextPtr bcPtr )
{
  if( !bcPtr ) {
    QMessageBox::information( 0, "CHEF: WARNING", 
      "Null pointer passed to BeamlineContext::resetPixmap." 
      "\nNo action will be taken." );
    return;
  }

  QBmlRoot* ptrRoot = dynamic_cast<QBmlRoot*>( this->firstChild() );
  bool found = ( bcPtr == ptrRoot->myBmlCon_ );
  while( !found ) {
    ptrRoot = dynamic_cast<QBmlRoot*>( ptrRoot->nextSibling() );
    found = ( bcPtr == ptrRoot->myBmlCon_ );
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

BmlContextPtr BeamlineBrowser::readMADFile( const char* fileName, 
                                               const char* lineName, 
                                               double      brho )
{
  MAD8Factory bf( fileName, brho );
  BmlPtr pBml = bf.create_beamline( lineName ); 

  BmlContextPtr www;
  if( 0 == strcmp( "PROTON", bf.getParticleType() ) ) {
    www = BmlContextPtr( new BeamlineContext( Proton(pBml->Energy()), pBml ) );
  }
  else if( 0 == strcmp( "POSITRON", bf.getParticleType() ) ) {
    www =  BmlContextPtr( new BeamlineContext( Positron(pBml->Energy()), pBml) );
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

  displayBeamline( www );
  emit sig_newContext( www );
  return www;
}



void BeamlineBrowser::displayBeamline( ConstBmlContextPtr ptr )
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

  QBmlRoot* root = new QBmlRoot( this, boost::const_pointer_cast<BeamlineContext>(ptr), s );
  if( ptr->isTreatedAsRing() ) {
    root->setPixmap( BeamlineBrowser::bmrBlackSymbol ); 
  }
  else {
    root->setPixmap( BeamlineBrowser::bmlBlackSymbol );
  }
  topLevelItems_.push_front( root );

  str2.setNum(s);

  beamline::const_reverse_iterator rbi = ptr->cheatBmlPtr()->rbegin();

  displayLine( root, rbi, s );

  str1.setNum( s );
  root->setText( 2, str1 + "-" + str2 );

  this->setSelected ( root, true );
  this->setCurrentItem( root );
  this->ensureItemVisible( root );

  if(  root->myBmlCon_ ) {
    emit sig_bmlLeftClicked( root->myBmlCon_, root );
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


void BeamlineBrowser::displayLine( QBmlRoot* root, beamline::const_reverse_iterator bi, double& s )
{
  ElmPtr q;
  static bool firstTime = true;
  QString str1, str2;

  for ( ;  bi != bi.rend(); ++bi )
  {
    q = (*bi);
    if( 0 == strcmp("beamline",q->Type()) ) {

      QBmlRoot* newBml = new QBmlRoot( root, boost::static_pointer_cast<beamline>(q) , s);
      newBml->setPixmap( BeamlineBrowser::bmlBlackSymbol );

      beamline::const_reverse_iterator newbi = (*boost::static_pointer_cast<const beamline>(q) ).rbegin();

      str2.setNum(s);
      BeamlineBrowser::displayLine( newBml, newbi, s );
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


int BeamlineBrowser::removeBeamline( BmlContextPtr ptr )
{
  if( !ptr ) { return 1; }
  if( typeid(*ptr) != typeid(BeamlineContext) ) { return 2; }

  QBmlRoot* w = 0;

  bool notfound = true;

  for ( std::list<QBmlRoot*>::iterator it = topLevelItems_.begin(); 
                                      it != topLevelItems_.end(); ++it) {

    if( (*it)->myBmlCon_ == ptr ) { 
      w = (*it);   // Just paranoia
      notfound = false;
    }
  }

  if( notfound ) { return 3; }
  else {
    takeItem(w);
    topLevelItems_.remove( w );
    delete w;
  }

  if ( childCount() == 0 ) emit sig_browserIsEmpty();

  return 0;
}



int BeamlineBrowser::findElement( QBml*                startpoint, 
                                  const BoolNode*      queryPtr, 
                                 std::list<ConstElmPtr>& foundElements )
{
  return this->findElement( startpoint, *queryPtr, foundElements );
}


int BeamlineBrowser::findElement( QBml*                startpoint, 
                                  BoolNode const&      query, 
                                 std::list<ConstElmPtr>& foundElements )
{
  int        ret = 1;
  QBml*      qbmlPtr;
  QBmlElmt*  qbmlelmtPtr; 

  // REMOVE: QBmlRoot*  qbmlrootPtr;

  ConstElmPtr elementPtr;
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
      if( query.evaluate( elementPtr.get() ) ) {
        foundElements.push_back( elementPtr );
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


std::list<ElmPtr> BeamlineBrowser::findAllSelected( QBmlRoot* startpoint ) const
{

  std::list<ElmPtr> ret;

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
        ret.push_back( boost::const_pointer_cast<bmlnElmnt>(qbmlelmtPtr->cheatElementPtr()) );
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

void BeamlineBrowser::infoWriter::visit( beamline const& x )
{
  QString cap( "Beamline" );
  cap += QString( " " );
  cap += QString( x.Name() );

  QString stl;
  QString msg;
  msg.setNum( x.countHowManyDeeply() );
  msg += QString( " elements. \nReference energy = " );
  stl.setNum( x.Energy() );
  msg += stl;
  msg += QString( " Gev" );

  QMessageBox::information( 0, cap, msg );
}


void BeamlineBrowser::infoWriter::visit( bmlnElmnt const& x )
{
  // This code is similar to what appears in 
  // SiteViewer::Wndw::mousePressEvent
  // Keep it that way!
  QString cap( x.Type() );
  cap += QString( " " );
  cap += QString( x.Name() );

  QString msg( "Length = " );
  QString stl;
  stl.setNum( x.Length() );
  msg += stl;
  stl.setNum( x.Strength() );
  msg += QString( ";  Strength = " );
  msg += stl;

  QMessageBox::information( 0, cap, msg );
}


void BeamlineBrowser::infoWriter::visit( drift const& x )
{
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog;
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Length [m] = ", qhb1 );
        QString stl;
        stl.setNum( x.Length() );
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

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}


void BeamlineBrowser::infoWriter::visit( Slot const& x )
{
  
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

         xstr.setNum( (x.getInFrame().getOrigin())(0) );
         ystr.setNum( (x.getInFrame().getOrigin())(1) );
         zstr.setNum( (x.getInFrame().getOrigin())(2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 1 );
         xstr.setNum( (x.getOutFrame().getOrigin())(0) );
         ystr.setNum( (x.getOutFrame().getOrigin())(1) );
         zstr.setNum( (x.getOutFrame().getOrigin())(2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 1, 2 );

         xstr.setNum( (x.getInFrame().getxAxis()) (0) );
         ystr.setNum( (x.getInFrame().getxAxis()) (1) );
         zstr.setNum( (x.getInFrame().getxAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 1 );
         xstr.setNum( (x.getInFrame().getyAxis()) (0) );
         ystr.setNum( (x.getInFrame().getyAxis()) (1) );
         zstr.setNum( (x.getInFrame().getyAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 1 );
         xstr.setNum( (x.getInFrame().getzAxis()) (0) );
         ystr.setNum( (x.getInFrame().getzAxis()) (1) );
         zstr.setNum( (x.getInFrame().getzAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 4, 1 );
    
         xstr.setNum( (x.getOutFrame().getxAxis()) (0) );
         ystr.setNum( (x.getOutFrame().getxAxis()) (1) );
         zstr.setNum( (x.getOutFrame().getxAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 2, 2 );
         xstr.setNum( (x.getOutFrame().getyAxis()) (0) );
         ystr.setNum( (x.getOutFrame().getyAxis()) (1) );
         zstr.setNum( (x.getOutFrame().getyAxis()) (2) );
         qgl->addWidget( new QLabel( lparen+xstr+comma+ystr+comma+zstr+rparen, qwa )
                         , 3, 2 );
         xstr.setNum( (x.getOutFrame().getzAxis()) (0) );
         ystr.setNum( (x.getOutFrame().getzAxis()) (1) );
         zstr.setNum( (x.getOutFrame().getzAxis()) (2) );
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

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
 
}


void BeamlineBrowser::infoWriter::visit( sbend const& x )
{
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
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
  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
}


void BeamlineBrowser::infoWriter::visit( CF_sbend const& x )
{
  QString stl;
  QString sts;
  QString stq;
  stl.setNum( x.Length() );
  sts.setNum( x.Strength() );
  stq.setNum( x.getQuadrupole()/x.Length() );  
  QMessageBox::information( 
          0, 
          QString(x.Type())+QString(": ")+QString(x.Name()),
          QString("Length [m]: ")+stl+
          QString("  Bend field [T]: ")+sts+
          QString("  Quad gradient [T/m]: ")+stq
          );
}



void BeamlineBrowser::infoWriter::visit( rbend const& x )
{
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
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
          connect( closeBtn,  SIGNAL(pressed()),
                   wpu,       SLOT(close()) );
	  // REMOVE:       QPushButton* editBtn = new QPushButton( "Edit", qhb4 );
	  // REMOVE:         connect( editBtn, SIGNAL(pressed()),
	  // REMOVE:                  wpu,     SLOT(accept()) );

      qhb4->setMargin(5);
      qhb4->setSpacing(3);
      qhb4->adjustSize();

    qvb->adjustSize();

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
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
void BeamlineBrowser::infoWriter::visit( CF_rbend const& x )
{
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
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
  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
}


void BeamlineBrowser::infoWriter::visit( quadrupole const& x )
{
  QString theValue;
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog;
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( QString("Gradient [T/m]: "), qhb1 );
        theValue.setNum( x.Strength() );
        new QLabel( theValue, qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        new QLabel( QString("Length: "), qhb2 );
        theValue.setNum( x.Length() );
        new QLabel( theValue, qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        new QLabel( QString("Roll angle [mrad]: "), qhb3 );
        alignmentData ad(x.Alignment());
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
  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
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


void BeamlineBrowser::infoWriter::visit( thinQuad const& x )
{
  QString theValue;
  QDialog* wpu = new QDialog(0,0,false,Qt::WDestructiveClose);
  // REMOVE: QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QWidget* qwa = new QWidget( qvb );
         QGridLayout* qgl = new QGridLayout( qwa, 2, 3, 5 );

         qgl->addWidget( new QLabel( QString("Integrated gradient"), qwa ), 0, 0 );
         qgl->addWidget( new QLabel( QString(" [T]  "), qwa ), 0, 1 );
           theValue.setNum( x.Strength() );
         qgl->addWidget( new QLabel( theValue, qwa ), 0, 2 );

         qgl->addWidget( new QLabel( QString("Roll angle"), qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString(" [mrad]  "), qwa ), 1, 1 );
           theValue.setNum( 1000.*(x.Alignment().tilt /*[rad]*/) );
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

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
}


void BeamlineBrowser::infoWriter::visit( sextupole const& x )
{
  QString stl;
  QString sts;
  stl.setNum( x.Length() );
  sts.setNum( x.Strength() );
  QMessageBox::information( 
          0, 
          QString(x.Type())+QString(": ")+QString(x.Name()),
          QString("Length: ")+stl+QString("\nStrength [T/m**2]: ")+sts
          );
}


void BeamlineBrowser::infoWriter::visit( marker const& x )
{
  QString stl;
  QString sts;
  stl.setNum( x.Length() );
  sts.setNum( x.Strength() );
  QMessageBox::information( 
          0, 
          QString(x.Type())+QString(": ")+QString(x.Name()),
          "What do you expect here?\nIt's a marker!"
          );
}



void BeamlineBrowser::infoWriter::visit( sector const& x )
{
  Mapping theMap( x.getMap() );
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

  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
}


void BeamlineBrowser::infoWriter::visit( monitor const& x )
{
  if( 6 != Particle::PSD ) {
    QMessageBox::warning( 0, QString( "monitor: " ) + QString( x.Name() ),
                             "SORRY: Current version of software"
                             "\nrequires 6 dimensional phase space." );
    return;
  }

  // Load the coordinates into an array
  int n = Particle::PSD;
  double w[n];
  x.getState(w);  // This is the only statement that requires
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
           theValue.setNum( 1000.0*w[ Particle::xIndex()  ] );
         qgl->addWidget( new QLabel( theValue,         qwa ), 0, 3 );

         qgl->addWidget( new QLabel( QString("y"),     qwa ), 1, 0 );
         qgl->addWidget( new QLabel( QString("[mm]"),  qwa ), 1, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 1, 2 );
           theValue.setNum( 1000.0*w[ Particle::yIndex()  ] );
         qgl->addWidget( new QLabel( theValue,         qwa ), 1, 3 );

         qgl->addWidget( new QLabel( QString("p_x/p"), qwa ), 2, 0 );
         qgl->addWidget( new QLabel( QString("[mrad]"),qwa ), 2, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 2, 2 );
           theValue.setNum( 1000.0*w[ Particle::npxIndex() ] );
         qgl->addWidget( new QLabel( theValue,         qwa ), 2, 3 );

         qgl->addWidget( new QLabel( QString("p_y/p"), qwa ), 3, 0 );
         qgl->addWidget( new QLabel( QString("[mrad]"),qwa ), 3, 1 );
         qgl->addWidget( new QLabel( QString("="),     qwa ), 3, 2 );
           theValue.setNum( 1000.0*w[ Particle::npyIndex() ] );
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
  wpu->setCaption( QString(x.Type())+QString(": ")+QString(x.Name()) );
  wpu->adjustSize();

  // Show the information and exit
  wpu->show();
  // REMOVE: wpu->exec();
  // REMOVE: delete wpu;
}
