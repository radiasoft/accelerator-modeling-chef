/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE : C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons
******
******  File:      ElementTagVisitor.cc
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

#include <physics_toolkit/ElementTagVisitor.h>

#include <beamline/beamline.h>
#include <boost/any.hpp>
#include <basic_toolkit/iosetup.h>


using boost::any;
using boost::any_cast;
using std::vector;
using std::string;
using FNAL::pcout;
using FNAL::pcerr;



ElementTagVisitor::ElementTagVisitor( vector<Location>& locations, 
                                      string tag, 
                                      string azimuth_up_tag, 
                                      string azimuth_down_tag): 
  _locations(locations), 
  _loc_it(_locations.begin()), 
  _tag_name(tag), 
  _azimuth_up_tag(azimuth_up_tag),  
  _azimuth_down_tag(azimuth_down_tag),
  _ntags(0)

{  



}
  

//...............................................................................................................


ElementTagVisitor::~ElementTagVisitor() {}


//...............................................................................................................



bool ElementTagVisitor::overlap(double x1, double x2, double y1, double y2) {


 if (x1 == x2) { 

     if ( (x1>y1) && (x1<y2) )
       return true;
     else 
       return false;

  }

  if (y1 == y2) { 

     if ( (y1>x1) && (y1<x2) )  
       return true;
     else 
       return false;
    
  }

  double num =0;
  if ( (num = std::min(x2,y2) - std::max(x1,y1)) > 0.0 )
     return ( num/std::min((x2-x1),(y2-y1)) > _overlap_tolerance );
  else
    return false;

}

//...............................................................................................................


void ElementTagVisitor::visitBmlnElmnt( bmlnElmnt* e) { 
   
  double e_up   = any_cast<double>( (*e)[_azimuth_up_tag   ]   );  
  double e_down = any_cast<double>( (*e)[_azimuth_down_tag ] );


  // If the element is a match for the current interval
  // tag the element and make the next interval current.
   

  if ( overlap( e_up,  e_down,  _loc_it->azimuth_up,   _loc_it->azimuth_down ) ) {

    (*e)[_tag_name] = _loc_it->label;
    
    ++_loc_it;

    ++_ntags;

  };  


}


//................................................................................................................

void ElementTagVisitor::visitBeamline( beamline* e) {   
   
  // ... Start visiting from the origin marker  
  
   DeepBeamlineIterator it ( e );
  
   bmlnElmnt* p = 0;
   while( p = it++ ) 
   {
       visitBmlnElmnt( p ); 

   };

   (*pcout) << "Tag table contains " << _locations.size()  << " entries." << std::endl;  
   (*pcout) << "Number of beamline elements tagged : "      <<  _ntags   << std::endl;  

}

