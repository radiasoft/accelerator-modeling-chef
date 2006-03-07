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
**************************************************************************/
/* Creation Date:  August 2004                                           */
/* Revision Date:  July,  2005                                           */
/*   - added five slots: _editEditElement                                */
/*                       _editFlatten                                    */
/*                       _editMisalign                                   */
/*                       _editNewOrder                                   */
/*                       _pushParticles                                  */
/*   - LPJM                                                              */
/*                                                                       */
/*************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <ostream>
#include <istream>
#include <streambuf>

#include <CHEFGUI.h>
#include <filters.h>
#include <BeamlineBrowser.h>
#include <BmlSelectionDialog.h>
#include <InitCondDialogLF.h>
#include <EditDialog.h>
#include <DistributionWidget.h>
#include <FramePusher.h>
#include <SurveyMatcher.h>
#include <DbConnectDialog.h>
#include <DbRetrieveDialog.h>
#include <appworkspace.h>
#include <about.h>

#include <qwt/qwt_plot.h>

#include <GenericException.h>
#include <ParserException.h>
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

#include <CF_Editor.h>
#include <qpychef.h>
#include <devicetable.h>

#include <LattFncData.h>
#include <ETFncData.h>
#include <LBFncData.h>
#include <DspnFncData.h>
#include <MomentsFncData.h>

#include <chefplotmain.h>
#include <DbConnectDialog.h>

#include <iosetup.h>

using FNAL::pcout;
using FNAL::pcerr;

using namespace std;

extern beamline* DriftsToSlots( /* const */ beamline& original );



CHEFGUI::CHEFGUI(QWidget* parent, const char* name, WFlags f):
CHEFGUIBase(parent,name,f), _plotWidget(0),
                            _ETplotWidget(0),
                            _MMplotWidget(0),
                            _LBplotWidget(0),
                            _DspnplotWidget(0),
                            _p_vwr(0)

{

  _userOptions.couplingOption = Options::ignore_coupling;

  _p_currBmlCon  = 0;
  _p_currQBmlRoot = 0;
  _p_clickedQBml = 0;

  // Create an initial Jet environment
  double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };
  Jet__environment::BeginEnvironment( 1 );

  _x   = new coord(0.0);
  _y   = new coord(0.0);
  _z   = new coord(0.0);
  _px  = new coord(0.0);
  _py  = new coord(0.0);
  _pz  = new coord(0.0);

  _p_JetEnv  = Jet__environment::EndEnvironment(scale);
  _p_JetCEnv = JetC::_lastEnv = *_p_JetEnv; // implicit conversion

  if( _p_JetEnv != Jet::_lastEnv ) {
    std::stringstream uic;
    uic << "An impossibility has occurred\nin file "
        << __FILE__
        << " at line " << __LINE__
        << ":\n_p_JetEnv != Jet::lastEnv";
    QMessageBox::information( 0, "CHEF: ERROR",
                              uic.str().c_str() );
  }

  // Make connections
  connect( this, SIGNAL(_new_beamline()),
           this, SLOT  (_launch_browser()) );



  _centralWidget = new AppWorkspace(this, "AppWorkspace");

  setCentralWidget( _centralWidget );


  // the browser initial set of windows are created in QShowEvent handler. This allow for correct initial geometry
  // based on the initial main widget dimensions.   _launch_browser();


  _eventfilter = new WindowCloseEventEater();


  // initialize the beamline browser
  // -------------------------------

   _p_vwr = new BeamlineBrowser( centralWidget(), "BeamlineBrowser" );

   _p_vwr->hide();
   _p_vwr->installEventFilter( _eventfilter );

   windowsTree_BrowserAction->setOn(true);
   _p_vwr->setAllColumnsShowFocus( true );



    connect( this,   SIGNAL(_modeChanged( const BeamlineContext* )),
             _p_vwr, SLOT(resetPixmap( const BeamlineContext* ))        );

    connect( _p_vwr, SIGNAL(sig_bmlLeftClicked( BeamlineContext*,   QBmlRoot* )),
             this,   SLOT( _set_p_clickedContext( BeamlineContext*, QBmlRoot* ))    );

    connect( _p_vwr, SIGNAL( sig_bmlLeftClicked( QBml* )),
             this,   SLOT(_set_p_clickedQBml( QBml* ))    );

    connect( _p_vwr, SIGNAL( sig_newContext( BeamlineContext* )),
             this,   SLOT(_slot_contextGenerated( BeamlineContext* ))    );

   // initialize the Message window and the associated Message io
   // --------------------------------------------------------
 
     _messages  = new QTextEdit(_centralWidget, "Messages");
     _messages->setCaption("Messages");
     _messages->setFont(QFont("MiscFixed"));
     _messages->hide();
     _messages->installEventFilter(_eventfilter);


     _messages_stdout = new Messages<0>(_messages);
     _messages_stdwar = new Messages<1>(_messages);
     _messages_stderr = new Messages<2>(_messages);

     _p_messages_stdout_stream = new std::ostream( _messages_stdout ); 
     _p_messages_stdwar_stream = new std::ostream( _messages_stdwar ); 
     _p_messages_stderr_stream = new std::ostream( _messages_stderr );
  
#ifdef _WIN32
     _messages->append( QString("CHEF (Win32) Beta Version, July 2005)") );
#else
     _messages->append( QString("CHEF (Unix/Linux) Beta Version,  July 2005)") );
