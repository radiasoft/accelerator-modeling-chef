/***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      chefmain.cpp                                      ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Author:    Jean-Francois Ostiguy                             ******
******             Fermilab                                          ****** 
******             ostiguy@fnal.gov                                  ****** 
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

#include <boost/python.hpp>
#include <iostream>

#include <CHEFGUI.h>

extern boost::python::class_ <CHEFGUI, CHEFGUI*,  boost::noncopyable>*  PythonTypeCHEFGUIPtr;

#include "CHEFGUI.h"
#include "../images/chef-logo-preview.xpm"
#include <qapplication.h>
#include <qsplashscreen.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qwindowsstyle.h>
#include <qdatetime.h>
#include <time.h>
#include <exception>
#include <GL/glut.h>

using namespace boost::python;


CHEFGUI* gui = 0;

CHEFGUI* get_existing() { return gui; } 

int main( int argc, char** argv )
{

  glutInit( &argc, argv); 

  QApplication a(argc,argv);


  a.setStyle( new QWindowsStyle );

  QDate expiration(2007,3,1);
 
 
  if (QDate::currentDate() > expiration ) {
    QMessageBox::information(0, "Expired", "This beta version executable has expired.",
                             QMessageBox::Ok);  
    exit(0);
  };

  QSplashScreen *splash = new QSplashScreen( (const char**) chef_logo_preview_xpm );
  splash->message("BUILD DATE: 20060130", (Qt::AlignHCenter | Qt::AlignBottom), QColor("White") );

  splash->show();

  gui = new CHEFGUI( 0, "CHEF", Qt::WDestructiveClose);

 #if 0 
  static boost::python::object py_get_existing = 
     make_function( &get_existing, return_value_policy< reference_existing_object>() );

  object myobject = py_get_existing();

  boost::python::handle<> chef_module ( boost::python::borrowed( PyImport_AddModule("chef") ) );
  boost::python::handle<> chef_dict   ( boost::python::borrowed( PyModule_GetDict(chef_module.get()) ) );
  PyDict_SetItemString( chef_dict.get(), "gui",  myobject.ptr() );
 #endif
   
  int w = a.desktop()->width();
  int h = a.desktop()->height();

  gui->setGeometry(w/20,h/20, w*0.90, h*0.90);

  clock_t start_time, cur_time;

  time(&start_time);
  do {
    time(&cur_time);
  }
  while ( (cur_time - start_time) < 2 );

  a.setMainWidget( gui ) ;
  gui->show();
  splash->finish( gui );
  delete splash;

  try 
  {
     return a.exec();
  }
  catch( std::exception& e)
  {

  QMessageBox mb(QString("Fatal Error"), QString( e.what() ), 
                 QMessageBox::Critical, 
                 QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
 
  mb.show();
  
  while (mb.isVisible() ) qApp->processEvents();
  
  return 1;
 
  } 
 
  
}
