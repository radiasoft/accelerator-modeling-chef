/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******         interfaces to exercise the functionality        
******         of BEAMLINE.                                    
******                                                                
******  File:      CHEF.cc
******  Version:   3.5
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
#include <sstream>
#include <typeinfo>
// #include <process.h>

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qptrlist.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>
// #include <qprocess.h>

#include <qwt/qwt_plot.h>

#include "GenericException.h"
#include "beamline.h"
#include "InsertionList.h"
#include "BeamlineContext.h"
#include "BeamlineExpressionTree.h"
#include "EditDialog.h"
#include "FramePusher.h"
#include "SurveyMatcher.h"
#include "DriftEliminator.h"
#include "QuadEliminator.h"
#include "bmlfactory.h"
#include "BeamlineBrowser.h"
#include "BmlUtil.h"

// #include "LattFncPlt.h"
// #include "ETFncPlt.h"
// #include "MomentsFncPlt.h"
// #include "LBFncPlt.h"
// #include "DspnFncPlt.h"

#include "chefplotmain.h"
#include "LattFncData.h"
#include "ETFncData.h"
#include "MomentsFncData.h"
#include "LBFncData.h"
#include "DspnFncData.h"

#include "Tracker.h"
#include "RayTrace.h"
#include "SiteViewer.h"
#include "QtMonitor.h"
#include "QueryDialog.h"
#include "ModifierVisitor.h"
#include "InitCondDialogLF.h"
#include "DistributionWidget.h"

// This undef is needed because of the compiler.
// #undef connect

#include "CHEF.h"

extern beamline* DriftsToSlots( beamline& original );



// 
// Constructors and destructor
// 


CHEF::CHEF( beamline* xbml, int argc, char** argv )
: _p_vwr(0), _plotWidget(0), 
  _ETplotWidget(0), _MMplotWidget(0), _LBplotWidget(0), _DspnplotWidget(0)
{
  int i;
  for( i = 0; i < CHEF_numargs; i++ )
  {  _toolArgs[i] = 0;
  }

  _userOptions.couplingOption = Options::ignore_coupling;


  // ??? !!! This needs to be fixed. !!! ???
  if( 0 != xbml ) 
  { // This is not good: by default, a proton machine is assumed.
    Proton reggie( xbml->Energy() );
    _p_currBmlCon = new BeamlineContext( reggie, xbml, true );
    _contextList.append( _p_currBmlCon );
  }
  else 
  { _p_currBmlCon  = 0;
  }
  // REMOVE: _p_clickedCon = _p_currBmlCon;
  _p_currQBmlRoot = 0;
  _p_clickedQBml = 0;


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
    _id_EditSelectMenu =
    _editMenu->insertItem( "Select", _edit_selectMenu );
    _editMenu->setItemEnabled( _id_EditSelectMenu, false);

    _editMenu->insertSeparator();
    _editMenu->insertItem( "Mode...", this, SLOT(_editMode()) );
    _editMenu->insertItem( "Remove", this, SLOT(_editRemoveLine()) );
    _editMenu->insertItem( "Duplicate", this, SLOT(_editCopyLine()) );
    _editMenu->insertItem( "Condense", this, SLOT(_editCondense()) );
    _editMenu->insertItem( "Flatten", this, SLOT(_editFlatten()) );
    _editMenu->insertItem( "Permute", this, SLOT(_editNewOrder()) );

    _editMenu->insertSeparator();
    _editMenu->insertItem( "Modify ...", this, SLOT(_editEditElement()) );

    _editMenu->insertSeparator();
    _editMenu->insertItem( "Insert markers", this, SLOT(_editAddMarkers()) );
    _editMenu->insertItem( "Insert monitors", this, SLOT(_editAddQtMons()) );

    _editMenu->insertSeparator();
    _editMenu->insertItem( "Merge equivalent quads", this, SLOT(_editMergeQuads()) );
    _editMenu->insertItem( "Convert to Slots", this, SLOT(_editD2S()) );
    _editMenu->insertItem( "Partition", this, SLOT(_editPartition()) );
    int id_PartSect = 
    _editMenu->insertItem( "Partition and sectorize", this, SLOT(_editPartAndSect()) );
    _editMenu->setItemEnabled( id_PartSect, false );

    _editMenu->insertSeparator();
    _editMenu->insertItem( "Align ...", this, SLOT(_editAlign()) );
    _editMenu->insertItem( "Align to data ...", this, SLOT(_editAlignData()) );
    _editMenu->insertItem( "Align CF_rbends...", this, SLOT(_editAlignBends()) );
  _mainMenu->insertItem( "Edit", _editMenu );

    _calcsMenu = new QPopupMenu;
       // TBC:  _calcEnterMenu = new QPopupMenu;
       // TBC:  _calcEnterMenu->insertItem( "Line", this, SLOT(_enterContextArg()) );
       // TBC:  _calcEnterMenu->insertItem( "Map", this, SLOT(_enterMapArg()) );
       // TBC:  _calcEnterMenu->insertSeparator();
       // TBC:  _calcEnterMenu->insertItem( "Clear", this, SLOT(_clearArgs()) );
    // TBC:  _calcsMenu->insertItem( "Enter...", _calcEnterMenu);

      _calcLattFuncMenu = new QPopupMenu;
      _calcLattFuncMenu->insertItem( "Uncoupled", this, SLOT(_launchLatt()) );
      _calcLattFuncMenu->insertItem( "Factorization", this, SLOT(_launchET()) );
      _calcLattFuncMenu->insertItem( "Eigenvectors", this, SLOT(_launchLB()) );
      _calcLattFuncMenu->insertItem( "Moments", this, SLOT(_launchMoments()) );
    _calcsMenu->insertItem( "Lattice Functions", _calcLattFuncMenu );

      _calcPushMenu = new QPopupMenu;
      _calcPushMenu->insertItem( "Uncoupled Lattice Functions", this, SLOT(_pushULF()) );
      _calcPushMenu->insertItem( "Moments", this, SLOT(_pushMoments()) );
      _calcPushMenu->insertItem( "Dispersion", this, SLOT(_pushDispersion()) );
    _calcsMenu->insertItem( "Push", _calcPushMenu );

    _calcsMenu->insertItem( "Dispersion", this, SLOT(_launchDispersion()) );
    _calcsMenu->insertItem( "Emittance Dilution", this, SLOT(_launchDilution()) );
  _mainMenu->insertItem( "Calculations", _calcsMenu );

    _ctrlMenu = new QPopupMenu;
    _ctrlMenu->insertItem( "Build hor. tune circuit", this, SLOT(_horTuneCtrl()) );
    _ctrlMenu->insertItem( "Build ver. tune circuit", this, SLOT(_verTuneCtrl()) );
    _ctrlMenu->insertItem( "Adjust tune", this, SLOT(_tuneCtrl()) );
    _ctrlMenu->insertSeparator();
      int id_chrom = 
    _ctrlMenu->insertItem( "Chromaticity", this, SLOT(_chromCtrl()) );
      _ctrlMenu->setItemEnabled( id_chrom, false );
    _ctrlMenu->insertSeparator();
    _ctrlMenu->insertItem( "Misalign all...",    this, SLOT(_toolMisalign()) );
  _mainMenu->insertItem( "Controls", _ctrlMenu );

    _dsgnMenu = new QPopupMenu;
    int dsgnMenuID = 
  _mainMenu->insertItem( "Design", _dsgnMenu );
  _mainMenu->setItemEnabled( dsgnMenuID, false );

    _toolMenu = new QPopupMenu;
    _toolMenu->insertItem( "Generate Bunch", this, SLOT(_pushParticles()) );
    _toolMenu->insertItem( "Site Viewer", this, SLOT(_launchSiteVu()) );
    _toolMenu->insertItem( "Track", this, SLOT(_launchTrack()) );
    _toolMenu->insertItem( "Trace", this, SLOT(_launchRayTrace()) );
    // NEW: *** START HERE ***
    // NEW: _toolMenu->insertItem( "Register Reference Proton", this, SLOT(_register()) );
  _mainMenu->insertItem( "Tools", _toolMenu );

    _mach_imagMenu = new QPopupMenu;
    _mach_imagMenu->insertItem( "FODO Cells", this, SLOT(_makeFODO()) );
    _mach_imagMenu->insertItem( "Single Sextupole", this, SLOT(_makeSingSext()) );
  _mainMenu->insertItem( "Examples", _mach_imagMenu );

#if 0
    For some reason, this shows up to the right of the help menu item.
    QLabel*  spacerPtr = new QLabel( "SPACER", _mainMenu );
    // QWidget* spacerPtr = new QWidget( _mainMenu );
    spacerPtr->setFocusPolicy( QWidget::NoFocus );
    spacerPtr->setSizePolicy( QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed) );
  _mainMenu->insertItem( spacerPtr );
