#include <iostream>
#include <string>
#include <appworkspace.h>

// Note: widget->parentWidget() is necessary here. For MDI, The "parent" is actually a widget that simulates the
// window decoration. The decoration of windows that have 0 as parent is the responsibilty of the X window manager. 

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
     widget->parentWidget()->resize(0.75*width(), 0.75*height());
 } 
 

   
 }
