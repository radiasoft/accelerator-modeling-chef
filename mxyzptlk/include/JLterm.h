/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      JLterm.h
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab     
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
******  ostiguy@fnal.gov
******
******  stub to preserve src compatibility
******  with pre-template code base.  
******                                               
**************************************************************************
*************************************************************************/

#ifndef JLTERM_H
#define JLTERM_H

#include <TJLterm.h>

typedef TJLterm<double>                JLterm;
typedef TJLterm<std::complex<double> > JLCterm;


#endif // JLTerm.h
