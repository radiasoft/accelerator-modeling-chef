/**********************************************************************/
/*                                                                    */
/* File:           CHEFGUI.h                                          */
/* Authors:                                                           */
/*                 Leo Michelotti                                     */
/*                 michelotti@fnal.gov                                */     
/*                                                                    */ 
/*                 Jean-Francois Ostiguy                              */
/*                 ostiguy@fnal.gov                                   */
/*                                                                    */
/*                                                                    */
/* Creation Date:  August 2004                                        */
/*                                                                    */ 
/* Copyright:      (C) URA/Fermilab                                   */
/*                                                                    */
/**********************************************************************/
#ifndef CHEFGUI_H
#define CHEFGUI_H

#include <ostream>
#include <chefguibase.h>

#include <dlist.h>
#include <bmlnElmnt.h>
#include <GenericException.h>
#include <beamline.h>
#include <InsertionList.h>
#include <BeamlineContext.h>
#include <BeamlineExpressionTree.h>
#include <FramePusher.h>
#include <DriftEliminator.h>
#include <QuadEliminator.h>
#include <bmlfactory.h>
#include <BeamlineBrowser.h>
#include <Tracker.h>
#include <RayTrace.h>
#include <SiteViewer.h>
#include <QtMonitor.h>
#include <QueryDialog.h>
#include <ModifierVisitor.h>
#include <complexAddon.h>
#include <InitCondDialogLF.h>

#include <qptrlist.h>
#include <qguardedptr.h>
#include <list>

class BeamlineContext;
class ReverseBeamlineIterator;
class Jet__environment;
class JetC__environment;
class Mapping;


class CHEF_domain::QBml;
class CHEF_domain::QBmlRoot;
class CHEFPlotMain;
class Tracker;
class RayTrace;
class SiteViewer;
class QTextEdit;
class QPyCHEF;
class DeviceTable;
class QAssistantClient;
class BmlSelectionDialog;
class InitCondDialogLF;


#include "messages.h"

//template<> class Messages;


// Utility classes
// ---------------

struct Options
{
  enum Coupling { ignore_coupling=0, edwards_teng, moments } couplingOption;
};


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



#define CHEF_numargs 5


class CHEFGUI: public CHEFGUIBase {

 typedef int (*ACTFUNC00)( CHEFGUI*, bmlnElmnt* );
 typedef int (*ACTFUNC01)( CHEFGUI*, const bmlnElmnt* );
 typedef int (*ACTFUNC10)( const CHEFGUI*, bmlnElmnt* );
 typedef int (*ACTFUNC11)( const CHEFGUI*, const bmlnElmnt* );


 typedef bool (*BOOLFUNC00)( CHEFGUI*, bmlnElmnt* );
 typedef bool (*BOOLFUNC01)( CHEFGUI*, const bmlnElmnt* );
 typedef bool (*BOOLFUNC10)( const CHEFGUI*, bmlnElmnt* );
 typedef bool (*BOOLFUNC11)( const CHEFGUI*, const bmlnElmnt* );


  Q_OBJECT

public:

  CHEFGUI(QWidget* parent, const char* name, WFlags w);
  virtual ~CHEFGUI();    

private:

  int             _id_FileWriteTree;
  int             _id_analMenu;
  int             _id_ctrlMenu;
  int             _id_EditSelectMenu;
  int             _id_FilePrint;
  argPtr*         _toolArgs[CHEF_numargs];

  coord*          _x; 
  coord*          _y; 
  coord*          _z; 
  coord*          _px;
  coord*          _py;
  coord*          _pz;

  QObject*        _eventfilter;
 
  // Key widgets

  QApplication*                    _appl;
  QMainWindow*                     _mainWindow;
  QWidget*                         _centralWidget;

  QGuardedPtr<CHEFPlotMain>                    _plotWidget;
  QGuardedPtr<CHEFPlotMain>                   _ETplotWidget;
  QGuardedPtr<CHEFPlotMain>                   _MMplotWidget;
  QGuardedPtr<CHEFPlotMain>                   _LBplotWidget;
  QGuardedPtr<CHEFPlotMain>                   _DspnplotWidget;
  

  CHEF_domain::Tracker*            _trackWidget;
  CHEF_domain::RayTrace*           _traceWidget;
  CHEF_domain::SiteViewer*         _siteWidget;
 