#endif
     _messages->append( QString("(C) URA/Fermilab, All Rights Reserved.\n") );
    
     iosetup(_p_messages_stdout_stream, _p_messages_stdwar_stream);

     windowsMessagesAction->setOn(true);


   // initialize the Python Interpreter window
   // ------------------------------------------

     _interpreter = 0;
     _interpreter = new QPyCHEF(_centralWidget , "Interpreter");
     _interpreter->hide();
     _interpreter->installEventFilter(_eventfilter);
     windowsInterpreterAction->setOn(true);


   // initialize the ACNET Devices Monitor window
   // ------------------------------------------

    _devices = new DeviceTable(_centralWidget, "Devices");
    _devices->hide();
    _devices->installEventFilter(_eventfilter);
     windowsDevicesAction->setOn(true);


   // initialize the Database support
   //---------------------------------

#if 0
     // there seems to be a bug in Qt when windows with a fixed size are displayed in a 
     // QWorkspace.
 
    _dbconnect_dlg  = new DbConnectDialog(  dynamic_cast<QWorkspace*>(_centralWidget), 
                                            "Database Connection");  
    _dbretrieve_dlg = new DbRetrieveDialog( dynamic_cast<QWorkspace*>(_centralWidget), 
                                            "Retrieve Data from Database");
#endif
    _dbconnect_dlg  = new DbConnectDialog( 0, "Database Connection");  
    _dbretrieve_dlg = new DbRetrieveDialog(0, "Retrieve Data from Database");

    connect( _dbconnect_dlg, SIGNAL( buttonOkClicked(const DbConnectStruct&)    ), 
             this,             SLOT(_handleDBConnection(const DbConnectStruct&) ) );

    _default_db = 0;

    databaseRetrieveDataAction->setEnabled(false);

   // initialize the Text Editor
   // --------------------------



    // initialize the help window
    //----------------------------

    _assistanthelp = new QAssistantClient("", this ); // path is empty; use system path to find assistant executable

    QStringList cmdLst;
    cmdLst << "-hideSidebar";
    cmdLst << "-profile"<<QString("doc") + QDir::separator() + QString("chefdoc.adp");
    _assistanthelp->setArguments( cmdLst );

    // initialize the Beamline Section Dialog

    _bmlSelectionDialog = new BmlSelectionDialog(this,"BmlSelectionDialog", WType_Popup );
    _bmlSelectionDialog->hide();


    connect(_p_vwr , SIGNAL( sig_browserIsEmpty()    ),    this, SLOT( _disableMenus() ) );
    connect(_p_vwr , SIGNAL( sig_browserIsNotEmpty() ),    this, SLOT( _enableMenus()  )  );

    _disableMenus();


    // initialize persistent dialogs

    _initCondDialogLF       = new InitCondDialogLF(this, "InitCondDialogLF", 0);
    _initCondDialogLF->ULFMode();
    _initCondDialogLF->setCaption("Uncoupled Lattice Functions: Initial Conditions");

    _initCondDialogDisp     = new InitCondDialogLF(this, "InitCondDialogLF", 0);
    _initCondDialogDisp->dispersionMode();
    _initCondDialogDisp->setCaption("Dispersion: Initial Conditions");

    _initCondDialogMoments  = new InitCondDialogLF(this, "InitCondDialogLF", 0);
    _initCondDialogMoments->momentsMode();
    _initCondDialogMoments->setCaption("Moments (Including Transverse Coupling) : Initial Conditions");

}


CHEFGUI::~CHEFGUI()
{

   // remove the eventfilters

    if (_p_vwr)       _p_vwr->removeEventFilter( _eventfilter );
    if (_messages)    _messages->removeEventFilter( _eventfilter );
    if (_interpreter) _interpreter->removeEventFilter( _eventfilter );
    if (_devices)     _devices->removeEventFilter( _eventfilter );

    // delete the message window ostream and streambuf objects

    //if (_p_messages_stdout_stream) delete _p_messages_stdout_stream;
    //if (_p_messages_stderr_stream) delete _p_messages_stderr_stream;

    //if (_messages_stdout) delete _messages_stdout;
    //if (_messages_stderr) delete _messages_stderr;

    if (_x)  delete _x;
    if (_y)  delete _y;
    if (_z)  delete _z;
    if (_px) delete _px;
    if (_py) delete _py;
    if (_pz) delete _pz;

}

