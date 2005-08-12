***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      filters.cpp                                       ****** 
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

#include <filters.h>
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


 