#endif

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
    std::ostringstream uic;
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
  QueryDialog* qdl = new QueryDialog( 0, 0, false, Qt::WDestructiveClose );
  connect( qdl,  SIGNAL(_useThis( const BoolNode& )), 
           this, SLOT  (_processFilter( const BoolNode& )) );
  qdl->show();
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
  _p_currBmlCon   = 0;
  _p_currQBmlRoot = 0;
  _p_clickedQBml  = 0;
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
  if( 0 != _p_currBmlCon ) { 
    _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                         _p_currBmlCon->cloneBeamline(), 
                                         false );
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
    delete _p_currBmlCon;
    _p_currBmlCon = 0;
    _p_currQBmlRoot = 0;
  }
  else 
  { QMessageBox::information( 0, "CHEF: ERROR",
                              "Operation not successful." );
  }
}


void CHEF::_horTuneCtrl()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( _p_currBmlCon->isTreatedAsRing() ) {
    _testFC( CHEF::_buildHTuneCircuit );
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }
}


void CHEF::_verTuneCtrl()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( _p_currBmlCon->isTreatedAsRing() ) {
    _testFC( CHEF::_buildVTuneCircuit );
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }
}


void CHEF::_tuneCtrl()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }
  // --------------------------------------------------------------
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
      std::ostringstream uic;
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
      std::ostringstream uic;
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


void CHEF::_editAlignBends()
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

      // REMOVE: double inState [] = { 0., 0., 0., 0., 0., 0. };
      // REMOVE: inState[ p.npxIndex() ]
      // REMOVE:             =  ( p.Momentum()/p.ReferenceMomentum() )
      // REMOVE:                * sin( prototype->getPoleFaceAngle() / 2.0 );
      Particle* pPtr = (_p_currBmlCon->getParticle()).Clone();
      pPtr->SetReferenceEnergy( _p_currBmlCon->getEnergy() );
        // This line is almost certainly unnecessary.
      pPtr->setStateToZero();
      pPtr->set_npx( ( pPtr->Momentum()/pPtr->ReferenceMomentum() )
                     * sin( prototype->getPoleFaceAngle() ) );

      double delta = ( prototype->AdjustPosition(*pPtr) );

      delete pPtr; pPtr = 0;

      // Dialog: permission to realign the CF_rbends
      QDialog* wpu = new QDialog( 0, 0, true );
        QVBox* qvb = new QVBox( wpu );
          QString str( "Similar rbends will be moved " );
          QString str2;
          str2.setNum(1000.0*delta);
          str += str2;
          str += " mm.";
          /////QLabel* qlb = new QLabel( str, qvb );

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


// REMOVE: void CHEF::_toolAlignBends()
// REMOVE: {
// REMOVE:   // This menu function changes the beamline in place
// REMOVE:   // without creating a new one.
// REMOVE: 
// REMOVE:   const bmlnElmnt* selected = _p_vwr->getSelectedElement(_p_currBmlCon);
// REMOVE:   if( 0 == selected ) 
// REMOVE:   {
// REMOVE:     QMessageBox::information( 0, "CHEF: INFO",
// REMOVE:                               "You must select a prototype CF_rbend in the browser." );
// REMOVE:   }
// REMOVE: 
// REMOVE:   else 
// REMOVE:   {
// REMOVE:     if( typeid(*selected) == typeid(CF_rbend) ) 
// REMOVE:     {
// REMOVE:       // The following was modeled on rrv19.cfrbend.cfg
// REMOVE:       CF_rbend* prototype = (CF_rbend*) selected->Clone();
// REMOVE: 
// REMOVE:       Proton p( _p_currBmlCon->getEnergy() );
// REMOVE:       double inState [] = { 0., 0., 0., 0., 0., 0. };
// REMOVE:       // inState[ p.npxIndex() ] 
// REMOVE:       //             =  ( p.Momentum()/p.ReferenceMomentum() )
// REMOVE:       //                * sin( prototype->getPoleFaceAngle() / 2.0 );
// REMOVE:       inState[ p.npxIndex() ] 
// REMOVE:                   =  ( p.Momentum()/p.ReferenceMomentum() )
// REMOVE:                      * sin( prototype->getPoleFaceAngle() );
// REMOVE:       p.setState( inState );
// REMOVE: 
// REMOVE:       double delta = ( prototype->AdjustPosition(p) );
// REMOVE: 
// REMOVE:       // Dialog: permission to realign the CF_rbends
// REMOVE:       QDialog* wpu = new QDialog( 0, 0, true );
// REMOVE:         QVBox* qvb = new QVBox( wpu );
// REMOVE:           QString str( "Similar rbends will be moved " );
// REMOVE:           QString str2;
// REMOVE:           str2.setNum(1000.0*delta);
// REMOVE:           str += str2;
// REMOVE:           str += " mm.";
// REMOVE:           QLabel* qlb = new QLabel( str, qvb );
// REMOVE: 
// REMOVE:           QHBox* qhb2 = new QHBox( qvb );
// REMOVE:             QPushButton* okayBtn = new QPushButton( "Proceed", qhb2 );
// REMOVE:               okayBtn->setDefault( true );
// REMOVE:               connect( okayBtn, SIGNAL(pressed()),
// REMOVE:                        wpu,     SLOT(accept()) );
// REMOVE:             QPushButton* cancelBtn = new QPushButton( "Cancel", qhb2 );
// REMOVE:               connect( cancelBtn, SIGNAL(pressed()),
// REMOVE:                        wpu,       SLOT(reject()) );
// REMOVE:           qhb2->setMargin(5);
// REMOVE:           qhb2->setSpacing(3);
// REMOVE:           qhb2->adjustSize();
// REMOVE:     
// REMOVE:         qvb->adjustSize();
// REMOVE:     
// REMOVE:       wpu->setCaption( "CHEF: CF_rbend Alignment" );
// REMOVE:       wpu->adjustSize();
// REMOVE:     
// REMOVE:       int returnCode = wpu->exec();
// REMOVE: 
// REMOVE:       // Realign bends
// REMOVE:       if( returnCode == QDialog::Accepted ) 
// REMOVE:       {
// REMOVE:         CF_rbendFinder_quad cf( prototype->getQuadrupole() );
// REMOVE:         _p_currBmlCon->setAlignment( cf, prototype->Alignment() );
// REMOVE:       }
// REMOVE: 
// REMOVE:       delete wpu;
// REMOVE:       delete prototype;
// REMOVE:     }
// REMOVE: 
// REMOVE:     else 
// REMOVE:     {
// REMOVE:       QMessageBox::information( 0, "CHEF: INFO",
// REMOVE:                                 "Selected element is not CF_rbend." );
// REMOVE:     }
// REMOVE: 
// REMOVE:   }
// REMOVE: }


