/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******         interfaces to exercise the functionality        
******         of BEAMLINE.                                    
******                                                                
******  File:      CHEF.cc
******  Version:   3.2
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


// 
// Header files.
// 

// #include <stdlib.h>   // Needed for call to system( const char* )
#include <fstream>
#include <typeinfo>
// #include <process.h>

#include <qmainwindow.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qinputdialog.h>
#include <qfiledialog.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qptrlist.h>
// #include <qprocess.h>

#include "GenericException.h"
#include "beamline.h"
#include "InsertionList.h"
#include "BeamlineContext.h"
#include "BeamlineExpressionTree.h"
#include "FramePusher.h"
#include "DriftEliminator.h"
#include "QuadEliminator.h"
#include "bmlfactory.h"
#include "BeamlineBrowser.h"
#include "LattFncPlt.h"
#include "ETFncPlt.h"
#include "MomentsFncPlt.h"
#include "DspnFncPlt.h"
#include "Tracker.h"
#include "RayTrace.h"
#include "SiteViewer.h"
#include "QtMonitor.h"
#include "QueryDialog.h"
#include "ModifierVisitor.h"

// This undef is needed because of the compiler.
// #undef connect

#include "CHEF.h"

extern beamline* DriftsToSlots( /* const */ beamline& original );

using namespace CHEF_domain;
using namespace std;

// 
// Constructors and destructor
// 

CHEF::CHEF( beamline* xbml, int argc, char** argv )
: _p_vwr(0), _plotWidget(0), 
  _ETplotWidget(0), _MMplotWidget(0), _DspnplotWidget(0)
{
  int i;
  for( i = 0; i < CHEF_numargs; i++ )
  {  _toolArgs[i] = 0;
  }

  _userOptions.couplingOption = Options::ignore_coupling;

  if( 0 != xbml ) 
  { _p_currBmlCon = new BeamlineContext( true, xbml );
    _contextList.append( _p_currBmlCon );
  }
  else 
  { _p_currBmlCon = 0;
  }
  // REMOVE: _p_clickedCon = _p_currBmlCon;


  bool kludged = false;
  if( 0 == argc ) {
    argc = 1;
    argv = new char* [1];
    argv[0] = "CHEF";
    kludged = true;
  }
  _appl = new QApplication( argc, argv );


  _mainWindow = new QMainWindow;
  _mainWindow->setCaption( "CHEF" );
  // _mainWindow->resize( 600, 400  );

  _centralWidget = new QLabel( _mainWindow );
  // _centralWidget->setText( "DUMMY CENTRAL WIDGET" );

  #include "chef_logo.xpm"  // This file instantiates array chef_logo_xpm

  _splash = new QPixmap( chef_logo_xpm );    // not deleted
  _centralWidget->setPixmap( *_splash );
  _mainWindow->setCentralWidget( _centralWidget );

  // Construct main menu
  _mainMenu = _mainWindow->menuBar();

    _fileMenu = new QPopupMenu;
    _fileMenu->insertItem( "Open...", this, SLOT(_openFile()) );
    _fileMenu->insertItem( "Save as...", this, SLOT(_fileSaveAs()) );
      _id_FilePrint = 
    _fileMenu->insertItem( "Peek at",   this, SLOT(_print()) );
      _fileMenu->setItemEnabled( _id_FilePrint, false);
      _id_FileWriteTree = 
    _fileMenu->insertItem( "Write tree", this, SLOT(_fileWriteTree()) );
      _fileMenu->setItemEnabled( _id_FileWriteTree, false);
    _fileMenu->insertSeparator();
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( "Exit", qApp, SLOT(quit()) );
  _mainMenu->insertItem( "File", _fileMenu );
    

    _editMenu = new QPopupMenu;
      _edit_selectMenu = new QPopupMenu;
      _edit_selectMenu->insertItem( "Filter...", this, SLOT(_editFindFilter()) );
      _edit_selectMenu->insertSeparator();
      _edit_selectMenu->insertItem( "All", this, SLOT(_editSelectAll()) );
      _edit_selectMenu->insertItem( "None", this, SLOT(_editSelectNone()) );
      // REMOVE: _edit_selectMenu->insertItem( "Line", this, SLOT(_editSelectLine()) );
    _id_EditSelectMenu =
    _editMenu->insertItem( "Select", _edit_selectMenu );
    _editMenu->setItemEnabled( _id_EditSelectMenu, false);
    
    _editMenu->insertItem( "Copy line", this, SLOT(_editCopyLine()) );
    _editMenu->insertItem( "Remove line", this, SLOT(_editRemoveLine()) );
    _editMenu->insertItem( "(Mis)align ...", this, SLOT(_editAlign()) );
    _editMenu->insertItem( "Condense", this, SLOT(_editCondense()) );
    _editMenu->insertItem( "Merge equivalent quads", this, SLOT(_editMergeQuads()) );
    _editMenu->insertItem( "Convert to Slots", this, SLOT(_editD2S()) );
    int id_AddMon =
    _editMenu->insertItem( "Insert monitors", this, SLOT(_editAddQtMons()) );
    _editMenu->setItemEnabled( id_AddMon, false );   
    int id_PartSect = 
    _editMenu->insertItem( "Partition and sectorize", this, SLOT(_editPartAndSect()) );
    _editMenu->setItemEnabled( id_PartSect, false );
  _mainMenu->insertItem( "Edit", _editMenu );

    _mach_imagMenu = new QPopupMenu;
    _mach_imagMenu->insertItem( "FODO Cell", this, SLOT(_makeFODO()) );
    _mach_imagMenu->insertItem( "Single Sextupole", this, SLOT(_makeSingSext()) );
  _mainMenu->insertItem( "Examples", _mach_imagMenu );

    _toolMenu = new QPopupMenu;

      _tool_calcMenu = new QPopupMenu;

        // _calcEnterMenu = new QPopupMenu;
        // _calcEnterMenu->insertItem( "Line", this, SLOT(_enterContextArg()) );
        // _calcEnterMenu->insertItem( "Map", this, SLOT(_enterMapArg()) );
        // _calcEnterMenu->insertSeparator();
        // _calcEnterMenu->insertItem( "Clear", this, SLOT(_clearArgs()) );
      // _tool_calcMenu->insertItem( "Enter...", _calcEnterMenu);

        _calcCalcFuncMenu = new QPopupMenu;

          _calcLattFuncMenu = new QPopupMenu;
          _calcLattFuncMenu->insertItem( "Uncoupled", this, SLOT(_launchLatt()) );
          _calcLattFuncMenu->insertItem( "Edwards-Teng", this, SLOT(_launchET()) );
          _calcLattFuncMenu->insertItem( "Moments", this, SLOT(_launchMoments()) );
        _calcCalcFuncMenu->insertItem( "Lattice Functions", _calcLattFuncMenu );

        _calcCalcFuncMenu->insertItem( "Dispersion", this, SLOT(_launchDispersion()) );
        _calcCalcFuncMenu->insertItem( "Emittance Dilution", this, SLOT(_launchDilution()) );
      _tool_calcMenu->insertItem( "Calculations", _calcCalcFuncMenu );

      _tool_calcMenu->insertItem( "Site Viewer", this, SLOT(_launchSiteVu()) );
      _tool_calcMenu->insertItem( "Track", this, SLOT(_launchTrack()) );
      _tool_calcMenu->insertItem( "Trace", this, SLOT(_launchRayTrace()) );

      _id_analMenu = 
    _toolMenu->insertItem( "Analysis", _tool_calcMenu );
    _toolMenu->setItemEnabled( _id_analMenu, false );

      _tool_ctrlMenu = new QPopupMenu;
      _tool_ctrlMenu->insertItem( "Build hor. tune circuit", this, SLOT(_horTuneCtrl()) );
      _tool_ctrlMenu->insertItem( "Build ver. tune circuit", this, SLOT(_verTuneCtrl()) );
      _tool_ctrlMenu->insertItem( "Adjust tune", this, SLOT(_tuneCtrl()) );
      _tool_ctrlMenu->insertSeparator();
        int id_chrom = 
      _tool_ctrlMenu->insertItem( "Chromaticity", this, SLOT(_chromCtrl()) );
        _tool_ctrlMenu->setItemEnabled( id_chrom, false );
      _tool_ctrlMenu->insertSeparator();
      _tool_ctrlMenu->insertItem( "Align CF_rbends...", this, SLOT(_toolAlignBends()) );
      _tool_ctrlMenu->insertItem( "Misalign all...",    this, SLOT(_toolMisalign()) );
      _id_ctrlMenu = 
    _toolMenu->insertItem( "Control", _tool_ctrlMenu );
    _toolMenu->setItemEnabled( _id_ctrlMenu, false );

      _tool_dsgnMenu = new QPopupMenu;
      _id_dsgnMenu = 
    _toolMenu->insertItem( "Design", _tool_dsgnMenu );
      _toolMenu->setItemEnabled( _id_dsgnMenu, false );
  _mainMenu->insertItem( "Tools", _toolMenu );

    _helpMenu = new QPopupMenu;
    _helpMenu->insertItem( "Contents", this, SLOT(_helpContents()) );
    _helpMenu->insertItem( "About", this, SLOT(_helpAbout()) );
    int id_helpMenu = 
  _mainMenu->insertItem( "Help", _helpMenu );
  _mainMenu->setItemEnabled( id_helpMenu, false );


  // Make connections
  connect( this, SIGNAL(_new_beamline()), 
           this, SLOT  (_launch_browser()) );


  // Set geometry and show _mainWindow
  _mainWindow->adjustSize();
  QRect geom = _mainWindow->geometry();
  int u = ( QApplication::desktop()->width()  - geom.width()  ) / 2;
  int v = ( QApplication::desktop()->height() - geom.height() ) / 2;
  v /= 3;
  geom.moveTopLeft( QPoint(u,v) );
  _mainWindow->setGeometry( geom );
  _mainWindow->show();

  _appl->setMainWidget( _mainWindow );


  // Create an initial Jet environment 
  double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };
  Jet::BeginEnvironment( 1 );
  // coord x(0.0),  y(0.0),  z(0.0),
  //      px(0.0), py(0.0), pz(0.0);
  // ??? This needs to be fixed!!!
  coord* x = new coord(0.0);
  coord* y = new coord(0.0);
  coord* z = new coord(0.0);
  coord* px = new coord(0.0);
  coord* py = new coord(0.0);
  coord* pz = new coord(0.0);
  _p_JetEnv = Jet::EndEnvironment( scale );
  _p_JetCEnv = JetC::_lastEnv = JetC::CreateEnvFrom( _p_JetEnv );
 
  if( _p_JetEnv != Jet::_lastEnv ) {
    ostringstream uic;
    uic << "An impossibility has occurred\nin file "
        << __FILE__
        << " at line " << __LINE__
        << ":\n_p_JetEnv != Jet::lastEnv";
    QMessageBox::information( 0, "CHEF: ERROR",
                              uic.str().c_str() );
  }

  // Cleaning up
  if( kludged ) {delete [] argv;}
}


