/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CHEFGUI.h                                         ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
******                                                               ******
******              Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ****** 
******                                                               ******
******              Jean-Francois Ostiguy                            ******
******              Fermilab                                         ****** 
******              ostiguy@fnal.gov                                 ****** 
******                                                               ******  
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
**************************************************************************/
/**********************************************************************/
/* Creation Date:  August 2004                                        */
/* Revision Date:  July,  2005                                        */
/*   - added five slots: _editEditElement                             */
/*                       _editFlatten                                 */
/*                       _editMisalign                                */
/*                       _editNewOrder                                */
/*                       _pushParticles                               */
/*   - LPJM                                                           */
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
#include <messages.h>
#include <DbConnectDialog.h>

#include <qptrlist.h>
#include <qguardedptr.h>
#include <list>

#include <Jet__environment.h>
#include <Mapping.h>

class  BeamlineContext;
class  ReverseBeamlineIterator;
class  DbConnectDialog;
class  DbRetrieveDialog;
class  QBml;
class  QBmlRoot;
class  CHEFPlotMain;
class  Tracker;
class  RayTrace;
class  SiteViewer;
class  QTextEdit;
class  QPyCHEF;
class  DeviceTable;
class  QAssistantClient;
class  BmlSelectionDialog;
class  InitCondDialogLF;
class  QSqlDatabase;
class  CF_Editor;



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
    virtual ~CF_rbendFinder_quad();

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
    virtual ~RollAccumulator();

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

  struct insDlgData
  {
    bool    accepted;
    QString namePrefix;
    bool    upstream;
    bool    downstream;
  };
  insDlgData _insertionDialog() const;

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

  QGuardedPtr<CHEFPlotMain>                   _plotWidget;
  QGuardedPtr<CHEFPlotMain>                   _ETplotWidget;
  QGuardedPtr<CHEFPlotMain>                   _MMplotWidget;
  QGuardedPtr<CHEFPlotMain>                   _LBplotWidget;
  QGuardedPtr<CHEFPlotMain>                   _DspnplotWidget;
  

  Tracker*            _trackWidget;
  RayTrace*           _traceWidget;
  SiteViewer*         _siteWidget;
 
  Messages<0>*                      _messages_stdout;
  Messages<1>*                      _messages_stdwar;
  Messages<2>*                      _messages_stderr;

  std::ostream *                   _p_messages_stdout_stream;
  std::ostream *                   _p_messages_stdwar_stream;
  std::ostream *                   _p_messages_stderr_stream;
  QTextEdit*                       _messages;

  DbConnectDialog*                 _dbconnect_dlg;
  DbRetrieveDialog*                _dbretrieve_dlg;
  QSqlDatabase*                    _default_db; 
 
  InitCondDialogLF*                _initCondDialogLF;       
  InitCondDialogLF*                _initCondDialogDisp;     
  InitCondDialogLF*                _initCondDialogMoments;  


  DeviceTable*                     _devices;
  QAssistantClient*                _assistanthelp;
  BmlSelectionDialog*              _bmlSelectionDialog;

  BeamlineBrowser*                 _p_vwr;
  QPyCHEF*                         _interpreter;


  QPopupMenu*      _fileMenu;
  QPopupMenu*      _toolMenu;
  QPopupMenu*      _editMenu;


  
  QPtrList<QBml>    _selListItems;  // selected QListViewItems
                                    // Doesn't seem to be used so far
                                    // (9/24/03)
  QPtrList<bmlnElmnt> _foundList;   // List of beamline elements found by the editor.

  dlist                  _contextList;
  BeamlineContext*       _p_currBmlCon;  // Currently selected beamline
  QBmlRoot*              _p_currQBmlRoot;// ... and its widget
  QBml*                  _p_clickedQBml; // ... and its QBml
  
 

  EnvPtr<double>::Type                  _p_JetEnv;
  EnvPtr<std::complex<double> >::Type   _p_JetCEnv;

  Options             _userOptions;

  static int _buildVTuneCircuit( const CHEFGUI*, const bmlnElmnt* );
  static int _buildHTuneCircuit( const CHEFGUI*, const bmlnElmnt* );

  void _testFC( ACTFUNC11 ) const;
  void _traverseTree( const QBmlRoot*, ACTFUNC11 ) const;
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
   void _databaseConnect();
   void _databaseRetrieve();
   void _editFindFilter();
   void _editSelectAll();
   void _editSelectNone();
   void _editCopyLine();
   void _editEditElement();
   void _editFlatten();
   void _editNewOrder();
   void _editRemoveLine();
   void _editRenameLine();
   void _editCondense();
   void _editAlign();
   void _editAlignData();
   void _editAlignBends();
   void _editMergeQuads();
   void _editD2S();
   void _editAddQtMons();
   void _editMode();
   void _editPartAndSect();
   void _editAddMarkers();
   void _editPartition();   
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
   void _pushParticles();
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
   void _set_p_clickedContext(BeamlineContext*, QBmlRoot*);
   void _set_p_clickedQBml( QBml* );
   void _windowsCascade();
   void _windowsMinimizeAll();
   void _windowsTile();
   void _windowsDefaultLayout();
   void _handleDBConnection( const DbConnectStruct& st );
   void _exit();

   void _do_nothing();

   void showEvent( QShowEvent* event); 

   void windowsMessagesAction_toggled( bool set );
   void windowsTree_BrowserAction_toggled( bool set );
   void windowsInterpreterAction_toggled( bool  set);
   void windowsDevicesAction_toggled( bool  set);
  
   void _disableMenus();
   void _enableMenus();

 private:

   void _parseEditorPython( CF_Editor* editor );
   void _parseEditorMAD8( CF_Editor* editor );

     
};


#endif
