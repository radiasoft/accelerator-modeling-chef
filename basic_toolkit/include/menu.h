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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                                
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