  Messages<>*                      _messages_stdout;
  Messages<>*                      _messages_stderr;

  InitCondDialogLF*                _initCondDialogLF;       
  InitCondDialogLF*                _initCondDialogDisp;     
  InitCondDialogLF*                _initCondDialogMoments;  


  std::ostream *                   _p_messages_stdout_stream;
  std::ostream *                   _p_messages_stderr_stream;
  QTextEdit*                       _messages;
  DeviceTable*                     _devices;
  QAssistantClient*                _assistanthelp;
  BmlSelectionDialog*              _bmlSelectionDialog;

  CHEF_domain::BeamlineBrowser*    _p_vwr;
  QPyCHEF*                         _interpreter;


  QPopupMenu*      _fileMenu;
  QPopupMenu*      _toolMenu;
  QPopupMenu*      _editMenu;


  
  QPtrList<CHEF_domain::QBml>    _selListItems;  // selected QListViewItems
                                    // Doesn't seem to be used so far
                                    // (9/24/03)
  QPtrList<bmlnElmnt> _foundList;   // List of beamline elements found by the editor.

  dlist                  _contextList;
  BeamlineContext*       _p_currBmlCon;  // Currently selected beamline
  CHEF_domain::QBmlRoot* _p_currQBmlRoot;// ... and its widget
  CHEF_domain::QBml*     _p_clickedQBml; // ... and its QBml
  
 

  Jet__environment*   _p_JetEnv;
  JetC__environment*  _p_JetCEnv;

  Options             _userOptions;

  static int _buildVTuneCircuit( const CHEFGUI*, const bmlnElmnt* );
  static int _buildHTuneCircuit( const CHEFGUI*, const bmlnElmnt* );

  void _testFC( ACTFUNC11 ) const;
  void _traverseTree( const CHEF_domain::QBmlRoot*, ACTFUNC11 ) const;
  void _pushArgs();
  void _enableMenus(bool set);
  
  void _updateEditorWindowLists(); 


signals:

  void  _new_beamline();
  void _modeChanged( const BeamlineContext* );

  public slots:

  void modeRing(bool set);
  void modeLine(bool set);
  void devicesAutoRefresh(bool);
  void fileEditPython();
  void fileEditMAD8();
  void editParse();

  private slots:

   void _openFile(); 
   void _openDeviceFile(); 
   void _openScriptFile(); 
   void _devicesClearSelection();
   void _devicesRefreshAll();
   void _print();
   void _fileWriteTree();
   void _fileSaveAs();
   void _fileEditorSaveAs();
   void _fileEditorSave();
   void _editFindFilter();
   void _editSelectAll();
   void _editSelectNone();
   void _editCopyLine();
   void _editRemoveLine();
   void _editRenameLine();
   void _editCondense();
   void _editAlign();
   void _editMergeQuads();
   void _editD2S();
   void _editAddQtMons();
   void _editMode();
   void _editPartAndSect();
   void _editAddMarkers();
   void _editPartition();   
   void _toolAlignBends();
   void _toolMisalign();
   void _helpContents();
   void _helpAbout();
   void _processFilter( const BoolNode& );
   void _slot_contextGenerated( BeamlineContext* );
   void _makeFODO();
   void _makeSingSext();
   void _uncoupledLFDispatch();
   void _momentsDispatch();
   void _dispersionDispatch();
   void _launchLatt();
   void _launchET();
   void _launchMoments();
   void _launchLB();
   void _pushDispersion();
   void _pushMoments();
   void _pushULF();
   void _launchTrack();
   void _launchRayTrace();
   void _launchDispersion();
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
   void _set_p_clickedContext(BeamlineContext*, CHEF_domain::QBmlRoot*);
   void _set_p_clickedQBml( CHEF_domain::QBml* );
   void _windowsCascade();
   void _windowsMinimizeAll();
   void _windowsTile();
   void _windowsDefaultLayout();
   void _exit();

   void _do_nothing();

   void showEvent( QShowEvent* event); 

   void windowsMessagesAction_toggled( bool set );
   void windowsTree_BrowserAction_toggled( bool set );
   void windowsInterpreterAction_toggled( bool  set);
   void windowsDevicesAction_toggled( bool  set);
  
   void _disableMenus();
   void _enableMenus();


     
};


#endif
