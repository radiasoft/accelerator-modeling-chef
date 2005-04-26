/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      BeamlineBrowser.h
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


#ifndef BEAMLINEBROWSER_H
#define BEAMLINEBROWSER_H

#include <qlistview.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>
#include <qptrlist.h>

#include "BmlVisitor.h"
#include "slist.h"

class QWidget;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;

class bmlnElmnt;
class beamline;
class BeamlineContext;
class ReverseBeamlineIterator;
class BoolNode;

class QBmlRoot;


class QBml : public QListViewItem
{
public:
  QBml( QPixmap*, QBmlRoot*,  const char*, const char* );
  QBml( QPixmap*, QListView*, const char*, const char* );
  QBml( const QBml& );
  ~QBml();

  const QPixmap *pixmap( int i ) const;
  void  setPixmap( QPixmap *p );
  const QBmlRoot* bmlParent();
  const QBmlRoot* topBmlParent();  // Does not return null pointer.

  virtual QString fullName() const = 0;

protected:
  QPixmap*  _pix;
  QBmlRoot* _p;                   // the   QBmlRoot parent
};


class QBmlElmt : public QBml
{
public:
  QBmlElmt( QBmlRoot* parent, /* const */ bmlnElmnt*, double& /* azimuth */ );
  ~QBmlElmt();

  inline double Azimuth() { return _azimuth; }
  inline const bmlnElmnt* cheatElementPtr() const { return _myElement; }
  QString fullName() const;

  friend class BeamlineBrowser;

private:
  const bmlnElmnt* _myElement;
  double           _azimuth;   // Is this necessary???
};


class QBmlRoot : public QBml
{
public:
    QBmlRoot( QListView* parent, /* const */ BeamlineContext*, double& );
    QBmlRoot( QListView* parent, /* const */ beamline*,        double& );
    QBmlRoot( QBmlRoot*  parent, /* const */ beamline*,        double& );
    ~QBmlRoot();

    // QString text( int column ) const;
    QString fullName() const;

    void setOpen( bool );
    void setup();

    const QBmlElmt* findSelectedElement() const;

    friend class BeamlineBrowser;

private:
    BeamlineContext* _myBmlCon;      // if there is no QBmlRoot parent
    beamline*        _myBeamline;    // if there is a  QBmlRoot parent
};


class BeamlineBrowser : public QListView
{
    Q_OBJECT

public:

// Interface for infoWriter subclass
struct infoWriter : public ConstBmlVisitor 
{
  void visitBmlnElmnt  ( const bmlnElmnt*  );
  void visitBeamline   ( const beamline*   );
  void visitDrift      ( const drift*      );
  void visitSlot       ( const Slot*       );
  void visitSbend      ( const sbend*      );
  void visitCF_sbend   ( const CF_sbend*   );
  void visitRbend      ( const rbend*      );
  void visitCF_rbend   ( const CF_rbend*   );
  void visitQuadrupole ( const quadrupole* );
  void visitThinQuad   ( const thinQuad*   );
  void visitSextupole  ( const sextupole*  );
  void visitMarker     ( const marker*     );
  void visitSector     ( const sector*     );
  void visitMonitor    ( const monitor*    );

  BeamlineContext*     _contextPtr;
};


// Interface for editDialog subclass
struct editDialog : public BmlVisitor 
{
  void visitBmlnElmnt  ( bmlnElmnt*  );
  void visitRbend      ( rbend*      );
  void visitQuadrupole ( quadrupole* );
  void visitThinQuad   ( thinQuad*   );

  BeamlineContext*     _contextPtr;
};


  BeamlineBrowser( QWidget *parent = 0, const char *name = 0, bool sdo = FALSE );
  ~BeamlineBrowser();
  
  const bmlnElmnt* getSelectedElement( BeamlineContext* ) const;

  static const char* drift_xpm[19];
  static const char* slot_xpm[19];
  static const char* bml_black_xpm[19];
  static const char* bmr_black_xpm[19];
  static const char* bml_orange_xpm[19];
  static const char* bmr_orange_xpm[19];
  static const char* dquad_xpm[19];
  static const char* fquad_xpm[19];
  static const char* sextupole_xpm[19];
  static const char* dipole_xpm[19];
  static const char* elmnt_xpm[19];
  
  static QPixmap* driftSymbol;
  static QPixmap* slotSymbol;
  static QPixmap* bmlBlackSymbol;
  static QPixmap* bmrBlackSymbol;
  static QPixmap* sextupoleSymbol;
  static QPixmap* fquadSymbol;
  static QPixmap* dquadSymbol;
  static QPixmap* bmlOrangeSymbol;
  static QPixmap* bmrOrangeSymbol;
  static QPixmap* dipoleSymbol;
  static QPixmap* elmntSymbol;
  
  BeamlineContext* readMADFile( const char* fileName, const char* lineName, double brho );
  // void displayBeamline( const beamline* );
  void displayBeamline( const BeamlineContext* );
  int removeBeamline( BeamlineContext* );   // eliminates the beamline as well
  // REMOVE int findElement( QBml*, const QString& );
  int findElement( QBml*, const BoolNode&, QPtrList<bmlnElmnt>& );
  int findElement( QBml*, const BoolNode*, QPtrList<bmlnElmnt>& );
  QPtrList<bmlnElmnt> findAllSelected( QBmlRoot* ) const;

public slots:
    void setDir( const QString & );
    void resetPixmap( const BeamlineContext* );

signals:
    void folderSelected( const QString & );
    void sig_bmlLeftClicked( BeamlineContext*, QBmlRoot* );
    void sig_bmlLeftClicked( QBml* );
    void sig_newContext( BeamlineContext* );
    void sig_browserIsEmpty();
    void sig_browserIsNotEmpty();

protected slots:
    void slotFolderSelected( QListViewItem * );
    // void slotShowData( QListViewItem* );
    void slotShowData( QBmlRoot*, QBml* );
    void openFolder();

protected:
    void contentsDragEnterEvent( QDragEnterEvent *e );
    void contentsDragMoveEvent( QDragMoveEvent *e );
    void contentsDragLeaveEvent( QDragLeaveEvent *e );
    void contentsDropEvent( QDropEvent *e );
    void contentsMouseMoveEvent( QMouseEvent *e );
    void contentsMousePressEvent( QMouseEvent *e );
    void contentsMouseReleaseEvent( QMouseEvent *e );

private:
    QString fullPath(QListViewItem* item);
    QListViewItem *oldCurrent;
    QListViewItem *dropItem;
    QTimer* autoopen_timer;
    QPoint presspos;
    bool mousePressed;

    static void _displayLine( QBmlRoot*, ReverseBeamlineIterator&, double& );
    slist _topLevelItems;
    QBmlRoot* _lastClickedRootPtr;
};


#endif // BEAMLINEBROWSER_H
