/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      menu.h
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


#ifndef MENU_HXX_LPJM
#define MENU_HXX_LPJM

typedef void (*MENUFUNC)();
#define MAXMENUCHOICES 10

class ttMenu {
  int numChoices;
  MENUFUNC menuFunction[ MAXMENUCHOICES ];
  char* menuMessage[ MAXMENUCHOICES ];
  char* promptMessage;
public:
  ttMenu();
  void go();
  void print();
  void setItem( char*, MENUFUNC );
  void setOperation( int, MENUFUNC );
  void setMessage( int, char* );
  void setPrompt( char* );
} ;

#endif // MENU_HXX_LPJM