void CHEF::_editEditElement()
{
  // The code in this method was patterned after that
  // in CHEF::_editNewOrder()

  // One quick test ...
  if( 0 == _p_clickedQBml ) {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "A single beamline element must be chosen first." );
    return;
  }

  // Locate the element and its root beamline
  QBmlElmt* qbmlElPtr = dynamic_cast<QBmlElmt*>(_p_clickedQBml);

  // This test should be made more flexible after
  //   allowing non-flat beamlines to be processed.
  if( 0 == qbmlElPtr ) {
    // Allow modifying the beamline only
    if( 0 != _p_currBmlCon ) {
      BeamlineContext* contextPtr = _p_currBmlCon;
      if( 0 == (_p_vwr->removeBeamline( contextPtr )) ) {
        editDialog edg;
        edg._contextPtr = contextPtr;
        beamline* bmlPtr = const_cast<beamline*>(_p_currBmlCon->cheatBmlPtr());
        bmlPtr->accept(edg);
        contextPtr->reset();
        emit _new_beamline();
        return;
      }
    }
    else {
      std::ostringstream uic;
      uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
             "\nIn function void CHEF::_editNewOrder():"
             "\nFailure: beamline element not chosen correctly.";
             "\nOperation will abort.";
      QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
      return;
    }
  }
  const bmlnElmnt* elmntPtr = qbmlElPtr->cheatElementPtr();
  QBmlRoot* theRoot = const_cast<QBmlRoot*>(_p_clickedQBml->topBmlParent());

  // Invoke this slot to reset the current settings.
  BeamlineContext* contextPtr = const_cast<BeamlineContext*>(theRoot->cheatContextPtr());
  _set_p_clickedContext( contextPtr, theRoot );
  beamline* bmlPtr = const_cast<beamline*>(_p_currBmlCon->cheatBmlPtr());

  // This restriction should be removed ... but it isn't:
  //   a test for a flat beamline.
  int bmlLevel = bmlPtr->depth();
  if( bmlLevel != 0 ) {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "SORRY: Current implementation requires flat beamline." );
    return;
  }

  // Finally, do the editing with the help of 
  // and editDialog visitor.
  if( 0 == (_p_vwr->removeBeamline( contextPtr )) ) {
    editDialog edg;
    edg._contextPtr = contextPtr;
    const_cast<bmlnElmnt*>(elmntPtr)->accept(edg);
    contextPtr->reset();
    emit _new_beamline();
  }
  else { 
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function void CHEF::_editNewOrder():"
           "\nFailure: Unable to remove old beamline."
           "\nOperation will abort.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }
}


void CHEF::_editAlign()
{
  bool handleSpace   = true;
  bool handleSbend   = true;

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
      //   Weed out undesirable elements; this probably should
      //   be done by AlignVisitor itself.
      BmlPtrList theOnes;
      bmlnElmnt* elementPtr;
      while( 0 != (elementPtr = theChosenOnes.take()) ) {
        if( BmlUtil::isSpace(elementPtr) ) {
          if(handleSpace) {
            QMessageBox::warning( 0, "CHEF: WARNING", 
              "This function will not align empty space."
              "\nSend complaints to: michelotti@fnal.gov" );
            handleSpace = false;
          }
        }
        else if(    ( 0 == strcasecmp("sbend",elementPtr->Type())    ) 
                 || ( 0 == strcasecmp("CF_sbend",elementPtr->Type()) ) ) {
          if(handleSbend) {
            QMessageBox::warning( 0, "CHEF: WARNING", 
              "This function handles only transverse movements."
              "\nIt is not written to manipulate sector bends."
              "\nSend complaints to: michelotti@fnal.gov" );
            handleSbend = false;
          }
        }
        else {
          theOnes.append(elementPtr);
        }
      }

      // Do the alignment
      AlignVisitor euclid( euclidData, theOnes );
      // REMOVE: _p_clickedCon->accept( euclid );
      if( 0 != _p_currBmlCon) { _p_currBmlCon->accept( euclid ); }
    }
  }
}


void CHEF::_editAlignData()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
    return;
  }

  // Store survey data read from a file.
  QString s = QFileDialog::getOpenFileName( QString::null, 
                   "Data file (*.dat *.txt);;All files (*.*)" );
  if( s.isEmpty() ) { return; }
  std::ifstream dataStream( s );


  struct DataStore {
    double az;
    double x1;
    double x2;
    double x3;
  } oneSite;
  slist dataBag;
  DataStore* dtmPtr;

  char oneLine[1024];
  if( dataStream ) {
    dataStream.getline(oneLine,1000);
    while( !dataStream.eof() ) {
      std::istringstream lineStream( oneLine );
      dtmPtr = new DataStore;
      lineStream >> dtmPtr->az >> dtmPtr->x1 >> dtmPtr->x2 >> dtmPtr->x3;
      dataBag.append(dtmPtr);
      dataStream.getline(oneLine,1000);
    }
    dataStream.close();
  }
  else {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "Survey data file could not be opened." );
    return;
  }

  int n = dataBag.size();


  // Invoke a SurveyMatcher object
  beamline* bmlPtr = (beamline*)(_p_currBmlCon->cheatBmlPtr());
  Vector r(3);
  std::vector<Vector> rawData;
  slist_iterator getNext(dataBag);
  while((  0 != (dtmPtr = (DataStore*)getNext() )  )) {
    r(0) = dtmPtr->x1;
    r(1) = dtmPtr->x2;
    r(2) = dtmPtr->x3;
    rawData.push_back(r);
  }

  SurveyMatcher sm( rawData, bmlPtr );
  


  // Preparing the plot
  // Load the shared arrays
  boost::shared_array<double> azimuthArray( new double[n] );
  boost::shared_array<double>       xArray( new double[n] );
  boost::shared_array<double>       yArray( new double[n] );
  boost::shared_array<double>       zArray( new double[n] );

  bool firstPass = true;
  for( int i = 0; i < n; i++ ) {
    dtmPtr = (DataStore*) dataBag.get();
    if( 0 != dtmPtr ) {
      azimuthArray[i] = dtmPtr->az;
      r = sm.getLocalDisplacement(i);
      xArray[i]       = r(0);
      yArray[i]       = r(1);
      zArray[i]       = r(2);
      delete dtmPtr;
    }
    else {
      if( firstPass ) {
        std::ostringstream uic;
        uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
               "\nIn function void CHEF::_editAlignData()"
               "\nImpossible condition occurred: not enough data."
               "\nOperation will abort.";
        QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
        return;
      }
    }
  }

  if( 0 != dataBag.get() ) {
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function void CHEF::_editAlignData()"
           "\nImpossible condition occurred: too much data."
           "\nOperation will abort.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }


  boost::shared_ptr<CHEFCurve> 
    dxCurve( new CHEFCurve( CurveData(azimuthArray,xArray,n), "dx" ));
  boost::shared_ptr<CHEFCurve> 
    dyCurve( new CHEFCurve( CurveData(azimuthArray,yArray,n), "dy" ));
  boost::shared_ptr<CHEFCurve> 
    dzCurve( new CHEFCurve( CurveData(azimuthArray,zArray,n), "dz" ));

  dxCurve->setPen( QPen( "black", 1, Qt::SolidLine ) );
  dyCurve->setPen( QPen( "red", 1, Qt::SolidLine ) );
  dzCurve->setPen( QPen( "green",  1, Qt::SolidLine ) );

  dxCurve->setAxis( QwtPlot::xBottom, QwtPlot::yLeft );
  dyCurve->setAxis( QwtPlot::xBottom, QwtPlot::yLeft );
  dzCurve->setAxis( QwtPlot::xBottom, QwtPlot::Right );


  // Create a CHEFPlotData object for handing over to the plotter
  CHEFPlotData plotData;
  plotData.addCurve(dxCurve);
  plotData.addCurve(dyCurve);
  plotData.addCurve(dzCurve);
  plotData.setXLabel( "Arc Length [m]"              );
  plotData.setYLabel( "Data",       QwtPlot::yLeft  );
  plotData.setBeamline( _p_currBmlCon->cheatBmlPtr(), false );
  // false = do not clone line   


  // Plot and display the results
  CHEFPlotMain* surveyPlotter
    = new CHEFPlotMain( 0, "Survey Plotter", Qt::WDestructiveClose );
  surveyPlotter->setCaption( "CR&P" );
  surveyPlotter->addData( plotData );
  surveyPlotter->show();

  #if 0
  DataAlignWidget* dawPtr 
    = new DataAlignWidget( *(_p_currBmlCon), 0, 0, Qt::WDestructiveClose );
  dawPtr->show();
  #endif
}