void CHEFGUI::_openFile()
{
  std::auto_ptr<bmlfactory> bfp;

  // Open file dialog
  QString s = QFileDialog::getOpenFileName( QString::null,
                   "MAD (*.mad *.lat);;Beamline (*.bml)");

  if( s.isEmpty() )
  {
      return;
  }
  else
  {

    if( QString(".mad") == s.right(4) ||
        QString(".lat") == s.right(4)    )
    {

      try
      {
        bfp.reset(new bmlfactory(s.ascii(), (char*)0 )); // cast needed to avoid ambiguity
      }
      catch (ParserException& e)
      {
        QMessageBox mb(QString("Error"), QString( e.what() ), 
                    QMessageBox::Critical, 
                    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        mb.show();
        while (mb.isVisible())  qApp->processEvents(); 
        return;
      }
      catch (GenericException& e)
      {
        QMessageBox::information( 0, "CHEF: ERROR", e.what() );
        return;
      }

       std::list<std::string>& beamline_list = bfp->getBeamlineList();

        //  instantiate only the n last beamlines defined in the mad file.

        _bmlSelectionDialog->setList( beamline_list, bfp->getUseStatementBeamlineName() );
        _bmlSelectionDialog->setBeamParameters( *bfp.get() );
        _bmlSelectionDialog->show();

        while (_bmlSelectionDialog->isVisible()) qApp->processEvents();

        double brho = _bmlSelectionDialog->getBRHO();

        _bmlSelectionDialog->getSelected(); // this function alters beamline_list

        std::list<std::string>::iterator it;
        int nlines = 0;
        for ( it = beamline_list.begin(); it != beamline_list.end(); it++) {
	    beamline* bmlPtr = 0;
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

            _p_vwr->displayBeamline( _p_currBmlCon );
            nlines++;
	}
	

    }
     else
     { // Read .bml file, not .mad file

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

      _contextList.insert( _p_currBmlCon );
      emit _new_beamline();
     }
    }

  _p_vwr->clearSelection();

}


void CHEFGUI::_openDeviceFile()
{


 QString s = QFileDialog::getOpenFileName( QString::null,
                   "Devices (*.dev)" );

 if ( s.isEmpty() ) return;

 std::ifstream is( s.ascii());

 _devices->readDeviceList(is);
 is.close();

}

void CHEFGUI::_openScriptFile()
{

 QString s = QFileDialog::getOpenFileName( QString::null,
                   "Script (Python) (*.py)" );

  if ( s.isEmpty() ) return;

  _interpreter->readFile( s.ascii() );

}


void CHEFGUI::_print()
{
  double s = 0.0;
  if( 0 != _p_currBmlCon ) 
  { _p_currBmlCon->peekAt( s, _p_currBmlCon->getParticle() ); }
}


void CHEFGUI::_fileWriteTree()
{
  if( 0 != _p_currBmlCon ) { _p_currBmlCon->writeTree(); }
}


void CHEFGUI::_fileSaveAs()
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



}

void CHEFGUI::_fileEditorSaveAs()
{

  // save the file in the opened editor window that has focus


CF_Editor* editor = dynamic_cast<CF_Editor*>( dynamic_cast<QWorkspace*>(_centralWidget)->activeWindow() );

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

void CHEFGUI::_fileEditorSave()
{

  // save the file in the opened editor window that has focus


CF_Editor* editor = dynamic_cast<CF_Editor*>( dynamic_cast<QWorkspace*>(_centralWidget)->activeWindow() );

 if (!editor) return;

 std::ofstream outStream( editor->caption().latin1() );
 outStream << editor->text();
 outStream.close();

}

void 
CHEFGUI::editParse()
{
 
  CF_Editor* editor = 0;
  
  // **** determine which editor (if any) has focus; 

  QWidgetList wlist = dynamic_cast<QWorkspace*>(centralWidget())->windowList(QWorkspace::StackingOrder);

  for (QWidget* w = wlist.first(); w; w = wlist.next())
  {
   if ( dynamic_cast<QWorkspace*>(_centralWidget)->activeWindow() == w ) 
    {
      if ( QString((w)->name()) == QString("MAD8 Editor") ) 
      { 
	if ( editor = dynamic_cast<CF_Editor*>(w) ) _parseEditorMAD8( editor );
      }
      if ( QString((w)->name()) == QString("Python Editor") ) 
      { 
        if( editor = dynamic_cast<CF_Editor*>(w) )  _parseEditorPython(editor);
      }
    }
  }
  return;
}
 

void 
CHEFGUI::_parseEditorMAD8( CF_Editor* editor )
{
  std::auto_ptr<bmlfactory> bfp;
 
  try 
  {
    bfp.reset( new bmlfactory( editor->caption().ascii(), editor->text().ascii()) ); // the 2nd argument is the actual text buffer. 
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

  std::list<std::string>& beamline_list = bfp->getBeamlineList();
       
  //  instantiate only the n last beamlines defined in the mad file. 

  _bmlSelectionDialog->setList( beamline_list, bfp->getUseStatementBeamlineName() );
  _bmlSelectionDialog->setBeamParameters( *bfp.get() );
  _bmlSelectionDialog->show();

  while (_bmlSelectionDialog->isVisible()) qApp->processEvents();

  double brho = _bmlSelectionDialog->getBRHO();

  _bmlSelectionDialog->getSelected(); // this function alters beamline_list 

  std::list<std::string>::iterator it;
  int nlines = 0;
  beamline* bmlPtr = 0;
  
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
        _p_vwr->displayBeamline( _p_currBmlCon );
        nlines++;
  } 
}


void 
CHEFGUI::_parseEditorPython( CF_Editor* /* editor */ )
{
  //  FIXME ! QPyCHEF needs to be fixed
  //  _interpreter->runBuffer( editor->text().ascii() );
}

void CHEFGUI::_editFindFilter()
{
  QueryDialog* qdl = new QueryDialog( 0, 0, false, Qt::WDestructiveClose );
  connect( qdl,  SIGNAL(_useThis( const BoolNode& )),
           this, SLOT  (_processFilter( const BoolNode& )) );
  qdl->show();
}


void CHEFGUI::_editSelectAll()
{
  _p_vwr->selectAll( true );
}


void CHEFGUI::_editSelectNone()
{
  _p_vwr->clearSelection();
  _p_currBmlCon   = 0;
  _p_currQBmlRoot = 0;
  _p_clickedQBml  = 0;
}


void CHEFGUI::_editCopyLine()
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

void CHEFGUI::_editRenameLine()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  QDialog* wpu = new QDialog( 0, 0, true );
    QVBox* qvb = new QVBox( wpu );

      QHBox* qhb1 = new QHBox( qvb );
        new QLabel( "Name: ", qhb1 );
        QLineEdit* qle = new QLineEdit( _p_currBmlCon->name(), qhb1 );
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
    _p_currBmlCon->rename( qle->text().ascii() );
    _p_currQBmlRoot->setText( 0, qle->text() );
  }

  delete wpu;
}


