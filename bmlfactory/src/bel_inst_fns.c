/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2                    
******                                    
******  File:      bel_inst_fns.c
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


   // from MXYZPLTK
#include <beamline.h>

static inline bmlnElmnt*
make_multipole( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_solenoid( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_instrument( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_ecollimator( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_rcollimator( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_yrot( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_beambeam( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_matrix( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_lump( char* name, double length ) {
  return new drift( name, length );
}
