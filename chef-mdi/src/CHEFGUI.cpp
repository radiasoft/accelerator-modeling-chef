/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CHEFGUI.cpp                                       ****** 
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
***************************************************************************
***************************************************************************
**************************************************************************/

/*************************************************************************/
/* Creation Date:  August 2004                                           */
/*                                                                       */
/* Revision Date:  July,  2005                                           */
/*   - added five slots: editEditElement                                 */
/*                       editFlatten                                     */
/*                       editMisalign                                    */
/*                       editNewOrder                                    */
/*                       pushParticles                                   */
/*   - LPJM                                                              */
/*                                                                       */
/* Revision Date:  August, 2008                                          */
/*   - added to CHEFGUI::tuneCtrl: ability to change tune                */
/*     absolutely as well as relatively.                                 */
/*   - LPJM                                                              */
/*                                                                       */
/*************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>
#include <istream>
#include <streambuf>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/marker.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/sector.h>
#include <beamline/marker.h>
#include <beamline/quadrupole.h>
#include <beamline/sextupole.h>
#include <bmlfactory/MAD8Factory.h>
#include <bmlfactory/ParserException.h>
#include <parsers/xsif/XSIFFactory.h>
#include <physics_toolkit/BeamlineContext.h>
#include <physics_toolkit/BendEliminator.h>

#include <CHEFGUI.h>
#include <filters.h>
#include <BeamlineBrowser.h>
#include <BmlSelectionDialog.h>
#include <DialogGenerateBunch.h>
#include <DialogInitialState.h>
#include <InitCondDialogLF.h>
#include <EditDialog.h>
#include <DistributionWidget.h>
#include <FramePusher.h>
#include <SurveyMatcher.h>
#include <DbConnectDialog.h>
#include <DbRetrieveDialog.h>
#include <appworkspace.h>
#include <about.h>

#include <BmlUtil.h>

#include <qapplication.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qfiledialog.h>
#include <qtextedit.h>
#include <qaction.h>
#include <qsizepolicy.h>
#include <qassistantclient.h>
#include <qaction.h>
#include <qsqldatabase.h>
#include <qwt_plot.h>


#include <CF_Editor.h>
#include <qpychef.h>
#include <devicetable.h>


#include <CHEFPlotMain.h>
#include <CurveData.h>
#include <PlotData.h>
#include <beamline/ParticleBunch.h>
#include <BunchData.h>
#include <DbConnectDialog.h>
#include <DataBrowser.h>


#include <CommandReferenceOrbit.h>
#include <CommandPropagateReferenceOrbit.h>
#include <CommandTwiss.h>
#include <CommandPropagateTwiss.h>
#include <CommandMoments.h>
#include <CommandPropagateMoments.h>
#include <CommandEigenmodes.h>
#include <CommandPropagateEigenmodes.h>
#include <CommandDispersion.h>
#include <CommandPropagateDispersion.h>

#include <CommandEdwardsTeng.h>

#include <boost/algorithm/string.hpp>

using FNAL::pcout;
using FNAL::pcerr;

using namespace std;

extern beamline* DriftsToSlots( beamline const& original );


struct DataStore {
    double az;
    double x1;
    double x2;
    double x3;
};



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CHEFGUI::CHEFGUI(QWidget* parent, char const* name, WFlags f)
  :CHEFGUIBase(parent,name,f), browser_(0)

{

  userOptions_.couplingOption = Options::ignore_coupling;

  p_currQBmlRoot_ = 0;
  p_clickedQBml_  = 0;

  //==================================================================================
  // Create an initial Jet environment
  //==================================================================================

   env_  =                TJetEnvironment<double>::makeJetEnvironment(1, 6, 6);
   envc_ = TJetEnvironment<std::complex<double> >::makeJetEnvironment(1, 6, 6);  
   
    Jet__environment::setLastEnv( env_ );
   JetC__environment::setLastEnv( envc_ );
  
  // Make connections

  connect( this, SIGNAL(new_beamline()    ),
           this, SLOT  (launch_browser()) );



  centralWidget_ = new AppWorkspace(this, "AppWorkspace");

  setCentralWidget( centralWidget_ );


  // the browser initial set of windows are created in QShowEvent handler. This allow for correct initial geometry
  // based on the initial main widget dimensions.   

   eventfilter_ = new WindowCloseEventEater();


  // initialize the beamline browser
  // -------------------------------

   browser_ = new BeamlineBrowser( centralWidget(), "BeamlineBrowser" );

   browser_->hide();
   browser_->installEventFilter( eventfilter_ );

   windowsTree_BrowserAction->setOn(true);
   browser_->setAllColumnsShowFocus( true );



    connect( this,   SIGNAL( modeChanged( ConstBmlContextPtr )),
             browser_, SLOT(resetPixmap( ConstBmlContextPtr ))        );

    connect( browser_, SIGNAL(sig_bmlLeftClicked( BmlContextPtr,   QBmlRoot* )),
             this,   SLOT(  set_p_clickedContext( BmlContextPtr,  QBmlRoot* ))    );

    connect( browser_, SIGNAL( sig_bmlLeftClicked( QBml* )),
             this,   SLOT( set_p_clickedQBml( QBml* ))    );

    connect( browser_, SIGNAL( sig_newContext( BmlContextPtr )),
             this,   SLOT( slot_contextGenerated( BmlContextPtr ))    );



   // initialize the Message window and the associated Message io
   // --------------------------------------------------------
 
     messages_  = new QTextEdit(centralWidget_, "Messages");
     messages_->setCaption("Messages");
     messages_->setFont(QFont("MiscFixed"));
     messages_->hide();
     messages_->installEventFilter( eventfilter_);


     messages_stdout_ = new Messages<0>(messages_);
     messages_stdwar_ = new Messages<1>(messages_);
     messages_stderr_ = new Messages<2>(messages_);

     p_messages_stdout_stream_ = new std::ostream( messages_stdout_ ); 
     p_messages_stdwar_stream_ = new std::ostream( messages_stdwar_ ); 
     p_messages_stderr_stream_ = new std::ostream( messages_stderr_ );
  
#ifdef _WIN32
     messages_->append( QString("CHEF (Win32)  Preview Version ") );
#else
     messages_->append( QString("CHEF (Unix/Linux) Preview Version " ) );
#endif
     messages_->append( QString("(C) Fermi Research Alliance/Fermilab, All Rights Reserved.\n") );
    
     iosetup( p_messages_stdout_stream_, p_messages_stdwar_stream_);

     windowsMessagesAction->setOn(true);


   // initialize the Python Interpreter window
   // ------------------------------------------

     interpreter_ = 0;
     interpreter_ = new QPyCHEF(centralWidget_ , "Interpreter");
     interpreter_->hide();
     interpreter_->installEventFilter( eventfilter_ );
     windowsInterpreterAction->setOn(true);


   // initialize the ACNET Devices Monitor window
   // ------------------------------------------

    devices_ = new DeviceTable(centralWidget_, "Devices");
    devices_->hide();
    devices_->installEventFilter( eventfilter_ );
     windowsDevicesAction->setOn(true);


   // initialize the Database support
   //---------------------------------

#if 0
     // there seems to be a bug in Qt when windows with a fixed size windows
     // are displayed in a QWorkspace.
 
    dbconnect_dlg_  = new DbConnectDialog(  dynamic_cast<QWorkspace*>( centralWidget_ ), 
                                            "Database Connection");  
    dbretrieve_dlg_ = new DbRetrieveDialog( dynamic_cast<QWorkspace*>( centralWidget_ ), 
                                            "Retrieve Data from Database");
#endif
    dbconnect_dlg_  = new DbConnectDialog( 0, "Database Connection");  
    dbretrieve_dlg_ = new DbRetrieveDialog(0, "Retrieve Data from Database");

    connect( dbconnect_dlg_, SIGNAL( buttonOkClicked(const DbConnectStruct&)    ), 
             this,             SLOT( handleDBConnection(const DbConnectStruct&) ) );

    default_db_ = 0;

    databaseRetrieveDataAction->setEnabled(false);

   // initialize the Text Editor
   // --------------------------



    // initialize the help window
    //----------------------------

    assistanthelp_ = new QAssistantClient("", this ); // path is empty; use system path to find assistant executable

    QStringList cmdLst;
    cmdLst << "-hideSidebar";
    cmdLst << "-profile"<<QString("doc") + QDir::separator() + QString("chefdoc.adp");
    assistanthelp_->setArguments( cmdLst );

    // initialize the Beamline Section Dialog

    bmlSelectionDialog_ = new BmlSelectionDialog(this,"BmlSelectionDialog", WType_Popup );
    bmlSelectionDialog_->hide();


    connect(browser_ , SIGNAL( sig_browserIsEmpty()    ),    this, SLOT( disableMenus() ) );
    connect(browser_ , SIGNAL( sig_browserIsNotEmpty() ),    this, SLOT( enableMenus()  )  );

    disableMenus();


    // initialize persistent dialogs

    initCondDialogLF_       = new InitCondDialogLF(this, "InitCondDialogLF", 0);
    initCondDialogLF_->ULFMode();
    initCondDialogLF_->setCaption("Uncoupled Lattice Functions: Initial Conditions");

    initCondDialogDisp_     = new InitCondDialogLF(this, "InitCondDialogLF", 0);
    initCondDialogDisp_->dispersionMode();
    initCondDialogDisp_->setCaption("Dispersion: Initial Conditions");

    initCondDialogMoments_  = new InitCondDialogCovariance(this, "InitCondDialogCovariance", 0);
    initCondDialogMoments_->setCaption("Moments (Including Transverse Coupling) : Initial Conditions");


    // -------------------------------------------------------------------------
    // initialize the commands
    //--------------------------------------------------------------------------

   command_computeReferenceOrbit_ = 
          boost::function<QWidget* (QWidget*, BmlContextPtr)  >                                        ( CommandReferenceOrbit() ); 
 
   command_propagateReferenceOrbit_ = 
          boost::function<QWidget* (QWidget*, BmlContextPtr,CSLattFuncs const&  )  >                   ( CommandPropagateReferenceOrbit() ); 

    command_computePeriodicTwiss_ = 
          boost::function<void ( BmlContextPtr )           >                                            ( CommandComputePeriodicTwiss() ); 

    command_computePropagateTwiss_ = 
          boost::function<void( BmlContextPtr,  CSLattFuncs )        >                                  ( CommandComputePropagateTwiss() ); 

    command_plotTwiss_ = 
          boost::function<void (QWidget*, sqlite::connection& )  >                                     ( CommandPlotTwiss() ); 

    command_computeMoments_ = 
          boost::function<QWidget* ( QWidget*, BmlContextPtr) >                                        ( CommandMoments() );  

    command_propagateMoments_ = 
           boost::function<QWidget* ( QWidget*, BmlContextPtr,  CSLattFuncs  const&)    >              ( CommandPropagateMoments());  

    command_computeEigenmodes_ = 
          boost::function<QWidget* ( QWidget*, BmlContextPtr) >                                        ( CommandEigenmodes()); 

    command_propagateEigenmodes_ = 
          boost::function<QWidget* ( QWidget*, BmlContextPtr,    CSLattFuncs  const& ) >               ( CommandPropagateEigenmodes()); 

    command_computeDispersion_ = 
      boost::function<QWidget* ( QWidget*, BmlContextPtr) >                                            ( CommandDispersion()        ); 
    command_propagateDispersion_ = 
          boost::function<QWidget* ( QWidget*, BmlContextPtr,   CSLattFuncs const&)  >                 ( CommandPropagateDispersion() ); 

    command_computeEdwardsTeng_ = 
      boost::function<QWidget* ( QWidget*, BmlContextPtr) >                                            ( CommandEdwardsTeng());           



}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CHEFGUI::~CHEFGUI()
{

   // remove the eventfilters

    if (browser_)           browser_->removeEventFilter( eventfilter_ );
    if (messages_)         messages_->removeEventFilter( eventfilter_ );
    if (interpreter_)   interpreter_->removeEventFilter( eventfilter_ );
    if (devices_)           devices_->removeEventFilter( eventfilter_ );

    // delete the message window ostream and streambuf objects

    //if ( pmessages_stdout_stream_ ) delete pmessages__stdout_stream_;
    //if ( pmessages_stderr_stream_ ) delete pmessages__stderr_stream_;

    //if (messages_stdout_) delete messages_stdout_;
    //if (messages_stderr_) delete messages_stderr_;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::readBmlFile( QString s) 
{
  BmlPtr bmlPtr = BmlPtr( new beamline );

  std::ifstream inputStream( s );
  inputStream >> (*bmlPtr);
  inputStream.close();

  QDialog* wpu = new QDialog( 0, 0, true );
  QVBox*   qvb = new QVBox( wpu );

  new QLabel( "A particle species must be specified.", qvb );
  QRadioButton* qrb_proton_ptr = new QRadioButton( "proton", qvb );
  // QRadioButton* qrb_positron_ptr =
  new QRadioButton( "positron", qvb );

  QPushButton* okayBtn = new QPushButton( "OK", qvb );
    connect( okayBtn, SIGNAL(pressed()),
             wpu,     SLOT(accept()) );

  qvb->setMargin(5);
  qvb->setSpacing(3);
  qvb->adjustSize();

  wpu->setCaption( "CHEF: Particle Choice" );
  wpu->adjustSize();

  wpu->exec();

  if( qrb_proton_ptr->isDown() ) {
    bmlc_ = BmlContextPtr( new BeamlineContext(Proton(bmlPtr->Momentum()), *bmlPtr) );
  }
  else {
    bmlc_ = BmlContextPtr( new BeamlineContext(Positron(bmlPtr->Momentum()),*bmlPtr) );
  }

  delete wpu;

  contextList_.push_front( bmlc_ );
  emit new_beamline();
  browser_->clearSelection();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::openFile()
{
  std::auto_ptr<bmlfactory> bfp;

  // Open file dialog
  QString s = QFileDialog::getOpenFileName( QString::null,
                   "MAD (*.mad *.lat);; XSIF (*.xsif);; Beamline (*.bml)");

  if( s.isEmpty() ) return;
 
 
  enum  { mad_format, xsif_format, bml_format } lattice_format;

  if( (QString(".mad")  == s.right(4)) || (QString(".lat")  == s.right(4)) ) lattice_format = mad_format; 
  if( QString(".bml")  == s.right(4) )                                       lattice_format = bml_format; 
  if( QString(".xsif") == s.right(5) )                                       lattice_format = xsif_format; 
    
   try {
        switch ( lattice_format ) {
           case mad_format:   
                       bfp.reset(new MAD8Factory(s.ascii(), (char*)0 )); // cast needed to avoid ambiguity
                       break;
           case xsif_format:  
                       bfp.reset(new XSIFFactory(s.ascii(), (char*)0 )); // cast needed to avoid ambiguity
                       break;
           case bml_format:  
                       readBmlFile( s );
             return;
                       break;
           default:    
                       return; // unknown parser type 
        }
    }

    catch (ParserException& e) {
        QMessageBox mb(QString("Error"), QString( e.what() ), 
                    QMessageBox::Critical, 
                    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        mb.show();
        while (mb.isVisible())  qApp->processEvents(); 
        return;
    }
    catch (GenericException& e){
        QMessageBox::critical( 0, "CHEF: ERROR", e.what() );
        return;
    }

    std::list<std::string> beamline_list = bfp->getBeamlineList();

    //  instantiate only the n last beamlines defined in the mad file.

    bmlSelectionDialog_->setList( beamline_list, bfp->getUseStatementBeamlineName() );
    bmlSelectionDialog_->setBeamParameters( *bfp.get() );
    bmlSelectionDialog_->show();

    while ( bmlSelectionDialog_->isVisible()) qApp->processEvents();

    double brho = bmlSelectionDialog_->getBRHO();

    beamline_list = bmlSelectionDialog_->getSelected(); 

    std::list<std::string>::reverse_iterator it;
    int nlines = 0;
    for ( it = beamline_list.rbegin(); it != beamline_list.rend(); it++) {
            BmlPtr bmlPtr;
            try {
            bmlPtr = bfp->create_beamline( (*it).c_str() , brho);
          } 
          catch (ParserException& e) {
              QMessageBox mb(QString("Error"), QString( e.what() ), 
              QMessageBox::Critical, 
              QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
              mb.show();
              while (mb.isVisible())  qApp->processEvents(); 
              return;
          }

          std::string type_string( bfp->getParticleType());
         
          using boost::algorithm::iequals; // case insensitive equal test 

          if(      iequals( "PROTON",    type_string ) ) {
            bmlc_ = BmlContextPtr( new BeamlineContext(     Proton(bmlPtr->Momentum()), *bmlPtr) );
          }
          else if( iequals( "ANTIPROTON", type_string) ) {
            bmlc_ = BmlContextPtr( new BeamlineContext( AntiProton(bmlPtr->Momentum()), *bmlPtr) );
          }
          else if( iequals( "POSITRON",   type_string) ) {
            bmlc_ = BmlContextPtr( new BeamlineContext(   Positron(bmlPtr->Momentum()), *bmlPtr) );
          }
          else if( iequals( "ELECTRON",   type_string ) ) {
            bmlc_ = BmlContextPtr( new BeamlineContext(   Electron(bmlPtr->Momentum()), *bmlPtr) );
          }
          else if( iequals( "MUON",       type_string ) ) {
            bmlc_ = BmlContextPtr( new BeamlineContext(       Muon(bmlPtr->Momentum()), *bmlPtr) );
          }
          else if( iequals( "ANTIMUON",   type_string) ) {
            bmlc_ = BmlContextPtr( new BeamlineContext(   AntiMuon(bmlPtr->Momentum()), *bmlPtr) );
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

          contextList_.push_front( bmlc_ );

          browser_->displayBeamline( bmlc_ );

          ++nlines;
     }
     

    // set initial lattice functions 

    if ( bmlc_) bmlc_->setInitial( bfp->getInitialValues() );

 }

 
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::openDeviceFile()
{


 QString s = QFileDialog::getOpenFileName( QString::null,
                   "Devices (*.dev)" );

 if ( s.isEmpty() ) return;

 std::ifstream is( s.ascii());

 devices_->readDeviceList(is);
 is.close();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::openScriptFile()
{

 QString s = QFileDialog::getOpenFileName( QString::null,
                   "Script (Python) (*.py)" );

  if ( s.isEmpty() ) return;

  interpreter_->readFile( s.ascii() );

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::print()
{
  double s = 0.0;
  if( 0 != bmlc_ ) 
  { bmlc_->peekAt( s, bmlc_->getParticle() ); }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::fileWriteTree()
{
  if( 0 != bmlc_ ) { bmlc_->writeTree(); }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::fileSaveAs()
{
  // REMOVE: if( _p_clickedCon == 0 ) {
  if( bmlc_ == 0 ) {
    QMessageBox::information( 0, "CHEF",
                "Left click on a beamline first." );
    return;
  }

  // REMOVE: QString startName(_p_clickedCon->name());
  QString startName(bmlc_->Name());
  startName += ".bml";

  QString s = QFileDialog::getSaveFileName(
                   startName,
                   "Beamline (*.bml)",
                   0,
                   0,
                   "Save File As" );


  beamline*  bml = dynamic_cast<beamline*>(this);

  if( s.length() > 0 ) {
    std::ofstream outStream( s.latin1() );
    outStream << *bml;
    outStream.close();
  }



}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::fileEditorSaveAs()
{

  // save the file in the opened editor window that has focus


CF_Editor* editor = dynamic_cast<CF_Editor*>( dynamic_cast<QWorkspace*>(centralWidget_)->activeWindow() );

 if (!editor) return;

QString s = QFileDialog::getSaveFileName(
                   editor->caption(),
                   "Beamline (*.lat)",
                   0,
                   0,
                   "Save File As" );

 editor->setCaption( s );
 std::ofstream outStream( s.latin1() );
 outStream << editor->text();
 outStream.close();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::fileEditorSave()
{

  // save the file in the opened editor window that has focus


CF_Editor* editor = dynamic_cast<CF_Editor*>( dynamic_cast<QWorkspace*>(centralWidget_)->activeWindow() );

 if (!editor) return;

 std::ofstream outStream( editor->caption().latin1() );
 outStream << editor->text();
 outStream.close();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
CHEFGUI::editParse()
{
 
  CF_Editor* editor = 0;
  
  // **** determine which editor (if any) has focus; 

  QWidgetList wlist = dynamic_cast<QWorkspace*>(centralWidget())->windowList(QWorkspace::StackingOrder);

  for (QWidget* w = wlist.first(); w; w = wlist.next())
  {
   if ( dynamic_cast<QWorkspace*>(centralWidget_)->activeWindow() == w ) 
    {
      if ( QString((w)->name()) == QString("MAD8 Editor") ) 
      { 
        if ( (editor = dynamic_cast<CF_Editor*>(w)) ) parseEditorMAD8( editor );
      }
      if ( QString((w)->name()) == QString("Python Editor") ) 
      { 
        if( (editor = dynamic_cast<CF_Editor*>(w)) )  parseEditorPython(editor);
      }
    }
  }
  return;
}
 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
CHEFGUI::parseEditorMAD8( CF_Editor* editor )
{
  std::auto_ptr<bmlfactory> bfp;
 
  try 
  {
    bfp.reset( new MAD8Factory( editor->caption().ascii(), editor->text().ascii()) ); // the 2nd argument is the actual text buffer. 
                                                                                     // It is used only if != 0  
  }

  catch (ParserException& e)
  {
     QMessageBox mb(QString("Error"), QString( e.what() ), 
                    QMessageBox::Critical, 
                    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
     mb.show();
     while (mb.isVisible())  qApp->processEvents(); 
 
     if (e._inmemory) { // may not be in memory if other files have been included
         
      editor->setSelection( e._lineno-1, 0, e._lineno, 0,  1); 
      editor->setSelectionAttributes (1, QColor("Red"), false); 
     }

     return;
  }
  catch (GenericException& e)
  {
    QMessageBox mb(QString("Error"), QString( e.what() ), 
                 QMessageBox::Critical, 
                  QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    mb.show();
    while (mb.isVisible())  qApp->processEvents(); 
 
    return;


  }

  std::list<std::string> beamline_list = bfp->getBeamlineList();
       
  //  instantiate only the n last beamlines defined in the mad file. 

   bmlSelectionDialog_->setList( beamline_list, bfp->getUseStatementBeamlineName() );
   bmlSelectionDialog_->setBeamParameters( *bfp.get() );
   bmlSelectionDialog_->show();

  while ( bmlSelectionDialog_->isVisible()) qApp->processEvents();

  double brho =  bmlSelectionDialog_->getBRHO();

  beamline_list = bmlSelectionDialog_->getSelected(); 

  std::list<std::string>::iterator it;
  int nlines = 0;
  BmlPtr bmlPtr;
  
  for ( it = beamline_list.begin(); it != beamline_list.end(); it++) {

       try { 
         bmlPtr = bfp->create_beamline( (*it).c_str() , brho);
       }

       catch (ParserException& e) {
         QMessageBox mb(QString("Error"), QString( e.what() ), 
                    QMessageBox::Critical, 
                    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
          mb.show();
          while (mb.isVisible())  qApp->processEvents(); 
 
          if (e._inmemory) { // may not be in memory if other files have been included
         
             editor->setSelection( e._lineno-1, 0, e._lineno, 0,  1); 
             editor->setSelectionAttributes (1, QColor("Red"), false); 
          }

          return;
        } // catch

        const char* typeStringPtr = bfp->getParticleType();
        if( 0 == strcmp( "PROTON", typeStringPtr ) ) {
          bmlc_ = BmlContextPtr(new BeamlineContext( Proton(bmlPtr->Momentum()), *bmlPtr ));
        }
        else if( 0 == strcmp( "POSITRON", typeStringPtr ) ) {
          bmlc_ = BmlContextPtr(new BeamlineContext( Positron(bmlPtr->Momentum()),*bmlPtr) );
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

        contextList_.push_front( bmlc_ );
        browser_->displayBeamline( bmlc_ );
        nlines++;
  } 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void 
CHEFGUI::parseEditorPython( CF_Editor* /* editor */ )
{
  //  FIXME ! QPyCHEF needs to be fixed
  //  interpreter_->runBuffer( editor->text().ascii() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editFindFilter()
{
  QueryDialog* qdl = new QueryDialog( 0, 0, Qt::WDestructiveClose );
  connect( qdl,  SIGNAL( useThis( const BoolNode& )),
           this, SLOT  ( processFilter( const BoolNode& )) );
  qdl->show();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editSelectAll()
{
  browser_->selectAll( true );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editSelectNone()
{
  browser_->clearSelection();
  bmlc_   = BmlContextPtr();
  p_currQBmlRoot_ = 0;
  p_clickedQBml_  = 0;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editCopyLine()
{
  // Procedure copied from CHEF.builders.cc
  // and slightly modified.

  if( bmlc_ ) {
    bmlc_ = 
     BmlContextPtr( new BeamlineContext( bmlc_->getParticle(), *bmlc_ ) );

    contextList_.push_front( bmlc_ );
    emit new_beamline();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editRenameLine()
{
  if( 0 == bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Name: ", qhb1 );
        QLineEdit* qle = new QLineEdit( bmlc_->Name(), qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "OK", qhb2 );
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

  wpu->setCaption( "CHEF: Rename beamline" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    bmlc_->rename( qle->text().ascii() );
    p_currQBmlRoot_->setText( 0, qle->text() );
  }

  delete wpu;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editRemoveLine()
{
  if( !bmlc_ ) { return; }
  if( !browser_->removeBeamline( bmlc_ ) )
  { 
    contextList_.remove( bmlc_ );

    bmlc_ = BmlContextPtr();
    p_currQBmlRoot_ = 0;
  }
  else
  { QMessageBox::information( 0, "CHEF: ERROR",
                              "Operation not successful." );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::horTuneCtrl()
{
 
 if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  ActionOnElm action = boost::bind( &CHEFGUI::buildHTuneCircuit, this, _1);             

  if( bmlc_->isTreatedAsRing() ) {
      testFC( action );
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::verTuneCtrl()
{

  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  ActionOnElm action = boost::bind( &CHEFGUI::buildVTuneCircuit, this, _1);             

  if( bmlc_->isTreatedAsRing() ) {
     testFC(action);
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::horChromCtrl()
{

 if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  ActionOnElm action = boost::bind( &CHEFGUI::buildHChromCircuit, this, _1);            

  if( bmlc_->isTreatedAsRing() ) {
     testFC(action);
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::verChromCtrl()
{

  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  ActionOnElm action = boost::bind( &CHEFGUI::buildVChromCircuit, this, _1);            

  if( bmlc_->isTreatedAsRing() ) {
      testFC( action );
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::buildHTuneCircuit(  ElmPtr elm ) 
{
  bmlc_->addHTuneCorrector( elm );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::buildVTuneCircuit(  ElmPtr elm ) 
{
  bmlc_->addVTuneCorrector( elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::buildHChromCircuit(  ElmPtr elm ) 
{
 bmlc_->addHChromCorrector( elm );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::buildVChromCircuit(  ElmPtr elm ) 
{
  bmlc_->addVChromCorrector(elm);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editCondense()
{

  DriftEliminator de;

  if (!bmlc_)
  {
     QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
     return;
  };

  bmlc_->accept(de);

  BmlPtr bmlPtr( de.beamlinePtr()->Clone() ) ;
  bmlc_ = BmlContextPtr( new BeamlineContext( bmlc_->getParticle(), *bmlPtr) );
  contextList_.push_front( bmlc_ );

  emit new_beamline();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editNewOrder()
{

#if 0 
----------------------------------------------------------------------------------------
  // One quick test ...
  if( !p_clickedQBml_ ) {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "A single beamline element must be chosen first." );
    return;
  }

  // Locate the element and its root beamline
  QBmlElmt* qbmlElPtr = dynamic_cast<QBmlElmt*>(p_clickedQBml_);

  // This test should be made more flexible after
  //   allowing non-flat beamlines to be processed.

  if( !qbmlElPtr ) {
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function void CHEF::editNewOrder():"
           "\nFailure: beamline element not chosen correctly."
           "\nOperation will abort.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }


  ConstElmPtr elmntPtr = qbmlElPtr->cheatElementPtr();

  QBmlRoot* theRoot = const_cast<QBmlRoot*>(p_clickedQBml_->topBmlParent());

  // Invoke this slot to reset the current settings.

  ConstBmlContextPtr contextPtr = theRoot->cheatContextPtr();

  set_p_clickedContext( boost::const_pointer_cast<BeamlineContext>(contextPtr), theRoot );
  ConstBmlPtr bmlPtr = boost::static_pointer_cast<beamline const>(bmlc_->cheatBmlPtr());

  // This restriction should be removed ... but it isn't:
  //   a test for a flat beamline.
  int bmlLevel = bmlPtr->depth();
  if( bmlLevel != 0 ) {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "SORRY: Current implementation requires flat beamline." );
    return;
  }

  // Do the operation.

  if( 0 == (browser_->removeBeamline( boost::const_pointer_cast<BeamlineContext>(contextPtr) )) ) {

    boost::const_pointer_cast<beamline>(bmlPtr)->startAt(elmntPtr);
    boost::const_pointer_cast<BeamlineContext>(contextPtr)->clear();

    emit new_beamline();
  }
  else { 
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function void CHEF::editNewOrder():"
           "\nFailure: Unable to remove old beamline."
           "\nOperation will abort.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }

#endif
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editMergeQuads()
{
  QuadEliminator qe;
  bmlc_->accept(qe);

  BmlPtr bmlPtr( qe.beamlinePtr()->Clone() );
  bmlc_ = BmlContextPtr( new BeamlineContext( bmlc_->getParticle(),
                                       *bmlPtr ) );
  contextList_.push_front( bmlc_ );

  emit new_beamline();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editMergeBends()
{
  BendEliminator qe;
  bmlc_->accept(qe);

  BmlPtr bmlPtr( qe.beamlinePtr()->Clone() );
  bmlc_ = BmlContextPtr( new BeamlineContext( bmlc_->getParticle(),
                                       *bmlPtr ) );
  contextList_.push_front( bmlc_ );

  emit new_beamline();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::toolMisalign()
{
  if( !bmlc_ ) {
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

        new QLabel( " Roll [mrad]: ", qhb1 );

        QLineEdit* qle = new QLineEdit( "0.0", qhb1 );

      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "OK", qhb2 );
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

  QString qstr( "CHEFGUI: " );
  qstr += bmlc_->Name();
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
        int n = bmlc_->processElements( actn );

        QString numString;
        numString.setNum(n);
        numString += " CF_rbends were rolled.";
        QMessageBox::information( 0, "CHEF: INFO", numString );
      }

      else if( quadPtr == qbg->selected() ) {
        RollAccumulator actn( "quadrupole", rollAngle );
        int n = bmlc_->processElements( actn );

        QString numString;
        numString.setNum(n);
        numString += " quadrupoles were rolled.";
        QMessageBox::information( 0, "CHEF: INFO", numString );
      }

    }
  }

  delete wpu;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editAlignBends()
{
  // This menu function changes the beamline in place
  // without creating a new one.

  ConstElmPtr  selected = browser_->getSelectedElement(bmlc_);
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
      
      CFRbendPtr prototype( boost::static_pointer_cast<CF_rbend const>(selected)->Clone() );

      Particle particle (  bmlc_->getParticle() );
      particle.SetReferenceMomentum( bmlc_->Momentum() );

        // This line is almost certainly unnecessary.
      particle.setStateToZero();

      // ************ FIXME ! --- gettPoleFaceAngle() ??? *****/
      //particle.set_npx( ( particle.Momentum()/particle.ReferenceMomentum() )
      //               * sin( prototype->getPoleFaceAngle() ) );

      double delta = ( prototype->AdjustPosition( particle) );


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
        bmlc_->setElmAlignment( prototype->Alignment(), cf );
      }

      delete wpu;
    }

    else
    {
      QMessageBox::information( 0, "CHEF: INFO",
                                "Selected element is not CF_rbend." );
    }

  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editFlatten()
{
  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
    return;
  }

  // Create a cloned, flattened beamline
  beamline  tempbml = bmlc_->flatten();
  BmlPtr bmlPtr( tempbml.Clone() );
    //   This step is necessary,
    //   because the same objects exist both
    //   the original and flattened beamlines.
    //   Does not delete beamline elements.

  bmlc_ = BmlContextPtr ( new BeamlineContext( bmlc_->getParticle(), *bmlPtr ) );
  contextList_.push_front( bmlc_ );

  emit new_beamline();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void CHEFGUI::pushParticles()
{

  DistributionWidget* dwPtr
    = new DistributionWidget( *(bmlc_), 0, 0, Qt::WDestructiveClose );
  dwPtr->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::toolsGenerateBunch()
{
  static DialogGenerateBunch* dialog_generate_bunch = new DialogGenerateBunch(this,"Generate Particle Bunch", 0 );
  dialog_generate_bunch->show(); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::toolsDisplayBunch()
{
  bunch_ = std::auto_ptr<ParticleBunch>( new ParticleBunch( Positron(), 2048, 1.0 ) );
  bunch_->populateGaussian ( ParticleBunch::x_npx, 0.001, 0.001, 0.0);
  bunch_->populateGaussian ( ParticleBunch::y_npy, 0.001, 0.001, 0.0);

  CHEFPlotMain* plot =  new CHEFPlotMain( centralWidget_ , "Bunch X-Y projection", Qt::WDestructiveClose );
  BunchData data ( *bunch_ );
  plot->addData( data );
  plot->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editEditElement()
{
#if  0

  // The code in this method was patterned after that
  // in CHEF::editNewOrder()

  // One quick test ...
  if( 0 == p_clickedQBml_ ) {
    QMessageBox::warning( 0, "CHEF: WARNING", 
                          "A single beamline element must be chosen first." );
    return;
  }

  // Locate the element and its root beamline
  QBmlElmt* qbmlElPtr = dynamic_cast<QBmlElmt*>(p_clickedQBml_);

  // This test should be made more flexible after
  //   allowing non-flat beamlines to be processed.
  if( 0 == qbmlElPtr ) {
    // Allow modifying the beamline only
    if(bmlc_ ) {
      BmlContextPtr contextPtr = bmlc_;
      if( 0 == (browser_->removeBeamline( contextPtr )) ) {
        editDialog edg;
        edg._contextPtr = contextPtr;
        BmlPtr bmlPtr = boost::const_pointer_cast<beamline>(bmlc_->cheatBmlPtr() );
        bmlPtr->accept(edg);
        contextPtr->clear();
        emit new_beamline();
        return;
      }
    }
    else {
      std::ostringstream uic;
      uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
             "\nIn function void CHEF::editElement():"
             "\nFailure: beamline element not chosen correctly."
             "\nOperation will abort.";
      QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
      return;
    }
  }
  ConstElmPtr elmntPtr = qbmlElPtr->cheatElementPtr();
  QBmlRoot* theRoot = const_cast<QBmlRoot*>(p_clickedQBml_->topBmlParent());

  // Invoke this slot to reset the current settings.
  BmlContextPtr contextPtr = boost::const_pointer_cast<BeamlineContext>(theRoot->cheatContextPtr());
  set_p_clickedContext( contextPtr, theRoot );
  ConstBmlPtr  bmlPtr = bmlc_->cheatBmlPtr();

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
  if( 0 == (browser_->removeBeamline( contextPtr )) ) {
    editDialog edg;
    edg._contextPtr = contextPtr;
    boost::const_pointer_cast<bmlnElmnt>(elmntPtr)->accept(edg);
    contextPtr->clear();
    emit new_beamline();
  }
  else { 
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function void CHEF::editNewOrder():"
           "\nFailure: Unable to remove old beamline."
           "\nOperation will abort.";
    QMessageBox::critical( 0, "CHEF: ERROR", uic.str().c_str() );
    return;
  }
#endif
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editAlign()
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
        QPushButton* okayBtn = new QPushButton( "OK", qhbp );
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
     
      std::list<ElmPtr> theOnes = browser_->findAllSelected(dynamic_cast<QBmlRoot*>(p_clickedQBml_));

      ElmPtr elementPtr;

      for ( std::list<ElmPtr>::iterator it  = theOnes.begin();  
                                        it != theOnes.end(); ++it )
      { 
        if( BmlUtil::isSpace(**it) ) {
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
      }

     // Do the alignment

      AlignVisitor euclid( euclidData, theOnes );
      if( bmlc_) { bmlc_->accept( euclid ); }
     }
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editAlignData()
{

#if 0
  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
    return;
  }

  // Store survey data read from a file.
  QString s = QFileDialog::getOpenFileName( QString::null, 
                   "Data file (*.dat *.txt);;All files (*.*)" );
  if( s.isEmpty() ) { return; }
  std::ifstream dataStream( s );


  std::list< boost::shared_ptr<DataStore> > dataBag;

  char oneLine[1024];
  if( dataStream ) {
    dataStream.getline(oneLine,1000);
    while( !dataStream.eof() ) {
      std::istringstream lineStream( oneLine );
      boost::shared_ptr<DataStore> dtmPtr( new DataStore );
      lineStream >> dtmPtr->az >> dtmPtr->x1 >> dtmPtr->x2 >> dtmPtr->x3;
      dataBag.push_back(dtmPtr);
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


  Vector r(3);
  std::vector<Vector> rawData;

  for ( std::list<boost::shared_ptr<DataStore> >::iterator it = dataBag.begin(); it != dataBag.end(); ++it ) 
  {                   
    r[0] = (*it)->x1;
    r[1] = (*it)->x2;
    r[2] = (*it)->x3;
    rawData.push_back(r);
  }

  sqlite::connection db ( "./" + bmlc_->Name() + ".db" );

  SurveyMatcher sm( rawData, *bmlc_, db );


  // Preparing the plot
  // Load the shared arrays

  std::vector<double> azimuthArray;
  std::vector<double>       xArray;
  std::vector<double>       yArray;
  std::vector<double>       zArray;

  int i =0;
  for ( std::list<boost::shared_ptr<DataStore> >::iterator it = dataBag.begin(); it != dataBag.end(); ++it, ++i ) 
  {

      azimuthArray.push_back( (*it)->az );
      r = sm.getLocalDisplacement(i);
      xArray.push_back( r[0] );
      yArray.push_back( r[1] );
      zArray.push_back( r[2] );
  }

  CurveData  dxCurve( azimuthArray.size(), "dx"  );
  CurveData  dyCurve( azimuthArray.size(), "dy"  );
  CurveData  dzCurve( azimuthArray.size(), "dz"  );

  CurveData::iterator itx =dxCurve.begin();
  CurveData::iterator ity =dyCurve.begin();
  CurveData::iterator itz =dzCurve.begin();
  
  for (int i=0; i< azimuthArray.size(); ++i) { 
    *(itx++) = CurveData::Point( azimuthArray[i], xArray[i] ); 
    *(ity++) = CurveData::Point( azimuthArray[i], yArray[i] ); 
    *(itz++) = CurveData::Point( azimuthArray[i], zArray[i] ); 
  }
    
//dxCurve->setPen( QPen( "black", 1, Qt::SolidLine ) );
//dyCurve->setPen( QPen( "red", 1, Qt::SolidLine ) );
//dzCurve->setPen( QPen( "green",  1, Qt::SolidLine ) );

  dxCurve.setAxes( CurveData::xBottom, CurveData::yLeft );
  dyCurve.setAxes( CurveData::xBottom, CurveData::yLeft );
  dzCurve.setAxes( CurveData::xBottom, CurveData::yRight );


  // Create a CHEFPlotData object for handing over to the plotter

  PlotData plotData;
  plotData.addCurve(dxCurve);
  plotData.addCurve(dyCurve);
  plotData.addCurve(dzCurve);
  plotData.setXLabel( "Arc Length [m]"         );
  plotData.setYLabel( CurveData::yLeft, "Data" );
  //plotData.setBeamline( bmlc_->cheatBmlPtr() );


  // Plot and display the results

  CHEFPlotMain* surveyPlotter = new CHEFPlotMain( 0, "Survey Plotter", Qt::WDestructiveClose );
  surveyPlotter->setCaption( "CR&P" );
  surveyPlotter->addData( plotData );
  surveyPlotter->show();

  #if 0
  DataAlignWidget* dawPtr 
    = new DataAlignWidget( *(bmlc_), 0, 0, Qt::WDestructiveClose );
  dawPtr->show();
  #endif

#endif

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editD2S()
{

  beamline* bp = DriftsToSlots( *bmlc_ );
  
  BmlPtr bmlPtr(bp);

  bmlc_ = BmlContextPtr (new BeamlineContext( bmlc_->getParticle(), *bmlPtr) );
  contextList_.push_front( bmlc_ );
  emit new_beamline();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

insDlgData CHEFGUI::insertionDialog() const
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
      /*QRadioButton* midPtr  = */ new QRadioButton( "Middle",     qbg );
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
  
  insDlgData ret;
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

  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editAddMarkers()
{
  insDlgData options = insertionDialog();

  if( options.accepted ) {
    // Preliminaries
    QString markerName( options.namePrefix );

    // Locate the root beamline
    QBmlRoot* theRoot = dynamic_cast<QBmlRoot*>(p_clickedQBml_);
    if( !theRoot ) {
      theRoot = const_cast<QBmlRoot*>(p_clickedQBml_->topBmlParent());
    }

    // Find selected elements
    std::list<ElmPtr> theChosenOnes = browser_->findAllSelected( theRoot );

    if( theChosenOnes.empty() ) {
      std::ostringstream uic;
      uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
             "\nIn function: void CHEFGUI::editAddMarkers():"
             "\nNo elements selected; no action will be taken.";
      QMessageBox::warning( 0, "CHEF: WARNING", uic.str().c_str() );
      return;
    }

    // Load BmlPtrLists, for use by lower level tools
    std::list<ElmPtr> theOnes;
    std::list<ElmPtr> insertions;

    for ( std::list<ElmPtr>::iterator it = theChosenOnes.begin(); it != theChosenOnes.end(); ++it )
    {
      theOnes.push_back(*it);
      insertions.push_back( ElmPtr( new marker(markerName.ascii() ) ));
    }

    // Create the new beamline with inserted markers included
 
    BmlPtr bmlPtr;

    if( options.upstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.0, insertions, theOnes, *bmlc_ );
    }
    else if( options.downstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 1.0, insertions, theOnes, *bmlc_ );
    }
    else {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.5, insertions, theOnes, *bmlc_ );
    }

    QString newName( bmlc_->Name() );
    newName += ".marked";
    bmlPtr->rename( newName.ascii() );
    bmlPtr->setMomentum( bmlc_->Momentum() );  // Probably unnecessary.


    // Generate a new BeamlineContext to handle the new line
    bool makeRing = bmlc_->isTreatedAsRing();
    bmlc_ = BmlContextPtr( new BeamlineContext( bmlc_->getParticle(), *bmlPtr) );

    if( makeRing ) { bmlc_->handleAsRing(); }
    else           { bmlc_->handleAsLine(); }
    contextList_.push_front( bmlc_ );

    // Finished!
    emit new_beamline();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editAddQtMons()
{

  insDlgData options = insertionDialog();

  if( options.accepted ) {
    QString markerName( options.namePrefix );

    // Locate the root beamline

    QBmlRoot* theRoot = dynamic_cast<QBmlRoot*>(p_clickedQBml_);
    if( 0 == theRoot ) {
      theRoot = const_cast<QBmlRoot*>(p_clickedQBml_->topBmlParent());
    }

    // Find selected elements

    std::list<ElmPtr> theOnes = browser_->findAllSelected( theRoot );

    if( theOnes.empty() ) {
      std::ostringstream uic;
      uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
             "\nIn function: void CHEFGUI::editAddQtMons():"
             "\nNo elements selected; no action will be taken.";
      QMessageBox::information( 0, "CHEF: WARNING", uic.str().c_str() );
      return;
    }

    std::list<ElmPtr> insertions;

    QtMonitorPtr qtmPtr;

    for ( std::list<ElmPtr>::iterator   it  = theOnes.begin();  
                                   it != theOnes.end(); ++it) {

      
      qtmPtr = QtMonitorPtr( new QtMonitor(markerName.ascii()) );
      qtmPtr->setStrength(5.0);  // to survive condensation
      insertions.push_back( qtmPtr );

    }

    // Create the new beamline with inserted monitors included


    BmlPtr bmlPtr;

    if( options.upstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.0, insertions, theOnes, *bmlc_ );
    }
    else if( options.downstream ) {
      bmlPtr = BmlUtil::cloneLineAndInsert( 1.0, insertions, theOnes, *bmlc_ );
    }
    else {
      bmlPtr = BmlUtil::cloneLineAndInsert( 0.5, insertions, theOnes, *bmlc_ );
    }

    // Register the azimuth at all QtMonitor locations.
    QtMonitor::setAzimuth( *bmlPtr );

    // Final steps

    QString newName( bmlc_->Name() );
    newName += ".monitored";
    bmlPtr->rename( newName.ascii() );
    bmlPtr->setMomentum( bmlc_->Momentum() );  // Probably unnecessary.

    // Generate a new BeamlineContext to handle the new line

    bool makeRing = bmlc_->isTreatedAsRing();

    bmlc_ = BmlContextPtr(  new BeamlineContext( bmlc_->getParticle(),
                                         *bmlPtr) );
    if( makeRing ) { bmlc_->handleAsRing(); }
    else           { bmlc_->handleAsLine(); }
    contextList_.push_front( bmlc_ );

    // Signal the new line
    emit new_beamline();
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editMode()
{

  if( 0 == bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  // Set up the query
  bool makeRing = false;
  QString message(bmlc_->Name());

  if( bmlc_->isRing() ) {
    if( bmlc_->isTreatedAsRing() ) {
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
    if( bmlc_->isTreatedAsRing() ) {
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
    bmlc_->clear();
    // "Yes" answer indicates change of mode.

    if( makeRing ) {
      bmlc_->handleAsRing();
    }
    else {
      bmlc_->handleAsLine();
    }

    emit modeChanged( bmlc_ );
  }

  delete wpu;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editPartition()
{

  // Locate the root beamline

  QBmlRoot* theRoot = dynamic_cast<QBmlRoot*>(p_clickedQBml_);

  if( !theRoot ) {
    theRoot = const_cast<QBmlRoot*>(p_clickedQBml_->topBmlParent());
  }

  // Find selected elements
 
  std::list<ElmPtr> theOnes = browser_->findAllSelected( theRoot );

  if( theOnes.empty() ) {
    std::ostringstream uic;
    uic << "File " << __FILE__ << ", line " << __LINE__ << ":"
           "\nIn function: void CHEFGUI::editPartition():"
           "\nSelection list is empty; no action taken.";
    QMessageBox::information( 0, "CHEF: WARNING", uic.str().c_str() );
    return;
  }


  // Create the new beamline with split elements


  QString newName( bmlc_->Name() );
  newName += ".split";

  BmlPtr bmlPtr( new beamline( newName.ascii() ) );
  bmlPtr->setMomentum( bmlc_->Momentum() );

  ElmPtr spa, spb;

  std::list<ElmPtr>::iterator it   = theOnes.begin();
  
  for ( beamline::const_deep_iterator dit  = bmlc_->deep_begin();  
                                      dit != bmlc_->deep_end();  ++dit ) 
  {
    if( (*dit) == (*it) ) 
    {
      (*it)->Split( 0.5, spa, spb );
      bmlPtr->append(spa);
      bmlPtr->append(spb);
      ++it;
    }
    else 
    {
      bmlPtr->append( ElmPtr( (*dit)->Clone() ) );
    }
  }


  // Generate a new BeamlineContext to handle the new line

  bool makeRing = bmlc_->isTreatedAsRing();
  bmlc_ = BmlContextPtr( new BeamlineContext( bmlc_->getParticle(), *bmlPtr) );

  if( makeRing ) { bmlc_->handleAsRing(); }
  else           { bmlc_->handleAsLine(); }
  contextList_.push_front( bmlc_ );

  emit new_beamline();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::editPartAndSect()
{


  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }


  BmlPtr bmlPtr( bmlc_->beamline::Clone() );

  double momentum   = bmlPtr->Momentum();

  // Insert equally spaced markers throughout the model.
  double bmlLength = 0.0;

  for ( beamline::deep_iterator it = bmlPtr->deep_begin(); it != bmlPtr->deep_end(); ++it) 
  { 
    bmlLength += (*it)->Length();
  }

  // Dialog to obtain number of markers
  // ----------------------------------
  int numberOfSectors = 5;

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
    QHBox* qhb1 = new QHBox( qvb );
    ////QLabel* qlbh = new QLabel( "Number of partitions", qhb1 );
        QLineEdit* qle = new QLineEdit( "5", qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "OK", qhb2 );
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

  BmlPtr splitBmlPtr( new beamline( "Sectorized beamline" ) );

  splitBmlPtr->setMomentum( momentum);

  if( returnCode == QDialog::Accepted ) {

    bool ok;
    numberOfSectors = (qle->text()).toInt( &ok);

#if 0
FIXME
    if( ok && ( numberOfSectors > 1 )) {
      // Insert equally spaced markers throughout
      // the bml model.

      marker* spaceCharge[ numberOfSectors + 1 ];

      double markerInterval = bmlLength / ((double) numberOfSectors);
      double insertionPoint = markerInterval;

      Particle particle( bmlc_->getParticle()) );
      particle.setStateToZero();

      spaceCharge[ 0 ] = new marker;

      for( int i = 1; i < numberOfSectors; i++ ) {
        spaceCharge[i] = new marker;
        insl.Append( new InsertionListElement( insertionPoint, spaceCharge[i] ));
        insertionPoint += markerInterval;
      }
      spaceCharge[ numberOfSectors ] = new marker;

      double s = 0.0;

      bmlPtr->insert( spaceCharge[0] );
      try {
        bmlPtr->InsertElementsFromList( s, insl );
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

        // OBSOLETE bmlPtr->eliminate();
        // OBSOLETE splitBmlPtr->eliminate();

        return;
      }
      bmlPtr->append( spaceCharge[numberOfSectors] );

#endif
      // Create a temporary Jet environment
      // ----------------------------------
      Jet__environment_ptr  formerJetEnv  = Jet__environment::getLastEnv();
      JetC__environment_ptr formerJetCEnv = JetC__environment::getLastEnv();

      Jet__environment::BeginEnvironment( order );

      std::auto_ptr<coord>  x( new coord(0.0) );
      std::auto_ptr<coord>  y( new coord(0.0) );
      std::auto_ptr<coord>  z( new coord(0.0) );
      std::auto_ptr<coord> px( new coord(0.0) );
      std::auto_ptr<coord> py( new coord(0.0) );
      std::auto_ptr<coord> pz( new coord(0.0) );

      Jet__environment::EndEnvironment();
      JetC__environment::setLastEnv( env_ ); // implicit conversion


      // Sectorize between the partition markers.
      // ----------------------------------------

      Particle particle(bmlc_->getParticle() );

      particle.SetReferenceMomentum( bmlc_->Momentum() );
        // This line is almost certainly unnecessary.
      particle.setStateToZero();

      for( int i = 0; i < numberOfSectors; ++i) {

        JetParticle jetparticle(particle);
#if 0
FIXME
        splitBmlPtr->append( bmlPtr->MakeSector( *(spaceCharge[i]), *(spaceCharge[i+1]), order, *jpPtr  ) );
        splitBmlPtr->append( spaceCharge[i+1]->Clone() );
#endif
      }


      // Restore former environment...
      // -----------------------------

      Jet__environment::setLastEnv( formerJetEnv );
      JetC__environment::setLastEnv( formerJetCEnv );

  }
  
  delete wpu;

  bmlc_ = BmlContextPtr( new BeamlineContext( bmlc_->getParticle(), *splitBmlPtr ) );
  contextList_.push_front( bmlc_ );

  emit new_beamline();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::helpContents()
{
  assistanthelp_->openAssistant();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::helpAbout()
{

  QWidget* helpabout = new About( this,"About CHEF", Qt::WDestructiveClose );
  helpabout->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void CHEFGUI::slot_contextGenerated( BmlContextPtr x )
{
  if( x ) contextList_.push_front( x );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::computeCS4D()
{ 

   if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
   }

   QWidget* plot = 0;

   if( bmlc_->isTreatedAsRing())  {
       plot = command_computeEigenmodes_( centralWidget(), bmlc_ ); 
   } 
   else {
       initCondDialogLF_->setInitCond( bmlc_->getInitial() );

       if( QDialog::Accepted == initCondDialogLF_->exec() ) 
       {
           CSLattFuncs ic = initCondDialogLF_->getInitCond();
           plot = command_propagateEigenmodes_( centralWidget(), bmlc_, ic ); 
      }
   }

   if (plot) { plot->show(); }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::periodicET()
{ 

   if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
   }
   if( !(bmlc_->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

   QWidget* plot = command_computeEdwardsTeng_( centralWidget(), bmlc_ ); 

   if (plot) { plot->show(); }


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::periodicMoments()
{

   if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
   }
   if( !(bmlc_->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

   QWidget* plot = command_computeMoments_( centralWidget(), bmlc_ ); 

   if (plot) { plot->show(); }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::propagateMoments()
{ 

  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( bmlc_->isTreatedAsRing() ) {
    QMessageBox::information( 0, "CHEF: ERROR",
      "Moments are \"pushed\" through beamlines only."
      "\nTry fixing with Edit/Mode function." );
    return;
  }

  QWidget* plot = 0;

  if( QDialog::Accepted == initCondDialogMoments_->exec() ){
    CSLattFuncs initialConditions = initCondDialogMoments_->getInitCond();

    plot = command_propagateMoments_( centralWidget(), bmlc_, initialConditions); 

  }

  if (plot) { plot->show(); }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::computeReferenceOrbit()
{

  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

   static DialogInitialState* dialog = 0;



  bmlc_->clear(); //  *************** FIXME 

  QWidget* plot = 0;
  if( bmlc_->isTreatedAsRing() ) {

       plot = command_computeReferenceOrbit_( centralWidget(), bmlc_ ); 

  }
  else {     

     if (!dialog) { dialog = new  DialogInitialState( 0," "); }
     dialog->show();

     CSLattFuncs initial = bmlc_->getInitial();

     Vector state = dialog->values();
     
     state[0] = initial.reference_orbit.x;  
     state[3] = initial.reference_orbit.xp;
     state[1] = initial.reference_orbit.y; 
     state[4] = initial.reference_orbit.yp;

     dialog->setValues( state );

     if( QDialog::Accepted == dialog->exec() ) {

       Vector state = dialog->values();

       initial = bmlc_->getInitial();
       
       initial.reference_orbit.x  = state[0]; 
       initial.reference_orbit.xp = state[3];
       initial.reference_orbit.y  = state[1];
       initial.reference_orbit.yp = state[4];

       plot = command_propagateReferenceOrbit_( centralWidget(), bmlc_ , initial ) ; 
     }    

  }
  
  if (plot) { plot->show(); }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::computeDispersion()
{ 

  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  
  QWidget* plot = 0;
  if( bmlc_->isTreatedAsRing() ) {

       plot = command_computeDispersion_( centralWidget(), bmlc_ ); 

  }
  else {     

     initCondDialogDisp_->setInitCond( bmlc_->getInitial() );

     if( QDialog::Accepted == initCondDialogDisp_->exec() ) {
       CSLattFuncs ic = initCondDialogDisp_->getInitCond();
       plot = command_propagateDispersion_( centralWidget(), bmlc_ , ic); 
     }    

  }
  
  if (plot) { plot->show(); }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::computeTwiss()
{ 

  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( bmlc_->isTreatedAsRing() ) {

         command_computePeriodicTwiss_( bmlc_ ); 
  }
  else {

     initCondDialogLF_->setInitCond( bmlc_->getInitial() );

       if( QDialog::Accepted == initCondDialogLF_->exec() ) 
       {
          CSLattFuncs ic = initCondDialogLF_->getInitCond();
          command_computePropagateTwiss_( bmlc_ , ic ); 

       } 
       else { 
         return; // initial condition dialog not accepted
       }    
  }
  
  command_plotTwiss_ ( centralWidget_, bmlc_->dbConnection() );


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::launchTrack()
{
  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  if( !(bmlc_->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

  tracker_ = new Tracker( bmlc_, centralWidget_, "Tracker", Qt::WDestructiveClose);
  tracker_->setCaption( "CHEF:: Phase Space Tracking" );


  // Centrally locate the track widget.
  // ----------------------------------
  tracker_->adjustSize();

  int cw = centralWidget_->width();
  int ch = centralWidget_->height();
  int  w = tracker_->width();
  int  h = tracker_->height();

  if( w < cw && h < ch ) {
    tracker_->parentWidget()->move( (cw-w)/2, (ch-h)/2 );  // Accessing the parent
  }                                                            // is required by the
  else if( h < ch ) {                                          // MDI interface, that
    tracker_->parentWidget()->move( 0, (ch-h)/2 );         // is, the QWorkspace.
  }
  else if( w < cw ) {
    tracker_->parentWidget()->move( (cw-w)/2, 0 );
  }
  else {
    tracker_->parentWidget()->move( 0, 0 );
  }

  tracker_->show();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::launchRayTrace()
{
  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  tracer_ = new RayTrace(bmlc_, centralWidget_, "RayTrace", Qt::WDestructiveClose );
  tracer_->setCaption( "CHEF:: Orbit Trace" );


  // Centrally locate the trace widget.
  // ----------------------------------
  tracer_->adjustSize();

  int cw = centralWidget_->width();
  int ch = centralWidget_->height();
  int  w = tracer_->width();
  int  h = tracer_->height();

  if( w < cw && h < ch ) {
    tracer_->parentWidget()->move( (cw-w)/2, (ch-h)/2 );  // Accessing the parent
  }                                                            // is required by the
  else if( h < ch ) {                                          // MDI interface, that
    tracer_->parentWidget()->move( 0, (ch-h)/2 );         // is, the QWorkspace.
  }
  else if( w < cw ) {
    tracer_->parentWidget()->move( (cw-w)/2, 0 );
  }
  else {
    tracer_->parentWidget()->move( 0, 0 );
  }

  tracer_->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::launchDilution()
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::launchSiteVu()
{
  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  viewer_ = new SiteViewer( bmlc_ , centralWidget_, "SiteViewer", Qt::WDestructiveClose );
  // Must be deleted by itself.
  viewer_->setCaption( QString("CHEF:: Site Viewer")+
                           QString("    ")+
                           QString(bmlc_->Name()) );

  // Centrally locate the site widget.
  // ----------------------------------
  // viewer_->adjustSize();    //    !! Must not do this here, for some reason. !!

  // int a = int( sqrt( double( (viewer_->parentWidget()->width())*(viewer_->parentWidget()->height()) )/3.0 ) );
  // viewer_->resize(a,a);                    // Order is important
  // viewer_->adjustSize();  // NEW           // These two interfere in some bizarre way.

  int cw = centralWidget_->width();
  int ch = centralWidget_->height();
  int  w = viewer_->width();
  int  h = viewer_->height();

  #if 1
  if( w < cw && h < ch ) {                                     // ??? THIS WORKS ???
    viewer_->move( (cw-w)/2, (ch-h)/2 );                   
  }                                                            
  else if( h < ch ) {                                          
    viewer_->move( 0, (ch-h)/2 );                          
  }
  else if( w < cw ) {
    viewer_->move( (cw-w)/2, 0 );
  }
  else {
    viewer_->move( 0, 0 );
  }
  #endif
  #if 0
  if( w < cw && h < ch ) {                                     // ??? BUT THIS DOES NOT ???
    viewer_->parentWidget()->move( (cw-w)/2, (ch-h)/2 );   // Accessing the parent
  }                                                            // is required by the
  else if( h < ch ) {                                          // MDI interface, that
    viewer_->parentWidget()->move( 0, (ch-h)/2 );          // is, the QWorkspace.
  }
  else if( w < cw ) {
    viewer_->parentWidget()->move( (cw-w)/2, 0 );
  }
  else {
    viewer_->parentWidget()->move( 0, 0 );
  }
  #endif

  viewer_->show();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::launch_browser()
{


  browser_->clearSelection();

  if( bmlc_) browser_->displayBeamline( bmlc_ ); // display the beamline only if it exists;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::tuneCtrl()
{
  if( 0 == bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( !(bmlc_->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }
  // --------------------------------------------------------------

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QButtonGroup* qbg =
      new QButtonGroup( 2, Qt::Horizontal, QString("Mode"), qvb );
        QRadioButton* absPtr  = new QRadioButton( "Absolute",   qbg );
        QRadioButton* relPtr  = new QRadioButton( "Relative",   qbg );
        relPtr->setChecked( true );
      qbg->setExclusive( true );
      qbg->setMargin(5);
      qbg->adjustSize();
      
      QHBox* qhb1 = new QHBox( qvb );
      ///QLabel* qlbh = new QLabel( "Delta H tune", qhb1 );
        QLineEdit* qleh = new QLineEdit( "0.0", qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
      ////QLabel* qlbv = new QLabel( "Delta V tune", qhb2 );
        QLineEdit* qlev = new QLineEdit( "0.0", qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "OK", qhb3 );
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
      if( relPtr == qbg->selected() ){
        if( (0. != deltaNuH) || (0. != deltaNuV) ) {
          if( 0 != (returnCode = bmlc_->changeTunesBy( deltaNuH, deltaNuV )) ) {
            (*pcerr) << "*** WARNING *** File "
                 << __FILE__
                 << ", Line "
                 << __LINE__
                 << ": Tune adjustment returned error condition "
                 << returnCode
                 << std::endl;
            QMessageBox::critical( 0, "CHEF", "Tune adjustment error." );
          }
        }
      }
      else if( absPtr == qbg->selected() ){
        if( 0 != (returnCode = bmlc_->changeTunesTo( deltaNuH, deltaNuV )) ) {
          (*pcerr) << "*** WARNING *** File "
               << __FILE__
               << ", Line "
               << __LINE__
               << ": Tune adjustment returned error condition "
               << returnCode
               << std::endl;
          QMessageBox::critical( 0, "CHEF", "Tune adjustment error." );
        }
      }
    }
  }

  delete wpu;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::chromCtrl()
{

  if( !bmlc_ ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }
  if( !(bmlc_->isTreatedAsRing()) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
    return;
  }

  // --------------------------------------------------------------

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );
      QHBox* qhb1 = new QHBox( qvb );
        QLineEdit* qleh = new QLineEdit( "0.0", qhb1 );
      qhb1->setMargin(5);
      qhb1->setSpacing(3);
      qhb1->adjustSize();

      QHBox* qhb2 = new QHBox( qvb );
        QLineEdit* qlev = new QLineEdit( "0.0", qhb2 );
      qhb2->setMargin(5);
      qhb2->setSpacing(3);
      qhb2->adjustSize();

      QHBox* qhb3 = new QHBox( qvb );
        QPushButton* okayBtn = new QPushButton( "OK", qhb3 );
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

  wpu->setCaption( "CHEF: Chromaticity Adjuster" );
  wpu->adjustSize();

  int returnCode = wpu->exec();

  if( returnCode == QDialog::Accepted ) {
    bool okh, okv;
    double deltaChrH = (qleh->text()).toDouble( &okh);
    double deltaChrV = (qlev->text()).toDouble( &okv);
    if( okh && okv ) {
      if( (0. != deltaChrH) || (0. != deltaChrV) ) {
        try 
        {
          if( 0 != (returnCode = bmlc_->changeChromaticityBy( deltaChrH, deltaChrV )) ) {
            (*pcerr) << "*** WARNING *** File "
                 << __FILE__
                 << ", Line "
                 << __LINE__
                 << ": Chromaticity adjustment returned error condition "
                 << returnCode
                 << std::endl;
            QMessageBox::information( 0, "CHEF",
                        "Chromaticity adjustment error." );
          }
        }
        catch (GenericException& e)
        {
          QMessageBox::information( 0, "CHEF: ERROR", e.what() );
          delete wpu;
          return;
        }
      }
    }
  }

  delete wpu;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::set_p_clickedContext( BmlContextPtr x, QBmlRoot* y )
{
  bmlc_   = x;
  p_currQBmlRoot_ = y;
  p_clickedQBml_  = y;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::set_p_clickedQBml( QBml* x )
{
  p_clickedQBml_ = x;
}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::do_nothing()

{
  QMessageBox::information( 0, "CHEF",
                            "Sorry. This function is not implemented." );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::showEvent(QShowEvent* event) {

  // The initial size and positions of all windows are set here.
  // Receiving a QShowEvent indicates that the main window has been displayed.

  if ( event->spontaneous() ) return QMainWindow::showEvent(event);

  QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  browser_->setSizePolicy(policy);
  browser_->show();

  messages_->setSizePolicy(policy);
  messages_->show();

  interpreter_->setSizePolicy(policy);
  interpreter_->show();


  devices_->setSizePolicy(policy);
  devices_->show();

  windowsDefaultLayout();

  return QMainWindow::showEvent(event);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::processFilter( BoolNode const& query )
{

  foundList_.clear();

  if( browser_->findElement( p_clickedQBml_, query, foundList_ ) ) {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Operation not successful." );
    return;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::testFC( ActionOnElm& actfcn ) 
{

  QListViewItem* fc = browser_->firstChild();

  while( fc ) 
  {
    if( typeid(*fc) == typeid(QBmlRoot)) 
    {
       traverseTree( dynamic_cast<QBmlRoot*>(fc), actfcn );
    }

    fc = fc->nextSibling();
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::traverseTree( QBmlRoot const* x, ActionOnElm& actfcn ) 
{

  // Applies actfcn on selected elements. If element is a beamline, 
  // the function is called recursively.
 
  QListViewItem* fc = x->firstChild();

  while( fc )
  {
    if(typeid(*fc) == typeid(QBmlElmt)) {
      if( fc->isSelected() ) {
        actfcn( dynamic_cast<QBmlElmt*>(fc)->cheatElementPtr() );
      }
    }

    else if(typeid(*fc) == typeid(QBmlRoot)) {
      traverseTree( dynamic_cast<QBmlRoot*>(fc), actfcn );
    }

    fc = fc->nextSibling();
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::windowsMessagesAction_toggled( bool set )
{
  if (set)
    messages_->show();
  else
    messages_->hide();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::windowsTree_BrowserAction_toggled( bool set )
{
  if (set)
    browser_->show();
  else
    browser_->hide();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::windowsInterpreterAction_toggled( bool  set){

  if (set) {
    interpreter_->show();
  }
  else {
    interpreter_->hide();
  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::windowsDevicesAction_toggled( bool  set)
{
  if (set) {
    devices_->show();
  }
  else {
    devices_->hide();
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::windowsCascade() 
{
  dynamic_cast<AppWorkspace*>(centralWidget_)->cascade();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::windowsMinimizeAll()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void CHEFGUI::windowsTile()
{
  dynamic_cast<AppWorkspace*>(centralWidget_)->tile();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::windowsDefaultLayout()
{
  dynamic_cast<AppWorkspace*>(centralWidget_)->defaultLayout();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::disableMenus()
{
  enableMenus(false);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::enableMenus()
{
 enableMenus(true);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::enableMenus( bool set )
{

//  Enable/Disable actions that do/do not make sense if
//  at least one beamline is/is not defined;

    calculationsReferenceOrbitAction->setEnabled(set);
    calculationsLatticeFunctionsUncoupledAction->setEnabled(set);
    calculationsLatticeFunctionsCoupledAction->setEnabled(set);
    calculationsDispersionAction->setEnabled(set);

    toolsSite_ViewerAction->setEnabled(set);
    toolsTrackAction->setEnabled(set);
    toolsTraceAction->setEnabled(set);
    calculationsDispersionAction->setEnabled(set);
    calculationsEmittanceDilutionAction->setEnabled(set);
    controlsBuildHTuneCctAction->setEnabled(set);
    controlsBuildVTuneCctAction->setEnabled(set);
    controlsAdjustTuneAction->setEnabled(set);
    controlsBuildHChromCctAction->setEnabled(set);
    controlsBuildVChromCctAction->setEnabled(set);
    controlsAdjustChromAction->setEnabled(set);
    toolsGenerateBunchAction->setEnabled(set);

    editCopyLineAction->setEnabled(set);
    editRemoveLineAction->setEnabled(set);
    editRenameLineAction->setEnabled(set);
    editInsertMarkersAction->setEnabled(set);
    editModeAction->setEnabled(set);
    editPartitionElementsAction->setEnabled(set);
    editCondenseAction->setEnabled(set);
    editFlattenAction->setEnabled(set);
    editPermuteAction->setEnabled(set);
    editModifyAction->setEnabled(set);
    editAlignAction->setEnabled(set);
    editAlignDataAction->setEnabled(set);
    editAlignBendsAction->setEnabled(set);


    editSelectFilterAction->setEnabled(set);
    editSelectallAction->setEnabled(set);
    editSelectnoneAction->setEnabled(set);
    editSelectLineAction->setEnabled(set);

    editMergeEquivalentQuadsAction->setEnabled(set);
    editMergeEquivalentBendsAction->setEnabled(set);
    editConvertToSlotsAction->setEnabled(set);
    editInsertMonitorAction->setEnabled(set);
    editPartitionAndSectorizeAction->setEnabled(set);

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void CHEFGUI::exit()
{

  close();

  // exit(1); // this is for debugging (memory usage diags) only

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::devicesClearSelection()
{

  devices_->clearSelection();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::devicesRefreshAll()
{

  devicesRefreshAction->setEnabled(false);
  devices_->refreshAll();
  devicesRefreshAction->setEnabled(true);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::devicesAutoRefresh(bool set)
{
  devices_->setAutoRefresh(set);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::modeRing(bool  set)
{
  // obsolete ??
  // modeLineAction->setOn(!set);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::modeLine(bool  set )
{
  // obsolete ??
  // modeRingAction->setOn( !set);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::fileEditPython() {

  std::ifstream is;
  QString theText;
  char buffer[1024];

  QString s = QFileDialog::getOpenFileName( QString::null,
                                            "Script (python) (*.py )" );

  if( s.isEmpty() ) return;

  is.open(s);

   CF_Editor* editor = new CF_Editor( centralWidget_, "Python Editor", Qt::WDestructiveClose);
   editor->hide();


  while ( ! is.eof() )
  {
      is.getline(buffer,1024);
      theText += buffer;
      theText += "\n";
   }

   // the "magic numbers" used in the args for resize() and move() reflect the fact
   // that widget dimensions and positions *do not* include the frame. Since the decorations
   // are handled by the wmgr in X11 and since there is no universal reliable way of querying,
   // the wmgr,  there is no reliable way of obtaining info about the
   // decoration geometry.

   editor->setCaption(s);
   editor->setText( theText );
   editor->resize(centralWidget_->width()-browser_->width()-20, centralWidget_->height()-30);
   editor->move(browser_->width()+10, 0);
   editor->show();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::fileEditMAD8()
{
  std::ifstream is;
  QString theText;
  char buffer[1024];

  QString s = QFileDialog::getOpenFileName( QString::null,
                     "MAD (*.mad *.lat)");

  if( s.isEmpty() ) return;

   CF_Editor* editor = new CF_Editor( centralWidget_ , "MAD8 Editor", Qt::WDestructiveClose);
   //editor->setMarginLineNumbers(1,1);
   //editor->setLexer(0); // this should eventually be a MAD8 Lexer
   editor->hide();

  is.open(s);

  while ( ! is.eof() )
    {
      is.getline(buffer,1024);
      theText += buffer;
      theText += "\n";
    }

      editor->setCaption(s);
      editor->setText( theText );
      // the correction is for the dimensions **with** the frame.
      // width() is
      editor->resize(width()-browser_->frameGeometry().width()-20,
                     centralWidget_->height()-30);
      editor->move( browser_->frameGeometry().width()+10, 0);
      editor->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::databaseConnect() 
{ 
  dbconnect_dlg_->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::handleDBConnection( const DbConnectStruct& st )
{
 
  if ( default_db_ ) { 
     QSqlDatabase::removeDatabase( default_db_); // drop existing connection
      default_db_ = 0;
  }

   default_db_ = QSqlDatabase::addDatabase( st.driver );
   default_db_->setDatabaseName(st.dbname ); 
   default_db_->setUserName(st.username);
   default_db_->setPassword(st.passwd);
   default_db_->setHostName(st.hostname); 

   if ( ! default_db_->open() ) {
      QMessageBox::warning(this,"Database Connection", "Database Connection Failed");
      databaseRetrieveDataAction->setEnabled(false);
   }   
   else {  
      QMessageBox::information(this,"Database Connection", "Database Connection Successful"); 
      databaseRetrieveDataAction->setEnabled(true);
   }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::databaseRetrieve()
{ 

  if ( default_db_ )  
      dbretrieve_dlg_->show();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::showCS2DData()
{
 computeTwiss();   
 QWidget* w = new CS2DDataBrowser( bmlc_->dbConnection(), centralWidget_ , "Uncoupled Lattice Functions");
 w->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void CHEFGUI::showCS4DData()
{
  QWidget* w = new CS4DDataBrowser( bmlc_->dbConnection(), centralWidget_ , "Coupled Lattice Functions" );
  w->show();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



