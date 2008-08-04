/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/Mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CHEFGUI.h                                         ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
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
/*   - added five slots: editEditElement                              */
/*                       editFlatten                                  */
/*                       editMisalign                                 */
/*                       editNewOrder                                 */
/*                       pushParticles                                */
/*   - LPJM                                                           */
/*                                                                    */
/**********************************************************************/
#ifndef CHEFGUI_H
#define CHEFGUI_H

namespace sqlite { class connection; }

#include <ostream>
#include <chefguibase.h>

#include <basic_toolkit/GenericException.h>

#include <mxyzptlk/TJet.h>
#include <mxyzptlk/EnvPtr.h>
#include <mxyzptlk/Mapping.h>
#include <mxyzptlk/Jet__environment.h>
#include <mxyzptlk/JetC__environment.h>

#include <beamline/FramePusher.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/beamline.h>

#include <physics_toolkit/DriftEliminator.h>
#include <physics_toolkit/QuadEliminator.h>


#include <BeamlineExpressionTree.h>
#include <BeamlineBrowser.h>
#include <Tracker.h>
#include <RayTrace.h>
#include <SiteViewer.h>
#include <QtMonitor.h>
#include <QueryDialog.h>
#include <AlignVisitor.h>
#include <InitCondDialogLF.h>
#include <messages.h>
#include <DbConnectDialog.h>
#include <RollAccumulator.h>
#include <CF_rbendFinder_quad.h>

#include <qptrlist.h>
#include <qguardedptr.h>

#include <list>
#include <boost/function.hpp>



// Forward declarations
// --------------------
class BeamlineContext;
class ReverseBeamlineIterator;

class QBml;
class QBmlRoot;


class CHEFPlotMain;
class AppWorkspace;
class RayTrace;
class SiteViewer;
class Tracker;

class DbConnectDialog;
class DbRetrieveDialog;
class QTextEdit;
class QPyCHEF;
class DeviceTable;
class QAssistantClient;
class BmlSelectionDialog;
class InitCondDialogLF;
class QSqlDatabase;
class CF_Editor;


// Utility classes
// ---------------

struct Options
{
  enum Coupling { ignore_coupling=0, edwards_teng, moments } couplingOption;
};


struct insDlgData
{
    bool    accepted;
    QString namePrefix;
    bool    upstream;
    bool    downstream;
};
 


class CHEFGUI: public CHEFGUIBase {

  Q_OBJECT

  typedef boost::function<void( ElmPtr ) >      ActionOnElm;
  typedef boost::function<void( ConstElmPtr)>   ActionOnConstElm;
  typedef boost::function<void( ConstElmPtr)>   BoolOnConstElm;

public:

  CHEFGUI(QWidget* parent, const char* name, WFlags w);
 ~CHEFGUI();    


public slots:

  void modeRing(bool set);
  void modeLine(bool set);
  void devicesAutoRefresh(bool);
  void fileEditPython();
  void fileEditMAD8();
  void editParse();

private:

  Jet__environment_ptr             env_;
  JetC__environment_ptr            envc_;

  int                              id_FileWriteTree_;
  int                              id_analMenu_;
  int                              id_ctrlMenu_;
  int                              id_EditSelectMenu_;
  int                              id_FilePrint_;

  std::auto_ptr<ParticleBunch>     bunch_;                        

  QObject*                         eventfilter_;
 
  QApplication*                    appl_;
  QMainWindow*                     mainWindow_;
  AppWorkspace*                    centralWidget_;

  QGuardedPtr<CHEFPlotMain>        plotWidget_;

  BeamlineBrowser*                 browser_;
  Tracker*                         tracker_;
  RayTrace*                        tracer_;
  SiteViewer*                      viewer_;


  Messages<0>*                     messages_stdout_;
  Messages<1>*                     messages_stdwar_;
  Messages<2>*                     messages_stderr_;

  std::ostream *                   p_messages_stdout_stream_;
  std::ostream *                   p_messages_stdwar_stream_;
  std::ostream *                   p_messages_stderr_stream_;

  QTextEdit*                       messages_;

  DbConnectDialog*                 dbconnect_dlg_;
  DbRetrieveDialog*                dbretrieve_dlg_;
  QSqlDatabase*                    default_db_; 
 
  InitCondDialogLF*                initCondDialogLF_;       
  InitCondDialogLF*                initCondDialogDisp_;     
  InitCondDialogCovariance*        initCondDialogMoments_;  


  DeviceTable*                     devices_;
  QAssistantClient*                assistanthelp_;
  BmlSelectionDialog*              bmlSelectionDialog_;

  QPyCHEF*                         interpreter_;


  QPopupMenu*                      fileMenu_;
  QPopupMenu*                      toolMenu_;
  QPopupMenu*                      editMenu_;


  std::list<ConstElmPtr>           foundList_;    // List of beamline elements found
                                                  // by the editor.

  std::list<BmlContextPtr>         contextList_;

