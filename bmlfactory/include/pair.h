/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      pair.h
******                                                                
******  Copyright (c) 1999  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Dmitri Mokhov and Oleg Krivosheev                  
******                                                                
******  Contact:   Leo Michelotti or Jean-Francois Ostiguy            
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef pair_h
#define pair_h

typedef struct {
    void* first_;
    void* second_;
} pair;

#endif /* pair_h */
