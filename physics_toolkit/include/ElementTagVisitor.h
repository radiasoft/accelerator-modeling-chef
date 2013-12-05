/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE : C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons
******
******  File:      ElementTagVisitor.h
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
******  Author:    Jean-Francois Ostiguy
******             ostiguy@fnal.gov
****** 
******
**************************************************************************
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef ELEMNENTTAGVISITOR_H
#define ELEMNENTTAGVISITOR_H

#include <beamline/BmlVisitor.h>
#include <string>
#include <vector>


class ElementTagVisitor: public BmlVisitor {

public: 

struct Location {

   Location(std::string name, double au, double ad): label(name), azimuth_up(au),  azimuth_down(ad) { };

   std::string label;
   double azimuth_up;
   double azimuth_down;

 };


  ElementTagVisitor( std::vector<Location>& locations, 
                     std::string tag_name, 
                     std::string azimuth_up_tag  ="azimuth_up", 
                     std::string azimuth_down_tag="azimuth_down" );

 ~ElementTagVisitor();

 virtual void visitBmlnElmnt( bmlnElmnt* e);
 virtual void visitBeamline( beamline* e);
 
 bool overlap(double x1, double x2, double y1, double y2);

private:
  
 std::string                       tag_name_;
 std::string                       azimuth_up_tag_;
 std::string                       azimuth_down_tag_;
 
 std::vector<Location>&            locations_;
 std::vector<Location>::iterator   loc_it_;
 static const double               overlap_tolerance_;
 int                               ntags_; 


};

#endif