void CHEFGUI::_editRemoveLine()
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


void CHEFGUI::_horTuneCtrl()
{

 if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( _p_currBmlCon->isTreatedAsRing() ) {
    _testFC( CHEFGUI::_buildHTuneCircuit );
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }
}


void CHEFGUI::_verTuneCtrl()
{

  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( _p_currBmlCon->isTreatedAsRing() ) {
    _testFC( CHEFGUI::_buildVTuneCircuit );
  }
  else {
    QMessageBox::information( 0, "CHEF: ERROR",
                              "Selected line is not periodic."
                              "\nTry fixing with Edit/Mode function." );
  }

}


int
CHEFGUI::_buildHTuneCircuit( const CHEFGUI* aPtr, const bmlnElmnt* bPtr )
{
  aPtr->_p_currBmlCon->addHTuneCorrector( bPtr );
  return 0; // this should really be a void function

}


int
CHEFGUI::_buildVTuneCircuit( const CHEFGUI* aPtr, const bmlnElmnt* bPtr )
{
  aPtr->_p_currBmlCon->addVTuneCorrector( bPtr );
  return 0; // this should really be a void function

}


void CHEFGUI::_editCondense()
{
  DriftEliminator de;

  if (!_p_currBmlCon)
  {
     QMessageBox::information( 0, "CHEF: INFO",
                              "You must select a beamline first." );
     return;
  };

  _p_currBmlCon->accept(de);

  beamline* bmlPtr = de.clonedBeamlinePtr();
  _p_currBmlCon = new BeamlineContext( _p_currBmlCon->getParticle(),
                                       bmlPtr, false );
  _p_currBmlCon->setClonedFlag( true );
  _contextList.insert( _p_currBmlCon );

  emit _new_beamline();
}


void CHEFGUI::_editNewOrder()
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
           "\nFailure: beamline element not chosen correctly."
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


void CHEFGUI::_editMergeQuads()
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


void CHEFGUI::_toolMisalign()
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


void CHEFGUI::_editAlignBends()
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

      Particle* pPtr = (_p_currBmlCon->getParticle()).Clone();
      pPtr->SetReferenceEnergy( _p_currBmlCon->getEnergy() );
        // This line is almost certainly unnecessary.
      pPtr->setStateToZero();
      // REMOVE: double inState [] = { 0., 0., 0., 0., 0., 0. };
      // REMOVE: inState[ p.npxIndex() ]
      // REMOVE:             =  ( p.Momentum()/p.ReferenceMomentum() )
      // REMOVE:                * sin( prototype->getPoleFaceAngle() / 2.0 );
      // REMOVE: inState[ pPtr->npxIndex() ]
      // REMOVE:             =  ( p.Momentum()/p.ReferenceMomentum() )
      // REMOVE:                * sin( prototype->getPoleFaceAngle() );
      // REMOVE: pPtr->setState( inState );
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


void CHEFGUI::_editFlatten()
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


void CHEFGUI::_pushParticles()
{
  DistributionWidget* dwPtr
    = new DistributionWidget( *(_p_currBmlCon), 0, 0, Qt::WDestructiveClose );
  dwPtr->show();
}


void CHEFGUI::_editEditElement()
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
             "\nFailure: beamline element not chosen correctly."
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


void CHEFGUI::_editAlign()
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
      if( 0 != _p_currBmlCon) { _p_currBmlCon->accept( euclid ); }
     }
  }
}


void CHEFGUI::_editAlignData()
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
  };
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


void CHEFGUI::_editD2S()
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


CHEFGUI::insDlgData CHEFGUI::_insertionDialog() const
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
  CHEFGUI::insDlgData ret;
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


void CHEFGUI::_editAddMarkers()
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
             "\nIn function: void CHEFGUI::_editAddMarkers():"
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


void CHEFGUI::_editAddQtMons()
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
             "\nIn function: void CHEFGUI::_editAddQtMons():"
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


void CHEFGUI::_editMode()
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


