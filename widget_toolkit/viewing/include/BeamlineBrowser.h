/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      BeamlineBrowser.h
******  Version:   3.4
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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
****** Mar 2007     ostiguy@fnal.gov
******
****** - removed dependencies on dlist/slist
****** - support for reference-counted elements and beamlines 
****** Aug 2008 ostiguy@fnal.gov
****** - eliminated BeamlineContext dependencies
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
#include <list>

#include <beamline/BmlVisitor.h>
#include <beamline/beamline.h>
#include <beamline/BmlPtr.h>

class Particle;

class QWidget;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;

class BoolNode;
class QBmlRoot;


class QBml : public QListViewItem {

public:
  QBml( QPixmap*, QBmlRoot*,  char const*, char const* );
  QBml( QPixmap*, QListView*, char const*, char const* );
  QBml( QBml const& );
 ~QBml();

  QPixmap const*  pixmap( int i ) const;
  void            setPixmap( QPixmap *p );
  QBmlRoot const* bmlParent();
  QBmlRoot const* topBmlParent();  // Does not return null pointer.

  virtual QString fullName() const = 0;

protected:
  QPixmap*  pix_;
  QBmlRoot*   p_;                   // the   QBmlRoot parent
};


class QBmlElmt : public QBml
{
public:
  QBmlElmt( QBmlRoot* parent, ElmPtr const&, double& azimuth );
 ~QBmlElmt();

  double             Azimuth()         const     { return  azimuth_; }
  ConstElmPtr        elmPtr()          const     { return  myElement_; }
       ElmPtr        elmPtr()                    { return  myElement_; }
  QString fullName() const;

  friend class BeamlineBrowser;

private:
  ElmPtr            myElement_;
  double            azimuth_;   
};


class QBmlRoot : public QBml
{
public:
    QBmlRoot( QListView* parent, BmlPtr,                  double& );
    QBmlRoot( QListView* parent, Particle const&, BmlPtr, double& );
    QBmlRoot( QBmlRoot*  parent, BmlPtr,                  double& );
   ~QBmlRoot();

    // QString text( int column ) const;
    QString fullName() const;

    void setOpen( bool );
    void setup();

    QBmlElmt const* findSelectedElement() const;

    friend class BeamlineBrowser;

    ConstBmlPtr bmlPtr() const; 

private:
    BmlPtr    bml_;      // if there is no QBmlRoot parent
};


class BeamlineBrowser : public QListView
{
    Q_OBJECT

public:

// Interface for infoWriter subclass
struct infoWriter : public ConstBmlVisitor 
{
  void visit( bmlnElmnt  const& );
  void visit( beamline   const& );
  void visit( drift      const& );
  void visit( Slot       const& );
  void visit( sbend      const& );
  void visit( CF_sbend   const& );
  void visit( rbend      const& );
  void visit( CF_rbend   const& );
  void visit( quadrupole const& );
  void visit( thinQuad   const& );
  void visit( sextupole  const& );
  void visit( marker     const& );
  void visit( sector     const& );
  void visit( Monitor    const& );

  BmlPtr     bml_;
};


  BeamlineBrowser( QWidget *parent = 0, const char *name = 0, bool sdo = FALSE );
 ~BeamlineBrowser();
  
  ConstElmPtr  getSelectedElement( BmlPtr ) const;

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
  
  void displayBeamline( ConstBmlPtr );
  int        removeBeamline( BmlPtr );   // eliminates the beamline as well

  int findElement( QBml*,  BoolNode const&, std::list<ConstElmPtr>& );

  std::list<ElmPtr> findAllSelected( QBmlRoot* ) const;

public slots:
    void setDir( const QString & );
    void resetPixmap( ConstBmlPtr );

signals:
    void folderSelected( const QString & );
    void sig_bmlLeftClicked( BmlPtr, QBmlRoot* );
    void sig_bmlLeftClicked( QBml* );
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

    static void  displayLine( QBmlRoot*, BmlPtr, beamline::const_reverse_iterator rit, double& );

    std::list<QBmlRoot*>       topLevelItems_;
    QBmlRoot*                  lastClickedRootPtr_;
};


#endif // BEAMLINEBROWSER_H
