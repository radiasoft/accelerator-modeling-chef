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
******                                                                
**************************************************************************
*************************************************************************/

#include <iostream>
#include <basic_toolkit/GenericException.h>

using namespace std;

GenericException::GenericException( string fileName, 
                                    int lineNumber, 
                                    const char* fcn, 
                                    const char* msg ):
  _lineno(0)
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

  _lineno = lineNumber;
}

const char* GenericException::what() const throw()
{
  return errorString.c_str();
}


int GenericException::lineNumber() const throw()
{
  return _lineno;
}
