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
******* REVISION HISTORY
******* Mar 2007       ostiguy@fnal.gov
******* - use new-style STL-compatible beamline iterators
******* - use new-style type safe Barnacles.
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


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ElementTagVisitor::ElementTagVisitor( vector<Location>& locations, 
                                      string tag, 
                                      string azimuth_up_tag, 
                                      string azimuth_down_tag): 
  locations_(locations), 
  loc_it_(locations.begin()), 
  tag_name_(tag), 
  azimuth_up_tag_(azimuth_up_tag),  
  azimuth_down_tag_(azimuth_down_tag),
  ntags_(0)
{}
  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||



ElementTagVisitor::~ElementTagVisitor() {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool ElementTagVisitor::overlap(double x1, double x2, double y1, double y2) {


 if (x1 == x2) { 

     return ( (x1>y1) && (x1<y2) ) ? true : false;
  }

  if (y1 == y2) { 

    return ( (y1>x1) && (y1<x2) ) ? true : false;  
    
  }

  double num =0;
  
  if ( (num = std::min(x2,y2) - std::max(x1,y1)) > 0.0 ) 
    return  ( num/std::min((x2-x1),(y2-y1)) > overlap_tolerance_ );
  else
    return false;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ElementTagVisitor::visitBmlnElmnt( bmlnElmnt* e) { 
   
  double e_up   = any_cast<double>( (*e)[azimuth_up_tag_   ]   );  
  double e_down = any_cast<double>( (*e)[azimuth_down_tag_ ] );


  // If the element is a match for the current interval
  // tag the element and make the next interval current.
   

  if ( overlap( e_up,  e_down,  loc_it_->azimuth_up,   loc_it_->azimuth_down ) ) {

    (*e)[tag_name_] = loc_it_->label;
    
    ++loc_it_;

    ++ntags_;

  };  


}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void ElementTagVisitor::visitBeamline( beamline* e) {   
   
  // ... Start visiting from the origin marker  
  

   for ( beamline::deep_iterator it  = e->deep_begin(); 
                                 it != e->deep_end(); ++it ) {
       visitBmlnElmnt( (*it).get() ); 

   };

   (*pcout) << "Tag table contains " << locations_.size()  << " entries." << std::endl;  
   (*pcout) << "Number of beamline elements tagged : "      <<  ntags_   << std::endl;  

}