CHEF::~CHEF()
{
  int i;
  for( i = 0; i < CHEF_numargs; i++ )
  { if( 0 != _toolArgs[i] ) { delete _toolArgs[i]; } }

  delete _mainWindow;
}



// 
// Slots
// 

int CHEF::run()
{
  return _appl->exec();
}


void CHEF::_editFindFilter()
{
  QueryDialog* qdl = new QueryDialog( 0, 0, false );
  connect( qdl,  SIGNAL(_useThis( const BoolNode& )), 
           this, SLOT  (_processFilter( const BoolNode& )) );
  qdl->exec();
  delete qdl;
}


void CHEF::_processFilter( const BoolNode& query )
{
  int i = 1;
  _foundList.clear();
  if( 0 == (_p_vwr->findElement( _p_clickedQBml, query, _foundList )) ) {
    bmlnElmnt* elementPtr;
    QPtrListIterator<bmlnElmnt> it( _foundList );
    while ( (elementPtr = it.current()) != 0 ) {
      ++it;
    }
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Operation not successful." );
  }
}


void CHEF::_editSelectAll()
{ 
  _p_vwr->selectAll( true );
}


void CHEF::_editSelectNone()
{
  _p_vwr->clearSelection();
}


// REMOVE: void CHEF::_editSelectLine()
// REMOVE: {
// REMOVE:   _editSelectNone();
// REMOVE:   _p_vwr->setSelected( _p_clickedQBml, true );
// REMOVE:   _p_currBmlCon = _p_clickedCon;
// REMOVE: }


void CHEF::_editCopyLine()
{
  // Procedure copied from CHEF.builders.cc
  // and slightly modified.
  // REMOVE: if( 0 != _p_clickedCon ) { 
  // REMOVE:   _p_currBmlCon = new BeamlineContext( false, 
  // REMOVE:                                        _p_clickedCon->cloneBeamline() );
  if( 0 != _p_currBmlCon ) { 
    _p_currBmlCon = new BeamlineContext( false, 
                                         _p_currBmlCon->cloneBeamline() );
    _p_currBmlCon->setClonedFlag( true );
    _contextList.insert( _p_currBmlCon );
    emit _new_beamline();
  }
}