  BmlContextPtr                    bmlc_;           // Currently selected beamline context
  QBmlRoot*                        p_currQBmlRoot_; // ... and its widget
  QBml*                            p_clickedQBml_;  // ... and its QBml


  Options                          userOptions_;

  void buildVTuneCircuit ( ElmPtr );
  void buildHTuneCircuit ( ElmPtr );
  void buildVChromCircuit( ElmPtr );
  void buildHChromCircuit( ElmPtr );

  void testFC( ActionOnElm& );

  void traverseTree( QBmlRoot const*, ActionOnElm& );
  void pushArgs();

  insDlgData     insertionDialog() const;
  void           enableMenus(bool set);
  void           updateEditorWindowLists(); 


private slots:

  void openFile();
  void print();
  void fileWriteTree();
  void fileSaveAs();
  void editFindFilter();
  void editSelectAll();
  void editSelectNone();
  void editCopyLine();
  void editRemoveLine();
  void editCondense();
  void editEditElement();
  void editAlign();
  void editAlignData();
  void editAlignBends();
  void editFlatten();
  void editNewOrder();
  void editMergeQuads();
  void editMergeBends();
  void editD2S();
  void editAddMarkers();
  void editAddQtMons();
  void editMode();
  void editPartition();
  void editPartAndSect();
  void toolMisalign();
  void helpContents();
  void helpAbout();
  void processFilter( const BoolNode& );

  void slot_contextGenerated( BmlContextPtr );

  void makeFODO();
  void makeSingSext();

  void showCS2DData();
  void showCS4DData();

  void computeReferenceOrbit();
  void computeDispersion();
  void computeTwiss();
  void computeCS4D();

  void periodicMoments();
  void propagateMoments();
  void periodicET();

  void pushParticles();
  void launchTrack();
  void launchRayTrace();
  void launchDilution();
  //void enterMapArg();
  //void enterContextArg();
  //void clearArgs();
  void launchSiteVu();
  void launch_browser();

  void horTuneCtrl();
  void verTuneCtrl();
  void tuneCtrl();
  void horChromCtrl();
  void verChromCtrl();
  void chromCtrl();

  void set_p_clickedContext( BmlContextPtr, QBmlRoot* );
  void set_p_clickedQBml( QBml* );
  void do_nothing();

  void editRenameLine();

  void openDeviceFile(); 
  void openScriptFile(); 
  void devicesClearSelection();
  void devicesRefreshAll();
  void fileEditorSaveAs();
  void fileEditorSave();
  void databaseConnect();
  void databaseRetrieve();

  void windowsCascade();
  void windowsMinimizeAll();
  void windowsTile();
  void windowsDefaultLayout();
  void handleDBConnection( const DbConnectStruct& st );
  void exit();

  void showEvent( QShowEvent* event); 

  void windowsMessagesAction_toggled( bool set );
  void windowsTree_BrowserAction_toggled( bool set );
  void windowsInterpreterAction_toggled( bool  set);
  void windowsDevicesAction_toggled( bool  set);
  
  void disableMenus();
  void enableMenus();

  void toolsGenerateBunch();
  void toolsDisplayBunch();

 private:

  void readBmlFile( QString s);

  void parseEditorPython( CF_Editor* editor );
  void parseEditorMAD8( CF_Editor* editor );


signals:

  void new_beamline();
  void modeChanged( ConstBmlContextPtr );

  // ========================= commands ==========================================

 private:

  boost::function<QWidget*( QWidget*, BmlContextPtr) >                                  command_computeReferenceOrbit_; 
  boost::function<QWidget*( QWidget*, BmlContextPtr,  CSLattFuncs const&)>              command_propagateReferenceOrbit_; 

  boost::function< void( BmlContextPtr) >                                               command_computePeriodicTwiss_; 
  boost::function< void( BmlContextPtr,  CSLattFuncs )>                                 command_computePropagateTwiss_; 
  boost::function< void( QWidget*, sqlite::connection& )>                               command_plotTwiss_; 

  boost::function<QWidget*( QWidget*, BmlContextPtr) >                                  command_computeMoments_; 
  boost::function<QWidget*( QWidget*, BmlContextPtr,  CSLattFuncs const&)>              command_propagateMoments_; 

  boost::function<QWidget*( QWidget*, BmlContextPtr) >                                  command_computeEigenmodes_; 
  boost::function<QWidget*( QWidget*, BmlContextPtr,  CSLattFuncs const&) >             command_propagateEigenmodes_; 

  boost::function<QWidget*( QWidget*, BmlContextPtr) >                                  command_computeDispersion_; 
  boost::function<QWidget*( QWidget*, BmlContextPtr,  CSLattFuncs const&) >             command_propagateDispersion_; 

  boost::function<QWidget*( QWidget*, BmlContextPtr) >                                  command_computeEdwardsTeng_; 
  boost::function<QWidget*( QWidget*, BmlContextPtr,  CSLattFuncs const&)>              command_propagateEdwardsTeng_; 


};


#endif // CHEFGUI_H