void CHEFGUI::_editPartition()
{
  // This is a stripped down version.
  // Code to be entered like that in CHEFGUI::_editAddMarkers()

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
           "\nIn function: void CHEFGUI::_editPartition():"
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


void CHEFGUI::_editPartAndSect()
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
      bmlPtr->InsertElementsFromList( s, insl, removedElements );
      bmlPtr->append( spaceCharge[numberOfSectors] );

      // Display and delete the removed elements
      *pcout << "Removed elements\n";

      slist_iterator fembril( removedElements );
      bmlnElmnt* qq;
      while(( qq = (bmlnElmnt*) fembril() )) {
        //cout << qq->Type() << "  " << qq->Name() << "\n";
        delete qq;
      }
      //cout << std::endl;

      // Create a temporary Jet environment
      // ----------------------------------
      EnvPtr<double>::Type                 formerJetEnv  = Jet::_lastEnv;
      EnvPtr<std::complex<double> >::Type  formerJetCEnv = JetC::_lastEnv;
      double scale[]  = { 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3, 1.0e-3 };
      Jet__environment::BeginEnvironment( order );
      // coord x(0.0),  y(0.0),  z(0.0),
      //      px(0.0), py(0.0), pz(0.0);
      // ??? This needs to be fixed!!!
      coord* x = new coord(0.0);
      coord* y = new coord(0.0);
      coord* z = new coord(0.0);
      coord* px = new coord(0.0);
      coord* py = new coord(0.0);
      coord* pz = new coord(0.0);
      Jet__environment::EndEnvironment(scale);
      JetC::_lastEnv = *_p_JetEnv ; // implicit conversion


      // Sectorize between the partition markers.
      // ----------------------------------------
      pPtr = (_p_currBmlCon->getParticle()).Clone();
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
      // -----------------------------
      Jet::_lastEnv  = formerJetEnv;
      JetC::_lastEnv = formerJetCEnv;

      delete x;
      delete y;
      delete z;
      delete px;
      delete py;
      delete pz;
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



void CHEFGUI::_helpContents()
{
 _assistanthelp->openAssistant();

}


void CHEFGUI::_helpAbout()
{

  QWidget* helpabout = new About( this,"About CHEF", Qt::WDestructiveClose );
  helpabout->show();

}



void CHEFGUI::_slot_contextGenerated( BeamlineContext* x )
{
  if( x )
  { if( x->getClonedFlag() )
    { _p_currBmlCon = x;
      _contextList.insert( x );
    }
  }
}


void CHEFGUI::_uncoupledLFDispatch(){

 if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( _p_currBmlCon->isTreatedAsRing() ) {
    CHEFGUI::_launchLatt();
  } else {
     CHEFGUI::_pushULF();
  }
}

void CHEFGUI::_momentsDispatch(){

 if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( _p_currBmlCon->isTreatedAsRing() ) {
    CHEFGUI::_launchMoments();
  } else {
     CHEFGUI::_pushMoments();
  }
}

void CHEFGUI::_dispersionDispatch(){

 if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF", "Must select a beamline first." );
    return;
  }

  if( _p_currBmlCon->isTreatedAsRing() ) {
    CHEFGUI::_launchDispersion();
  } else {
     CHEFGUI::_pushDispersion();
  }
}


void CHEFGUI::_launchLatt()
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

    if (!_plotWidget)
    {
      _plotWidget  =  new CHEFPlotMain( _centralWidget, "plotWidget", Qt::WDestructiveClose );
       char theCaption[1024];
       for( int i = 0; i < 1024; i++ ) {
          theCaption[i] = '\0';
       }
       strcat( theCaption, "CHEF: Lattice Functions (uncoupled): " );
       strcat( theCaption, _p_currBmlCon->name() );
       _plotWidget->setCaption( theCaption );
       _plotWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
       _plotWidget->setAutoClear(true);
       _plotWidget->show();

    };

    _plotWidget->addData( *lfd );
    delete lfd;
    lfd = 0;
  }

  //std::cout << " TJL<double, std::complex<double> >::_initstorecalls = " << TJL<double, std::complex<double> >::_initstorecalls << std::endl;
  //std::cout << " TJL<std::complex<double>, double >::_initstorecalls = " << TJL<std::complex<double>, double >::_initstorecalls << std::endl;
  // std::cout << " double  pool size = " <<  TJL<double, std::complex<double> >::_thePool.size() << std::endl;
  //std::cout << " complex pool size = " <<  TJL<std::complex<double>, double >::_thePool.size() << std::endl;


}


void CHEFGUI::_launchLB()
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
    if (!_LBplotWidget)
    {
       _LBplotWidget = new CHEFPlotMain( _centralWidget, "plotWidget", Qt::WDestructiveClose); // destructive close needed !

       char theCaption[1024];
       for( int i = 0; i < 1024; i++ ) {
         theCaption[i] = '\0';
       }
       strcat( theCaption, "CHEF: Lattice Functions (eigenvectors): " );
       strcat( theCaption, _p_currBmlCon->name() );
       _LBplotWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
       _LBplotWidget->setCaption( theCaption );
       _LBplotWidget->setAutoClear(true);
       _LBplotWidget->show();
    };
    _LBplotWidget->addData( *lbfd );
    delete lbfd;
    lbfd = 0;
  }
}



void CHEFGUI::_launchET()
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
    if (!_ETplotWidget)
    {
       _ETplotWidget = new CHEFPlotMain( _centralWidget, "plotWidget",
                                                      Qt::WDestructiveClose);
       // destructive close needed !


     char theCaption[1024];
     for( int i = 0; i < 1024; i++ ) {
       theCaption[i] = '\0';
     };
     strcat( theCaption, "CHEF: Lattice Functions (factorization): " );
     strcat( theCaption, _p_currBmlCon->name() );
     _ETplotWidget->setCaption( theCaption );
     _ETplotWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
     _ETplotWidget->setAutoClear(true);
     _ETplotWidget->show();
    };

    _ETplotWidget->addData( *etfd );
    delete etfd;
    etfd = 0;
  }
}


void CHEFGUI::_launchMoments()
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
    if (!_MMplotWidget)
    {
       _MMplotWidget = new CHEFPlotMain( _centralWidget, "plotWidget",
                                                      Qt::WDestructiveClose);
       // destructive close needed !

       char theCaption[1024];
       for( int i = 0; i < 1024; i++ ) {
        theCaption[i] = '\0';
       }
      strcat( theCaption, "CHEF: Lattice Functions (covariance): " );
      strcat( theCaption, _p_currBmlCon->name() );
      _MMplotWidget->setCaption( theCaption );
      _MMplotWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
      _MMplotWidget->setAutoClear(true);
      _MMplotWidget->show();
   };

   _MMplotWidget->addData( *mfd );
    delete mfd;
    mfd = 0;
  }

}

void CHEFGUI::_pushMoments()
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

#if 0
   CovarianceSage::Info info_default;
  _p_currBmlCon->getInitial( &info_default);
  _initCondDialogMoments->setInitCond( );
