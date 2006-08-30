/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.
******                                    
******  File:      JetWriter.h
******                                                                
******  Copyright (c) 2006  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 234
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
****** Revision History:
****** ----------------
******   May, 2006: 
******     - Still just a prototype but nonetheless useful.
******
**************************************************************************
*************************************************************************/

#ifndef JETWRITER_H
#define JETWRITER_H

#include <physics_toolkit/TJetWriter.h>

typedef TJetWriter<double>                 JetWriter;
typedef TJetWriter<std::complex<double> >  JetCWriter;

#endif // JETWRITER_H