void CHEF::_editRemoveLine()
{
  if( 0 == _p_currBmlCon ) { return; }
  if( 0 == (_p_vwr->removeBeamline( _p_currBmlCon )) )
  { // Note: Beamline has been eliminated as well.
    _contextList.remove( _p_currBmlCon );
    // REMOVE: if( _p_currBmlCon == _p_clickedCon ) { _p_currBmlCon = 0; }
    _p_currBmlCon = 0;
    // REMOVE: _p_clickedCon = 0;
  }
  else 
  { QMessageBox::information( 0, "CHEF: ERROR",
                              "Operation not successful." );
  }
}


void CHEF::_horTuneCtrl()
{
  _testFC( CHEF::_buildHTuneCircuit );
}


void CHEF::_verTuneCtrl()
{
  _testFC( CHEF::_buildVTuneCircuit );
}


void CHEF::_tuneCtrl()
{
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlbh = new QLabel( "Delta H tune", qhb1 );
        QLineEdit* qleh = new QLineEdit( "0.0", qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QLabel* qlbv = new QLabel( "Delta V tune", qhb2 );
        QLineEdit* qlev = new QLineEdit( "0.0", qhb2 );
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

  wpu->setCaption( "CHEF: Tune Adjuster" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    bool okh, okv;
    double deltaNuH = (qleh->text()).toDouble( &okh);
    double deltaNuV = (qlev->text()).toDouble( &okv);
    if( okh && okv ) {
      if( (0. != deltaNuH) || (0. != deltaNuV) ) {
        if( 0 != (returnCode = _p_currBmlCon->changeTunesBy( deltaNuH, deltaNuV )) ) {
          cerr << "*** WARNING *** File "
               << __FILE__ 
               << ", Line "
               << __LINE__
               << ": Tune adjustment returned error condition "
               << returnCode
               << endl;
          QMessageBox::information( 0, "CHEF",
                      "Tune adjustment error." );
        }
      }
    }
  }

  delete wpu;
}


void CHEF::_testFC( ACTFUNC11 actfcn ) const
{
  QListViewItem* fc = _p_vwr->firstChild();

  while( 0 != fc ) {
    if(typeid(*fc) == typeid(QBmlRoot)) {
      this->_traverseTree( dynamic_cast<QBmlRoot*>(fc), 
                           actfcn );
    }

    else {
      ostringstream uic;
      uic << "An impossibility has occurred\nin file "
          << __FILE__
          << " at line " << __LINE__ ;
      QMessageBox::information( 0, "CHEF: ERROR",
                                uic.str().c_str() );
    }

    fc = fc->nextSibling();
  }
}


void CHEF::_traverseTree( const QBmlRoot* x, ACTFUNC11 actfcn ) const
{
  // cout << "Recognized QBmlRoot " 
  //      << x->fullName()
  //      << endl;

  QListViewItem* fc = x->firstChild();

  while( 0 != fc ) 
  {
    if(typeid(*fc) == typeid(QBmlElmt)) {
      // cout << "Recognized QBmlElmt "
      //      << dynamic_cast<QBmlElmt*>(fc)->fullName()
      //      << endl;
      if( fc->isSelected() ) {
        (*actfcn)( this, dynamic_cast<QBmlElmt*>(fc)->cheatElementPtr() );
      }
    }

    else if(typeid(*fc) == typeid(QBmlRoot)) {
      this->_traverseTree( dynamic_cast<QBmlRoot*>(fc), actfcn );
    }

    else {
      ostringstream uic;
      uic << "An impossibility has occurred\nin file "
          << __FILE__
          << " at line " << __LINE__ ;
      QMessageBox::information( 0, "CHEF: ERROR",
                                uic.str().c_str() );
    }
    
    fc = fc->nextSibling();
  }
}


int CHEF::_buildHTuneCircuit( const CHEF* aPtr, const bmlnElmnt* bPtr )
{
  aPtr->_p_currBmlCon->addHTuneCorrector( bPtr );
}


int CHEF::_buildVTuneCircuit( const CHEF* aPtr, const bmlnElmnt* bPtr )
{
  aPtr->_p_currBmlCon->addVTuneCorrector( bPtr );
}


void CHEF::_toolMisalign()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
    return;
  }

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        QButtonGroup* qbg = new QButtonGroup( 2, Qt::Vertical, QString("Misalign"), qhb1 );
          QRadioButton* bendPtr 
            = new QRadioButton( "Bends: ", qbg );
            bendPtr->setChecked( true );
          QRadioButton* quadPtr 
            = new QRadioButton( "Quads: ", qbg );
        qbg->setExclusive( true );
        qbg->setMargin(5);
        qbg->adjustSize();
        
        QLabel* qlb = new QLabel( " Roll [mrad]: ", qhb1 );

        QLineEdit* qle = new QLineEdit( "0.0", qhb1 );

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

  QString qstr( "CHEF: " );
  qstr += _p_currBmlCon->name();
  wpu->setCaption( qstr );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) 
  {
    bool ok = false;
    double rollAngle = 0.001*( (qle->text()).toDouble(&ok) );
    if( ok ) 
    {
      if( fabs(rollAngle) < 1.0e-6 ) {
        QMessageBox::information( 0, "CHEF: INFO",
                                  "Roll must exceed 1 microradian.\nWill do nothing." );
      }

      else if( bendPtr == qbg->selected() ) {
        RollAccumulator actn( "CF_rbend", rollAngle );
        int n = _p_currBmlCon->processElements( actn );

        QString numString;
        numString.setNum(n);
        numString += " CF_rbends were rolled.";
        QMessageBox::information( 0, "CHEF: INFO", numString );
      }

      else if( quadPtr == qbg->selected() ) {
        RollAccumulator actn( "quadrupole", rollAngle );
        int n = _p_currBmlCon->processElements( actn );

        QString numString;
        numString.setNum(n);
        numString += " quadrupoles were rolled.";
        QMessageBox::information( 0, "CHEF: INFO", numString );
      }

    }
  }
    
  delete wpu;
}


