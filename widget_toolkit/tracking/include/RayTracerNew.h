/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  File:      RayTracerNew.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC 
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
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
**************************************************************************
*************************************************************************/

#ifndef RAYTRACERNEW_H
#define RAYTRACERNEW_H

#include <RayTracerBase.h>

class RayTracerNew : public RayTracerBase {

 public:
      
  RayTracerNew( QWidget* parent=0, char const* name=0, WFlags f=0);    

};

#endif //RAYTRACERNEW_H

