/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******                        Beamline
******                                    
******  File:      PhaseSpaceIndexing.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************/

#ifndef PHASESPACEINDEXING_H
#define PHASESPACEINDEXING_H


struct PhaseSpaceIndexing { 

 static int const  BMLN_dynDim = 6;

 enum index { i_x   = 0, i_y   = 1, i_cdt =  2, 
              i_npx = 3, i_npy = 4, i_ndp =  5 };

};


#endif // PHASESPACEINDEXING_H