void CHEF::_toolAlignBends()
{
  // This menu function changes the beamline in place
  // without creating a new one.

  const bmlnElmnt* selected = _p_vwr->getSelectedElement(_p_currBmlCon);
  if( 0 == selected ) 
  {
    QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a prototype CF_rbend in the browser." );
  }

  else 
  {
    if( typeid(*selected) == typeid(CF_rbend) ) 
    {
      // The following was modeled on rrv19.cfrbend.cfg
      CF_rbend* prototype = (CF_rbend*) selected->Clone();

      Proton p( _p_currBmlCon->getEnergy() );
      double inState [] = { 0., 0., 0., 0., 0., 0. };
      // inState[ p.npxIndex() ] 
      //             =  ( p.Momentum()/p.ReferenceMomentum() )
      //                * sin( prototype->getPoleFaceAngle() / 2.0 );
      inState[ p.npxIndex() ] 
                  =  ( p.Momentum()/p.ReferenceMomentum() )
                     * sin( prototype->getPoleFaceAngle() );
      p.setState( inState );

      double delta = ( prototype->AdjustPosition(p) );

      // Dialog: permission to realign the CF_rbends
      QDialog* wpu = new QDialog( 0, 0, true );
        QVBox* qvb = new QVBox( wpu );
          QString str( "Similar rbends will be moved " );
          QString str2;
          str2.setNum(1000.0*delta);
          str += str2;
          str += " mm.";
          QLabel* qlb = new QLabel( str, qvb );

          QHBox* qhb2 = new QHBox( qvb );
            QPushButton* okayBtn = new QPushButton( "Proceed", qhb2 );
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
    
      wpu->setCaption( "CHEF: CF_rbend Alignment" );
      wpu->adjustSize();
    
      int returnCode = wpu->exec();

      // Realign bends
      if( returnCode == QDialog::Accepted ) 
      {
        CF_rbendFinder_quad cf( prototype->getQuadrupole() );
        _p_currBmlCon->setAlignment( cf, prototype->Alignment() );
      }

      delete wpu;
      delete prototype;
    }

    else 
    {
      QMessageBox::information( 0, "CHEF: INFO",
                                "Selected element is not CF_rbend." );
    }

  }
}


void CHEF::_editAlign()
{
  // Specify an offset and angle
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhbx = new QHBox( qvb );
        QLabel* qlbx = new QLabel( "Horizontal displacement [mm]: ", qhbx );
        QLineEdit* qlex = new QLineEdit( "0.0", qhbx );
      qhbx->setMargin(5);
      qhbx->setSpacing(3);

      QHBox* qhby = new QHBox( qvb );
        QLabel* qlby = new QLabel( "Vertical displacement [mm]: ", qhby );
        QLineEdit* qley = new QLineEdit( "0.0", qhby );
      qhby->setMargin(5);
      qhby->setSpacing(3);

      QHBox* qhbr = new QHBox( qvb );
        QLabel* qlbr = new QLabel( "Roll angle [mrad]: ", qhbr );
        QLineEdit* qler = new QLineEdit( "0.0", qhbr );
      qhbr->setMargin(5);
      qhbr->setSpacing(3);

      QHBox* qhbp = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "Okay", qhbp );
          okayBtn->setDefault( true );
          connect( okayBtn, SIGNAL(pressed()),
                   wpu,     SLOT(accept()) );
        QPushButton* cancelBtn = new QPushButton( "Cancel", qhbp );
          connect( cancelBtn, SIGNAL(pressed()),
                   wpu,       SLOT(reject()) );
      qhbp->setMargin(5);
      qhbp->setSpacing(3);

      // Fix widths of labels
      int max = qlbx->sizeHint().width();
      if( max < qlby->sizeHint().width() ) { max = qlby->sizeHint().width(); }
      if( max < qlbr->sizeHint().width() ) { max = qlbr->sizeHint().width(); }
      qlbx->setFixedWidth(max);
      qlby->setFixedWidth(max);
      qlbr->setFixedWidth(max);

      qhbx->adjustSize();
      qhby->adjustSize();
      qhbr->adjustSize();
      qhbp->adjustSize();
    qvb->adjustSize();

  wpu->setCaption( "CHEF: Alignment parameters" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    // Read parameters
    bool okx, oky, okr;
    alignmentData euclidData;
    euclidData.xOffset = (qlex->text()).toDouble(&okx);
    euclidData.yOffset = (qley->text()).toDouble(&oky);
    euclidData.tilt    = (qler->text()).toDouble(&okr);

    // Process ...
    if(okr && okx && oky) {
      euclidData.xOffset *= 0.001;  // millimeters  -> meters
      euclidData.yOffset *= 0.001;  // millimeters  -> meters
      euclidData.tilt    *= 0.001;  // milliradians -> radians

      // Search for selected elements
      QPtrList<bmlnElmnt> theChosenOnes;
      theChosenOnes = _p_vwr->findAllSelected(dynamic_cast<QBmlRoot*>(_p_clickedQBml));

      // Convert to a BmlPtrList, for use by AlignVisitor
      BmlPtrList theOnes;
      bmlnElmnt* elementPtr;
      while( 0 != (elementPtr = theChosenOnes.take()) ) {
        theOnes.append(elementPtr);
      }

      // Do the (mis)alignment
      AlignVisitor euclid( euclidData, theOnes );
      // REMOVE: _p_clickedCon->accept( euclid );
      if( 0 != _p_currBmlCon) { _p_currBmlCon->accept( euclid ); }
    }
  }
}


