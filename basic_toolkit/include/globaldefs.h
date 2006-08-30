/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   1.0                    
******                                    
******  File:      globaldefs.h
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
#ifndef GLOBALDEFS_H 
#define GLOBALDEFS_H 

#if 0
=================================================================
#ifdef _MSC_VER
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
  #endif
  #define DLLLOCAL
#else /* GCC */
    #define DLLEXPORT __attribute__ ((visibility("default")))
    #define DLLLOCAL  __attribute__ ((visibility("hidden")))
#endif
==================================================================
#endif

/*----------------------------------------------------------------*/
/* for the moment, ignore visibility attributes ...         - jfo */  
/*----------------------------------------------------------------*/

#define DLLEXPORT
#define DLLLOCAL


#endif /* GLOBALDEFS_H */
