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
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
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
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif


/* -------------------------------------------------- 
		Required for use of menu tools....... */

#include <stdio.h>
#include <basic_toolkit/menu.h>

/* --------------------------------------------------- */

ttMenu::ttMenu() {
  MENUFUNC menuNullFunc = 0;
  for( int i = 0; i < MAXMENUCHOICES; i++ ) menuFunction[i] = menuNullFunc;
  promptMessage = "";
  numChoices = 0;
}

void ttMenu::go() {
  int menuChoice = 0;
  this->print();
  printf( "\n%s", promptMessage.c_str() );
  int status = scanf( "%d", &menuChoice );
  getchar();
  while( ( menuChoice >= 0 )&& ( menuChoice <= numChoices ) ) {
    if( menuChoice == 0 ) this->print();
    else (*menuFunction[menuChoice])();
    printf( "\n%s", promptMessage.c_str() );
    status = scanf( "%d", &menuChoice );
    getchar();
    }
}

void ttMenu::print() {
int i = 0;
printf( "\n" );
 while( ++i <= numChoices ) printf( "%2d) %s\n", i, menuMessage[i].c_str() );
 printf( "\n" );
}

void ttMenu::setItem( std::string message, MENUFUNC functionName ) {
  if( numChoices < MAXMENUCHOICES - 2 ) {
    menuMessage[ ++numChoices ] = message;
    menuFunction[ numChoices ] = functionName;
    }
}

void ttMenu::setOperation( int choice, MENUFUNC functionName ) {
  if( ( choice > 0 ) && ( choice <= numChoices ) )
    menuFunction[ choice ] = functionName;
}

void ttMenu::setMessage( int choice, std::string message ) {
  if( ( choice > 0 ) && ( choice <= numChoices ) )
    menuMessage[ choice ] = message;
}

void ttMenu::setPrompt( std::string message ) {
  promptMessage = message;
}

void menuNullFunc() {
return;
}