void CHEF::_editCondense()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
    return;
  }

  DriftEliminator de;
  _p_currBmlCon->accept(de);
  // if( !(_contextList.contains(_p_currBmlCon)) ) 
  // { _contextList.insert( _p_currBmlCon );
  // }


  beamline* bmlPtr = de.clonedBeamlinePtr();
  _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                       bmlPtr, false );
  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_editFlatten()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
    return;
  }

  // Create a cloned, flattened beamline
  beamline* tempPtr = _p_currBmlCon->cheatBmlPtr()->flatten();
  beamline* bmlPtr  = dynamic_cast<beamline*>(tempPtr->Clone());
    // This step is necessary,
    //   because the same objects exist both
    //   the original and flattened beamlines.
  delete tempPtr;  
    // Does not delete beamline elements.

  _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                       bmlPtr, false );

  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_editNewOrder()
{
  // One quick test ...
  if( 0 == _p_clickedQBml ) {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "A single beamline element must be chosen first." );
    return;
  }

  // Locate the element and its root beamline
  QBmlElmt* qbmlElPtr = dynamic_cast<QBmlElmt*>(_p_clickedQBml);

  // This test should be made more flexible after
  //   allowing non-flat beamlines to be processed.
  if( 0 == qbmlElPtr ) {
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function void CHEF::_editNewOrder():"
           "\nFailure: beamline element not chosen correctly.";
           "\nOperation will abort.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }
  const bmlnElmnt* elmntPtr = qbmlElPtr->cheatElementPtr();
  QBmlRoot* theRoot = const_cast<QBmlRoot*>(_p_clickedQBml->topBmlParent());

  // Invoke this slot to reset the current settings.
  BeamlineContext* contextPtr = const_cast<BeamlineContext*>(theRoot->cheatContextPtr());
  _set_p_clickedContext( contextPtr, theRoot );
  beamline* bmlPtr = const_cast<beamline*>(_p_currBmlCon->cheatBmlPtr());

  // This restriction should be removed ... but it isn't:
  //   a test for a flat beamline.
  int bmlLevel = bmlPtr->depth();
  if( bmlLevel != 0 ) {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "SORRY: Current implementation requires flat beamline." );
    return;
  }

  // Do the operation.
  if( 0 == (_p_vwr->removeBeamline( contextPtr )) ) {
    bmlPtr->startAt(elmntPtr);
    contextPtr->reset();
    emit _new_beamline();
  }
  else { 
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function void CHEF::_editNewOrder():"
           "\nFailure: Unable to remove old beamline."
           "\nOperation will abort.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }
}


void CHEF::_editMergeQuads()
{
  QuadEliminator qe;
  _p_currBmlCon->accept(qe);

  beamline* bmlPtr = qe.clonedBeamlinePtr();
  _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                       bmlPtr, false );

  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEF::_editD2S() 
{
  beamline* bmlPtr = DriftsToSlots( (beamline&) *(_p_currBmlCon->cheatBmlPtr()) );
  if( bmlPtr != _p_currBmlCon->cheatBmlPtr() ) {
    _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                         bmlPtr, false );
    _p_currBmlCon->setClonedFlag( true );
    _contextList.insert( _p_currBmlCon );
    emit _new_beamline();
  }
  else {
    QMessageBox::warning( 0
                          , "CHEF: WARNING"
                          , "Operation not successful. See error message."
                          ,  QMessageBox::Ok
                          ,  QMessageBox::NoButton
                          ,  QMessageBox::NoButton
                        );
  }
}


CHEF::insDlgData CHEF::_insertionDialog() const
{
  // Dialog to determine where to insert new elements.
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Name: ", qhb1 );
        QLineEdit* qle = new QLineEdit( "NEW", qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QButtonGroup* qbg =
      new QButtonGroup( 3, Qt::Horizontal, QString("Misalign"), qvb );
        QRadioButton* upPtr   = new QRadioButton( "Upstream",   qbg );
        QRadioButton* midPtr  = new QRadioButton( "Middle",     qbg );
        QRadioButton* downPtr = new QRadioButton( "Downstream", qbg );
        upPtr->setChecked( true );
      qbg->setExclusive( true );
      qbg->setMargin(5);
      qbg->adjustSize();

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

  wpu->setCaption( "CHEF: Marker Insertion" );
  wpu->adjustSize();

  // Execute dialog and continue
  int returnCode = wpu->exec();

  // Package and return dialog choices
  CHEF::insDlgData ret;
  if( returnCode != QDialog::Accepted ) {
    ret.accepted   = false;
    ret.upstream   = false;
    ret.downstream = false;
  }
  else {
    // Preliminaries
    // DATUM: QString markerName( qle->text() );
    ret.accepted   = true;
    ret.namePrefix = qle->text();
    ret.upstream   = ( upPtr   == qbg->selected() );
    ret.downstream = ( downPtr == qbg->selected() );
  }

  delete wpu;
  return ret;
}


void CHEF::_editAddMarkers()
{
  insDlgData options;
  options = _insertionDialog();

  if( options.accepted ) {
    // Preliminaries
    QString markerName( options.namePrefix );

    // Locate the root beamline
    QBmlRoot* theRoot = dynamic_cast<QBmlRoot*>(_p_clickedQBml);
    if( 0 == theRoot ) {
      theRoot = const_cast<QBmlRoot*>(_p_clickedQBml->topBmlParent());
    }

    // Find selected elements
    QPtrList<bmlnElmnt> theChosenOnes;
    theChosenOnes = _p_vwr->findAllSelected( theRoot );
    if( theChosenOnes.isEmpty() ) {
      std::ostringstream uic;
      uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
             "\nIn function: void CHEF::_editAddMarkers():"
             "\nNo elements selected; no action will be taken.";
      QMessageBox::warning( 0, "CHEF: WARNING", uic.str().c_str() );
      return;
    }

    // Load BmlPtrLists, for use by lower level tools
    BmlPtrList theOnes;
    BmlPtrList insertions;
    bmlnElmnt* elementPtr;
    while( 0 != (elementPtr = theChosenOnes.take()) ) {
      theOnes.append(elementPtr);
      insertions.append( new marker(markerName.ascii()) );
    }

    // Create the new beamline with inserted markers included
    const beamline* oldbmlPtr = _p_currBmlCon->cheatBmlPtr();
    beamline* bmlPtr = 0;

    if( options.upstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.0, insertions, theOnes, oldbmlPtr );
    }
    else if( options.downstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 1.0, insertions, theOnes, oldbmlPtr );
    }
    else {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.5, insertions, theOnes, oldbmlPtr );
    }

    QString newName( oldbmlPtr->Name() );
    newName += ".marked";
    bmlPtr->Rename( newName.ascii() );
    bmlPtr->setEnergy( oldbmlPtr->Energy() );  // Probably unnecessary.


    // Generate a new BeamlineContext to handle the new line
    bool makeRing = _p_currBmlCon->isTreatedAsRing();
    _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                         bmlPtr, false );
    _p_currBmlCon->setClonedFlag( true );
    if( makeRing ) { _p_currBmlCon->handleAsRing(); }
    else           { _p_currBmlCon->handleAsLine(); }
    _contextList.insert( _p_currBmlCon );

    // Finished!
    emit _new_beamline();
  }
}


