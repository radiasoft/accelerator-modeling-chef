/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      DialogElmInfo.h
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
*************************************************************************
*************************************************************************
************************************************************************/
#ifndef DIALOGELMINFO_H
#define DIALOGELMINFO_H

#include <DialogElmInfoBase.h>

class DialogElmInfo : public DialogElmInfoBase  {

 public:

  DialogElmInfo( QWidget* parent, char const* name =0, bool modal=true, WFlags f=0 );

  void addPropertyRow( QString prop, QString units,  QString val );

};

#endif // DIALOGELMINFO_H