#endif

  if( QDialog::Accepted == _initCondDialogMoments->exec() ){
    // Two steps are needed here:
    //   (1) read the text widgets and
    //   (2) return the lattice functions.
    _initCondDialogMoments->readInputValues();
    LattFuncSage::lattFunc initialConditions( _initCondDialogMoments->getInitCond() );

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
      QMessageBox::information( this, "CHEF", "Calculation failed." );
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

      if( 0 != mfd) { delete mfd; mfd = 0; }
      return;
    }

    if(mfd) {
      if (!_MMplotWidget)
      {
         _MMplotWidget = new CHEFPlotMain( _centralWidget, "MMplotWidget", Qt::WDestructiveClose );
         // destructive close needed !


         char theCaption[1024];
         for( int i = 0; i < 1024; i++ ) {
           theCaption[i] = '\0';
         }
         strcat( theCaption, "CHEF: Lattice Functions (covariance): " );
         strcat( theCaption, _p_currBmlCon->name() );

         _MMplotWidget->setCaption( theCaption );
         _MMplotWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
         _MMplotWidget->setAutoClear(true);
         _MMplotWidget->show();
      };

      _MMplotWidget->addData( *mfd  );
      delete mfd;
      mfd = 0;
    }
  }

}


void CHEFGUI::_pushDispersion()
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

#if 0
  DispersionSage::Info disp_defaults;
  _p_currBmlCon->getInitial(&disp_defaults);
#endif

  if( QDialog::Accepted == _initCondDialogDisp->exec() ){
    // Two steps are needed here:
    //   (1) read the text widgets and
    //   (2) return the lattice functions.
    _initCondDialogDisp->readInputValues();
    LattFuncSage::lattFunc initialConditions( _initCondDialogDisp->getInitCond() );

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

      if( 0 != dfd) { delete dfd; dfd = 0; }
      return;
    }

    if(dfd) {
      if (!_DspnplotWidget)
      {
         _DspnplotWidget = new CHEFPlotMain( _centralWidget, "DspnplotWidget", Qt::WDestructiveClose );
         // destructive close needed !


         char theCaption[1024];
         for( int i = 0; i < 1024; i++ ) {
           theCaption[i] = '\0';
         };

         strcat( theCaption, "CHEF: Dispersion: " );
         strcat( theCaption, _p_currBmlCon->name() );
         _DspnplotWidget->setCaption( theCaption );
         _DspnplotWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
         _DspnplotWidget->setAutoClear(true);
         _DspnplotWidget->show();

      };

      _DspnplotWidget->addData( *dfd  );
      delete dfd;
      dfd = 0;
    }
  }

}


void CHEFGUI::_pushULF()
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

#if 0
  LattFuncSage::lattFunc lfic;
   _p_currBmlCon->getInitial( &lfic);
   _initCondDialogLF->setInitCond( lfic );
#endif

  if( QDialog::Accepted == _initCondDialogLF->exec() ){
    // Two steps are needed here:
    //   (1) read the text widgets and
    //   (2) return the lattice functions.
    _initCondDialogLF->readInputValues();
    LattFuncSage::lattFunc initialConditions( _initCondDialogLF->getInitCond() );

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

      if( 0 != lfd) { delete lfd; lfd = 0; }
      return;
    }

    if(lfd) {
      if (!_plotWidget)
      {
         _plotWidget = new CHEFPlotMain( _centralWidget, "PlotWidget", Qt::WDestructiveClose );
         // destructive close needed !


         char theCaption[1024];
         for( int i = 0; i < 1024; i++ ) {
           theCaption[i] = '\0';
         };

         strcat( theCaption, "CHEF: Uncoupled Lattice Functions: " );
         strcat( theCaption, _p_currBmlCon->name() );
         _plotWidget->setCaption( theCaption );
         _plotWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
         _plotWidget->show();
      };

      _plotWidget->addData( *lfd  );
      delete lfd;
      lfd = 0;
    }
  }


}


void CHEFGUI::_launchTrack()
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

  _trackWidget = new Tracker( _p_currBmlCon, _centralWidget, "Tracker", Qt::WDestructiveClose);
  _trackWidget->setCaption( "CHEF:: Phase Space Tracking" );
  _trackWidget->setGeometry(0, 0,_centralWidget->width(), _centralWidget->height());
  _trackWidget->show();
}


void CHEFGUI::_launchRayTrace()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  _traceWidget = new RayTrace(_p_currBmlCon, _centralWidget, "RayTrace", Qt::WDestructiveClose );
  _traceWidget->setCaption( "CHEF:: Orbit Trace" );
  _traceWidget->setGeometry(0,0, _centralWidget->width(), _centralWidget->height() );
  _traceWidget->show();
}

void CHEFGUI::_launchDispersion()
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
    if (!_DspnplotWidget)
    {
       _DspnplotWidget = new CHEFPlotMain( _centralWidget, "plotWidget",
                                                        Qt::WDestructiveClose);
       // destructive close needed !


       char theCaption[1024];
       for( int i = 0; i < 1024; i++ ) {
         theCaption[i] = '\0';
       };

       strcat( theCaption, "CHEF: Dispersion: " );
       strcat( theCaption, _p_currBmlCon->name() );
       _DspnplotWidget->setCaption( theCaption );
       _DspnplotWidget->setGeometry( 0, 0, _centralWidget->width(), _centralWidget->height() );
       _DspnplotWidget->setAutoClear(true);
       _DspnplotWidget->show();
     };

      _DspnplotWidget->addData( *dfd  );
      delete dfd;
      dfd=0;
  }

}

