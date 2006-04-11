/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      mad_interface_internals.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.
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
******  URA/FNAL reserves all rights.
******                                          
******                                                                
******  Authors:    Dmitri Mokhov and Oleg Krivosheev                  
******                                                                
******
******  Contacts:            Leo Michelotti michelotti@fnal.gov
******             Jean-Francois Ostiguy    ostiguy@fnal.gov
******                                                              
**************************************************************************
*************************************************************************/
#ifndef MAD_INTERFACE_INTERNALS_H
#define MAD_INTERFACE_INTERNALS_H

void variable_init( variable* );
void variable_destroy( variable* );

void constant_init( constant* );
void constant_destroy( constant* );

void beam_element_init( beam_element* );
void beam_element_destroy( beam_element* );

void beam_line_init( beam_line* );
void beam_line_destroy( beam_line* );

#endif /*  MAD_INTERFACE_INTERNALS_H */