void CHEF::_editCondense()
{
  DriftEliminator de;
  _p_currBmlCon->accept(de);
  // if( !(_contextList.contains(_p_currBmlCon)) ) 
  // { _contextList.insert( _p_currBmlCon );
  // }


  beamline* bmlPtr = de.clonedBeamlinePtr();
  _p_currBmlCon = new BeamlineContext( false, bmlPtr );
  // _p_currBmlCon = new BeamlineContext( false, de.clonedBeamlinePtr() );


  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_editMergeQuads()
{
  QuadEliminator qe;
  _p_currBmlCon->accept(qe);

  beamline* bmlPtr = qe.clonedBeamlinePtr();
  _p_currBmlCon = new BeamlineContext( false, bmlPtr );
  // _p_currBmlCon = new BeamlineContext( false, qe.clonedBeamlinePtr() );


  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_editD2S() 
{
  beamline* bmlPtr = DriftsToSlots( (beamline&) *(_p_currBmlCon->cheatBmlPtr()) );
  _p_currBmlCon = new BeamlineContext( false, bmlPtr );
  // _p_currBmlCon = new BeamlineContext( false, de.clonedBeamlinePtr() );

  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_editAddQtMons()
{
  const beamline* oldbmlPtr = _p_currBmlCon->cheatBmlPtr();

  beamline* bmlPtr = new beamline( oldbmlPtr->Name() );
  bmlPtr->setEnergy( oldbmlPtr->Energy() );

  DeepBeamlineIterator dbi( oldbmlPtr );
  bmlnElmnt* q      = 0;
  QtMonitor* qtmPtr = 0;
  while((  q = dbi++  )) {
    bmlPtr->append( q->Clone() );
    // if( typeid(*q) == typeid(quadrupole) ) {
    if( typeid(*q) == typeid(drift) ) {
      qtmPtr = new QtMonitor( "QtMonitor" );
      qtmPtr->setStrength(5.0);  // to survive condensation
      bmlPtr->append( qtmPtr );
    }
  }
  QtMonitor::setAzimuth( bmlPtr );

  _p_currBmlCon = new BeamlineContext( false, bmlPtr );
  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_editPartAndSect()
{
  beamline* bmlPtr = _p_currBmlCon->cloneBeamline();
  // Must be eliminated before exiting.

  double energy    = bmlPtr->Energy();
  double momentum  = sqrt( energy*energy - PH_NORM_mp*PH_NORM_mp );
  double brho = momentum/PH_CNV_brho_to_p;

  // Insert equally spaced markers throughout the model.
  double bmlLength = 0.0;

  bmlnElmnt* q;
  DeepBeamlineIterator dbi( bmlPtr );
  while((  q = dbi++  )) {
    bmlLength += q->Length();
  }
  dbi.reset();

  // Dialog to obtain number of markers
  int numberOfSectors = 5;

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlbh = new QLabel( "Number of partitions", qhb1 );
        QLineEdit* qle = new QLineEdit( "5", qhb1 );
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

  wpu->setCaption( "CHEF: Partition" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  int order = 2;
  beamline* splitBmlPtr = new beamline( "Sectorized beamline" );
  splitBmlPtr->setEnergy( bmlPtr->Energy() );
  if( returnCode == QDialog::Accepted ) {
    bool ok;
    numberOfSectors = (qle->text()).toInt( &ok);

    if( ok && ( numberOfSectors > 1 )) {
      // Insert equally spaced markers throughout 
      // the bml model.

      marker* spaceCharge[ numberOfSectors + 1 ];
      double markerInterval = bmlLength / ((double) numberOfSectors);
      double insertionPoint = markerInterval;
      InsertionList insl( momentum );
      
      spaceCharge[ 0 ] = new marker;
      int i;
      for( i = 1; i < numberOfSectors; i++ ) {
        spaceCharge[i] = new marker;
        insl.Append( new InsertionListElement( insertionPoint, spaceCharge[i] ));
        insertionPoint += markerInterval;
      }
      spaceCharge[ numberOfSectors ] = new marker;
    
      double s = 0.0;  
      slist removedElements;
    
      bmlPtr->insert( spaceCharge[0] );
      bmlPtr->InsertElementsFromList( s, insl, removedElements );
      bmlPtr->append( spaceCharge[numberOfSectors] );
    
      // Display and delete the removed elements
      cout << "Removed elements\n";
      slist_iterator fembril( removedElements );
      bmlnElmnt* qq;
      while(( qq = (bmlnElmnt*) fembril() )) {
        cout << qq->Type() << "  " << qq->Name() << "\n";
        delete qq;
      }
      cout << endl;
    
      // Create a temporary Jet environment 
      Jet__environment*  formerJetEnv  = Jet::_lastEnv;
      JetC__environment* formerJetCEnv = JetC::_lastEnv;
      double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };
      Jet::BeginEnvironment( order );
      // coord x(0.0),  y(0.0),  z(0.0),
      //      px(0.0), py(0.0), pz(0.0);
      // ??? This needs to be fixed!!!
      coord* x = new coord(0.0);
      coord* y = new coord(0.0);
      coord* z = new coord(0.0);
      coord* px = new coord(0.0);
      coord* py = new coord(0.0);
      coord* pz = new coord(0.0);
      Jet::EndEnvironment( scale );
      JetC::_lastEnv = JetC::CreateEnvFrom( _p_JetEnv );
 
      // Sectorize between the partition markers.
      for( i = 0; i < numberOfSectors; i++ ) {
        JetProton jp( energy );
        splitBmlPtr->append( bmlPtr->MakeSector( *(spaceCharge[i]),
                                                 *(spaceCharge[i+1]),
                                                 order, jp  ) );
        splitBmlPtr->append( spaceCharge[i+1]->Clone() );
      }

      // Restore former environment...
      Jet::_lastEnv  = formerJetEnv;
      JetC::_lastEnv = formerJetCEnv;
    }
  }


  delete wpu;
  bmlPtr->eliminate();

  _p_currBmlCon = new BeamlineContext( false, splitBmlPtr );
  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_slot_contextGenerated( BeamlineContext* x )
{
  if( x ) 
  { if( x->getClonedFlag() ) 
    { _p_currBmlCon = x;
      _contextList.insert( x );
    }
  }
}


void CHEF::_launchLatt()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  try {
    _plotWidget = new LattFncPlt(_p_currBmlCon);
  }
  catch( const std::exception& ge ) {
    if( 0 != _plotWidget ) { delete _plotWidget; _plotWidget = 0; }
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown with message:\n"
        << ge.what();
    QMessageBox::information( 0, "CHEF: ERROR",
                             uic.str().c_str() );
    return;
  }

  char theCaption[1024];
  for( int i = 0; i < 1024; i++ ) {
    theCaption[i] = '\0';
  }
  strcat( theCaption, "CHEF: Uncoupled Lattice Functions: " );
  strcat( theCaption, _p_currBmlCon->name() );
  _plotWidget->setCaption( theCaption );
}


void CHEF::_launchET()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  try {
    _ETplotWidget = new ETFncPlt(_p_currBmlCon);
  }
  catch( const std::exception& ge ) {
    if( 0 != _ETplotWidget ) { delete _ETplotWidget; _ETplotWidget = 0; }
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown with message:\n"
        << ge.what();
    QMessageBox::information( 0, "CHEF: ERROR",
                              uic.str().c_str() );
    return;
  }

  char theCaption[1024];
  for( int i = 0; i < 1024; i++ ) {
    theCaption[i] = '\0';
  }
  strcat( theCaption, "CHEF: Edwards-Teng Lattice Functions: " );
  strcat( theCaption, _p_currBmlCon->name() );
  _ETplotWidget->setCaption( theCaption );
}


void CHEF::_launchMoments()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  try {
    _MMplotWidget = new MomentsFncPlt(_p_currBmlCon);
  }
  catch( const std::exception& ge ) {
    if( 0 != _MMplotWidget ) { delete _MMplotWidget; _MMplotWidget = 0; }
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown with message:\n"
        << ge.what();
    QMessageBox::information( 0, "CHEF: ERROR",
                              uic.str().c_str() );
    return;
  }

  char theCaption[1024];
  for( int i = 0; i < 1024; i++ ) {
    theCaption[i] = '\0';
  }
  strcat( theCaption, "CHEF: Covariance Lattice Functions: " );
  strcat( theCaption, _p_currBmlCon->name() );
  _MMplotWidget->setCaption( theCaption );
}


void CHEF::_launchTrack()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  _trackWidget = new Tracker(_p_currBmlCon);
  _trackWidget->setCaption( "CHEF:: Phase Space Tracking" );
  _trackWidget->show();
}


void CHEF::_launchRayTrace()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  _traceWidget = new RayTrace(_p_currBmlCon);
  _traceWidget->setCaption( "CHEF:: Orbit Trace" );
  _traceWidget->show();
}


