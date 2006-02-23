/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/Mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CHEF.h                                            ****** 
******  Version:   3.2                                               ******
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ******
******                                                               ******
******  Author:     Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ******
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

#ifndef CHEF_H
#define CHEF_H

                                                // BEGIN CHEF SPECIFIC CODE
#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QPTRLIST_H
#include <qptrlist.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif


#include "complexAddon.h"
                                                // END   CHEF SPECIFIC CODE

// Forward declarations
// --------------------
                                                // BEGIN CHEF SPECIFIC CODE
class Mapping;
                                                // END   CHEF SPECIFIC CODE
class BeamlineContext;
class ReverseBeamlineIterator;

class QBml;
class QBmlRoot;
                                                // BEGIN CHEF SPECIFIC CODE
class QMainWindow;
class QApplication;
class QLabel;
class QMenuBar;
class QPopupMenu;
class QPixmap;
                                                // END   CHEF SPECIFIC CODE

class CHEFPlotMain;
class RayTrace;
class SiteViewer;
class Tracker;

                                                // BEGIN CHEF SPECIFIC CODE
#ifndef BEAMLINEBROWSER_H
#include "BeamlineBrowser.h"
#endif
#ifndef DLIST_HXX
#include "dlist.h"
#endif
#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif
                                                // END   CHEF SPECIFIC CODE


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


                                                // BEGIN CHEF SPECIFIC CODE
class CHEF : public QObject
{
                                                // END   CHEF SPECIFIC CODE
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
                                                // BEGIN CHEF SPECIFIC CODE
  CHEF( beamline* = 0, int = 0, char** = 0 );
  ~CHEF();

  int run();
                                                // END   CHEF SPECIFIC CODE

  struct insDlgData
  {
    bool    accepted;
    QString namePrefix;
    bool    upstream;
    bool    downstream;
  };
  insDlgData _insertionDialog() const;

private:
  QApplication*                    _appl;
  QMainWindow*                     _mainWindow;
                                                // BEGIN CHEF SPECIFIC CODE
  QLabel*                          _centralWidget;
                                                // END   CHEF SPECIFIC CODE

  QGuardedPtr<CHEFPlotMain>        _plotWidget;
  QGuardedPtr<CHEFPlotMain>        _ETplotWidget;
  QGuardedPtr<CHEFPlotMain>        _MMplotWidget;
  QGuardedPtr<CHEFPlotMain>        _LBplotWidget;
  QGuardedPtr<CHEFPlotMain>        _DspnplotWidget;

  BeamlineBrowser*                 _p_vwr;
  Tracker*                         _trackWidget;
  RayTrace*                        _traceWidget;
  SiteViewer*                      _siteWidget;

  argPtr*                          _toolArgs[CHEF_numargs];

                                                // BEGIN CHEF SPECIFIC CODE
  QPixmap*                         _splash;
  
  // ??? Why not use a std::vector ???
    
  // Menus
  QMenuBar*        _mainMenu;
                                                // END   CHEF SPECIFIC CODE
  QPopupMenu*      _fileMenu;
  QPopupMenu*      _toolMenu;
                                                // BEGIN CHEF SPECIFIC CODE
  QPopupMenu*          _calcEnterMenu;
  QPopupMenu*          _calcCalcFuncMenu;
  QPopupMenu*            _calcLattFuncMenu;
  QPopupMenu*            _calcPushMenu;
                                                // END   CHEF SPECIFIC CODE
  QPopupMenu*      _editMenu;
                                                // BEGIN CHEF SPECIFIC CODE
  QPopupMenu*        _edit_selectMenu;
  QPopupMenu*      _mach_imagMenu;
  QPopupMenu*      _calcsMenu;
  QPopupMenu*      _ctrlMenu;
  QPopupMenu*      _dsgnMenu;
  QPopupMenu*      _helpMenu;

  // Menu identifiers
  int              _id_FilePrint;
  int              _id_FileWriteTree;
  int              _id_EditSelectMenu;
  int              _id_ctrlMenu;
  int              _id_dsgnMenu;
                                                // END   CHEF SPECIFIC CODE
  

  QPtrList<QBml>    _selListItems;  // selected QListViewItems
                                    // Doesn't seem to be used so far
                                    // (9/24/03)
  QPtrList<bmlnElmnt> _foundList;   // List of beamline elements found
                                    // by the editor.
  dlist               _contextList;
  BeamlineContext*    _p_currBmlCon;  // Currently selected beamline context
  QBmlRoot*           _p_currQBmlRoot;// ... and its widget
  QBml*               _p_clickedQBml; // ... and its QBml

  EnvPtr<double>::Type                  _p_JetEnv;
  EnvPtr<std::complex<double> >::Type   _p_JetCEnv;

  Options             _userOptions;

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
  void _editCopyLine();
  void _editRemoveLine();
  void _editCondense();
  void _editEditElement();
  void _editAlign();
  void _editAlignData();
  void _editAlignBends();
  void _editFlatten();
  void _editNewOrder();
  void _editMergeQuads();
  void _editD2S();
  void _editAddMarkers();
  void _editAddQtMons();
  void _editMode();
  void _editPartition();
  void _editPartAndSect();
  void _toolMisalign();
  void _helpContents();
  void _helpAbout();
  void _processFilter( const BoolNode& );

  void _slot_contextGenerated( BeamlineContext* );

  void _makeFODO();
  void _makeSingSext();

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

  void _set_p_clickedContext( BeamlineContext*, QBmlRoot* );
  void _set_p_clickedQBml( QBml* );
  void _do_nothing();


signals:
  void _new_beamline();
  void _modeChanged( const BeamlineContext* );
};


#endif // CHEF_H