void CHEF::_editAddQtMons()
{
  insDlgData options;
  options = _insertionDialog();

  if( options.accepted ) {
    QString markerName( options.namePrefix );

    // Locate the root beamline
    QBmlRoot* theRoot = dynamic_cast<QBmlRoot*>(_p_clickedQBml);
    if( 0 == theRoot ) {
      theRoot = const_cast<QBmlRoot*>(_p_clickedQBml->topBmlParent());
    }

    // Find selected elements
    QPtrList<bmlnElmnt> theChosenOnes;
    theChosenOnes = _p_vwr->findAllSelected( theRoot );
    if( theChosenOnes.isEmpty() ) {
      std::ostringstream uic;
      uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
             "\nIn function: void CHEF::_editAddQtMons():"
             "\nNo elements selected; no action will be taken.";
      QMessageBox::information( 0, "CHEF: WARNING", uic.str().c_str() );
      return;
    }

    // Load BmlPtrLists, for use by lower level tools
    BmlPtrList theOnes;
    BmlPtrList insertions;
    bmlnElmnt* elementPtr;
    QtMonitor* qtmPtr = 0;
    while( 0 != (elementPtr = theChosenOnes.take()) ) {
      qtmPtr = new QtMonitor(markerName.ascii());
      qtmPtr->setStrength(5.0);  // to survive condensation
      theOnes.append(elementPtr);
      insertions.append( qtmPtr );
    }

    // Create the new beamline with inserted monitors included
    const beamline* oldbmlPtr = _p_currBmlCon->cheatBmlPtr();
    beamline* bmlPtr = 0;

    if( options.upstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.0, insertions, theOnes, oldbmlPtr );
    }
    else if( options.downstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 1.0, insertions, theOnes, oldbmlPtr );
    }
    else {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.5, insertions, theOnes, oldbmlPtr );
    }

    // Register the azimuth at all QtMonitor locations.
    QtMonitor::setAzimuth( bmlPtr );

    // Final steps
    QString newName( oldbmlPtr->Name() );
    newName += ".monitored";
    bmlPtr->Rename( newName.ascii() );
    bmlPtr->setEnergy( oldbmlPtr->Energy() );  // Probably unnecessary.

    // Generate a new BeamlineContext to handle the new line
    bool makeRing = _p_currBmlCon->isTreatedAsRing();
    _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                         bmlPtr, false );
    _p_currBmlCon->setClonedFlag( true );
    if( makeRing ) { _p_currBmlCon->handleAsRing(); }
    else           { _p_currBmlCon->handleAsLine(); }
    _contextList.insert( _p_currBmlCon );

    // Signal the new line
    emit _new_beamline();
  }
}


void CHEF::_editMode()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  // Set up the query
  bool makeRing = false;
  QString message(_p_currBmlCon->name());

  if( _p_currBmlCon->isRing() ) {
    if( _p_currBmlCon->isTreatedAsRing() ) {
      message +=   " is closed and is currently considered"
                 "\n a periodic structure."
                 "\n Do you want to treat it only as a beamline?";
      makeRing = false;
    }
    else {
      message +=   " is closed but is handled only as a beamline."
                   "\n Do you want to recognize its periodicity again?";
      makeRing = true;
    }
  }
  else {
    if( _p_currBmlCon->isTreatedAsRing() ) {
      message +=   " is not closed but is considered periodic."
                   "\nDo you want to treat it only as a beamline?";
      makeRing = false;
    }
    else {
      message +=   " is not closed and is handled as a beamline."
                   "\nDo you want to consider it periodic?";
      makeRing = true;
    }
  }


  // Create the dialog
  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QLabel* qlb = new QLabel( message, qvb );
      qlb->setMargin(5);
      QHBox* qhb = new QHBox( qvb );
        QPushButton* yesBtn = new QPushButton( "Yes", qhb );
          yesBtn->setDefault( true );
          connect( yesBtn, SIGNAL(pressed()),
                   wpu,    SLOT(accept()) );
        QPushButton* noBtn = new QPushButton( "No", qhb );
          connect( noBtn, SIGNAL(pressed()),
                   wpu,   SLOT(reject()) );
      qhb->setMargin(5);
      qhb->setSpacing(3);
      qhb->adjustSize();
    qvb->adjustSize();
  wpu->adjustSize();
  wpu->setCaption( "CHEF: Line/Ring Mode" );


  // Process the dialog
  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    _p_currBmlCon->reset();
    // "Yes" answer indicates change of mode.

    if( makeRing ) {
      _p_currBmlCon->handleAsRing();
    }
    else {
      _p_currBmlCon->handleAsLine();
    }

    emit _modeChanged( _p_currBmlCon );
  }

  delete wpu;
}


void CHEF::_editPartition()
{
  // This is a stripped down version.
  // Code to be entered like that in CHEF::_editAddMarkers()

  // Locate the root beamline
  QBmlRoot* theRoot = dynamic_cast<QBmlRoot*>(_p_clickedQBml);
  if( 0 == theRoot ) {
    theRoot = const_cast<QBmlRoot*>(_p_clickedQBml->topBmlParent());
  }

  // Find selected elements
  QPtrList<bmlnElmnt> theChosenOnes;
  theChosenOnes = _p_vwr->findAllSelected( theRoot );
  if( theChosenOnes.isEmpty() ) {
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function: void CHEF::_editPartition():"
           "\nSelection list is empty; no action taken.";
    QMessageBox::information( 0, "CHEF: WARNING", uic.str().c_str() );
    return;
  }

  // Convert to a BmlPtrList, for use by lower level tools
  BmlPtrList theOnes;
  bmlnElmnt* elementPtr;
  while( 0 != (elementPtr = theChosenOnes.take()) ) {
    theOnes.append(elementPtr);
  }

  // Create the new beamline with split elements
  const beamline* oldbmlPtr = _p_currBmlCon->cheatBmlPtr();
  QString newName( oldbmlPtr->Name() );
  newName += ".split";
  beamline* bmlPtr = new beamline( newName.ascii() );
  bmlPtr->setEnergy( oldbmlPtr->Energy() );

  DeepBeamlineIterator dbi( oldbmlPtr );
  bmlnElmnt* qb  = dbi++;
  bmlnElmnt* qa  = theOnes.get();
  bmlnElmnt* spa = 0;
  bmlnElmnt* spb = 0;

  // Split selected elements
  while( (0 != qa) && ( 0 != qb ) ) {
    if( qa == qb ) {
      qa->Split( 0.5, &spa, &spb );
      bmlPtr->append(spa);
      bmlPtr->append(spb);
      qa = theOnes.get();
    }
    else {
      bmlPtr->append(qb->Clone());
    }

    qb = dbi++;
  }

  // Finish the line
  while( 0 != qb ) {
    bmlPtr->append(qb->Clone());
    qb = dbi++;
  }

  // Generate a new BeamlineContext to handle the new line
  bool makeRing = _p_currBmlCon->isTreatedAsRing();
  _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                       bmlPtr, false );
  _p_currBmlCon->setClonedFlag( true );
  if( makeRing ) { _p_currBmlCon->handleAsRing(); }
  else           { _p_currBmlCon->handleAsLine(); }
  _contextList.insert( _p_currBmlCon );


  // Finished!
  // delete wpu;
  emit _new_beamline();
}


