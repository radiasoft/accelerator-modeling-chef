/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******                                    
******  File:      GenericException.c
******  Version:   1.0
******                                                                
******  Copyright (c) 2004 Universities Research Association, Inc.    
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

#include <iostream>
#include "GenericException.h"

using namespace std;

GenericException::GenericException( string fileName, 
                                    int lineNumber, 
                                    const char* fcn, 
                                    const char* msg )
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    cerr << errorString;
    cerr << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}

const char* GenericException::what() const throw()
{
  return errorString.c_str();
}
