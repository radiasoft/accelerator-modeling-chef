/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
******                                    
******  File:      CHEF.h
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


#ifndef CHEF_H
#define CHEF_H

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QPTRLIST_H
#include <qptrlist.h>
#endif

// Forward declarations
class QMainWindow;
class QApplication;
class QLabel;
class QMenuBar;
class QPopupMenu;
class QPixmap;

class BeamlineContext;
class ReverseBeamlineIterator;

class Jet__environment;
class JetC__environment;
class Mapping;

#ifndef BEAMLINEBROWSER_H
#include "BeamlineBrowser.h"
#endif
#ifndef DLIST_HXX
#include "dlist.h"
#endif
#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

// ++----++++----++++----++++----++++----++++----++++----++

namespace CHEF_domain
{
class QBml;
class QBmlRoot;
class LattFncPlt;
class Tracker;
class RayTrace;
class SiteViewer;


// Utility classes
// ---------------

class CF_rbendFinder_quad : public beamline::Criterion
{
  public:
    CF_rbendFinder_quad( double = 0.0 );
    ~CF_rbendFinder_quad();

    void setQuadrupole( double );

    bool operator()( const bmlnElmnt* );
    bool operator()( const bmlnElmnt& );

  private:
    double _integratedQuad;
};


class RollAccumulator : public beamline::Action
{
  public:
    RollAccumulator( const char*, double = 0.0 );
    ~RollAccumulator();

    void setWeakType();
    void setStrongType();

    int operator()( bmlnElmnt* );
    int operator()( bmlnElmnt& );

  private:
    double _deltaRoll;
    char*  _type;
    bool   _weak;   // default false
};


class argPtr
{
  // This is an unsafe pure virtual class.
  // It is needed primarily for the virtual
  // destructor.
  public: 
    argPtr( void* x, bool=true );
    virtual ~argPtr();
    const void* ptr() const;
    bool isOwned() const;
  protected:
    bool  _owned;
    void* _ptr;  // The derived classes should
                 //   own this pointer and delete
                 //   it when their destructors
                 //   are called.
                 // If this is not the case,
                 //   then "owned" should be set
                 //   to false by the invoking program.
                 // Derived destructors must test
                 //   this!
};


struct MappingPtr : public argPtr
{
  MappingPtr( const Mapping& x, bool = true );  
                               // Do not make constructor
                               // with a pointer argument!
  ~MappingPtr();
};


struct BeamlineContextPtr : public argPtr
{
  BeamlineContextPtr( const BeamlineContext& x, bool = true );  
                               // Do not make constructor
                               // with a pointer argument!
  ~BeamlineContextPtr();
};


// Declaration: class CHEF
// ------------------------

#define CHEF_numargs 5

class CHEF : public QObject
{
Q_OBJECT

 typedef int (*ACTFUNC00)( CHEF*, bmlnElmnt* );
 typedef int (*ACTFUNC01)( CHEF*, const bmlnElmnt* );
 typedef int (*ACTFUNC10)( const CHEF*, bmlnElmnt* );
 typedef int (*ACTFUNC11)( const CHEF*, const bmlnElmnt* );

 typedef bool (*BOOLFUNC00)( CHEF*, bmlnElmnt* );
 typedef bool (*BOOLFUNC01)( CHEF*, const bmlnElmnt* );
 typedef bool (*BOOLFUNC10)( const CHEF*, bmlnElmnt* );
 typedef bool (*BOOLFUNC11)( const CHEF*, const bmlnElmnt* );


public:
  CHEF( beamline* = 0, int = 0, char** = 0 );
  ~CHEF();

  int run();


private:
  // Key widgets
  QApplication*    _appl;
  QMainWindow*     _mainWindow;
  QLabel*          _centralWidget;
  LattFncPlt*      _plotWidget;
  Tracker*         _trackWidget;
  RayTrace*        _traceWidget;
  SiteViewer*      _siteWidget;
  QPixmap*         _splash;
  
  argPtr*          _toolArgs[CHEF_numargs];
  // ??? Why not use a std::vector ???
    
  // Menus
  QMenuBar*        _mainMenu;
  QPopupMenu*      _fileMenu;
  QPopupMenu*      _editMenu;
  QPopupMenu*        _edit_selectMenu;
  QPopupMenu*      _mach_imagMenu;
  QPopupMenu*      _toolMenu;
  QPopupMenu*        _tool_calcMenu;
  QPopupMenu*          _calcEnterMenu;
  QPopupMenu*        _tool_ctrlMenu;
  QPopupMenu*        _tool_dsgnMenu;

  // Menu identifiers
  int              _id_FilePrint;
  int              _id_FileWriteTree;
  int              _id_ToolTracking;
  int              _id_EditSelectMenu;
  

  QPtrList<QBml>    _selListItems;  // selected QListViewItems
                                    // Doesn't seem to be used so far
                                    // (9/24/03)
  QPtrList<bmlnElmnt> _foundList;   // List of beamline elements found
                                    // by the editor.
  dlist             _contextList;
  BeamlineContext*  _p_currBmlCon;  // Currently selected beamline
  BeamlineContext*  _p_clickedCon;  // Last context clicked on ...
  QBml*             _p_clickedQBml; // ... and its QBml

  BeamlineBrowser*  _p_vwr;

  Jet__environment*  _p_JetEnv;
  JetC__environment* _p_JetCEnv;

  static int _buildVTuneCircuit( const CHEF*, const bmlnElmnt* );
  static int _buildHTuneCircuit( const CHEF*, const bmlnElmnt* );
  void _testFC( ACTFUNC11 ) const;
  void _traverseTree( const QBmlRoot*, ACTFUNC11 ) const;
  void _pushArgs();

private slots:
  void _openFile();
  void _print();
  void _fileWriteTree();
  void _fileSaveAs();
  void _editFindFilter();
  void _editSelectAll();
  void _editSelectNone();
  void _editSelectLine();
  void _editCopyLine();
  void _editRemoveLine();
  void _editCondense();
  void _editAlign();
  void _editMergeQuads();
  void _editD2S();
  void _editAddQtMons();
  void _editPartAndSect();
  void _toolAlignBends();
  void _toolMisalign();
  void _processFilter( const BoolNode& );

  void _slot_contextGenerated( BeamlineContext* );

  void _makeFODO();
  void _makeSingSext();

  void _launchLatt();
  void _launchTrack();
  void _launchRayTrace();
  void _launchDilution();
  void _enterMapArg();
  void _enterContextArg();
  void _clearArgs();
  void _launchSiteVu();
  void _launch_browser();

  void _horTuneCtrl();
  void _verTuneCtrl();
  void _tuneCtrl();
  void _chromCtrl();

  void _set_p_clickedContext( BeamlineContext* );
  void _set_p_clickedQBml( QBml* );
  void _do_nothing();


signals:
  void _new_beamline();

};

}; // end namespace CHEF

#endif // CHEF_H
