/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BasePropagator.cc
******
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
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
******  Author: Jean-Francois Ostiguy  
******          ostiguy@fnal.gov
******
******
**************************************************************************
*************************************************************************/

#include <beamline/ParticleBunch.h>
#include <beamline/BasePropagator.h>
#include <beamline/bmlnElmnt.h>
#include <iostream>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator::BasePropagator()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator::BasePropagator( BasePropagator const& o)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BasePropagator& BasePropagator::operator=( BasePropagator const& o)
{
  if ( this == &o ) return *this; 
  
  return *this; //default  
} 

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BasePropagator::setup( bmlnElmnt& elm) 
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BasePropagator::operator()(  bmlnElmnt const& elm,    ParticleBunch& b) 
{
 for (  ParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  {  
    (*this)( elm, *it ); 
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BasePropagator::operator()(  bmlnElmnt const& elm, JetParticleBunch& b)  
{
 for (  JetParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  {  
    (*this)( elm, *it ); 
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   BasePropagator::setAttribute( bmlnElmnt& elm, std::string const& attribute, boost::any const& value )
{ 
  // std::cout << "element   " << elm.Type() << "  " << elm.Name() << std::endl;   
  // std::cout << "attribute " << attribute << " has been modified." << std::endl;   
} 