void CHEF::_editPartAndSect()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  beamline* bmlPtr = _p_currBmlCon->cloneBeamline();
  // Must be eliminated before exiting.
  double energy    = bmlPtr->Energy();

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
  splitBmlPtr->setEnergy( energy );
  if( returnCode == QDialog::Accepted ) {
    bool ok;
    numberOfSectors = (qle->text()).toInt( &ok);

    if( ok && ( numberOfSectors > 1 )) {
      // Insert equally spaced markers throughout 
      // the bml model.

      marker* spaceCharge[ numberOfSectors + 1 ];
      double markerInterval = bmlLength / ((double) numberOfSectors);
      double insertionPoint = markerInterval;

      Particle* pPtr = (_p_currBmlCon->getParticle()).Clone();
      pPtr->setStateToZero();
      InsertionList insl( *pPtr );
      delete pPtr; pPtr = 0;
      
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
      try {
        bmlPtr->InsertElementsFromList( s, insl, removedElements );
      }
      catch (GenericException& ge){
        std::ostringstream uic;
        uic << __FILE__ << ", line " << __LINE__ << ": "
            << "Exception was thrown by beamline::InsertElementsFromList(..).\n"
               "The message was:\n"
            << ge.what();

        QMessageBox mb(QString("CHEF: ERROR"), QString(uic.str().c_str()), 
                       QMessageBox::Critical, 
                       QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        mb.show();
        while(mb.isVisible()) { qApp->processEvents(); }

        InsertionListElement* ptr_ile;
        while( 0 != (ptr_ile = insl.Get()) ) {
          delete ptr_ile;
        }
        for( int i = 0; i <= numberOfSectors; ++i ) {
          delete spaceCharge[i];
        }

        bmlPtr->eliminate();
        splitBmlPtr->eliminate();

        return;
      }
      bmlPtr->append( spaceCharge[numberOfSectors] );
    
      // Display and delete the removed elements
      // REMOVE: cout << "Removed elements\n";
      slist_iterator fembril( removedElements );
      bmlnElmnt* qq;
      while(( qq = (bmlnElmnt*) fembril() )) {
        // REMOVE: cout << qq->Type() << "  " << qq->Name() << "\n";
        delete qq;
      }
      // REMOVE: cout << endl;
    
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
      Particle* pPtr = (_p_currBmlCon->getParticle()).Clone();
      pPtr->SetReferenceEnergy( _p_currBmlCon->getEnergy() );
        // This line is almost certainly unnecessary.
      pPtr->setStateToZero();

      for( i = 0; i < numberOfSectors; i++ ) {
        JetParticle* jpPtr = pPtr->ConvertToJetParticle();
        splitBmlPtr->append( bmlPtr->MakeSector( *(spaceCharge[i]),
                                                 *(spaceCharge[i+1]),
                                                 order, *jpPtr  ) );
        delete jpPtr; jpPtr = 0;
        splitBmlPtr->append( spaceCharge[i+1]->Clone() );
      }

      delete pPtr; pPtr = 0;


      // Restore former environment...
      Jet::_lastEnv  = formerJetEnv;
      JetC::_lastEnv = formerJetCEnv;
    }
  }


  delete wpu;
  bmlPtr->eliminate();

  _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                       splitBmlPtr, false );
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
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

  LattFncData* lfd = 0;
  try {
    lfd = new LattFncData( _p_currBmlCon );
    lfd->doCalc();
    lfd->makeCurves();
  }
  catch( const std::exception& ge ) {
    std::ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown by LattFncData.\n"
           "The message was:\n"
        << ge.what();

    QMessageBox mb(QString("CHEF: ERROR"), QString(uic.str().c_str()), 
                   QMessageBox::Critical, 
                   QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    mb.show();
    while(mb.isVisible()) { qApp->processEvents(); }

    if( 0 != lfd ) { delete lfd; lfd = 0; }
    return;
  }

  if( lfd ) {
    _plotWidget  =  new CHEFPlotMain( 0, "plotWidget", Qt::WDestructiveClose );
    // destructive close needed !
    // *_plotWidget will delete itself upon closing.
    //   The problem is: _plotWidget is not reset to zero.
    _plotWidget->addData( *lfd );

    char theCaption[1024];
    for( int i = 0; i < 1024; i++ ) {
      theCaption[i] = '\0';
    }
    strcat( theCaption, "CHEF: Lattice Functions (uncoupled): " );
    strcat( theCaption, _p_currBmlCon->name() );
    _plotWidget->setCaption( theCaption );
    _plotWidget->show();
  
    delete lfd; 
    lfd = 0;
  }
}


void CHEF::_launchET()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

  ETFncData*  etfd = 0;
  try {
    etfd = new ETFncData( _p_currBmlCon );
    etfd->doCalc();
    etfd->makeCurves();
  }
  catch( const std::exception& ge ) {
    std::ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown by ETFncData.\n"
           "The message was:\n"
        << ge.what();

    QMessageBox mb(QString("Fatal Error"), QString( uic.str().c_str() ), 
                   QMessageBox::Critical, 
                   QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    mb.show();
    while(mb.isVisible()) { qApp->processEvents(); }

    if( 0 != etfd ) { delete etfd; etfd = 0; }
    return;
  }

  if( etfd ) {
    _ETplotWidget = new CHEFPlotMain( 0, "plotWidget", Qt::WDestructiveClose); 
    // destructive close needed !
    _ETplotWidget->addData( *etfd );

    char theCaption[1024];
    for( int i = 0; i < 1024; i++ ) {
      theCaption[i] = '\0';
    }
    strcat( theCaption, "CHEF: Lattice Functions (factorization): " );
    strcat( theCaption, _p_currBmlCon->name() );
    _ETplotWidget->setCaption( theCaption );
    _ETplotWidget->show();

    delete etfd;
  }
}


void CHEF::_launchLB()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

  LBFncData* lbfd = 0;
  try {
    lbfd = new LBFncData( _p_currBmlCon );
    lbfd->doCalc();
    lbfd->makeCurves(); 
  }
  catch( const std::exception& ge ) {
    std::ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown by LBFncData.\n"
           "The message was:\n"
        << ge.what();

    QMessageBox mb(QString("Fatal Error"), QString( uic.str().c_str() ), 
                   QMessageBox::Critical, 
                   QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    mb.show();
    while(mb.isVisible()) { qApp->processEvents(); }

    if( lbfd ) { delete lbfd; lbfd = 0; }
    return;
  }

  if( lbfd ) {
    _LBplotWidget = new CHEFPlotMain( 0, "plotWidget", Qt::WDestructiveClose); // destructive close needed !
    _LBplotWidget->addData( *lbfd );
  
    char theCaption[1024];
    for( int i = 0; i < 1024; i++ ) {
      theCaption[i] = '\0';
    }
    strcat( theCaption, "CHEF: Lattice Functions (eigenvectors): " );
    strcat( theCaption, _p_currBmlCon->name() );
    _LBplotWidget->setCaption( theCaption );
    _LBplotWidget->show();

    delete lbfd;
  }
}


