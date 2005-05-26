/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   1.0                    
******                                    
******  File:      iosetup.h
******                                                                
******  Copyright Universities Research Association, Inc.    
******            All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov
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
**************************************************************************
*************************************************************************/

#ifndef IOSETUP_H
#define IOSETUP_H

#include <iosfwd>

namespace FNAL {
extern std::ostream* pcout;
extern std::ostream* pcerr;

}

// This is a global function to redirect stdout and stderr for all libraries
// depending on basic_toolkit. pcout and pcerr are ptrs to the new ostreams.

void iosetup(std::ostream* pcout, std::ostream* pcerr); 


#endif // IOSETUP_H