void CHEF::_launchDispersion()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  try {
    _DspnplotWidget = new DspnFncPlt(_p_currBmlCon);
  }
  catch( const std::exception& ge ) {
    if( 0 != _DspnplotWidget ) { delete _DspnplotWidget; _DspnplotWidget = 0; }
    ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown with message:\n"
        << ge.what();
    QMessageBox::information( 0, "CHEF: ERROR",
                             uic.str().c_str() );
    return;
  }

  char theCaption[1024];
  for( int i = 0; i < 1024; i++ ) {
    theCaption[i] = '\0';
  }
  strcat( theCaption, "CHEF: Dispersion: " );
  strcat( theCaption, _p_currBmlCon->name() );
  _DspnplotWidget->setCaption( theCaption );
}


void CHEF::_launchDilution()
{
  // Test for arguments
  if( (0 == _toolArgs[0]) || (0 == _toolArgs[1]) ) {
    QMessageBox::information( 0, "CHEF",
                              "ERROR: You must set two arguments as Mappings." );
    return;
  }

  // Obtain arguments
  BeamlineContextPtr* onePtr = dynamic_cast<BeamlineContextPtr*>( _toolArgs[1] );
  BeamlineContextPtr* twoPtr = dynamic_cast<BeamlineContextPtr*>( _toolArgs[0] );

  if( (0 == onePtr) || (0 == twoPtr) ) {
    QMessageBox::information( 0, "CHEF",
                              "ERROR: Arguments were not set correctly." 
                              "\nERROR: Must have two beamlines." );
    return;
  }

  // Phase space dimension must be 6 (optional?)
  int n = Particle::PSD;
  if( 6 != n ) {
    QMessageBox::information( 0, "CHEF",
         "ERROR: Was expecting 6 dimensional phase space." );
    return;
  }

  const int index [] = { Particle::_x(), Particle::_xp(), 
                         Particle::_y(), Particle::_yp() };

  // Get invariant emittances
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
        QLabel* qlba 
          = new QLabel( "(pseudo-)horizontal invariant emittance", qhb1 );
        QLineEdit* qleh = new QLineEdit( "30", qhb1 );
        new QLabel( "pi mm-mr", qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);

      QHBox* qhb2 = new QHBox( qvb );
        QLabel* qlbb
          = new QLabel( "(pseudo-)vertical invariant emittance", qhb2 );
        QLineEdit* qlev = new QLineEdit( "30", qhb2 );
        new QLabel( "pi mm-mr", qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);


      // Fix widths of labels
      int max = qlba->sizeHint().width();
      if( max < qlbb->sizeHint().width() ) { max = qlbb->sizeHint().width(); }
      qlba->setFixedWidth(max);
      qlbb->setFixedWidth(max);


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

    // Adjust sizes
      qhb1->adjustSize();
      qhb2->adjustSize();
      qhb3->adjustSize();
    qvb->adjustSize();

  wpu->setCaption( "CHEF::Tool: Emittance Dilution" );
  wpu->adjustSize();

  int returnCode = wpu->exec();


  // Do the calculation
  if( returnCode == QDialog::Accepted ) {
    bool okh, okv;
    double emm_1 = (qleh->text()).toDouble( &okh);
    double emm_2 = (qlev->text()).toDouble( &okv);
    if( okh && okv ) {
      MatrixD cov1(n,n), cov2(n,n);
      BeamlineContext* bc1Ptr = (BeamlineContext*) (onePtr->ptr());
      BeamlineContext* bc2Ptr = (BeamlineContext*) (twoPtr->ptr());

      cov1 = bc1Ptr->equilibriumCovariance( emm_1, emm_2 );
      cov2 = bc2Ptr->equilibriumCovariance( emm_1, emm_2 );

      MatrixD cov_a(4,4), cov_b(4,4);
      int i, j;
      for( i = 0; i < 4; i++ ) {
        for( j = 0; j < 4; j++ ) {
          cov_a(i,j) = cov1( index[i], index[j] );
          cov_b(i,j) = cov2( index[i], index[j] );
        }
      }

      MatrixD ev(1,4);
      ev = real( (cov_a*(cov_b.inverse())).eigenValues() );
      double rho = - 1.0e88;
      for( i = 0; i < 4; i++ ) 
      { if( rho < ev(0,i) ) { rho = ev(0,i); } }
      double answer = rho*rho*rho*rho;
      answer *= cov_b.determinant();
      answer /= cov_a.determinant();
      answer = sqrt( answer );

      // Communicate answer to outside world.
      QString answerString;
      answerString.setNum( 100.0*( answer - 1.0 ) );
      QMessageBox::information( 0, "CHEF",
        "Emittance dilution = " + answerString + " %" );
    }
  }
}


void CHEF::_pushArgs()
{
  int i;
  int j = CHEF_numargs - 1;
  if( _toolArgs[j] ) { 
    delete _toolArgs[j];  // virtual destructor should work
    _toolArgs[j] = 0;
  }
  for( i = j; i >= 1; i-- ) {
    _toolArgs[i] = _toolArgs[i-1];
  }
}


void CHEF::_enterMapArg()
{
  _pushArgs();

  // REMOVE: if( 0 != _p_clickedCon ) {
  // REMOVE:   _toolArgs[0] = new MappingPtr( _p_clickedCon->getOneTurnMap() );
  // REMOVE: }
  if( 0 != _p_currBmlCon ) {
    _toolArgs[0] = new MappingPtr( _p_currBmlCon->getOneTurnMap() );
  }
  else {
    QMessageBox::information( 0, "CHEF",
                              "Left click on a beamline, idiot." );
  }
}


