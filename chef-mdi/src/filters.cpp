#include "filters.h"
#include <qevent.h>
#include <qwidget.h>
#include <qstring.h>
#include <iostream>

#include "CHEFGUI.h"
#include<qaction.h>

bool WindowCloseEventEater::eventFilter(QObject *o, QEvent *e)
{

  QCloseEvent* qce = 0;
  CHEFGUI*     w =  dynamic_cast<CHEFGUI*>(  o->parent()->parent()->parent() );



  if ( qce = dynamic_cast<QCloseEvent*>(e) ) {


    if ( QString( o->name()) == "BeamlineBrowser" ) {
      w->windowsTree_BrowserAction->toggle();
    
    } else if ( QString( o->name()) == "Messages" )  {
      w->windowsMessagesAction->toggle();

    } else if ( QString( o->name()) == "Interpreter" ) { 
      w->windowsInterpreterAction->toggle();

    } else if ( QString( o->name()) == "Devices" ) {
      w->windowsDevicesAction->toggle();

    }

    return TRUE;
  }

  else { 

    return QObject::eventFilter(o,e);
  }
}         


 