void CHEFGUI::_launchDilution()
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
  else
  {
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
        QPushButton* okayBtn = new QPushButton( "OK", qhb3 );
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


void CHEFGUI::_pushArgs()
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

void CHEFGUI::_enterMapArg()
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


void CHEFGUI::_enterContextArg()
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


void CHEFGUI::_clearArgs()
{
  for( int i = 0; i < CHEF_numargs; i++ ) {
    if( _toolArgs[i] ) {
      delete _toolArgs[i];  // virtual destructor should work
      _toolArgs[i] = 0;
    }
  }
}


void CHEFGUI::_launchSiteVu()
{
  if( 0 == _p_currBmlCon ) {
    QMessageBox::information( 0, "CHEF",
                              "Must select a beamline first." );
    return;
  }

  _siteWidget = new SiteViewer( *(_p_currBmlCon) , _centralWidget, "SiteViewer", Qt::WDestructiveClose );
  // Must be deleted by itself.
  _siteWidget->setCaption( QString("CHEF:: Site Viewer")+
                           QString("    ")+
                           QString(_p_currBmlCon->name()) );
  _siteWidget->setGeometry(0,0,_centralWidget->width(), _centralWidget->height() );
  _siteWidget->show();
}


void CHEFGUI::_launch_browser()
{


  _p_vwr->clearSelection();

  if( _p_currBmlCon) _p_vwr->displayBeamline( _p_currBmlCon ); // display the beamline only if it exists;

}


void CHEFGUI::_tuneCtrl()
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
      if( (0. != deltaNuH) || (0. != deltaNuV) ) {
        if( 0 != (returnCode = _p_currBmlCon->changeTunesBy( deltaNuH, deltaNuV )) ) {
          (*pcerr) << "*** WARNING *** File "
               << __FILE__
               << ", Line "
               << __LINE__
               << ": Tune adjustment returned error condition "
               << returnCode
               << std::endl;
          QMessageBox::information( 0, "CHEF",
                      "Tune adjustment error." );
        }
      }
    }
  }

  delete wpu;
}


void CHEFGUI::_chromCtrl()
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

}


void CHEFGUI::_set_p_clickedContext( BeamlineContext* x, QBmlRoot* y )
{
  _p_currBmlCon   = x;
  _p_currQBmlRoot = y;
  _p_clickedQBml  = y;
}


void CHEFGUI::_set_p_clickedQBml( QBml* x )
{
  _p_clickedQBml = x;
}



void CHEFGUI::_do_nothing()

{
  QMessageBox::information( 0, "CHEF",
                            "Sorry. This function is not implemented." );
}



void CHEFGUI::showEvent(QShowEvent* event) {

  // The initial size and positions of all windows are set here.
  // Receiving a QShowEvent indicates that the main window has been displayed.

  if ( event->spontaneous() ) return QMainWindow::showEvent(event);

  QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  _p_vwr->setSizePolicy(policy);
  _p_vwr->show();

  _messages->setSizePolicy(policy);
  _messages->show();

  _interpreter->setSizePolicy(policy);
  _interpreter->show();


  _devices->setSizePolicy(policy);
  _devices->show();

  _windowsDefaultLayout();

   return QMainWindow::showEvent(event);

}


void CHEFGUI::_processFilter( const BoolNode& query )
{

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


void CHEFGUI::_testFC( ACTFUNC11 actfcn ) const
{
  QListViewItem* fc = _p_vwr->firstChild();

  while( 0 != fc ) {
    if(typeid(*fc) == typeid(QBmlRoot)) {
      this->_traverseTree( dynamic_cast<QBmlRoot*>(fc),
                           actfcn );
    }

    else {
      std::stringstream uic;
      uic << "An impossibility has occurred\nin file "
          << __FILE__
          << " at line " << __LINE__ ;
      QMessageBox::information( 0, "CHEF: ERROR",
                                uic.str().c_str() );
    }

    fc = fc->nextSibling();
  }
}


void CHEFGUI::_traverseTree( const QBmlRoot* x, ACTFUNC11 actfcn ) const
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
      //      << std::endl;
      if( fc->isSelected() ) {
        (*actfcn)( this, dynamic_cast<QBmlElmt*>(fc)->cheatElementPtr() );
      }
    }

    else if(typeid(*fc) == typeid(QBmlRoot)) {
      this->_traverseTree( dynamic_cast<QBmlRoot*>(fc), actfcn );
    }

    else {
      std::stringstream uic;
      uic << "An impossibility has occurred\nin file "
          << __FILE__
          << " at line " << __LINE__ ;
      QMessageBox::information( 0, "CHEF: ERROR",
                                uic.str().c_str() );
    }

    fc = fc->nextSibling();
  }

}


void CHEFGUI::windowsMessagesAction_toggled( bool set )
{
  if (set)
    _messages->show();
  else
    _messages->hide();
}


void CHEFGUI::windowsTree_BrowserAction_toggled( bool set )
{
  if (set)
    _p_vwr->show();
  else
    _p_vwr->hide();
}


void CHEFGUI::windowsInterpreterAction_toggled( bool  set){

  if (set) {
    _interpreter->show();
  }
  else
  {
    _interpreter->hide();
  }
}

void CHEFGUI::windowsDevicesAction_toggled( bool  set)
{
  if (set)
  {
    _devices->show();
  }
  else
  {
    _devices->hide();
  }
}


void CHEFGUI::_windowsCascade() {

  dynamic_cast<AppWorkspace*>(_centralWidget)->cascade();

}

void CHEFGUI::_windowsMinimizeAll(){



}


