***************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:                                                        ****** 
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

#include <iostream>
#include <string>
#include <appworkspace.h>

// Note: widget->parentWidget() is necessary here. 
// For MDI, The "parent" is actually a widget that simulates the
// window decoration. The decoration of windows that have 0 as 
// parent is the responsibilty of the X window manager. 

void AppWorkspace::tileVertically() 
{
  
 QWidgetList windows = windowList();
 if (windows.count() < 2) {
            tile();
            return;
        }
        int wHeight = height() / windows.count();
        int y = 0;
        for (QWidget *widget = windows.first(); widget; widget = windows.next()) {
            widget->parentWidget()->resize(width(), wHeight);
            widget->parentWidget()->move(0, y);
            y += wHeight;
        }
}
    

      
void AppWorkspace::tileHorizontally()
{

}


void AppWorkspace::defaultLayout()
{
 
 QWidgetList windows = windowList();

 for (QWidget *widget = windows.first(); widget; widget = windows.next()) {
            
   if ( std::string( widget->name()) == std::string("BeamlineBrowser") ) {
     widget->parentWidget()->resize(width()/3, height());
     widget->parentWidget()->move(0, 0);
   } 
   else if ( std::string( widget->name()) == std::string("Messages") ) {
     widget->parentWidget()->resize(2*width()/3, height()/5);
     widget->parentWidget()->move(width()/3, 4*height()/5);
   }
   else if ( std::string( widget->name()) == std::string("Interpreter") ) {
     widget->parentWidget()->resize(2*width()/3, 2*height()/5);
     widget->parentWidget()->move(width()/3, 0);
   }
   else if ( std::string( widget->name()) == std::string("Devices") ) {
     widget->parentWidget()->resize(2*width()/3, 2*height()/5);
     widget->parentWidget()->move(width()/3, 2*height()/5);
   }
 }
}


void AppWorkspace::cascade()
{
 
 QWorkspace::cascade();  

 QWidgetList windows = windowList();

 for (QWidget *widget = windows.first(); widget; widget = windows.next()) {
     widget->parentWidget()->resize(width()*3/4, height()*3/4);
 } 
 
}
