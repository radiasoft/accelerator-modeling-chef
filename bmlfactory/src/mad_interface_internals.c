/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      mad_interface_internals.c
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


 /* -*- C -*- */

#include <math.h>

#if !defined(mad_interface_internals_h)
#include "mad_interface_internals.h"
#endif /* mad_interface_internals_h */

void
variable_init( variable* var ) {
   memset( var->name_, 0, VAR_NAME_MAX );
   var->value_ = HUGE;
}

void
variable_destroy( variable* var ) {
  variable_init( var );
}

void
constant_init( constant* cons ) {
   memset( cons->name_, 0, VAR_NAME_MAX );
   cons->value_ = HUGE;
}

void
constant_destroy( constant* cons ) {
  constant_init( cons );
}
