/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      about.cpp                                         ****** 
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

#include <about.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>



static QString credits(
#include "credits.txt.cstr"
);

static QString fnal_license(
#include <fnal-LICENCE.txt.cstr>
);

static QString boost_license(
#include <boost-LICENSE_1_0.txt.cstr>
);

static QString python_license(
#include <python-LICENSE.txt.cstr>
);

static QString qt_license(
#include <QTACADEMIC.txt.cstr>
);


// ..........................................................................


About::About(QWidget* parent, const char* name, WFlags f):
       AboutBase(parent,name,f) 
{

  QFont fnt("Lucida Console", 8);
  fnt.setFixedPitch(true);

  textBrowserCredits->setFont(fnt);
  textBrowserCredits->append(credits);
  textBrowserCredits->moveCursor(QTextEdit::MoveHome, false);

  textBrowserSupport->setFont(fnt); 
  textBrowserSupport->append("Please visit: http://www-ap.fnal.gov/CHEF");
  textBrowserSupport->moveCursor(QTextEdit::MoveHome, false);

  textBrowserLegal->setFont(fnt);
  textBrowserLegal->setColor( "red");
  textBrowserLegal->append( fnal_license );  
  textBrowserLegal->setColor("black");
  textBrowserLegal->append( qt_license ); 
  textBrowserLegal->setColor("black");
  textBrowserLegal->append( boost_license );
  textBrowserLegal->setColor("black");
  textBrowserLegal->append( python_license );
  textBrowserLegal->moveCursor(QTextEdit::MoveHome, false);
}
  

