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

AzimuthVisitor::AzimuthVisitor(Particle* p, 
                               string origin_tag, 
                               string azimuth_up_tag, 
                               string azimuth_down_tag):   
  BmlVisitor(), _p(p), 
  _origin_tag( origin_tag ), 
  _azimuth_up_tag(azimuth_up_tag), 
  _azimuth_down_tag(azimuth_down_tag), 
  _azimuth(0.0) 
{ }
  

//...............................................................................................................


AzimuthVisitor::~AzimuthVisitor() {}

//...............................................................................................................


// void setAzimuthLabelUp(std::string s)   
// {  _azimuth_up_tag      = s; } INLINED

// void setAzimuthLabelDown(std::string s) 
// {  _azimuth_down_tag    = s; } INLINED


//...............................................................................................................

void AzimuthVisitor::visitBmlnElmnt( bmlnElmnt* e) { 

  (*e)[_azimuth_up_tag]   = any(_azimuth); 

  _azimuth += e->OrbitLength( *_p); 

  (*e)[_azimuth_down_tag] = any(_azimuth); 

}

//................................................................................................................

void AzimuthVisitor::visitBeamline( beamline* e) {   
   
// ... Start visiting from the origin marker  

DeepBeamlineIterator it ( e );

bmlnElmnt* p = 0;
while( p = it++ ) 
{

   if (  ( std::string( p->Type() ) == std::string("marker") ) &&
	  ( std::string( p->Name() ) == _origin_tag         )        
	) break;
} 

// ... At this point, we should have found the origin marker. However, there might not be one;
// ... in this case, p == 0. 

if(p) visitBmlnElmnt( p );

// ... Continue and visit the elements between the origin marker and the end of the line
// ... If there was no origin marker, the effective origin becomes the first element in the line   

while ( p = it++ ) visitBmlnElmnt( p );

// Finally, visit elements between beginning of the line and beamline and the origin marker  

it.reset();

while ( p = it++ )  
{
  if (  ( std::string( p->Type() ) == std::string("marker") ) &&
	   ( std::string( p->Name() ) == _origin_tag         )         
     ) break;  

    visitBmlnElmnt( p ); 
}

}

