/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE : C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons
******
******  File:      AzimuthVisitor.h
******
******  Copyright (c) Universities Research Association, Inc.
******                All Rights Reserved
******
******  Author:    Jean-Francois Ostiguy
******             ostiguy@fnal.gov
****** 
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
*******
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/


#ifndef AZIMUTHVISITOR_H
#define AZIMUTHVISITOR_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Particle.h>
#include <string>

class beamline;
class bmlnElmnt;


class AzimuthVisitor: public BmlVisitor {

public: 

  AzimuthVisitor(Particle& p, 
                 std::string origin_tag       = "ORIGIN",
                 std::string azimuth_up_tag   = "azimuth_up", 
                 std::string azimuth_down_tag = "azimuth_down");
 
  virtual ~AzimuthVisitor();


  void   setAzimuthUpTag( std::string s )  {  azimuth_up_tag_      = s; }
  void setAzimuthDownTag( std::string s )  {  azimuth_down_tag_    = s; }
  void      setOriginTag( std::string s )  {  origin_tag_          = s; }

  void visit( beamline&  e);   
  void visit( bmlnElmnt& e); 


private:

 Particle           particle_;
 std::string        azimuth_up_tag_;
 std::string        azimuth_down_tag_;
 std::string        origin_tag_;
 double             azimuth_;

};

#endif
