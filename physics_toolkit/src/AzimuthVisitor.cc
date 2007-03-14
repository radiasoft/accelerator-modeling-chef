/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE : C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons
******
******  File:      AzimuthVisitor.cc
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

#include <physics_toolkit/AzimuthVisitor.h>

#include <beamline/beamline.h>
#include <boost/any.hpp>

using std::string;
using boost::any;
using boost::any_cast;

AzimuthVisitor::AzimuthVisitor(Particle& p, 
                               string origin_tag, 
                               string azimuth_up_tag, 
                               string azimuth_down_tag)
: BmlVisitor(), particle_(p), 
  origin_tag_( origin_tag ), 
  azimuth_up_tag_(azimuth_up_tag), 
  azimuth_down_tag_(azimuth_down_tag), 
  azimuth_(0.0) 
{ }
  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

AzimuthVisitor::~AzimuthVisitor() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// void setAzimuthLabelUp(std::string s)   
// {  _azimuth_up_tag      = s; } INLINED

// void setAzimuthLabelDown(std::string s) 
// {  _azimuth_down_tag    = s; } INLINED


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AzimuthVisitor::visit( bmlnElmnt& e) 
{ 

  e[azimuth_up_tag_]   = any(azimuth_); 

  azimuth_ += e.OrbitLength( particle_); 

  e[azimuth_down_tag_] = any(azimuth_); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void AzimuthVisitor::visit( beamline& bml) 
{   
   
// ... Start visiting from the origin marker  


beamline::deep_iterator dit = bml.deep_begin();

for ( ;  dit != bml.deep_end(); ++dit) 
{

   if (  ( std::string( (*dit)->Type() ) == std::string("marker") ) &&
	  ( std::string( (*dit)->Name() ) == origin_tag_         )        
	) break;
} 

// ... At this point, we should have found the origin marker. However, there might not be one;
// ... in this case, p == 0. 

if( dit != bml.deep_end() ) visit( **dit );

// ... Continue and visit the elements between the origin marker and the end of the line
// ... If there was no origin marker, the effective origin becomes the first element in the line   

for ( ;  dit != bml.deep_end(); ++dit) 
 visit( **dit );

// Finally, visit elements between beginning of the line and beamline and the origin marker  

dit = bml.deep_begin();

for ( ;  dit != bml.deep_end(); ++dit) 
{
  if (  ( std::string( (*dit)->Type() ) == std::string("marker") ) &&
	   ( std::string( (*dit)->Name() ) == origin_tag_         )         
     ) break;  

    visit( **dit ); 
}

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