void CHEFGUI::_windowsTile(){

  dynamic_cast<AppWorkspace*>(_centralWidget)->tile();

}


void CHEFGUI::_windowsDefaultLayout(){

  dynamic_cast<AppWorkspace*>(_centralWidget)->defaultLayout();

}


void CHEFGUI::_disableMenus()
{
  _enableMenus(false);

}

void CHEFGUI::_enableMenus()
{

 _enableMenus(true);

}

void CHEFGUI::_enableMenus( bool set )
{

//  Enable/Disable actions that do/do not make sense if
//  at least one beamline is/is not defined;

    calculationsLatticeFunctionsEigenvaluesAction->setEnabled(set);
    calculationsLattice_FunctionsUncoupledAction->setEnabled(set);
    calculationsLattice_FunctionsMomentsAction->setEnabled(set);
    calculationsLattice_FunctionsEdwards_TengAction->setEnabled(set);
    calculationsPushULFAction->setEnabled(set);
    calculationsPushMomentsAction->setEnabled(set);
    calculationsPushDispersionAction->setEnabled(set);

    toolsSite_ViewerAction->setEnabled(set);
    toolsTrackAction->setEnabled(set);
    toolsTraceAction->setEnabled(set);
    calculationsDispersionAction->setEnabled(set);
    calculationsEmittance_DilutionAction->setEnabled(set);
    controlsBuildHTuneCct->setEnabled(set);
    controlsBuildVTuneCct->setEnabled(set);
    controlsAdjustTune->setEnabled(set);
    toolsGenBunch->setEnabled(set);

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
    editSelectlineAction->setEnabled(set);

    editMergeEquivalentQuadsAction->setEnabled(set);
    editConvertToSlotsAction->setEnabled(set);
    editInsertMonitorAction->setEnabled(set);
    editPartitionAndSectorizeAction->setEnabled(set);

}


void CHEFGUI::_exit()
{

  close();

  // exit(1); // this is for debugging (memory usage diags) only

}


void CHEFGUI::_devicesClearSelection()
{

  _devices->clearSelection();

}


void CHEFGUI::_devicesRefreshAll()
{

  devicesRefreshAction->setEnabled(false);
  _devices->refreshAll();
  devicesRefreshAction->setEnabled(true);
}



void CHEFGUI::devicesAutoRefresh(bool set)
{
  _devices->setAutoRefresh(set);
}


void CHEFGUI::modeRing(bool /*set */)
{
  // obsolete ??
  // modeLineAction->setOn(!set);
}


void CHEFGUI::modeLine(bool /*set */)
{
  // obsolete ??
  // modeRingAction->setOn( !set);
}


void CHEFGUI::fileEditPython()
{
  std::ifstream is;
  QString theText;
  char buffer[1024];

  QString s = QFileDialog::getOpenFileName( QString::null,
                                            "Script (python) (*.py )" );

  if( s.isEmpty() ) return;

  is.open(s);

   CF_Editor* editor = new CF_Editor( _centralWidget, "Python Editor", Qt::WDestructiveClose);
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
   editor->resize(_centralWidget->width()-_p_vwr->width()-20, _centralWidget->height()-30);
   editor->move(_p_vwr->width()+10, 0);
   editor->show();
}


void CHEFGUI::fileEditMAD8()
{
  std::ifstream is;
  QString theText;
  char buffer[1024];

  QString s = QFileDialog::getOpenFileName( QString::null,
                     "MAD (*.mad *.lat)");

  if( s.isEmpty() ) return;

   CF_Editor* editor = new CF_Editor( _centralWidget , "MAD8 Editor", Qt::WDestructiveClose);
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
      editor->resize(width()-_p_vwr->frameGeometry().width()-20,
                     _centralWidget->height()-30);
      editor->move( _p_vwr->frameGeometry().width()+10, 0);
      editor->show();

}


void 
CHEFGUI::_databaseConnect() { 

  _dbconnect_dlg->show();

}

void CHEFGUI::_handleDBConnection( const DbConnectStruct& st )
{
 
  if (_default_db ) { 
     QSqlDatabase::removeDatabase(_default_db); // drop existing connection
     _default_db = 0;
  }

   _default_db = QSqlDatabase::addDatabase( st.driver );
   _default_db->setDatabaseName(st.dbname ); 
   _default_db->setUserName(st.username);
   _default_db->setPassword(st.passwd);
   _default_db->setHostName(st.hostname); 

   if ( ! _default_db->open() ) {
      QMessageBox::warning(this,"Database Connection", "Database Connection Failed");
      databaseRetrieveDataAction->setEnabled(false);
   }   
   else {  
      QMessageBox::information(this,"Database Connection", "Database Connection Successful"); 
      databaseRetrieveDataAction->setEnabled(true);
   }
}


void 
CHEFGUI::_databaseRetrieve(){ 

  if (_default_db )  
     _dbretrieve_dlg->show();

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
    (*pcerr) << "\n*** ERROR *** "
            "\n*** ERROR *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** ERROR *** argPtr::argPtr( void* x )"
            "\n*** ERROR *** Constructor does not accept null argument."
         << std::endl;
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
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** MappingPtr::MappingPtr( const Mapping& x, bool y ) "
            "\n*** WARNING *** Second argument is being ignored."
         << std::endl;
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
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** BeamlineContextPtr::BeamlineContextPtr( const BeamlineContext& x, bool y ) "
            "\n*** WARNING *** Second argument is being ignored."
         << std::endl;
  }
}


BeamlineContextPtr::~BeamlineContextPtr()
{
  if( _owned ) { delete ((BeamlineContext*) _ptr); }
}