void CHEF::_launchMoments()
{
  if( 0 == _p_currBmlCon ) { 
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }


  MomentsFncData*  mfd = 0;
  try {
    mfd = new MomentsFncData( _p_currBmlCon ); 
    mfd->doCalc();
    mfd->makeCurves();
  }
  catch( const std::exception& ge ) {
    std::ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown by MomentsFncData.\n"
           "The message was:\n"
        << ge.what();

    QMessageBox mb(QString("Fatal Error"), QString( uic.str().c_str() ), 
                   QMessageBox::Critical, 
                   QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    mb.show();
    while(mb.isVisible()) { qApp->processEvents(); }

    if( 0 != mfd ) { delete mfd; mfd = 0; }
    return;
  }

  if( mfd ) {
    _MMplotWidget = new CHEFPlotMain( 0, "plotWidget", Qt::WDestructiveClose);  
    // destructive close needed !
    _MMplotWidget->addData( *mfd );
  
    char theCaption[1024];
    for( int i = 0; i < 1024; i++ ) {
      theCaption[i] = '\0';
    }
    strcat( theCaption, "CHEF: Lattice Functions (covariance): " );
    strcat( theCaption, _p_currBmlCon->name() );
    _MMplotWidget->setCaption( theCaption );
    _MMplotWidget->show();

    delete mfd;
  }
}


void CHEF::_pushMoments()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( _p_currBmlCon->isTreatedAsRing() ) {
    QMessageBox::information( 0, "CHEF: ERROR",
      "Moments are \"pushed\" through beamlines only."
      "\nTry fixing with Edit/Mode function." );
    return;
  }

  InitCondDialogLF* wpu
    = new InitCondDialogLF( 0, "CHEF::Initial Lattice Functions" );

  if( QDialog::Accepted == wpu->exec() ){
    // Two steps are needed here: 
    //   (1) read the text widgets and 
    //   (2) return the lattice functions.
    wpu->readInputValues();
    LattFuncSage::lattFunc initialConditions( wpu->getInitCond() );

    CovarianceSage::Info initialCovariance;
    initialCovariance.arcLength = 0.0;
    initialCovariance.beta.hor  = initialConditions.beta.hor;
    initialCovariance.beta.ver  = initialConditions.beta.ver;
    initialCovariance.alpha.hor = initialConditions.alpha.hor;
    initialCovariance.alpha.ver = initialConditions.alpha.ver;
    
    int returnCode
      = BmlUtil::makeCovariance( initialCovariance, _p_currBmlCon->getParticle() );
    // This assumes, by default, an equal horizontal and vertical emittance.

    if( 0 != returnCode ) {
      QMessageBox::information( 0, "CHEF", "Calculation failed." );
      delete wpu;
      return;
    }

    _p_currBmlCon->setInitial( initialCovariance );

    MomentsFncData* mfd = 0;
    try {
      mfd = new MomentsFncData ( _p_currBmlCon );
      mfd->doCalc();
      mfd->makeCurves();
    }
    catch( const std::exception& ge ) {
      std::ostringstream uic;
      uic << __FILE__ << ", line " << __LINE__ << ": "
          << "Exception was thrown by MomentsFncData.\n"
             "The message was:\n"
          << ge.what();
      QMessageBox mb(QString("CHEF: ERROR"), QString( uic.str().c_str() ), 
                     QMessageBox::Critical, 
                     QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      mb.show();
      while(mb.isVisible()) { qApp->processEvents(); }

      if( 0 != wpu) { delete wpu; wpu = 0; }
      if( 0 != mfd) { delete mfd; mfd = 0; }
      return;
    }

    if(mfd) {
      _MMplotWidget = new CHEFPlotMain( 0, "MMplotWidget", Qt::WDestructiveClose ); 
      // destructive close needed !
      _MMplotWidget->addData( *mfd  );
  
       char theCaption[1024];
       for( int i = 0; i < 1024; i++ ) {
        theCaption[i] = '\0';
      }
      strcat( theCaption, "CHEF: Lattice Functions (covariance): " );
      strcat( theCaption, _p_currBmlCon->name() );
      _MMplotWidget->setCaption( theCaption );

      _MMplotWidget->show();

      delete mfd; 
      mfd = 0;
    }
  }

  // Clean up before exiting
  if( 0 != wpu) { delete wpu; wpu = 0; }
}


void CHEF::_pushDispersion()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( _p_currBmlCon->isTreatedAsRing() ) {
    QMessageBox::information( 0, "CHEF: ERROR",
      "Dispersion is \"pushed\" through beamlines only."
      "\nTry fixing with Edit/Mode function." );
    return;
  }

  InitCondDialogLF* wpu
    = new InitCondDialogLF( 0, "CHEF::Initial Lattice Functions" );

  if( QDialog::Accepted == wpu->exec() ){
    // Two steps are needed here: 
    //   (1) read the text widgets and 
    //   (2) return the lattice functions.
    wpu->readInputValues();
    LattFuncSage::lattFunc initialConditions( wpu->getInitCond() );

    DispersionSage::Info initialDispersion;
    initialDispersion.arcLength      = 0.0;
    initialDispersion.dispersion.hor = initialConditions.dispersion.hor;
    initialDispersion.dispersion.ver = initialConditions.dispersion.ver;
    initialDispersion.dPrime.hor     = initialConditions.dPrime.hor;
    initialDispersion.dPrime.ver     = initialConditions.dPrime.ver;

    _p_currBmlCon->setInitial( initialDispersion );

    DspnFncData* dfd = 0;
    try {
      dfd = new DspnFncData ( _p_currBmlCon );
      // dfd->doCalc( initialDispersion );
      dfd->doCalc();
      dfd->makeCurves();
    }
    catch( const std::exception& ge ) {
      std::ostringstream uic;
      uic << __FILE__ << ", line " << __LINE__ << ": "
          << "Exception was thrown by DspnFncData.\n"
             "The message was:\n"
          << ge.what();
      QMessageBox mb(QString("CHEF: ERROR"), QString( uic.str().c_str() ), 
                     QMessageBox::Critical, 
                     QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      mb.show();
      while(mb.isVisible()) { qApp->processEvents(); }

      if( 0 != wpu) { delete wpu; wpu = 0; }
      if( 0 != dfd) { delete dfd; dfd = 0; }
      return;
    }

    if(dfd) {
      _DspnplotWidget = new CHEFPlotMain( 0, "DspnplotWidget", Qt::WDestructiveClose ); 
      // destructive close needed !
      _DspnplotWidget->addData( *dfd  );
  
       char theCaption[1024];
       for( int i = 0; i < 1024; i++ ) {
        theCaption[i] = '\0';
      }
      strcat( theCaption, "CHEF: Dispersion: " );
      strcat( theCaption, _p_currBmlCon->name() );
      _DspnplotWidget->setCaption( theCaption );

      _DspnplotWidget->show();

      delete dfd; 
      dfd = 0;
    }
  }

  // Clean up before exiting
  if( 0 != wpu) { delete wpu; wpu = 0; }
}


void CHEF::_pushParticles()
{
  DistributionWidget* dwPtr
    = new DistributionWidget( *(_p_currBmlCon), 0, 0, Qt::WDestructiveClose );
  dwPtr->show();
}


void CHEF::_pushULF()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( _p_currBmlCon->isTreatedAsRing() ) {
    QMessageBox::information( 0, "CHEF: ERROR",
      "Lattice functions are \"pushed\" through beamlines only."
      "\nTry fixing with Edit/Mode function." );
    return;
  }

  InitCondDialogLF* wpu
    = new InitCondDialogLF( 0, "CHEF::Initial Lattice Functions" );

  if( QDialog::Accepted == wpu->exec() ){
    // Two steps are needed here: 
    //   (1) read the text widgets and 
    //   (2) return the lattice functions.
    wpu->readInputValues();
    LattFuncSage::lattFunc initialConditions( wpu->getInitCond() );
    
    _p_currBmlCon->setInitial( initialConditions );

    LattFncData* lfd = 0;
    try {
      lfd = new LattFncData ( _p_currBmlCon );
      // lfd->doCalc( initialConditions );
      lfd->doCalc();
      lfd->makeCurves();
    }
    catch( const std::exception& ge ) {
      std::ostringstream uic;
      uic << __FILE__ << ", line " << __LINE__ << ": "
          << "Exception was thrown by LattFncData.\n"
             "The message was:\n"
          << ge.what();
      QMessageBox mb(QString("CHEF: ERROR"), QString( uic.str().c_str() ), 
                     QMessageBox::Critical, 
                     QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
      mb.show();
      while(mb.isVisible()) { qApp->processEvents(); }

      if( 0 != wpu) { delete wpu; wpu = 0; }
      if( 0 != lfd) { delete lfd; lfd = 0; }
      return;
    }

    if(lfd) {
      _plotWidget = new CHEFPlotMain( 0, "DspnplotWidget", Qt::WDestructiveClose ); 
      // destructive close needed !
      _plotWidget->addData( *lfd  );
  
       char theCaption[1024];
       for( int i = 0; i < 1024; i++ ) {
        theCaption[i] = '\0';
      }
      strcat( theCaption, "CHEF: Uncoupled lattice functions: " );
      strcat( theCaption, _p_currBmlCon->name() );
      _plotWidget->setCaption( theCaption );

      _plotWidget->show();

      delete lfd; 
      lfd = 0;
    }
  }

  // Clean up before exiting
  if( 0 != wpu) { delete wpu; wpu = 0; }
}


void CHEF::_launchTrack()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }
  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

  _trackWidget = new Tracker(_p_currBmlCon,0,0,Qt::WDestructiveClose);
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

  _traceWidget = new RayTrace(_p_currBmlCon,0,0,Qt::WDestructiveClose);
  _traceWidget->setCaption( "CHEF:: Orbit Trace" );
  _traceWidget->show();
}


