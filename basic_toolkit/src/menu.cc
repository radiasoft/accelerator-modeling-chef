#if HAVE_CONFIG_H
#include <config.h>
#endif
/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      menu.cc
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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


/* -------------------------------------------------- 
		Required for use of menu tools....... */

#include <stdio.h>

#include "menu.h"

/* --------------------------------------------------- */

ttMenu::ttMenu() {
  MENUFUNC menuNullFunc = 0;
  for( int i = 0; i < MAXMENUCHOICES; i++ ) menuFunction[i] = menuNullFunc;
  promptMessage = "\0";
  numChoices = 0;
}

void ttMenu::go() {
  int menuChoice = 0;
  this->print();
  printf( "\n%s", promptMessage );
  scanf( "%d", &menuChoice );
  getchar();
  while( ( menuChoice >= 0 )&& ( menuChoice <= numChoices ) ) {
    if( menuChoice == 0 ) this->print();
    else (*menuFunction[menuChoice])();
    printf( "\n%s", promptMessage );
    scanf( "%d", &menuChoice );
    getchar();
    }
}

void ttMenu::print() {
int i = 0;
printf( "\n" );
while( ++i <= numChoices ) printf( "%2d) %s\n", i, menuMessage[i] );
printf( "\n" );
}

void ttMenu::setItem( char* message, MENUFUNC functionName ) {
  if( numChoices < MAXMENUCHOICES - 2 ) {
    menuMessage[ ++numChoices ] = message;
    menuFunction[ numChoices ] = functionName;
    }
}

void ttMenu::setOperation( int choice, MENUFUNC functionName ) {
  if( ( choice > 0 ) && ( choice <= numChoices ) )
    menuFunction[ choice ] = functionName;
}

void ttMenu::setMessage( int choice, char* message ) {
  if( ( choice > 0 ) && ( choice <= numChoices ) )
    menuMessage[ choice ] = message;
}

void ttMenu::setPrompt( char* message ) {
  promptMessage = message;
}

void menuNullFunc() {
return;
}