void CHEF::_enterContextArg()
{
  _pushArgs();

  // REMOVE: if( 0 != _p_clickedCon ) {
  // REMOVE:   _toolArgs[0] = new BeamlineContextPtr( *_p_clickedCon );
  // REMOVE: }
  if( 0 != _p_currBmlCon ) {
    _toolArgs[0] = new BeamlineContextPtr( *_p_currBmlCon );
  }
  else {
    QMessageBox::information( 0, "CHEF",
                              "Left click on a beamline, idiot." );
  }
}


void CHEF::_clearArgs()
{
  for( int i = 0; i < CHEF_numargs; i++ ) {
    if( _toolArgs[i] ) { 
      delete _toolArgs[i];  // virtual destructor should work
      _toolArgs[i] = 0;
    }
  }
}


void CHEF::_launchSiteVu()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  _siteWidget = new SiteViewer( *(_p_currBmlCon) );
  // Must be deleted by itself.
  _siteWidget->setCaption( QString("CHEF:: Site Viewer")+
                           QString("    ")+ 
                           QString(_p_currBmlCon->name()) );
  _siteWidget->show();
}


void CHEF::_chromCtrl()
{
  _do_nothing();
}


void CHEF::_openFile()
{
  // Open file dialog
  QString s = QFileDialog::getOpenFileName( QString::null, 
                   "MAD (*.mad *.lat);;Beamline (*.bml);;All files (*.*)" );
  if( s.isEmpty() ) { return; }


  bool madFile = false, bmlFile = false;
  if( (QString(".mad") != s.right(4)) &&
      (QString(".lat") != s.right(4)) &&
      (QString(".bml") != s.right(4))    )
  {
    ostringstream uic;
    uic <<   "You are attempting to read a file with"
           "\nnon-standard suffix:  " << s.right(4)
        << "\nI will proceed under the assumptions that"
           "\n(a) it describes a lattice in MAD format and"
           "\n(b) you know what you are doing.";
    QMessageBox::information( 0, "CHEF: WARNING", uic.str().c_str() );
    madFile = true;
  }
  else if( QString(".mad") == s.right(4) ||
           QString(".lat") == s.right(4)    ) {
    madFile = true;
  }
  else if( QString(".bml") == s.right(4) ) {
    bmlFile = true;
  }


  if( madFile ) {
    QDialog* wpu = new QDialog( 0, 0, true );
      QVBox* qvb = new QVBox( wpu );
        QHBox* qhb1 = new QHBox( qvb );
          QLabel* qlbh = new QLabel( "Beamline name: ", qhb1 );
          QLineEdit* qleh = new QLineEdit( "", qhb1 );
        qhb1->setMargin(5);
        qhb1->setSpacing(3);
        qhb1->adjustSize();
  
        QHBox* qhb2 = new QHBox( qvb );
          QButtonGroup* qbg = new QButtonGroup( 2, Qt::Vertical, qhb2 );
            QRadioButton* kePtr 
              = new QRadioButton( "Proton kinetic energy [GeV]: ", qbg );
            QRadioButton* pcPtr 
              = new QRadioButton( "Proton momentum [GeV/c]: ", qbg );
          qbg->setExclusive( true );
          qbg->setMargin(5);
          // qbg->setSpacing(3);
          qbg->adjustSize();

          QLineEdit* qlev = new QLineEdit( "0.0", qhb2 );
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
  
    wpu->setCaption( "CHEF: beamline selector" );
    wpu->adjustSize();

    wpu->show();
    int returnCode = wpu->exec();
  
    if( returnCode == QDialog::Accepted ) {
      QString bmlName ( qleh->text() );
      QString brhoText( qlev->text() );

      double brho;
      if( kePtr == qbg->selected() ) {
        brho = fabs( atof( brhoText ) );              // = kinetic energy
        brho = sqrt( brho*(brho + 2.0*PH_NORM_mp) );  // = momentum
        brho /= PH_CNV_brho_to_p;                     // = magnetic rigidity
      }
      else if( pcPtr == qbg->selected() ) {
        brho = ( fabs( atof( brhoText ) )/PH_CNV_brho_to_p );
      }
      else {
        ostringstream uic;
        uic << "An impossibility has occurred\nin file "
            << __FILE__
            << " at line " << __LINE__ ;
        QMessageBox::information( 0, "CHEF: ERROR",
                                  uic.str().c_str() );
      }

      // bmlfactory bf( s, brho );
      // beamline* bmlPtr = bf.create_beamline( bmlName );
      // ??? The bmlfactory destructor produces a segmentation
      // ??? fault on my laptop. My temporary kludge is to
      // ??? create one on the heap and never destroy it.

      bmlfactory* bfPtr = new bmlfactory( s, brho );
      beamline* bmlPtr = bfPtr->create_beamline( bmlName );

      _p_currBmlCon = new BeamlineContext( false, bmlPtr );
      _p_currBmlCon->setClonedFlag( true );
      _contextList.insert( _p_currBmlCon );

    }
  
    delete wpu;
  }


  else if( bmlFile ) { 
    beamline* bmlPtr = new beamline;
    ifstream inputStream( s );
    inputStream >> (*bmlPtr);
    inputStream.close();

    _p_currBmlCon = new BeamlineContext( false, bmlPtr );
    _p_currBmlCon->setClonedFlag( true );
    _contextList.insert( _p_currBmlCon );
  }


  emit _new_beamline();
}


void CHEF::_fileSaveAs()
{
  // REMOVE: if( _p_clickedCon == 0 ) {
  if( _p_currBmlCon == 0 ) {
    QMessageBox::information( 0, "CHEF",
                "Left click on a beamline first." );
    return;
  }
  
  // REMOVE: QString startName(_p_clickedCon->name());
  QString startName(_p_currBmlCon->name());
  startName += ".bml";

  QString s = QFileDialog::getSaveFileName( 
                   startName,
                   "Beamline (*.bml)", 
                   0,
                   0,
                   "Save File As" );

  // REMOVE: beamline* bmlPtr = (beamline*) _p_clickedCon->cheatBmlPtr();
  beamline* bmlPtr = (beamline*) _p_currBmlCon->cheatBmlPtr();

  if( s.length() > 0 ) {
    ofstream outStream( s.latin1() );
    outStream << *bmlPtr;
    outStream.close();
  }

  // Programming note: the following does NOT work. 
  // Only the address is sent to the file.
  // 
  // if( s.length() > 0 ) {
  //   ofstream outStream( s.latin1() );
  //   outStream << *(_p_clickedCon->cheatBmlPtr());
  //   outStream.close();
  // }

}