void CHEF::_launchDispersion()
{
  if( 0 == _p_currBmlCon ) { 
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }


  DspnFncData* dfd = 0;
  try {
    dfd = new DspnFncData ( _p_currBmlCon );
    dfd->doCalc();
    dfd->makeCurves();
  }
  catch( const std::exception& ge ) {
    std::ostringstream uic;
    uic << __FILE__ << ", line " << __LINE__ << ": "
        << "Exception was thrown by DspnFncData.\n"
           "The message was:\n"
        << ge.what();

    QMessageBox mb(QString("Fatal Error"), QString( uic.str().c_str() ), 
                   QMessageBox::Critical, 
                   QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    mb.show();
    while(mb.isVisible()) { qApp->processEvents(); }

    if( 0 != dfd ) { delete dfd; dfd = 0; }
    return;
  }

  if( dfd ) {
    _DspnplotWidget = new CHEFPlotMain( 0, "plotWidget", Qt::WDestructiveClose); 
    // destructive close needed !
    _DspnplotWidget->addData( *dfd  );
  
    char theCaption[1024];
    for( int i = 0; i < 1024; i++ ) {
      theCaption[i] = '\0';
    }
    strcat( theCaption, "CHEF: Dispersion: " );
    strcat( theCaption, _p_currBmlCon->name() );
    _DspnplotWidget->setCaption( theCaption );
    _DspnplotWidget->show();

    delete dfd;
  }
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
  else { 
    BeamlineContext* bc1Ptr = (BeamlineContext*) (onePtr->ptr());
    BeamlineContext* bc2Ptr = (BeamlineContext*) (twoPtr->ptr());

    if( !(bc1Ptr->isTreatedAsRing() && bc2Ptr->isTreatedAsRing()) ) {
      QMessageBox::information( 0, "CHEF: ERROR",
                                "Both lines must be periodic."
                                "\nTry fixing with Edit/Mode function." );
      return;
    }
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

  _siteWidget = new SiteViewer( *(_p_currBmlCon) , 0, 0, Qt::WDestructiveClose );
  // REMOVE: _siteWidget = new SiteViewer( *(_p_currBmlCon) );
  // Must be deleted by itself.
  _siteWidget->setCaption( QString("CHEF:: Site Viewer")+
                           QString("    ")+ 
                           QString(_p_currBmlCon->name()) );
  _siteWidget->show();
}


void CHEF::_chromCtrl()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( !(_p_currBmlCon->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

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
    std::ostringstream uic;
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
              = new QRadioButton( "Kinetic energy [GeV]: ", qbg );
            QRadioButton* pcPtr 
              = new QRadioButton( "Momentum [GeV/c]: ", qbg );
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
        std::ostringstream uic;
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

      const char* typeStringPtr = bfp->getParticleType();
      if( 0 == strcmp( "PROTON", typeStringPtr ) ) {
        _p_currBmlCon = new BeamlineContext( Proton(bmlPtr->Energy()), bmlPtr, false );
      }
      else if( 0 == strcmp( "POSITRON", typeStringPtr ) ) {
        _p_currBmlCon = new BeamlineContext( Positron(bmlPtr->Energy()), bmlPtr, false );
      }
      else {
        QMessageBox mb(  QString("*** ERROR ***")
                       , QString( "Unrecognized or unspecified particle type."
                                  "\nDeclare a particle using MAD's BEAM command." )
                       , QMessageBox::Critical
                       , QMessageBox::Ok
                       , QMessageBox::NoButton
                       , QMessageBox::NoButton );
        mb.show();
        while (mb.isVisible())  qApp->processEvents(); 
      }
      _p_currBmlCon->setClonedFlag( true ); // force beamline destruction (eliminate() is called)
                                            //when the context is destroyed;
      _contextList.insert( _p_currBmlCon );
    }
  
    // REMOVE: if( _p_currBmlCon->isRing() ) { _p_currBmlCon->handleAsRing(); }
    // REMOVE: else                          { _p_currBmlCon->handleAsLine(); }

    delete wpu;
  }

  else if( bmlFile ) { 
    beamline* bmlPtr = new beamline;
    std::ifstream inputStream( s );
    inputStream >> (*bmlPtr);
    inputStream.close();

    QDialog* wpu = new QDialog( 0, 0, true );
      QVBox* qvb = new QVBox( wpu );

      new QLabel( "I regret you must specify\na particle species.", qvb );
      QRadioButton* qrb_proton_ptr = new QRadioButton( "proton", qvb );
      QRadioButton* qrb_positron_ptr = new QRadioButton( "positron", qvb );

      QPushButton* okayBtn = new QPushButton( "OK", qvb );
        connect( okayBtn, SIGNAL(pressed()),
                 wpu,     SLOT(accept()) );

      qvb->setMargin(5);
      qvb->setSpacing(3);
      qvb->adjustSize();

    wpu->setCaption( "CHEF: Particle Choice" );
    wpu->adjustSize();

    int returnCode = wpu->exec();

    if( qrb_proton_ptr->isDown() ) {
      _p_currBmlCon = new BeamlineContext( Proton(bmlPtr->Energy()), bmlPtr, false );
    }
    else {
      _p_currBmlCon = new BeamlineContext( Positron(bmlPtr->Energy()), bmlPtr, false );
    }

    delete wpu;

    _p_currBmlCon->setClonedFlag( true ); // force beamline destruction (eliminate() is called)
                                          // when the context is destroyed;

    // REMOVE: if( _p_currBmlCon->isRing() ) { _p_currBmlCon->handleAsRing(); }
    // REMOVE: else                          { _p_currBmlCon->handleAsLine(); }

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
    std::ofstream outStream( s.latin1() );
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
  _toolMenu->setItemEnabled( _id_ctrlMenu,       true );

  if( _p_vwr == 0 ) {
    int minWidth = _mainMenu->width();
    _p_vwr = new BeamlineBrowser( _mainWindow );
    // _p_vwr = new BeamlineBrowser( 0 );
  
    _p_vwr->setCaption( "CHEF:: Beamline Browser" );
    _p_vwr->setAllColumnsShowFocus( TRUE );
    _p_vwr->setMinimumWidth( minWidth );
    _p_vwr->show();

    connect( this,   SIGNAL(_modeChanged( const BeamlineContext* )),
             _p_vwr, SLOT(resetPixmap( const BeamlineContext* ))        );
    connect( _p_vwr, SIGNAL(sig_bmlLeftClicked( BeamlineContext*, QBmlRoot* )),
             this,   SLOT(_set_p_clickedContext( BeamlineContext*, QBmlRoot* )) );
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


void CHEF::_set_p_clickedContext( BeamlineContext* x, QBmlRoot* y )
{
  _p_currBmlCon   = x;
  _p_currQBmlRoot = y;
  _p_clickedQBml  = y;
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
