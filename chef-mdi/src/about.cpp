#include "about.h"
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




//=============================================================================



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
  

