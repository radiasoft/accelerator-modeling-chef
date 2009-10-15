/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  File:      bel_inst_fns.h
******  Version:   1.3
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
******                                                                
**************************************************************************
*************************************************************************/

#include <beamline/beamline.h>

static inline BmlnElmnt*
make_multipole( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_elseparator( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_instrument( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_ecollimator( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_rcollimator( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_yrot( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_beambeam( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_matrix( const char* name, double length ) {
  return new Drift( name, length );
}

static inline BmlnElmnt*
make_lump( const char* name, double length ) {
  return new Drift( name, length );
}