void CHEF::_print()
{
  double s = 0.0;
  if( 0 != _p_currBmlCon ) { _p_currBmlCon->peekAt( s ); }
}


void CHEF::_fileWriteTree()
{
  if( 0 != _p_currBmlCon ) { _p_currBmlCon->writeTree(); }
}


void CHEF::_launch_browser()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF::_launch_browser",
                              "Beamline must be constructed first." );
    return;
  }
  
  _fileMenu->setItemEnabled( _id_FileWriteTree,  true);
  _fileMenu->setItemEnabled( _id_FilePrint,      true );
  _editMenu->setItemEnabled( _id_EditSelectMenu, true);
  _toolMenu->setItemEnabled( _id_analMenu,       true );
  _toolMenu->setItemEnabled( _id_ctrlMenu,       true );

  if( _p_vwr == 0 ) {
    _p_vwr = new BeamlineBrowser( _mainWindow );
    // _p_vwr = new BeamlineBrowser( 0 );
  
    _p_vwr->setCaption( "CHEF:: Beamline Browser" );
    _p_vwr->setAllColumnsShowFocus( TRUE );
    _p_vwr->show();

    connect( _p_vwr, SIGNAL(sig_bmlLeftClicked( BeamlineContext* )),
             this,   SLOT(_set_p_clickedContext( BeamlineContext* ))    );
    connect( _p_vwr, SIGNAL(sig_bmlLeftClicked( QBml* )),
             this,   SLOT(_set_p_clickedQBml( QBml* ))    );
    connect( _p_vwr, SIGNAL(sig_newContext( BeamlineContext* )),
             this,   SLOT(_slot_contextGenerated( BeamlineContext* ))    );

    // cout << "DGN: resize policy of browser = ";
    // if(_p_vwr->resizePolicy() == QScrollView::Default ) cout << "Default" << endl;
    // if(_p_vwr->resizePolicy() == QScrollView::Manual  ) cout << "Manual" << endl;
    // if(_p_vwr->resizePolicy() == QScrollView::AutoOne ) cout << "AutoOne" << endl;

    _mainWindow->setCentralWidget( _p_vwr );
    _mainWindow->adjustSize();
    _mainWindow->resize( 300, 500 );
  }

  _p_vwr->clearSelection();
  _p_vwr->displayBeamline( _p_currBmlCon );
}


void CHEF::_set_p_clickedContext( BeamlineContext* x )
{
  // REMOVE: _p_clickedCon = x;
  _p_currBmlCon = x;
}


void CHEF::_set_p_clickedQBml( QBml* x )
{
  _p_clickedQBml = x;
}


void CHEF::_do_nothing()
{
  QMessageBox::information( 0, "CHEF",
                            "Sorry. This function is not implemented." );
}


void CHEF::_helpContents()
{
  this->_do_nothing();
}


void CHEF::_helpAbout()
{
  this->_do_nothing();
}


// *****************************************
// Implementation: class CF_rbendFinder_quad
// *****************************************

CF_rbendFinder_quad::CF_rbendFinder_quad( double q )
: _integratedQuad(q)
{
}


CF_rbendFinder_quad::~CF_rbendFinder_quad()
{
}


void CF_rbendFinder_quad::setQuadrupole( double q )
{
  _integratedQuad = q;
}


bool CF_rbendFinder_quad::operator()( const bmlnElmnt* x )
{
  static bool ret;
  ret = false;

  if( 0 == strcmp( x->Type(), "CF_rbend" ) ) {
    if( _integratedQuad == ((CF_rbend*) x)->getQuadrupole() ) {
      ret = true;
    }
  }

  return ret;
}


bool CF_rbendFinder_quad::operator()( const bmlnElmnt& x )
{
  return this->operator()( &x );
}


// *****************************************
// Implementation: class RollAccumulator
// *****************************************

RollAccumulator::RollAccumulator( const char* theType, double theta )
: _deltaRoll(theta), _weak(false)
{
  _type = new char [ strlen(theType) + 1 ];
  strcpy( _type, theType );
}


RollAccumulator::~RollAccumulator()
{
  delete [] _type;
}


void RollAccumulator::setWeakType()
{
  _weak = true;
}


void RollAccumulator::setStrongType()
{
  _weak = false;
}


int RollAccumulator::operator()( bmlnElmnt* x )
{
  static int  ret;
  static bool doit;

  ret = -1;
  doit = false;

  if( _weak ) {
    doit = ( 0 != strstr( x->Type(), _type ) );
  }
  else {
    doit = ( 0 == strcmp( x->Type(), _type ) );
  }

  if( doit ) {
    alignmentData ad( x->Alignment() );
    ad.tilt += _deltaRoll;
    x->setAlignment( ad );
    ret = 0;
  }

  return ret;
}


int RollAccumulator::operator()( bmlnElmnt& x )
{
  return this->operator()( &x );
}


// ******************************************
// Implementation: class argPtr + descendants
// ******************************************

argPtr::argPtr( void* x, bool y )
: _owned(y)
{
  if(x) { _ptr = x; }
  else {
    cerr << "\n*** ERROR *** "
            "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** argPtr::argPtr( void* x )"
            "\n*** ERROR *** Constructor does not accept null argument."
	 << endl;
    _ptr = this;
  }
};

argPtr::~argPtr()
{
}

const void* argPtr::ptr() const
{
  return _ptr;
}


bool argPtr::isOwned() const
{
  return _owned;
}


MappingPtr::MappingPtr( const Mapping& x, bool y ) 
: argPtr( new Mapping(x), true )
{
  if( !y ) {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** MappingPtr::MappingPtr( const Mapping& x, bool y ) "
            "\n*** WARNING *** Second argument is being ignored."
         << endl;
  }
}


MappingPtr::~MappingPtr() 
{
  if( _owned ) { delete ((Mapping*) _ptr); }
}


BeamlineContextPtr::BeamlineContextPtr( const BeamlineContext& x, bool y ) 
: argPtr( ((void*) &x), false )
{
  if( y ) {
    cerr << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** BeamlineContextPtr::BeamlineContextPtr( const BeamlineContext& x, bool y ) "
            "\n*** WARNING *** Second argument is being ignored."
         << endl;
  }
}


BeamlineContextPtr::~BeamlineContextPtr() 
{
  if( _owned ) { delete ((BeamlineContext*) _ptr); }
}


