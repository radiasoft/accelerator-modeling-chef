/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      tuneChromAdjustBarnacle.h
******
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

#ifndef TUNE_CHROM_BARNACLE_H
#define TUNE_CHROM_BARNACLE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/Barnacle.h>
#include <beamline/beamline.h>

class DLLEXPORT tuneChromAdjustBarnacle : public BarnacleData {
public:
  tuneChromAdjustBarnacle() { h_tune = v_tune = h_chrom = v_chrom = 0; }
  int h_tune, v_tune, h_chrom, v_chrom;
};

#endif /* TUNE_CHROM_BARNACLE_H */
