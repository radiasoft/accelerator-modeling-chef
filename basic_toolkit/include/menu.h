/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                ****
******  MENU: C++ object for creating VT terminal menus.              ****
******                                                                ****
******  Copyright (c) 1989  Universities Research Association, Inc.   ****
******                All Rights Reserved                             ****
******                                                                ****
******  Author:    Leo Michelotti                                     ****
******                                                                ****
******             Fermilab                                           ****
******             P.O.Box 500                                        ****
******             Mail Stop 345                                      ****
******             Batavia, IL   60510                                ****
******                                                                ****
******             Phone: (708) 840 4956                              ****
******             Email: MICHELOTTI@FNALAD                           ****
******                    MICHELOTTI@ADCALC.FNAL.GOV                  ****
******                    ALMOND::MICHELOTTI                          ****
******                                                                ****
******  Release    Version 1.0 : Sep. 20, 1989                        ****
******  Dates:                                                        ****
**************************************************************************
**************************************************************************
******                                                                ****
******  This material resulted from work developed under a            ****
******  Government Contract and is subject to the following           ****
******  license: The Government retains a paid-up, nonexclusive,      ****
******  irrevocable worldwide license to reproduce, prepare           ****
******  derivative works, perform publicly and display publicly       ****
******  by or for the Government, including the right to distribute   ****
******  to other Government contractors.  Neither the United          ****
******  States nor the United States Department of Energy, nor        ****
******  any of their employees, makes any warranty, express or        ****
******  implied, or assumes any legal liability or responsibility     ****
******  for the accuracy, completeness or usefulness of any           ****
******  information, apparatus, product, or process disclosed, or     ****
******  represents that its use would not infringe privately owned    ****
******  rights.                                                       ****
******                                                                ****
******  These files are made avaliable for use by specific            ****
******  individuals or a specific group.  They are not to be          ****
******  reproduced or redistributed.                                  ****
******                                                                ****
******  These files are provided "as is" with no warranties of any    ****
******  kind including the warranties of design,                      ****
******  merchantibility and fitness for a particular purpose,         ****
******  or arising from a course of dealing, usage or trade           ****
******  practice.                                                     ****
******                                                                ****
******  These files are provided with no support or obligation on     ****
******  the part of U.R.A. or Fermilab to assist in their use,        ****
******  correction, modification or enhancement.                      ****
******                                                                ****
******  Neither U.R.A. nor Fermilab shall be liable with              ****
******  respect to the infringement of copyrights, trade              ****
******  secrets or any patents by these files or any part             ****
******  thereof.                                                      ****
******                                                                ****
******  In no event will U.R.A., Fermilab, or the author(s) be        ****
******  liable for any lost revenue or profits or other               ****
******  special, indirect and consequential damages, even if          ****
******  they have been advised of the possibility of such             ****
******  damages.                                                      ****
******                                                                ****
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
